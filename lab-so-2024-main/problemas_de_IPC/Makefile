OUTPUT=filosofos

CC=cc
CC_OPT=-std=c11 -w
# Removi a flag -pthread porque não funciona no MINIX

.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OUTPUT).c
	$(CC) $(CC_OPT) -o $(OUTPUT) $(OUTPUT).c

.PHONY: clean
clean:
	rm -f $(OUTPUT)

.PHONY: run
run:
	./$(OUTPUT)
