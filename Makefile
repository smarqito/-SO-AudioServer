# compilador
CC=gcc

# flags para o compilador
CFLAGS=-Wall -O -g

# diretorias
ODIR=obj
SDIR=src
IDIR=include
BDIR=bin

# includes do servidor
ISERVER=$(IDIR)/server

# src servidor
SSERVER=$(SDIR)/server
# includes de modulos
IMODULES=$(IDIR)/modules

# src modulos
SMODULES=$(SDIR)/modules

# includes do cliente
ICLIENT=$(IDIR)/client

# src do cliente
SCLIENT=$(SDIR)/client

INCLUDES=-I$(ISERVER) -I$(ICLIENT) -I$(IMODULES)

all: server client

server: $(BDIR)/aurrasd

client: $(BDIR)/aurras

$(BDIR)/aurrasd: $(OBJ)/aurrasd.o
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ)/aurrasd.o: $(SSERVER)/aurrasd.c $(OBJ)/dup_aux.o $(OBJ)/readln.o
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BDIR)/aurras: $(OBJ)/aurras.o
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ)/aurras.o: $(SCLIENT)/aurras.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ)/dup_aux.o: $(SMODULES)/dup_aux.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ)/readln.o: $(SMODULES)/readln.c
    $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean: rm obj/* tmp/* bin/{aurras,aurrasd}

test:
    $(BDIR)/aurras
    $(BDIR)/aurras status
    $(BDIR)/aurras transform samples/samples-1.m4a output.m4a alto eco rapido
