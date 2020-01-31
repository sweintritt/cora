.PHONY=all clean
GCC=g++

LIBVLC_FLAGS=`pkg-config --cflags libvlc`
LIBVLC_LIBS=`pkg-config --libs libvlc`

FLAGS=$(LIBVLC_FLAGS) -std=c++11 -pedantic -Wextra -Wall -Wundef -Wunused -Wshadow -ftrapv -Wunreachable-code
LIBS=$(LIBVLC_LIBS)

FILES=cora

all: $(FILES)

clean:
	rm -f *.o $(FILES)

cora: main.cpp
	$(GCC) $(FLAGS) -o $@ $< $(LIBS)

%.o: %.c
	$(GCC) $(FLAGS) -c $< $(LIBS)
