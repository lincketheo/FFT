# FFT 
My implementation of various fourier transforms in C 

# Compile
```
$ make
```

I suppose you'll have to change the compile options for fftw executables, or just remove them in the make file

# Generate a signal:

Generates a signal with 4 added signals (frequency initial phase and amplitude)
with fs 1000 and k ranging from 0 to 2^20
```
$ ./siggen 10.123f5p3a 50f10a 200f1.123a 900f0.1p -fs 1000 -k 0:1048576 > sig.bin
```

# Results:

## DFT
- Vanilla 
Average Time: Aborted, at least 5 minutes

## FFT_1 
- My first attempt, realized this was just fancy DFT, scrapped

## FFT_2
- Out of place
- Mallocs for each recursive call
- Recursive 

```
./fft_2 sig.bin > sig.fft.bin
```
Average Time: 0.466 seconds

## FFT_3 
- In place with bit reversal
- No recursion (iterative)
```
./fft_3 sig.bin > sig.fft.bin
```
Average Time: 0.296 seconds 

## FFT_4 
- Same as fft_3 but with O3 and fast math
```
./fft_4 sig.bin > sig.fft.bin
```
Average Time: 0.0891 seconds 

## FFT_WE 
- FFTW with estimation initialization
```
./fft_we sig.bin > sig.fft.bin
```
Average Time: 0.0257 seconds 

## FFT_WM
- FFTW with measure initialization 
- (Note, very slow set up time)
```
./fft_wm sig.bin > sig.fft.bin
```
Average Time: 0.0187 seconds 

## TODO 
- Verify power of 2's 
- Memory cleanups
- Make a nicer interface
- Specify block size
