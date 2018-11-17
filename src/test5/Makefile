# options and such
NOMETUDIANT := nometudiant
CC     = gcc
OPTS   = -O -Wall -I.. 
INCS   = 
LIBS   =  -L. -L../build -lFS 

# files we need
SRCS   = main_do.c main_eval.c 
OBJS   = $(SRCS:.c=.o)
TARGETDO = main_do 
TARGETEVAL = main_eval 

all: $(TARGETDO) $(TARGETEVAL)

clean:
	rm -f $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(INCS) $(OPTS) -c $< -o $@

$(TARGETDO): main_do.o
	$(CC) -o $(TARGETDO) main_do.o $(LIBS)
	cp ../build/libFS.so .

$(TARGETEVAL): main_eval.o
	$(CC) -o $(TARGETEVAL) main_eval.o $(LIBS)
	cp ../build/libFS.so .