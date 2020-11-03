#include "state_machine.h"

static unsigned char checkBuf[2]; // Para usar na verificação do BCC, guardando Address e Control Byte.
static int frameIndex, wrongC;    // Variáveis auxiliares para facilitar o uso em SM.type = READ.

void settingUpSM(enum stateMachineType type, enum stateMachineState state, unsigned char A, unsigned char C) {
    SM.type = type;
    SM.state = state;
    SM.A = A;
    SM.C = C;
}

int stateMachine(unsigned char byte, unsigned char **buf, int *size) {
    if (SM.type == READ) ll.frame[frameIndex] = byte;

    switch(SM.state) {
        case START:
            return SM_START(byte);
            break;
        case FLAG_RCV:
            return SM_FLAG_RCV(byte);
            break;
        case A_RCV:
            return SM_A_RCV(byte);
            break;
        case C_RCV:
            return SM_C_RCV(byte);
            break;
        case BCC_OK:
            return SM_BCC_OK(byte, buf, size);
            break;
        case SM_STOP:
            return 0;
            break;
        default:
            printf("Valor da máquina de estados desconhecido\n");
            return -1;
            break;
    }
}

int SM_START(unsigned char byte) {
    if (SM.type == READ) {
        frameIndex = 0;
        wrongC = FALSE;
        if (byte == FLAG) {
            SM.state = FLAG_RCV;
            frameIndex++;
        }
    }
    else {
        if (byte == FLAG) SM.state = FLAG_RCV;
    }
    return 0;
}

int SM_FLAG_RCV(unsigned char byte) {
    if (byte == SM.A) {
        SM.state = A_RCV;
        checkBuf[0] = byte;
        if (SM.type == READ) frameIndex++;
    }
    else if (byte != FLAG) SM.state = START;
    return 0;
}

int SM_A_RCV(unsigned char byte) {
    switch (SM.type) {
        case SV: 
            if (byte == SM.C) {
                SM.state = C_RCV;
                checkBuf[1] = byte;
            }
            else if (byte == FLAG) SM.state = FLAG_RCV;
            else SM.state = START;
            break;
        
        case WRITE:
            if (byte == C_REJ(XOR(ll.sequenceNumber, 0x01))) {
                printf("C_REJ recebido\n");
                return -1;
            }
            if (byte == SM.C) {
                SM.state = C_RCV;
                checkBuf[1] = byte;
            }
            else if (byte == FLAG) SM.state = FLAG_RCV;
            else SM.state = START;
            break;

        case READ:
            if (byte == C_I(XOR(ll.sequenceNumber, 0x01))) {
                printf("Ns errado\n");
                wrongC = TRUE;
                SM.state = C_RCV;
                checkBuf[1] = byte;
                frameIndex++;
            }
            else if (byte == SM.C) {
                SM.state = C_RCV;
                checkBuf[1] = byte;
                frameIndex++;
            }
            else if (byte == FLAG) {
                SM.state = FLAG_RCV;
                frameIndex = 1;
            }
            else SM.state = START;
            break;

        default:
            return -1;
            break;
    }
    return 0;
}

int SM_C_RCV(unsigned char byte) {
    if (byte == BCC(checkBuf[0], checkBuf[1])) {
        if (SM.type == READ && wrongC) {
            printf("Este pacote já tinha sido recebido\n");
            return -2;
        }
        SM.state = BCC_OK;
        if (SM.type == READ) frameIndex++;
    }
    else if (byte == FLAG) {
        SM.state = FLAG_RCV;
        if (SM.type == READ) frameIndex = 1;
    }
    else {
        switch(SM.type) {
            case SV: 
                SM.state = START;
                break;

            case WRITE:
                printf("Erro no byte BCC\n");
                return -1;
                break;

            case READ:
                printf("BCC recebido com erros1\n");
                return -1;
                break;

            default: 
                return -1;
                break;
        }
    }
    return 0;
}

int SM_BCC_OK(unsigned char byte, unsigned char **buf, int *size) {
    if (SM.type != READ) {
        if (byte == FLAG) SM.state = SM_STOP;
        else SM.state = START;
    }
    else {
        frameIndex++;
        if (byte == FLAG) {
            // De-Stuffing
            *buf = (unsigned char *)malloc(frameIndex-4-2);
            *size = 0;

            unsigned char destuffing;
            int lesssize = 2;
            if (ll.frame[frameIndex-3] != 0x7D) destuffing= ll.frame[frameIndex-2];
            else {
                destuffing = XOR(ll.frame[frameIndex-2], 0x20);
                lesssize = 3;
            }
            for (int i = 4; i < frameIndex - lesssize; i++) {
                if (ll.frame[i] != 0x7D) {
                    (*buf)[*size] = ll.frame[i];
                }
                else {
                    (*buf)[*size] = XOR(ll.frame[i+1], 0x20);
                    i++;
                }
                (*size)++;
            }
            *buf = (unsigned char *)realloc(*buf, (*size));
        
            unsigned char BCC2 = (*buf)[0];
            for (int i = 1; i < *size; i++) {
                BCC2 = BCC(BCC2, (*buf)[i]);
            }

            if (destuffing == BCC2) {
                ll.sequenceNumber = XOR(ll.sequenceNumber, 0x01);
                SM.state = SM_STOP;
            }
            else {
                printf("BCC recebido com erros2\n");
                return -1;
            }
        }
        else return 1;
    }
    return 0;
}