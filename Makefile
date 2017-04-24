CC = gcc
CFLAGS = -Wall -g -pthread

PROGRAMS = Hsb 

all: $(PROGRAMS)

clean:
	rm -f *~ $(PROGRAMS)
