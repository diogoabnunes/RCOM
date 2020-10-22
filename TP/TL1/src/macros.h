enum stateMachine {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    SM_STOP
};

#define NUM_TRIES 3

#define SET_UA_SIZE 5

#define FLAG 0x7E

#define A_EmiRec 0x03
#define A_RecEmi 0x01

#define BCC(a,c) (a^c)

#define XOR(a,b) (a^b)

#define C_SET 0b00000011 // 3
#define C_DISC 0b00001011 // 11

#define C_UA 0b00000111 // 7

#define C_RR(r) ((0b10000101) ^ (r) << (7)) // 0x05 | 0x85
#define C_REJ(r) ((0b10000001) ^ (r) << (7)) // 0x01 | 0x81
#define C_I(r) ((0b01000000) ^ (r) << (6)) // 0x00 | 0x40