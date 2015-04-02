EXEC=board
LIB=libbd.a

CFLAGS=-O3 -Wall -Werror
CXXFLAGS=-O3 -Wall -Werror `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv` -lstdc++

OBJS= board.o Socket.o
LIB_SRC=board_client.o
	
all: $(EXEC) $(LIB)
lib: $(LIB)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

$(LIB): board_client.o
	ar -r $@ $^
$(EXEC): $(OBJS)
clean:
	$(RM) $(EXEC) *.o *.a
.PHONY: all lib clean
