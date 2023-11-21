#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define PADDLE_HEIGHT 5
#define PADDLE_WIDTH  1
#define BALL_SPEED    50
#define PADDLE_SPEED  10
#define SCORE_LIMIT   20
#define PADDLE_INCREMENT (PADDLE_HEIGHT*2 / 5)
#define PORTAL_HEIGHT 4
#define PORTAL_SPEED  1

typedef struct {
    int x, y;
    int dx, dy;
} Ball;

typedef struct {
    int x, y;
} Paddle;

typedef struct {
    int x, y;
    int dx;
} Portal;

void movePortal(Portal *portal) {
    portal->x += portal->dx;

    // Verificar se o portal atingiu as bordas e inverter a direção se necessário
    if (portal->x <= SCRSTARTX || portal->x >= SCRENDX - 2) {
        portal->dx = -portal->dx;
    }
}
//desenha a tela do jogo com os elementos dentro
void drawFrame(Paddle paddle1, Paddle paddle2, Ball ball, Portal portalTop, Portal portalBottom, int score1, int score2) {
    screenClear();

    // Draw top border
    screenGotoxy(0, 0);
    for (int i = 0; i < SCRENDX; i++) {
        printf("-");
    }

    // Draw paddles
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        screenGotoxy(paddle1.x, paddle1.y + i);
        printf("|");
        screenGotoxy(paddle2.x, paddle2.y + i);
        printf("|");
    }

    // Draw ball
    screenGotoxy(ball.x, ball.y);
    printf("o");

    // Desenhar portais
    for (int i = 0; i < PORTAL_HEIGHT; i++) {
        screenGotoxy(portalTop.x, portalTop.y +2 + i);
        printf("][");
        screenGotoxy(portalBottom.x, portalBottom.y + i);
        printf("][");
    }

    // Draw bottom border
    screenGotoxy(0, SCRENDY);
    for (int i = 0; i < SCRENDX; i++) {
        printf("-");
    }

    // Draw scores
    screenGotoxy(2, 1);
    printf("Player 1: %2d | Player 2: %2d", score1, score2);

    screenUpdate();
}

int main() {
    // Initialize screen, keyboard, and timer
    screenInit(0);
    keyboardInit();
    timerInit(BALL_SPEED);

    // Initialize game variables
    Paddle paddle1 = {SCRSTARTX, SCRENDY / 2 - PADDLE_HEIGHT / 2};
    Paddle paddle2 = {SCRENDX - PADDLE_WIDTH, SCRENDY / 2 - PADDLE_HEIGHT / 2};

    // Altere as coordenadas iniciais dos portais para o centro da tela
    Portal portalTop = {SCRENDX / 2, SCRENDY / 4, PORTAL_SPEED};
    Portal portalBottom = {SCRENDX / 2, 3 * SCRENDY / 4 - PORTAL_HEIGHT, PORTAL_SPEED};

    // Ajuste a posição inicial da bola
    Ball ball = {SCRENDX / 2, SCRENDY / 2, -1, 1};
    int score1 = 0, score2 = 0;

    // Main game loop
    while (1) {
        // Check for input
        if (keyhit()) {
            char key = readch();
            if (key == 'w' && paddle1.y > SCRSTARTY + 1) {
                paddle1.y -= PADDLE_INCREMENT;
            } else if (key == 's' && paddle1.y < SCRENDY - PADDLE_HEIGHT) {
                paddle1.y += PADDLE_INCREMENT;
            } else if (key == 'i' && paddle2.y > SCRSTARTY + 1) {
                paddle2.y -= PADDLE_INCREMENT;
            } else if (key == 'k' && paddle2.y < SCRENDY - PADDLE_HEIGHT) {
                paddle2.y += PADDLE_INCREMENT;
            }
        }

        // Update ball position
        ball.x += ball.dx;
        ball.y += ball.dy;

        // Check for collisions with paddles
        if ((ball.x == paddle1.x + 1 && ball.y >= paddle1.y && ball.y < paddle1.y + PADDLE_HEIGHT) ||
            (ball.x == paddle2.x - 1 && ball.y >= paddle2.y && ball.y < paddle2.y + PADDLE_HEIGHT)) {
            ball.dx = -ball.dx;
        }

        // Check for collisions with walls
        if (ball.y <= SCRSTARTY || ball.y >= SCRENDY) {
            ball.dy = -ball.dy;
        }

        // Check for scoring
        if (ball.x < SCRSTARTX) {
            score2++;
            ball.x = SCRENDX / 2;
            ball.y = SCRENDY / 2;
        } else if (ball.x > SCRENDX) {
            score1++;
            ball.x = SCRENDX / 2;
            ball.y = SCRENDY / 2;
        }

        // Check for portal entry
        if (ball.x == portalTop.x && ball.y >= portalTop.y && ball.y < portalTop.y + PORTAL_HEIGHT) {
            ball.x = portalBottom.x;
            ball.y = portalBottom.y + PORTAL_HEIGHT;
            ball.dy = -ball.dy;
        } else if (ball.x == portalBottom.x && ball.y >= portalBottom.y && ball.y < portalBottom.y + PORTAL_HEIGHT) {
            ball.x = portalTop.x;
            ball.y = portalTop.y - PORTAL_HEIGHT;
            ball.dy = -ball.dy;
        }

        // Reset scores if limit is reached
        if (score1 >= SCORE_LIMIT || score2 >= SCORE_LIMIT) {
            if (score1 >= SCORE_LIMIT) {
                screenGotoxy(SCRENDX / 2 - 10, SCRENDY / 2);
                printf("Left side wins! Press any key to exit.");
            } else {
                screenGotoxy(SCRENDX / 2 - 10, SCRENDY / 2);
                printf("Right side wins! Press any key to exit.");
            }

            screenUpdate();

            while (!keyhit()) {}

            break;
        }

        drawFrame(paddle1, paddle2, ball, portalTop, portalBottom, score1, score2);

        timerPrint();
        while (!timerTimeOver()) {}
    }

    keyboardDestroy();
    timerDestroy();
    screenDestroy();

    return 0;
}
