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

typedef struct {
    int x, y;
    int dx, dy;
} Ball;

typedef struct {
    int x, y;
} Paddle;

void drawFrame(Paddle paddle1, Paddle paddle2, Ball ball, int score) {
    screenClear();

    // Draw paddles
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        screenGotoxy(paddle1.x, paddle1.y + i);
        printf("%c", BOX_BLOCK);
        screenGotoxy(paddle2.x, paddle2.y + i);
        printf("%c", BOX_BLOCK);
    }

    // Draw ball
    screenGotoxy(ball.x, ball.y);
    printf("%c", BOX_DOT);

    // Draw score
    screenGotoxy(SCRSTARTX - 10, SCRSTARTY);
    printf("Score: %d", score);

    screenUpdate();
}

int main() {
    // Initialize screen, keyboard, and timer
    screenInit(1);
    keyboardInit();
    timerInit(BALL_SPEED);

    // Initialize game variables
    Paddle paddle1 = {SCRSTARTX, SCRENDY / 2 - PADDLE_HEIGHT / 2};
    Paddle paddle2 = {SCRENDX - PADDLE_WIDTH, SCRENDY / 2 - PADDLE_HEIGHT / 2};
    Ball ball = {SCRENDX / 2, SCRENDY / 2, -1, 1};
    int score = 0;

    // Main game loop
    while (1) {
        // Check for input
        if (keyhit()) {
            char key = readch();
            if (key == 'w' && paddle1.y > SCRSTARTY) {
                paddle1.y -= PADDLE_SPEED;
            } else if (key == 's' && paddle1.y < SCRENDY - PADDLE_HEIGHT) {
                paddle1.y += PADDLE_SPEED;
            } else if (key == 'i' && paddle2.y > SCRSTARTY) {
                paddle2.y -= PADDLE_SPEED;
            } else if (key == 'k' && paddle2.y < SCRENDY - PADDLE_HEIGHT) {
                paddle2.y += PADDLE_SPEED;
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
        if (ball.x < SCRSTARTX || ball.x > SCRENDX) {
            score++;
            ball.x = SCRENDX / 2;
            ball.y = SCRENDY / 2;
        }

        // Draw the frame
        drawFrame(paddle1, paddle2, ball, score);

        // Check for game over
        if (score >= 20) {
            break;
        }

        // Wait for the next frame
        timerPrint();
        while (!timerTimeOver()) {}
    }

    // Clean up and exit
    keyboardDestroy();
    timerDestroy();
    screenDestroy();

    return 0;
}
