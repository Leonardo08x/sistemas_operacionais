# Makefile para compilar os programas filosofos, leitores_escritores_mux, produtor_consumidor_condicao e produtor_consumidor_semaforo

CC = cc
CFLAGS = -Wall
LDFLAGS = -lmthread
TARGETS = filosofos leitores_escritores_mux produtor_consumidor_condicao produtor_consumidor_semaforo

all: $(TARGETS)

filosofos: filosofos.c
	$(CC) $(CFLAGS) -o filosofos filosofos.c $(LDFLAGS)

leitores_escritores_mux: leitores_escritores_mux.c
	$(CC) $(CFLAGS) -o leitores_escritores_mux leitores_escritores_mux.c $(LDFLAGS)

produtor_consumidor_condicao: produtor_consumidor_condicao.c
	$(CC) $(CFLAGS) -o produtor_consumidor_condicao produtor_consumidor_condicao.c $(LDFLAGS)

produtor_consumidor_semaforo: produtor_consumidor_semaforo.c
	$(CC) $(CFLAGS) -o produtor_consumidor_semaforo produtor_consumidor_semaforo.c $(LDFLAGS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
