#ifndef _DAEMONIZER_H
#define _DAEMONIZER_H

static int PIDFileFD;
void LockPIDFile();
void Daemonize();

#endif //_DAEMONIZER_H