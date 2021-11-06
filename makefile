
default: make1

make1:
	g++ Launcher.c -o Launcher -lm
	g++ P.c -o P -lm
	g++ S.c -o S -lm
	g++ L.c -o L -lm
	g++ G.c -o G -lm


clean:
	rm -f Launcher
	rm -f P
	rm -f S
	rm -f L
	rm -f G

