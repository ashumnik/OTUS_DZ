/* GStreamer
 * Copyright (C) 2022 Aleksey Shumnik <ashumnik@list.ru>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-wavplayer
 *
 * The wavplayer element plays wav files.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 wavplayer filepath=test.wav ! autoaudiosink
 * ]|
 * Plays given audio.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <glib/gstdio.h>
#include <gst/audio/gstaudiosrc.h>
#include <gst/gst.h>

#include "wavplayer.h"

GST_DEBUG_CATEGORY_STATIC(wavplayer_debug_category);
#define GST_CAT_DEFAULT wavplayer_debug_category

#ifndef O_BINARY
#define O_BINARY (0)
#endif

#define SEC_SIZE 96000
#define SAMPLE_SIZE 2
#define HEADER_SIZE 44

const unsigned char CHUNKID_SIG[] = {'R', 'I', 'F', 'F'};
const unsigned char SUBCHUNK1ID_SIG[] = {'f', 'm', 't', ' '};
const unsigned char SUBCHUNK2ID_SIG[] = {'d', 'a', 't', 'a'};
const unsigned char FORMAT_SIG[] = {'W', 'A', 'V', 'E'};

/* prototypes */

static void _wavplayer_set_property(GObject *object, guint property_id,
                                     const GValue *value, GParamSpec *pspec);
static void _wavplayer_get_property(GObject *object, guint property_id,
                                     GValue *value, GParamSpec *pspec);
static void _wavplayer_dispose(GObject *object);
static void _wavplayer_finalize(GObject *object);

static gboolean _wavplayer_open(GstAudioSrc *src);
static gboolean _wavplayer_prepare(GstAudioSrc *src,
                                    GstAudioRingBufferSpec *spec);
static gboolean _wavplayer_unprepare(GstAudioSrc *src);
static gboolean _wavplayer_close(GstAudioSrc *src);
static guint _wavplayer_read(GstAudioSrc *src, gpointer data, guint length,
                              GstClockTime *timestamp);
static guint _wavplayer_delay(GstAudioSrc *src);
static void _wavplayer_reset(GstAudioSrc *src);

enum 
{ 
	PROP_0, 
	PROP_FILEPATH 
};

/* pad templates */

static GstStaticPadTemplate wavplayer_src_template = GST_STATIC_PAD_TEMPLATE(
    "src", GST_PAD_SRC, GST_PAD_ALWAYS,
    GST_STATIC_CAPS("audio/x-raw,WAVE=S16LE,rate=48000,"
                    "channels=1,layout=interleaved"));

/* class initialization */

G_DEFINE_TYPE_WITH_CODE(
    WavPlayer, wavplayer, GST_TYPE_AUDIO_SRC,
    GST_DEBUG_CATEGORY_INIT(wavplayer_debug_category, "wavplayer", 0,
                            "debug category for wavplayer element"));

