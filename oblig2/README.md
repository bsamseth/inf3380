[![Build Status](https://travis-ci.org/bsamseth/inf3380.svg?branch=master)](https://travis-ci.org/bsamseth/inf3380)
# Matrix-Matrix Multiplication Using MPI

In this project, an application for matrix-matrix multiplication using MPI has
been developed. This could be done by either 1D row-wise partitioning, or 2D block
partitioning of the input matrices. The latter was chosen, implementing
[Cannon's algorithm](http://cseweb.ucsd.edu/classes/fa12/cse260-b/Lectures/Lec13.pdf)
for a memory efficient approach.

## The Algorithm

We want to compute `C = A x B` in parallel. We do the parallel part by partitioning both
`A` and `B` into a given number of blocks, one for each worker process.
Cannon's algorithm is a way to, once every worker process has been given its own blocks
of the inputs `A` and `B`, systematically shift them between the different processes
in such a way that we end up with each process having computed its corresponding
block of the result `C`.

We can lay out the algorithm in the following steps:

Given `p` processes laid out in a `sqrt(p) x sqrt(p)` virtual grid:

1. Partition the input matrices into `p` blocks, `a` and `b`, and assign them to the corresponding worker.
2. Perform an initial alignment:
    1. Shift `a` on process `P(i,j)` `i` columns left (with wraparound).
    2. Shift `b` on process `P(i,j)` `j` rows up (with wraparound)
3. Compute partial block result `c += a x b`.
4. Shift `a` one column left, and `b` one row up.
5. While multiplication has been done less than `sqrt(p)` times, go to step 3.

The algorithm is usually described for square `(n x n)` matrices, and with `sqrt(p)/n`
being an whole number. The implementation handles both rectangular matrices, and
the case where the block sizes can't be the same for all processes. The latter is done
by padding out all blocks to the largest block size, filling empty values with zeros. When
the results is put back into the full matrix `C`, only the non-padding elements
are stored. This is to ensure that all multiplications of sub blocks are actually defined.  


## Usage
The [program](app/main.c) takes three filenames as input. The two first are
each a binary file with a matrix of a given size. The last is the filename where
the result of the multiplication is stored. In addition, the program can be run with a
given number of processes. Example of usage:

```
$ mpirun -n 10 main.x inputfile_A inputfile_B outputfile_C
```

Due to the algorithm being used, the number of processes is required to be a perfect
square plus 1 (e.g. `n = 3^2 + 1 = 10`). Other than that, the matrices can be of arbitrary
dimensions (as long as the product is defined).

## Build

To build the source, you can use the supplied [CMakeLists.txt](CMakeLists.txt) file:

```
$ mkdir build && cd build
$ cmake .. # '..' is location of CMakeLists.txt
$ make
```

This produces the `main.x` executable, as well as the test executable `tests.x`.
