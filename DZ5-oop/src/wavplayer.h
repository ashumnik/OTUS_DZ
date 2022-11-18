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

#ifndef _WAV_PLUGIN_H_
#define _WAV_PLUGIN_H_

#include <gst/audio/gstaudiosrc.h>
#include <stdio.h>

G_BEGIN_DECLS

#define _TYPE_WAVPLAYER (wavplayer_get_type())
#define _WAVPLAYER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), _TYPE_WAVPLAYER, WavPlayer))
#define _WAVPLAYER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), _TYPE_WAVPLAYER, WavPlayerClass))
#define _IS_WAVPLAYER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), _TYPE_WAVPLAYER))
#define _IS_WAVPLAYER_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((klass), _TYPE_WAVPLAYER))

typedef struct _WavPlayer WavPlayer;
typedef struct _WavPlayerClass WavPlayerClass;

struct _WavPlayer 
{
    GstAudioSrc base_wavplayer;
    gchar *filepath;
    int input;
	struct WavHeader 
	{
		/* RIFF Chunk Descriptor */
		uint8_t         RIFF[4];        	// RIFF Header Magic header
		uint32_t        ChunkSize;      	// RIFF Chunk Size
		uint8_t         WAVE[4];        	// WAVE Header
		/* "fmt" sub-chunk */
		uint8_t         fmt[4];         	// FMT header
		uint32_t        Subchunk1Size;  	// Size of the fmt chunk
		uint16_t        AudioFormat;    	// Audio format 1=PCM,6=mulaw,7=alaw,257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
		uint16_t        NumOfChan;      	// Number of channels 1=Mono 2=Stereo
		uint32_t        SamplesPerSec;  	// Sampling Frequency in Hz
		uint32_t        bytesPerSec;    	// bytes per second
		uint16_t        blockAlign;     	// 2=16-bit mono, 4=16-bit stereo
		uint16_t        bitsPerSample;  	// Number of bits per sample
		/* "data" sub-chunk */
		uint8_t         Subchunk2ID[4]; // "data"  string
		uint32_t        Subchunk2Size;  // Sampled data length
	} WavHeader;
};

struct _WavPlayerClass 
{
    GstAudioSrcClass base_wavplayer_class;
};

GType wavplayer_get_type(void);

G_END_DECLS

#endif
