# compiler
CC = gcc

# folders
INCLUDE_FOLDER = ./include/
BIN_FOLDER = ./bin/
OBJ_FOLDER = ./obj/
SRC_FOLDER = ./src/
PLT_FOLDER = ./plot/

# sources
MAIN = main
TARGET = tp3.out
SRC = $(wildcard $(SRC_FOLDER)*.c)
OBJ = $(patsubst $(SRC_FOLDER)%.c, $(OBJ_FOLDER)%.o, $(SRC))

$(OBJ_FOLDER)%.o: $(SRC_FOLDER)%.c
	$(CC) -c $< -o $@ -I$(INCLUDE_FOLDER) -g

all: $(OBJ)
	$(CC) -o $(BIN_FOLDER)$(TARGET) $(OBJ) -lm

clean:
	@rm -rf $(OBJ_FOLDER)* $(PLT_FOLDER)* $(BIN_FOLDER)tp3.out 