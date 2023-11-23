  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
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
  #define CORRECAO_RAQUETE (ALTURA_DAS_RAQUETES * 2 / 5)

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

  void mover_portais(Portal *portal) {
      portal->x += portal->dx;

      if (portal->x <= SCRSTARTX || portal->x >= SCRENDX - LARGURA_DO_PORTAL) {
          portal->dx = -portal->dx;
      }
  }

  void printar_frame(Raquete *raquete1, Raquete *raquete2, Bola bolas[MAXIMO_DE_BOLAS], int numero_de_bolas, Portal *portal_cima, Portal *portal_baixo, int pontuacao1, int pontuacao2) {
      screenClear();

      screenGotoxy(0, 0);
      for (int i = 0; i < SCRENDX; i++) {
          printf("-");
      }
    
      for (int i = 0; i < ALTURA_DO_PORTAL; i++) {
          screenGotoxy(portal_cima->x, portal_cima->y + 2 + i);
          printf("][");
          screenGotoxy(portal_baixo->x, portal_baixo->y + i);
          printf("][");
      }
    
      for (int i = 0; i < ALTURA_DAS_RAQUETES; i++) {
          screenGotoxy(raquete1->x, raquete1->y + i);
          printf("|");
          screenGotoxy(raquete2->x, raquete2->y + i);
          printf("|");
      }

      for (int i = 0; i < numero_de_bolas; i++) {
          screenGotoxy(bolas[i].x, bolas[i].y);
          printf("o");
      }

      screenGotoxy(0, SCRENDY);
      for (int i = 0; i < SCRENDX; i++) {
          printf("-");
      }

      screenGotoxy(2, 1);
      printf("Jogador 1: %2d | Jogador 2: %2d", pontuacao1, pontuacao2);

      screenUpdate();
  }

  int main() {
      screenInit(0);
      keyboardInit();
      timerInit(VELOCIDADE_DA_BOLA);

      Raquete raquete1 = {SCRSTARTX, SCRENDY / 2 - ALTURA_DAS_RAQUETES / 2};
      Raquete raquete2 = {SCRENDX - LARGURA_DAS_RAQUETES - 1, SCRENDY / 2 - ALTURA_DAS_RAQUETES / 2};
      Portal portal_cima = {SCRENDX / 4, SCRENDY / 4, VELOCIDADE_DO_PORTAL};
      Portal portal_baixo = {SCRENDX / 4, 3 * SCRENDY / 4 - ALTURA_DO_PORTAL, VELOCIDADE_DO_PORTAL};

      Bola bolas[MAXIMO_DE_BOLAS];
      int numero_de_bolas = 0;

      int pontuacao1 = 0, pontuacao2 = 0;

      int pausa = 0;

      while (1) {
          if (keyhit()) {
              char tecla = readch();
              if (tecla == 'w' && raquete1.y > SCRSTARTY + 1 && !pausa) {
                  raquete1.y -= CORRECAO_RAQUETE;
              } else if (tecla == 's' && raquete1.y < SCRENDY - ALTURA_DAS_RAQUETES && !pausa) {
                  raquete1.y += CORRECAO_RAQUETE;
              } else if (tecla == 'i' && raquete2.y > SCRSTARTY + 1 && !pausa) {
                  raquete2.y -= CORRECAO_RAQUETE;
              } else if (tecla == 'k' && raquete2.y < SCRENDY - ALTURA_DAS_RAQUETES && !pausa) {
                  raquete2.y += CORRECAO_RAQUETE;
              } else if (tecla == 'p') {
                  pausa = !pausa;
              } else if (tecla == 'b' && !pausa && numero_de_bolas < MAXIMO_DE_BOLAS) {
                  bolas[numero_de_bolas].x = SCRENDX / 2;
                  bolas[numero_de_bolas].y = SCRENDY / 2;
                  bolas[numero_de_bolas].dx = -1;
                  bolas[numero_de_bolas].dy = 1;
                  numero_de_bolas++;
              }
          }

          if (!pausa) {
              mover_portais(&portal_cima);
              mover_portais(&portal_baixo);

              for (int i = 0; i < numero_de_bolas; i++) {
                  bolas[i].x += bolas[i].dx;
                  bolas[i].y += bolas[i].dy;

                  if (bolas[i].x >= portal_cima.x && bolas[i].x < portal_cima.x + LARGURA_DO_PORTAL &&
                      bolas[i].y >= portal_cima.y && bolas[i].y < portal_cima.y + ALTURA_DO_PORTAL) {
                      bolas[i].x = portal_baixo.x;
                      bolas[i].y = portal_baixo.y + ALTURA_DO_PORTAL / 2;
                      bolas[i].dy = -bolas[i].dy;

                  } else if (bolas[i].x >= portal_baixo.x && bolas[i].x < portal_baixo.x + LARGURA_DO_PORTAL && bolas[i].y >= portal_baixo.y && bolas[i].y < portal_baixo.y + ALTURA_DO_PORTAL) {
                      bolas[i].x = portal_cima.x;
                      bolas[i].y = portal_cima.y + ALTURA_DO_PORTAL / 2;
                      bolas[i].dy = -bolas[i].dy;
                  }
                
                  if ((bolas[i].x == raquete1.x + 1 && bolas[i].y >= raquete1.y && bolas[i].y < raquete1.y + ALTURA_DAS_RAQUETES) ||
                      (bolas[i].x == raquete2.x - 1 && bolas[i].y >= raquete2.y && bolas[i].y < raquete2.y + ALTURA_DAS_RAQUETES)) {
                      bolas[i].dx = -bolas[i].dx;
                  }
                
                  if (bolas[i].y <= SCRSTARTY || bolas[i].y >= SCRENDY) {
                      bolas[i].dy = -bolas[i].dy;
                  }

                  // Verificar pontuação
                  if (bolas[i].x < SCRSTARTX) {
                      pontuacao2++;
                      bolas[i].x = SCRENDX / 2;
                      bolas[i].y = SCRENDY / 2;
                  } else if (bolas[i].x > SCRENDX) {
                      pontuacao1++;
                      bolas[i].x = SCRENDX / 2;
                      bolas[i].y = SCRENDY / 2;
                  }

              }

              if (pontuacao1 >= LIMITE_DE_PONTUACAO || pontuacao2 >= LIMITE_DE_PONTUACAO) {
                  if (pontuacao1 >= LIMITE_DE_PONTUACAO) {
                      screenGotoxy(SCRENDX / 3 - 10, SCRENDY / 2);
                      printf("    Jogador 1 vence! Pressione qualquer tecla para sair.");
                  } else {
                      screenGotoxy(SCRENDX / 3 - 10, SCRENDY / 2);
                      printf("    Jogador 2 vence! Pressione qualquer tecla para sair.");
                  }

                  screenUpdate();

                  while (!keyhit()) {}
                  break;
              }

              printar_frame(&raquete1, &raquete2, bolas, numero_de_bolas, &portal_cima, &portal_baixo, pontuacao1, pontuacao2);

              while (!timerTimeOver()) {}
          } else {
              screenGotoxy(SCRENDX / 2 - 5, SCRENDY / 2);
              printf("  Jogo pausa");
              screenUpdate();
          }
      }
      keyboardDestroy();
      timerDestroy();
      screenDestroy();

  return 0;
  }
