OUTPUT=filosofos

CC=gcc
CC_OPT=-std=c11 -w
# CC_PTH removido porque não existe pthread no MINIX

.PHONY: all
all: $(OUTPUT)

$(OUTPUT): $(OUTPUT).c
	$(CC) -o $(OUTPUT) $(CC_OPT) $(OUTPUT).c

.PHONY: clean
clean:
	rm -f $(OUTPUT)

.PHONY: run
run:
	./$(OUTPUT)

