IDIR=./include
COAP_SRC=/usr/local/include/coap/
COAP_LIB=/usr/local/lib/libcoap-2.la

IDIR_COAP=./include/coap
CC=g++ 
CFLAGS=-I$(IDIR) -I$(COAP_SRC) -I$(IDIR_COAP) -I$(COAP_LIB) -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -O2 -D_GNU_SOURCE -DWITH_POSIX
ODIR=obj
LIBS=-lmnl -lcoap-2
BIN=./bin

_DEPS =	network.hpp	iproute.hpp clipp.h coap/coapClient.hpp coap/coapCore.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=network.o iproute.o main.o coapClient.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.cpp  $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN)/irpc: $(OBJ)
	@mkdir -p $(BIN)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o