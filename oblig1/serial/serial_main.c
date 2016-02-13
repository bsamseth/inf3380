#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "functions.h"

// make use of two functions from the simplejpeg library
void import_JPEG_file(const char *filename, unsigned char **image_chars,
                      int *image_height, int *image_width,
                      int *num_components);

void export_JPEG_file(const char *filename, unsigned char *image_chars,
                      int image_height, int image_width,
                      int num_components, int quality);


int main(int argc, char *argv[])
{
  int m, n, c, iters;
  float kappa;
  Image u, u_bar;
  unsigned char *image_chars;
  char *input_jpeg_filename, *output_jpeg_filename;


  /* read from command line: kappa, iters, input_jpeg_filename, output_jpeg_filename */
  if (argc > 4) {
    iters = atoi(argv[1]);
    kappa = atof(argv[2]);
    input_jpeg_filename = argv[3];
    output_jpeg_filename = argv[4];
  }
  else {
    printf("Usage: %s iters kappa input_file output_file\n", argv[0]);
    exit(1);
  }
  
  /* ... */
  printf("Importing image from file ...\n");
  import_JPEG_file(input_jpeg_filename, &image_chars, &m, &n, &c);

  printf("Allocating space for Images ...\n");
  allocate_image (&u, m, n);
  allocate_image (&u_bar, m, n);

  printf("Converting jpeg to Image ...\n");
  convert_jpeg_to_image (image_chars, &u);

  printf("Performing %d isotropic duffusions ...\n", iters);
  iso_diffusion_denoising (&u, &u_bar, kappa, iters);

  printf("Converting Image to jpeg ...\n");
  convert_image_to_jpeg (&u_bar, image_chars);

  printf("Deallocating Images...\n");
  deallocate_image (&u);
  deallocate_image (&u_bar);

  printf("Exporting jpeg ...\n");
  export_JPEG_file(output_jpeg_filename, image_chars, m, n, c, 75);

  return 0;
}
