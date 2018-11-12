# options and such
CC     = gcc
OPTS   = -Wall -fpic
#votre nom ici
LIBS   = -L. -L./build
BUILD= build/
SRCS   = LibFS.c Disque.c
OBJS   = $(SRCS:.c=.o)
TARGET = libFS.so

all: $(TARGET)

clean:
	cd build
	rm -f $(TARGET) $(OBJS)
	cd ..

%.o: %.c
	$(CC) $(INCS) $(OPTS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LIBS) -shared -o $(BUILD)$(TARGET) $(OBJS)
	mv *.o $(BUILD) 