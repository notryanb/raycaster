all: clean build run

build:
	gcc -std=c99 ./src/*.c -lm -lSDL2 -o ./build/raycast;

run:
	./build/raycast;

clean:
	rm -rf ./build/*;

.PHONY: all build run clean
