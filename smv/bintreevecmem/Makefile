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
OBJS = $(OBJ)/node.o $(OBJ)/bintree.o $(OBJ)/bintreetest.o $(OBJ)/memlog.o
CFLAGS = -g -Wall -c -I$(INC)
HDRS= $(INC)/node.h $(INC)/bintree.h $(INC)/memlog.h $(INC)/msgassert.h

ANALISAMEM = ./analisamem
EXE = $(BIN)/bintreetest

all: use

use: $(EXE)
	$(EXE)

mem: $(EXE)
	$(EXE)
	rm -rf /tmp/bintreememlog
	mkdir /tmp/bintreememlog
	$(ANALISAMEM) -i /tmp/memlog.out -p /tmp/bintreememlog/btml

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

$(OBJ)/memlog.o: $(HDRS) $(SRC)/memlog.c
	$(CC) $(CFLAGS) -o $(OBJ)/memlog.o $(SRC)/memlog.c 

clean:
	rm -f $(EXE) $(OBJS)  
