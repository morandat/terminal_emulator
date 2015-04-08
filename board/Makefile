EXEC=board
LIB=libbd.a

CFLAGS=-O3 -Wall -Werror
CXXFLAGS=-O3 -Wall -Werror `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv` -lstdc++
ifdef EIRB
LIBEIRB=-L/net/ens/fmorandat/lib -Wl,-R/net/ens/fmorandat/lib
endif

OBJS= board.o Socket.o
LIB_SRC=board_client.o
	
all: $(EXEC) $(LIB)
lib: $(LIB)

$(LIB): board_client.o
	ar -r $@ $^

$(EXEC): $(OBJS)
	$(CXX) $(LIBEIRB) $(LDFLAGS) $(OBJS) -o $(EXEC)

clean:
	$(RM) $(EXEC) *.o *.a
.PHONY: all lib clean
