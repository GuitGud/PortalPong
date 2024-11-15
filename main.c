#include <stdio.h>
#include <stdlib.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define MAXIMO_DE_BOLAS        5
#define ALTURA_DAS_RAQUETES    5
#define LARGURA_DAS_RAQUETES   1
#define VELOCIDADE_DA_BOLA     50
#define LIMITE_DE_PONTUACAO    20
#define ALTURA_DO_PORTAL       4
#define LARGURA_DO_PORTAL      2
#define VELOCIDADE_DO_PORTAL   1
#define CORRECAO_RAQUETE       (ALTURA_DAS_RAQUETES * 2 / 5)

typedef struct {
    int x, y;
} Raquete;

typedef struct {
    int x, y;
    int dx, dy;
} Bola;

typedef struct {
    int x, y;
    int dx;
} Portal;

void printPontuacao(int pontuacao1, int pontuacao2);
void printRaquete(Raquete *raquete);
void clearRaquete(Raquete *raquete);
void printBola(Bola *bola);
void clearBola(Bola *bola);
void moverBola(Bola *bolas, int *numero_de_bolas, Raquete *raquete1, Raquete *raquete2, Portal *portal_cima, Portal *portal_baixo, int *pontuacao1, int *pontuacao2);
void moverPortais(Portal *portal);
void printPortais(Portal *portal_cima, Portal *portal_baixo);

int main() {
    int ch = 0;
    int pontuacao1 = 0, pontuacao2 = 0;
    int pausa = 0;
    int numero_de_bolas = 1;

    Raquete *raquete1 = (Raquete *)malloc(sizeof(Raquete));
    Raquete *raquete2 = (Raquete *)malloc(sizeof(Raquete));
    Bola *bolas = (Bola *)malloc(MAXIMO_DE_BOLAS * sizeof(Bola));
    Portal portal_cima = {SCRENDX / 4, SCRENDY / 4, VELOCIDADE_DO_PORTAL};
    Portal portal_baixo = {SCRENDX / 4, 3 * SCRENDY / 4 - ALTURA_DO_PORTAL, VELOCIDADE_DO_PORTAL};

    if (raquete1 == NULL || raquete2 == NULL || bolas == NULL) {
        fprintf(stderr, "Erro de alocação de memória\n");
        return 1;
    }

    raquete1->x = 1;
    raquete1->y = 10;
    raquete2->x = 78;
    raquete2->y = 10;

    bolas[0].x = SCRENDX / 2;
    bolas[0].y = SCRENDY / 2;
    bolas[0].dx = 1;
    bolas[0].dy = 1;

    screenInit(1);
    keyboardInit();
    timerInit(VELOCIDADE_DA_BOLA);

    printPontuacao(pontuacao1, pontuacao2);
    printRaquete(raquete1);
    printRaquete(raquete2);
    printPortais(&portal_cima, &portal_baixo);
    printBola(&bolas[0]);
    screenUpdate();

    while (ch != 10) {
        if (keyhit()) {
            ch = readch();
            if (!pausa) {
                if (ch == 'w' && raquete1->y > 1) {
                    clearRaquete(raquete1);
                    raquete1->y -= CORRECAO_RAQUETE;
                    printRaquete(raquete1);
                } else if (ch == 's' && raquete1->y < SCRENDY - ALTURA_DAS_RAQUETES) {
                    clearRaquete(raquete1);
                    raquete1->y += CORRECAO_RAQUETE;
                    printRaquete(raquete1);
                } else if (ch == 'i' && raquete2->y > 1) {
                    clearRaquete(raquete2);
                    raquete2->y -= CORRECAO_RAQUETE;
                    printRaquete(raquete2);
                } else if (ch == 'k' && raquete2->y < SCRENDY - ALTURA_DAS_RAQUETES) {
                    clearRaquete(raquete2);
                    raquete2->y += CORRECAO_RAQUETE;
                    printRaquete(raquete2);
                } else if (ch == 'b' && numero_de_bolas < MAXIMO_DE_BOLAS) {
                    bolas[numero_de_bolas].x = SCRENDX / 2;
                    bolas[numero_de_bolas].y = SCRENDY / 2;
                    bolas[numero_de_bolas].dx = -1;
                    bolas[numero_de_bolas].dy = 1;
                    printBola(&bolas[numero_de_bolas]);
                    numero_de_bolas++;
                }
            }
            if (ch == 'p') {
                pausa = !pausa;
            }
            screenUpdate();
        }

        if (!pausa && timerTimeOver() == 1) {
            moverPortais(&portal_cima);
            moverPortais(&portal_baixo);
            printPortais(&portal_cima, &portal_baixo);

            moverBola(bolas, &numero_de_bolas, raquete1, raquete2, &portal_cima, &portal_baixo, &pontuacao1, &pontuacao2);
            printPontuacao(pontuacao1, pontuacao2);
            printRaquete(raquete1);
            printRaquete(raquete2);

            if (pontuacao1 >= LIMITE_DE_PONTUACAO || pontuacao2 >= LIMITE_DE_PONTUACAO) {
                screenGotoxy(SCRENDX / 3 - 10, SCRENDY / 2);
                if (pontuacao1 >= LIMITE_DE_PONTUACAO) {
                    printf("Jogador 1 vence! Pressione qualquer tecla para sair.");
                } else {
                    printf("Jogador 2 vence! Pressione qualquer tecla para sair.");
                }
                screenUpdate();
                while (!keyhit()) {}
                break;
            }
            screenUpdate();
        } else if (pausa) {
            screenGotoxy(SCRENDX / 2 - 5, SCRENDY / 2);
            printf("  Jogo pausado");
            screenUpdate();
        }
    }

    free(raquete1);
    free(raquete2);
    free(bolas);

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    return 0;
}

