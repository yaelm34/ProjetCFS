# options and such
NOMETUDIANT := nometudiant
CC     = gcc
OPTS   = -O -Wall -I.. 
INCS   = 
LIBS   =  -L. -L../build -lFS 

# files we need
SRCS   = main.c 
OBJS   = $(SRCS:.c=.o)
TARGET = main 

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(INCS) $(OPTS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
	cp ../build/libFS.so .