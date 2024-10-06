
all: siggen dft_1 fft_2 fft_3 fft_4 fft_we fft_wm

siggen: siggen.c
	gcc -o $@ $^ -lm -g

fft_we: fft_we.c
	gcc -o fft_we.o -c $^ -I$(HOME)/.local/include -lm
	gcc -o $@ fft_we.o -lfftw3f -lm -L$(HOME)/.local/lib

fft_wm: fft_wm.c
	gcc -o fft_wm.o -c $^ -I$(HOME)/.local/include -lm
	gcc -o $@ fft_wm.o -lfftw3f -lm -L$(HOME)/.local/lib

fft_2: fft_2.c 
	gcc -o $@ $^ -lm -g

fft_3: fft_3.c 
	gcc -o $@ $^ -lm -g

fft_4: fft_3.c 
	gcc -o $@ $^ -lm -g -O3 -ffast-math # -fno-trapping-math -fno-math-errno

dft_1: dft_1.c 
	gcc -o $@ $^ -lm -g


