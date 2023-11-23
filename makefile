CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lncurses

SRC = main.c screen.c keyboard.c timer.c
OBJ = $(SRC:.c=.o)
EXEC = pong_game

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
