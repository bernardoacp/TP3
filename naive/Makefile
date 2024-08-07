all: biuaidinaive
	./biuaidinaive 610167.846027439 7795602.43183451
	gnuplot out.gp
	evince out.eps

biuaidinaive: biuaidinaive.c
	gcc -Wall -o biuaidinaive biuaidinaive.c -lm

clean:
	rm -rf biuaidinaive out.eps out.gp *.gpdat