static void wavplayer_class_init(WavPlayerClass *klass) 
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstAudioSrcClass *audio_src_class = GST_AUDIO_SRC_CLASS(klass);

    /* Setting up pads and setting metadata should be moved to
       base_class_init if you intend to subclass this class. */
    gst_element_class_add_static_pad_template(GST_ELEMENT_CLASS(klass),
                                              &wavplayer_src_template);

    gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass), "WAVplayer",
                                          "Generic", "WAVplayer",
                                          "Aleksey Shumnik <ashumnik@list.ru>");

    gobject_class->set_property = _wavplayer_set_property;
    gobject_class->get_property = _wavplayer_get_property;
    gobject_class->dispose = _wavplayer_dispose;
    gobject_class->finalize = _wavplayer_finalize;
    audio_src_class->open = GST_DEBUG_FUNCPTR(_wavplayer_open);
    audio_src_class->prepare = GST_DEBUG_FUNCPTR(_wavplayer_prepare);
    audio_src_class->unprepare = GST_DEBUG_FUNCPTR(_wavplayer_unprepare);
    audio_src_class->close = GST_DEBUG_FUNCPTR(_wavplayer_close);
    audio_src_class->read = GST_DEBUG_FUNCPTR(_wavplayer_read);
    audio_src_class->delay = GST_DEBUG_FUNCPTR(_wavplayer_delay);
    audio_src_class->reset = GST_DEBUG_FUNCPTR(_wavplayer_reset);

    g_object_class_install_property(
        gobject_class, PROP_FILEPATH,
        g_param_spec_string("filepath", "filepath", "filepath of the audio", "",
                            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void wavplayer_init(WavPlayer *wavplayer) 
{
}

void _wavplayer_set_property(GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec) 
{
    WavPlayer *wavplayer = _WAVPLAYER(object);

    GST_DEBUG_OBJECT(wavplayer, "set_property");

    switch (property_id) 
	{
    case PROP_FILEPATH:
        wavplayer->filepath = g_strdup(g_value_get_string(value));
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

void _wavplayer_get_property(GObject *object, guint property_id, 
							 GValue *value, GParamSpec *pspec) 
{
    WavPlayer *wavplayer = _WAVPLAYER(object);

    GST_DEBUG_OBJECT(wavplayer, "get_property");

    switch (property_id) 
	{
    case PROP_FILEPATH:
        g_value_set_string(value, wavplayer->filepath);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

void _wavplayer_dispose(GObject *object) 
{
    WavPlayer *wavplayer = _WAVPLAYER(object);

    GST_DEBUG_OBJECT(wavplayer, "dispose");

    G_OBJECT_CLASS(wavplayer_parent_class)->dispose(object);
}

void _wavplayer_finalize(GObject *object) 
{
    WavPlayer *wavplayer = _WAVPLAYER(object);

    GST_DEBUG_OBJECT(wavplayer, "finalize");

    if (wavplayer->filepath != NULL) 
	{
        free(wavplayer->filepath);
    }

    G_OBJECT_CLASS(wavplayer_parent_class)->finalize(object);
}

/* open device */
static gboolean _wavplayer_open(GstAudioSrc *src) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    GST_DEBUG_OBJECT(wavplayer, "open");

    int flags = O_RDONLY | O_BINARY;

    wavplayer->input = g_open(wavplayer->filepath, flags, 0);
    if (wavplayer->input < 0) 
	{
        goto error_exit;
    }

    int ret = 0;

    ret += read(wavplayer->input, &wavplayer->WavHeader.RIFF, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.ChunkSize, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.WAVE, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.fmt, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.Subchunk1Size, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.AudioFormat, 2);
    ret += read(wavplayer->input, &wavplayer->WavHeader.NumOfChan, 2);
    ret += read(wavplayer->input, &wavplayer->WavHeader.SamplesPerSec, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.bytesPerSec, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.blockAlign, 2);
    ret += read(wavplayer->input, &wavplayer->WavHeader.bitsPerSample, 2);
    ret += read(wavplayer->input, &wavplayer->WavHeader.Subchunk2ID, 4);
    ret += read(wavplayer->input, &wavplayer->WavHeader.Subchunk2Size, 4);

    if (ret < HEADER_SIZE ||
        memcmp(wavplayer->WavHeader.RIFF, CHUNKID_SIG, 4) ||
        memcmp(wavplayer->WavHeader.WAVE, FORMAT_SIG, 4) ||
        memcmp(wavplayer->WavHeader.fmt, SUBCHUNK1ID_SIG, 4) ||
        wavplayer->WavHeader.Subchunk1Size != 16 ||
        wavplayer->WavHeader.AudioFormat != 1 ||
        wavplayer->WavHeader.NumOfChan != 1 ||
        memcmp(wavplayer->WavHeader.Subchunk2ID, SUBCHUNK2ID_SIG, 4)) 
	{
		goto error_exit;
	}

    return TRUE;
error_exit:
    GST_ELEMENT_ERROR(src, RESOURCE, READ, (NULL), GST_ERROR_SYSTEM);
	g_close(wavplayer->input, NULL);
    return FALSE;
}

/* prepare resources and state */
static gboolean _wavplayer_prepare(GstAudioSrc *src,
                                    GstAudioRingBufferSpec *spec) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    GST_DEBUG_OBJECT(wavplayer, "prepare");

    return TRUE;
}

static gboolean _wavplayer_unprepare(GstAudioSrc *src) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    GST_DEBUG_OBJECT(wavplayer, "unprepare");

    return TRUE;
}

/* close device */
static gboolean _wavplayer_close(GstAudioSrc *src) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    g_close(wavplayer->input, NULL);

    GST_DEBUG_OBJECT(wavplayer, "close");

    return TRUE;
}

/* read samples */
static guint _wavplayer_read(GstAudioSrc *src, gpointer data, guint length,
                              GstClockTime *timestamp) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    int ret = 0;
    int delta = 0;

    while (ret < length) 
	{
        delta += read(wavplayer->input, data, length - ret);
        if (delta == 0)
		{
			kill(getpid(), SIGINT);
			break;
		}
        ret += delta;
    }

    int to_sleep = (length * 1000000) / SEC_SIZE;
    usleep(to_sleep);

    GST_DEBUG_OBJECT(wavplayer, "read");

    return ret;
}

/* get number of samples */
static guint _wavplayer_delay(GstAudioSrc *src) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    GST_DEBUG_OBJECT(wavplayer, "delay");

    return 0;
}

/* reset device */
static void _wavplayer_reset(GstAudioSrc *src) 
{
    WavPlayer *wavplayer = _WAVPLAYER(src);

    GST_DEBUG_OBJECT(wavplayer, "reset");
    GST_INFO("reset");
}

static gboolean plugin_init(GstPlugin *plugin) 
{
    return gst_element_register(plugin, "wavplayer", GST_RANK_NONE,
                                _TYPE_WAVPLAYER);
}

#ifndef VERSION
#define VERSION "0.0.1"
#endif
#ifndef PACKAGE
#define PACKAGE "wavplayer"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "wavplayer"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "https://github.com/ashumnik/OTUS_DZ/tree/main/DZ5-oop"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR, wavplayer,
                  "plugin description", plugin_init, VERSION, "LGPL",
                  PACKAGE_NAME, GST_PACKAGE_ORIGIN)
