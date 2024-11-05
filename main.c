#include <string.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define ALTURA_DAS_RAQUETES 5
#define CORRECAO_RAQUETE 1  

typedef struct {
    int x, y;
} Raquete;

int bolaX = 10, bolaY = 5;
int direcaoX = 1, direcaoY = 1;
Raquete raquete1 = {1, 10};  
Raquete raquete2 = {78, 10}; 
int pontuacao1 = 0, pontuacao2 = 0;

void printPontuacao() {
    screenSetColor(WHITE, DARKGRAY);
    screenGotoxy(0, 0);
    printf("Jogador 1: %d | Jogador 2: %d", pontuacao1, pontuacao2);
}

void printRaquete(Raquete *raquete) {
    screenSetColor(GREEN, DARKGRAY);
    for (int i = 0; i < ALTURA_DAS_RAQUETES; i++) {
        screenGotoxy(raquete->x, raquete->y + i);
        printf("|");
    }
}

void clearRaquete(Raquete *raquete) {
    for (int i = 0; i < ALTURA_DAS_RAQUETES; i++) {
        screenGotoxy(raquete->x, raquete->y + i);
        printf(" ");
    }
}

void printBola(int nextBolaX, int nextBolaY) {
    screenSetColor(WHITE, DARKGRAY);
    screenGotoxy(bolaX, bolaY);
    printf(" ");

    bolaX = nextBolaX;
    bolaY = nextBolaY;

    screenGotoxy(bolaX, bolaY);
    printf("O");
}

int main() {

    int ch = 0;
    screenInit(1);
    keyboardInit();
    timerInit(50);

    printPontuacao();
    printRaquete(&raquete1);
    printRaquete(&raquete2);
    screenUpdate();

    while (ch != 10) {
        if (keyhit()) {
            ch = readch();
            if (ch == 'w' && raquete1.y > 1) {
                clearRaquete(&raquete1);
                raquete1.y -= CORRECAO_RAQUETE;
            } else if (ch == 's' && raquete1.y < SCRENDY - ALTURA_DAS_RAQUETES) {
                clearRaquete(&raquete1);
                raquete1.y += CORRECAO_RAQUETE;
            } else if (ch == 'i' && raquete2.y > 1) {
                clearRaquete(&raquete2);
                raquete2.y -= CORRECAO_RAQUETE;
            } else if (ch == 'k' && raquete2.y < SCRENDY - ALTURA_DAS_RAQUETES) {
                clearRaquete(&raquete2);
                raquete2.y += CORRECAO_RAQUETE;
            }
            screenUpdate();
        }

        if (timerTimeOver() == 1) {
            int novaBolaX = bolaX + direcaoX;
            int novaBolaY = bolaY + direcaoY;

            if (novaBolaY <= 1 || novaBolaY >= SCRENDY - 1) direcaoY = -direcaoY;

            if ((novaBolaX == raquete1.x + 1 && novaBolaY >= raquete1.y && novaBolaY < raquete1.y + ALTURA_DAS_RAQUETES) ||
                (novaBolaX == raquete2.x - 1 && novaBolaY >= raquete2.y && novaBolaY < raquete2.y + ALTURA_DAS_RAQUETES)) {
                direcaoX = -direcaoX;
            }

            if (novaBolaX <= 0) {
                pontuacao2++;
                novaBolaX = SCRENDX / 2;
                novaBolaY = SCRENDY / 2;
                direcaoX = -direcaoX;
                printPontuacao();
            } else if (novaBolaX >= SCRENDX) {
                pontuacao1++;
                novaBolaX = SCRENDX / 2;
                novaBolaY = SCRENDY / 2;
                direcaoX = -direcaoX;
                printPontuacao();
            }

            printBola(novaBolaX, novaBolaY);
            printRaquete(&raquete1);
            printRaquete(&raquete2);
            screenUpdate();
        }
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}
