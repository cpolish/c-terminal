CC = gcc
CFLAGS = -Wall -Werror -g
TARGET = terminal
.RECIPEPREFIX = >

all: $(TARGET)

$(TARGET): $(TARGET).c

>$(CC) $(CFLAGS) -o $(TARGET).so $(TARGET).c

clean:
>rm $(TARGET).so
