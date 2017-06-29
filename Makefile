CC=gcc
CXXFLAGS=-lstdc++ -lSDL2
INC=/usr/include/SDL2/
SOURCE_FILES=./src/breakout.cpp

default: breakout
breakout: $(SOURCE_FILES)
	$(CC) -o breakout $(SOURCE_FILES) -I $(INC) $(CXXFLAGS)
clean: rm -r breakout