void printPontuacao(int pontuacao1, int pontuacao2) {
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

void printBola(Bola *bola) {
    screenSetColor(WHITE, DARKGRAY);
    screenGotoxy(bola->x, bola->y);
    printf("O");
}

void clearBola(Bola *bola) {
    screenGotoxy(bola->x, bola->y);
    printf(" ");
}

void moverBola(Bola *bolas, int *numero_de_bolas, Raquete *raquete1, Raquete *raquete2, Portal *portal_cima, Portal *portal_baixo, int *pontuacao1, int *pontuacao2) {
    for (int i = 0; i < *numero_de_bolas; i++) {
        clearBola(&bolas[i]);

        int novaBolaX = bolas[i].x + bolas[i].dx;
        int novaBolaY = bolas[i].y + bolas[i].dy;

        // Colisão com as bordas superior e inferior
        if (novaBolaY <= 1 || novaBolaY >= SCRENDY - 1)
            bolas[i].dy = -bolas[i].dy;

        // Colisão com as raquetes
        if ((novaBolaX == raquete1->x + 1 && novaBolaY >= raquete1->y && novaBolaY < raquete1->y + ALTURA_DAS_RAQUETES) ||
            (novaBolaX == raquete2->x - 1 && novaBolaY >= raquete2->y && novaBolaY < raquete2->y + ALTURA_DAS_RAQUETES)) {
            bolas[i].dx = -bolas[i].dx;
        }

        // Teletransporte pelos portais
        if (novaBolaX >= portal_cima->x && novaBolaX < portal_cima->x + LARGURA_DO_PORTAL &&
            novaBolaY >= portal_cima->y && novaBolaY < portal_cima->y + ALTURA_DO_PORTAL) {
            novaBolaX = portal_baixo->x;
            novaBolaY = portal_baixo->y + ALTURA_DO_PORTAL / 2;
            bolas[i].dy = -bolas[i].dy;
        } else if (novaBolaX >= portal_baixo->x && novaBolaX < portal_baixo->x + LARGURA_DO_PORTAL &&
                   novaBolaY >= portal_baixo->y && novaBolaY < portal_baixo->y + ALTURA_DO_PORTAL) {
            novaBolaX = portal_cima->x;
            novaBolaY = portal_cima->y + ALTURA_DO_PORTAL / 2;
            bolas[i].dy = -bolas[i].dy;
        }

        // Pontuação e reset da bola
        if (novaBolaX <= 0) {
            (*pontuacao2)++;
            novaBolaX = SCRENDX / 2;
            novaBolaY = SCRENDY / 2;
            bolas[i].dx = -bolas[i].dx;
        } else if (novaBolaX >= SCRENDX) {
            (*pontuacao1)++;
            novaBolaX = SCRENDX / 2;
            novaBolaY = SCRENDY / 2;
            bolas[i].dx = -bolas[i].dx;
        }

        bolas[i].x = novaBolaX;
        bolas[i].y = novaBolaY;

        printBola(&bolas[i]);
    }
}

void moverPortais(Portal *portal) {
    portal->x += portal->dx;

    if (portal->x <= 1 || portal->x >= SCRENDX - LARGURA_DO_PORTAL)
        portal->dx = -portal->dx;
}

void printPortais(Portal *portal_cima, Portal *portal_baixo) {
    screenSetColor(BLUE, DARKGRAY);

    // Limpar portais anteriores
    for (int i = 0; i < ALTURA_DO_PORTAL; i++) {
        screenGotoxy(portal_cima->x - portal_cima->dx, portal_cima->y + i);
        printf("  ");
        screenGotoxy(portal_baixo->x - portal_baixo->dx, portal_baixo->y + i);
        printf("  ");
    }

    // Desenhar novos portais
    for (int i = 0; i < ALTURA_DO_PORTAL; i++) {
        screenGotoxy(portal_cima->x, portal_cima->y + i);
        printf("][");
        screenGotoxy(portal_baixo->x, portal_baixo->y + i);
        printf("][");
    }
}
