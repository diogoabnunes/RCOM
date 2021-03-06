#ifndef DEFINES_H
#define DEFINES_H

#define BAUDRATE B38400
#define TIMEOUT 3
#define TRIES 3
#define MAX_SIZE 1024

#define BCC1_PROB_ERROR 0
#define BCC2_PROB_ERROR 0
#define DELAY_T_PROP 0

#define SET_UA_DISC_SIZE 5

#define FLAG 0x7E
#define A_EmiRec 0x03
#define A_RecEmi 0x01
#define C_SET 0b00000011 // 0x03
#define C_DISC 0b00001011 // 0x0B
#define C_UA 0b00000111 // 0x07

#define C_RR(r) ((r == 1) ? 0b10000101 : 0b00000101) // 0x05 | 0x85
#define C_REJ(r) ((r == 1) ? 0b10000001 : 0b00000001) // 0x01 | 0x81
#define C_I(r) ((r== 1) ? 0b00001000 : 0b00000000) // 0x00 | 0x40

#define XOR(a,b) (a^b)

#define C_DATA 0x01
#define C_START 0x02
#define C_END 0x03
#define T_SIZE 0x00
#define T_NAME 0x01

#define EMISSOR 0
#define RECETOR 1

#define FALSE 0
#define TRUE 1

#endif // DEFINES_H