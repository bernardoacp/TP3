all: biuaidinaive
	./bin/biuaidinaive
	gnuplot ./plot/out.gp
	evince ./plot/out.eps

biuaidinaive: ./src/biuaidinaive.c
	gcc -Wall -o ./bin/biuaidinaive ./src/biuaidinaive.c -lm -g

clean:
	rm -rf ./bin/biuaidinaive ./plot/out.eps ./plot/out.gp ./plot/*.gpdat
