#ifndef _TELNETPROTOCOL_H
#define _TELNETPROTOCOL_H

#define MAX_BUFFER_LEN 4096

const unsigned char IAC = 255;
const unsigned char DONT = 254;
const unsigned char DO = 253;
const unsigned char WONT = 252;
const unsigned char WILL = 251;
const unsigned char ECHO_C = 1;
const unsigned char STATUS = 5;
const unsigned char AUTH = 37;
const unsigned char WINDOWSIZE = 31;
const unsigned char SUPPRESS = 3;

#endif //_TELNETPROTOCOL_H
