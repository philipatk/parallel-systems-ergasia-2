# Ρυθμίσεις Μεταγλωττιστή
CC = gcc
CFLAGS = -Wall -O3 -fopenmp
TARGET = poly_mul
SRC = poly_mul.c

N ?= 100000
THREADS ?= 4

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	@echo "Εκτέλεση του $(TARGET) με n=$(N) και threads=$(THREADS)..."
	./$(TARGET) $(N) $(THREADS)