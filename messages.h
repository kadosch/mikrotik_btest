#ifndef MESSAGES_H
#define MESSAGES_H

extern unsigned char MSG_OK[4];
extern unsigned char MSG_NOOK[4];

extern unsigned char MSG_TCP_DOWN[16];
extern unsigned char MSG_TCP_UP[16];
extern unsigned char MSG_TCP_BOTH[16];

extern unsigned char CHALLENGE_HEADER[4];

#define CHALLENGE_TOTAL_SIZE 20
#define CHALLENGE_SIZE 16
#define RESPONSE_SIZE 48

#endif
