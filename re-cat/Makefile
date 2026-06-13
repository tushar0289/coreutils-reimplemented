CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = output
OBJ = re-cat.o functions.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

re-cat.o: re-cat.c functions.h
	$(CC) $(CFLAGS) -c re-cat.c

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c functions.c

.PHONY: clean

clean:
	@echo "Removing the executable and object files"
	@rm -rf *.o $(TARGET)
