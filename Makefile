# compilador
CC=gcc

# flags para o compilador
CFLAGS=-Wall -O -g

# diretorias
ODIR=./obj
SDIR=./src
IDIR=./include
BDIR=./bin

# includes do servidor
ISERVER=$(IDIR)/server

# src servidor
SSERVER=$(SDIR)/server

# includes do cliente
ICLIENT=$(IDIR)/client

# src do cliente
SCLIENT=$(SDIR)/client

all: server client

server: bin/aurrasd

client: bin/aurras

bin/aurrasd: $(OBJ)/aurrasd.o
    $(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/aurrasd.o: $(SSERVER)/aurrasd.c
    $(CC) $(CFLAGS) -c $< -o $@

bin/aurras: $(OBJ)/aurras.o
    $(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/aurras.o: $(SCLIENT)/aurras.c
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm obj/* tmp/* bin/{aurras,aurrasd}

test: 
    $(BDIR)/aurras
    $(BDIR)/aurras status
    $(BDIR)/aurras transform samples/samples-1.m4a output.m4a alto eco rapido
