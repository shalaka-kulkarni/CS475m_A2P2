CC = g++
LIBS = -lGL -lglut -lGLU -lm 

SRC = room.cpp
BIN = room

all	:	$(BIN)

$(BIN)	:	$(SRC)
	$(CC) $(SRC) -o $(BIN) $(LIBS)

clean	:
	rm -f $(BIN) *~
