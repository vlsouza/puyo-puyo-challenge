all:
	g++ -I src/include -L src/lib -o main src/*.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
