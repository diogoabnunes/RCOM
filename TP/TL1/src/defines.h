#ifndef DEFINES_H
#define DEFINES_H

#define SET_UA_SIZE 5

#define FLAG 0x7E
#define A_EmiRec 0x03
#define A_RecEmi 0x01
#define C_SET 0b00000011 // 0x03
#define C_DISC 0b00001011 // 0x0B
#define C_UA 0b00000111 // 0x07

#define BCC(a,c) (a^c)
#define XOR(a,b) (a^b)

#define C_RR(r) ((0b10000101) ^ (r) << (7)) // 0x05 | 0x85
#define C_REJ(r) ((0b10000001) ^ (r) << (7)) // 0x01 | 0x81
#define C_I(r) ((0b01000000) ^ (r) << (6)) // 0x00 | 0x40

#define BAUDRATE B38400

#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1

#define TIMEOUT 3
#define TRIES 3

#define MAX_SIZE 32

#define DATA 0x01
#define C_START 0x02
#define END 0x03

#define T_SIZE 0x00
#define T_NAME 0x01

#endif // DEFINES_H