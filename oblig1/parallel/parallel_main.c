#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#include "jpeglib.h"
#include "functions.h"

/* make use of two functions from the simplejpeg library */
void import_JPEG_file(const char *filename, unsigned char **image_chars,
		      int *image_height, int *image_width,
		      int *num_components);
void export_JPEG_file(const char *filename, unsigned char *image_chars,
		      int image_height, int image_width,
		      int num_components, int quality);


int main(int argc, char *argv[])
{
  int m, n, c, iters;
  int my_rank, num_procs, num_workers;
  int total_m, average_m, rest_work;
  float kappa;
  Image u, u_bar;
  unsigned char *image_chars, *my_image_chars;
  char *input_jpeg_filename, *output_jpeg_filename;

  int tag = 1;
  MPI_Status status;

  /* read from command line: kappa, iters, input_jpeg_filename, output_jpeg_filename */
  if (argc > 4) {
    iters = atoi(argv[1]);
    kappa = atof(argv[2]);
    input_jpeg_filename = argv[3];
    output_jpeg_filename = argv[4];
  }
  else {
    printf("Argument 1 is = %s\n", argv[1]);
    printf("Usage: %s iters kappa input_file output_file\n", argv[0]);
    exit(1);
  }
  
  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);


  if (my_rank == 0) {

    /* program will assume at least 3 processes */
    if (num_procs < 3) {
      printf("Parallel program must be run with a minimum of 3 processes.\n");
      exit(1);
    }

    import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);
  }


  /* TIME EXECUTION */
  double my_time, max_time, min_time, avg_time;
  MPI_Barrier(MPI_COMM_WORLD);  /*synchronize all processes*/
  my_time = MPI_Wtime();  

  
  /* ensure that all processes sees m and n */
  MPI_Bcast (&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);


  /* divide the m x n pixels evenly among the MPI processes */
  num_workers = num_procs - 1;
  average_m = m / num_workers;
  rest_work = m % num_workers;


  /* MASTER PROCESS */
  if (my_rank == 0) {
    
    /* SEND WORKLOAD TO WORKERS */
    int displ = 0; /* displacement index */
    for (int worker = 1; worker <= num_workers; worker++) {
      total_m = (worker == num_workers) ? average_m + rest_work : average_m;
      total_m += (worker == 1 || worker == num_workers) ? 1 : 2;

      MPI_Send(&total_m, 1, MPI_INT, worker, tag, MPI_COMM_WORLD); /* send total to it's worker */
      /* send image chars starting from index displ and total_m*n addresses forward */
      MPI_Send(&image_chars[displ], total_m * n, MPI_UNSIGNED_CHAR, worker, tag, MPI_COMM_WORLD );

      /* Displacement increased with total_m - 2 rows.
       * -2 because worker share 2 rows with neighbour worker  */
      displ += (total_m - 2) * n;
    }

    /* GATHER INFO FROM WORKERS */
    displ = 0;
    for (int worker = 1; worker <= num_workers; worker++) {
      int expected_rows = (worker == num_workers) ? average_m + rest_work : average_m;

      MPI_Recv(&image_chars[displ], expected_rows*n, MPI_UNSIGNED_CHAR, worker, tag, MPI_COMM_WORLD, &status);

      displ += expected_rows*n;
      
    }

    export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);
    free(image_chars);
  }
  
  /* WORKER PROCESSES */
  else {

    /* recive the total_m sent from master */
    MPI_Recv(&total_m, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

    allocate_image(&u, total_m, n);
    allocate_image(&u_bar, total_m, n);

    /* need to preallocate space for image chars to be filled by MPI-call */
    my_image_chars = (unsigned char*) malloc(total_m*n*sizeof(unsigned char));
    MPI_Recv(my_image_chars, total_m*n, MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD, &status);

    convert_jpeg_to_image(my_image_chars, &u);

    /* ISOTROPIC DIFFUSION ITERATIONS */
    for (int c_iter = 0; c_iter < iters; c_iter++ ) {
      
      iso_diffusion_denoising(&u, &u_bar, kappa, 1); /* 1 iteration */

      /* send and recive updated boundries */
      /* upper boundry */
      /* only needed for workers not at the top */
      if (my_rank != 1)
	MPI_Sendrecv(&u.image_data[1][0], n, MPI_FLOAT, my_rank - 1, tag,
		     &u.image_data[0][0], n, MPI_FLOAT, my_rank - 1, tag,
		     MPI_COMM_WORLD, &status);

      /* lower boundry */
      /* only needed for workers not at the bottom */ 
      if (my_rank != num_workers)
	MPI_Sendrecv(&u.image_data[total_m-2][0], n, MPI_FLOAT, my_rank + 1, tag,
		     &u.image_data[total_m-1][0], n, MPI_FLOAT, my_rank + 1, tag,
		     MPI_COMM_WORLD, &status);
      
    }
    
    /* SEND RESULT */
    convert_image_to_jpeg(&u, my_image_chars);
    deallocate_image(&u);
    deallocate_image(&u_bar);
    
    /* if not top process, don't include upper boundry (included in previous process) */
    int fromIndex = (my_rank == 1) ? 0 : n;
    /* don't return boundries unless top or bottom process. Then include 1 */ 
    int num_rows    = (my_rank == 1 || my_rank == num_workers) ? total_m - 1 : total_m - 2;

    /* send relevant image_chars to master */
    MPI_Send(&my_image_chars[fromIndex], num_rows * n, MPI_UNSIGNED_CHAR, 0,
	     tag, MPI_COMM_WORLD);

    /* my_image_chars no longer used, deallocate */
    free(my_image_chars);
    
  }



  /* CALCULATE TIME DATA */
  my_time = MPI_Wtime() - my_time; 
  /* compute max, min, and average timing statistics */
  MPI_Reduce(&my_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&my_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
  MPI_Reduce(&my_time, &avg_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    avg_time /= num_procs;
    printf("Min: %lf  Max: %lf  Avg:  %lf\n", min_time, max_time,avg_time);
  }
  
  
  MPI_Finalize ();
  return 0;
}
