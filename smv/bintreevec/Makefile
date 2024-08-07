#---------------------------------------------------------------------
# Arquivo	: Makefile
# Conteúdo	: compilar o programa geracarga
# Autor		: Wagner Meira Jr. (meira@dcc.ufmg.br)
# Histórico	: 2021-12-13 - arquivo criado
#---------------------------------------------------------------------
# Opções	: make all - compila tudo
#		: make clean - remove objetos e executável
#---------------------------------------------------------------------

CC = gcc
LIBS = -lm 
SRC = src
OBJ = obj
INC = include
BIN = bin
OBJS = $(OBJ)/node.o $(OBJ)/bintree.o $(OBJ)/bintreetest.o
CFLAGS = -g -Wall -c -I$(INC)
HDRS= $(INC)/node.h $(INC)/bintree.h

EXE = $(BIN)/bintreetest

all: use

use: $(EXE)
	$(EXE)

$(BIN)/bintreetest: $(OBJS)
	$(CC) -g -o $(BIN)/bintreetest $(OBJS) $(LIBS)

$(OBJ)/bintreetest.o: $(HDRS) $(SRC)/bintreetest.c
	$(CC) $(CFLAGS) -o $(OBJ)/bintreetest.o $(SRC)/bintreetest.c 

$(OBJ)/smv.o: $(HDRS) $(SRC)/smv.c
	$(CC) $(CFLAGS) -o $(OBJ)/smv.o $(SRC)/smv.c 

$(OBJ)/bintree.o: $(HDRS) $(SRC)/bintree.c
	$(CC) $(CFLAGS) -o $(OBJ)/bintree.o $(SRC)/bintree.c 

$(OBJ)/node.o: $(HDRS) $(SRC)/node.c
	$(CC) $(CFLAGS) -o $(OBJ)/node.o $(SRC)/node.c 

clean:
	rm -f $(EXE) $(OBJS)  
