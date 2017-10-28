SOURCES := $(shell find *.cpp)

all: build run

build:
	@clear
	g++ -std=gnu++14 $(SOURCES) -o a.out -fsanitize=leak

run:
	@./a.out > log.txt

clean:
	rm log.txt
	rm a.out