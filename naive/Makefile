all: biuaidinaive
	./biuaidinaive
	gnuplot out.gp
	evince out.eps

biuaidinaive: biuaidinaive.c
	gcc -Wall -o biuaidinaive biuaidinaive.c -lm -g

clean:
	rm -rf biuaidinaive out.eps out.gp *.gpdat
