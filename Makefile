# compilador
CC=gcc

# flags para o compilador
CFLAGS=-Wall -O -g

# diretorias
ODIR=obj
SDIR=src
IDIR=include
BDIR=bin

MKDIR_P = mkdir -p

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

all: directories server client

server: bin/aurrasd bin/pool

client: bin/aurras

directories: 
	$(MKDIR_P) obj tmp samples

bin/aurrasd: obj/aurrasd.o obj/dup_aux.o obj/readln.o
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

bin/pool: obj/pool.o obj/dup_aux.o obj/config.o obj/queue.o obj/readln.o obj/task.o obj/exec_helper.o
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

obj/aurrasd.o: $(SSERVER)/aurrasd.c obj/dup_aux.o obj/readln.o obj/pool.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

bin/aurras: obj/aurras.o obj/dup_aux.o
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

obj/aurras.o: $(SCLIENT)/aurras.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/dup_aux.o: $(SMODULES)/dup_aux.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/readln.o: $(SMODULES)/readln.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/config.o: $(SMODULES)/config.c $(IMODULES)/config.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/task.o: $(SMODULES)/task.c $(IMODULES)/task.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/queue.o: $(SMODULES)/queue.c $(IMODULES)/queue.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/pool.o: $(SMODULES)/pool.c $(IMODULES)/pool.h obj/queue.o obj/dup_aux.o obj/exec_helper.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj/exec_helper.o: $(SMODULES)/exec_helper.c $(IMODULES)/exec_helper.h obj/task.o obj/dup_aux.o obj/config.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm obj/* bin/aurras bin/aurrasd bin/pool tmp/*

test:
	bin/aurras
	bin/aurras status
	bin/aurras transform samples/samples-1.m4a output.m4a alto eco rapido
	bin/aurras transform samples/sample-1-so.m4a output.m4a alto eco rapido
