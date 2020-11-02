#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "data_link.h"

enum stateMachineType {
    SV,     // Supervisão
    WRITE,  // Escrita
    READ    // Leitura
};

enum stateMachineState {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    SM_STOP
};

struct stateMachine {
    enum stateMachineType type;
    enum stateMachineState state;
    unsigned char A;
    unsigned char C;
};

struct stateMachine SM;

void settingUpSM(enum stateMachineType type, enum stateMachineState state, unsigned char A, unsigned char C);

int stateMachine(unsigned char byte, unsigned char **buf, int *size);

int SM_START(unsigned char byte);

int SM_FLAG_RCV(unsigned char byte);

int SM_A_RCV(unsigned char byte);

int SM_C_RCV(unsigned char byte);

int SM_BCC_OK(unsigned char byte, unsigned char **buf, int *size);


#endif // STATE_MACHINE_H