### Exercise
To complete oblig 1 you have to first write a serial program
by filling out the `serial_main.c` file in the [serial/](serial/)
directory and finally write a parallel program by filling
out the `parallel_main.c` file in the [parallel/](parallel/) directory.

The two programs shall perform an iso diffusion denoising on
a JPEG image. The two programs shall also accept the
following parameters (in order)

```
$ ./program number_of_iterations kappa_value infile outfile
```

When you are in the serial/ or parallel/ directories you may
compile the programs by typing "make" in the terminal.

When you are ready to deliver you enter the base directory
and type "make delivery" in the terminal. A tarball with
your updated files will then be created ready for delivery.

### Build
To build the source code you can use the supplied CMake file (while in the current folder):

```
$ mkdir build && cd build
$ cmake ..
$ make
```

Executables will then be available in the `build` folder.
