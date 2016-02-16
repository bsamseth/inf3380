### Exercise
Make a program that uses isotropic diffusion to denoise a JPEG-image. This should be
done both in a serial program, [serial_main.c](serial/serial_main.c), and in a parallel
program, [parallel_main.c](parallel/parallel_main.c). 

The programs should take the following input:

```
$ ./program number_of_iterations kappa_value infile outfile
```

### Build
To build the source code you can use the supplied [CMake file](CMakeLists.txt) (while in this folder):

```
$ mkdir build && cd build
$ cmake [-Ddebug=ON/OFF] ..   # ".." is location of CMakelists.txt
$ make
```
Executables will then be available in the `build` folder.

The option to CMake is optional and decides whether to use `-O2` or `-g` as compile flags. It defaults to `OFF`.

If you have been supplied with a tar-ball then the build directory should be present. For any updates to the code
it is sufficient to just run `make`. 

### Running
The serial program can be run directly 

```
$ ./denoise_serial number_of_iterations kappa_value infile outfile
```

For the parallel you should run the executable with `mpirun`:

```
$ mpirun -n number_of_process denoise_parallel number_of_iterations kappa infile outfile
```

