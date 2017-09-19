CC=g++
CXXFLAGS=-lstdc++ -lSDL2 -lm -lSDL2_ttf
INC=/usr/include/SDL2/
SOURCE_FILES=./src/model.h ./src/model.cpp ./src/SDL_Implementation.cpp

default: breakout
breakout: $(SOURCE_FILES)
	$(CC) -o breakout $(SOURCE_FILES) -I $(INC) $(CXXFLAGS)
clean: rm -r breakout
