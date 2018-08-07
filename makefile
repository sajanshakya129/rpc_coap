IDIR=./include
CC=g++
CFLAGS=-I$(IDIR)
ODIR=obj
LIBS=-lmnl
BIN=./bin

_DEPS = network.hpp iproute.hpp clipp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=network.o iproute.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.cpp $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN)/irpc: $(OBJ)
	@mkdir -p $(BIN)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 