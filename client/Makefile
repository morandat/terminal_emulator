# Remplacez main par le nom de votre projet
EXEC=main
BOARD_PATH=../board
CFLAGS= -std=c99 -O3 -Wall -Werror -I../board
LDLIBS=-L$(BOARD_PATH) -lbd
CPPFLAGS=-D_XOPEN_SOURCE=500
# Completez par la liste de vos fichier objets (.o)
OBJS= main.o

all: $(EXEC)
$(EXEC): $(OBJS)
lib: $(BOARD_PATH)/libbd.a
clean:
	$(RM) $(EXEC) *.o
$(BOARD_PATH)/libbd.a:
	make -sC $(BOARD_PATH) libbd.a
.PHONY: all clean lib
