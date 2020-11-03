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
    SM_STOP // DONE
};

struct stateMachine {
    enum stateMachineType type;     // SV | WRITE | READ
    enum stateMachineState state;   // START | FLAG_RCV | A_RCV | C_RCV | BCC_OK | SM_STOP
    unsigned char A;                // Address Byte
    unsigned char C;                // Control Byte
} SM;

// Função para "preparar" a máquina de estados.
void settingUpSM(enum stateMachineType type, enum stateMachineState state, unsigned char A, unsigned char C);

// Função que processa um byte e atualiza o estado na máquina de estados.
int stateMachine(unsigned char byte, unsigned char **buf, int *size);

// Função auxiliar que processa um byte caso esteja no estado START.
int SM_START(unsigned char byte);

// Função auxiliar que processa um byte caso esteja no estado FLAG_RCV.
int SM_FLAG_RCV(unsigned char byte);

// Função auxiliar que processa um byte caso esteja no estado A_RCV.
int SM_A_RCV(unsigned char byte);

// Função auxiliar que processa um byte caso esteja no estado C_RCV.
int SM_C_RCV(unsigned char byte);

// Função auxiliar que processa um byte caso esteja no estado BCC_OK.
int SM_BCC_OK(unsigned char byte, unsigned char **buf, int *size);

#endif // STATE_MACHINE_H