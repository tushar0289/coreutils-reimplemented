CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
TARGET = build/output
OBJS = build/re-cat.o build/functions.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

build/re-cat.o: src/re-cat.c include/functions.h
	$(CC) $(CFLAGS) -c src/re-cat.c -o build/re-cat.o

build/functions.o: src/functions.c include/functions.h     
	$(CC) $(CFLAGS) -c src/functions.c -o build/functions.o

.PHONY: clean

clean:
	@echo "Removing the executable and object files"
	@rm -rf build/*
