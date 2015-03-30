EXEC=main

CC=g++
CXXFLAGS=-O3 `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv`
SRC= main.cpp Socket.cpp Window.cpp

	
OBJS= $(SRC:.cpp=.o)

all: $(EXEC)
$(EXEC): $(OBJS)

.PHONY: all clean mrproper

clean:
	$(RM) $(EXEC) $(OBJS)
