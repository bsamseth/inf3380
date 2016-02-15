#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

void allocate_image(Image* u, int m, int n) {
  u->m = m;
  u->n = n;
  u->image_data = (float**) malloc(m * sizeof(float*));

  for(int i = 0; i < m; i++)
    u->image_data[i] = (float*) malloc(n * sizeof(float));
}

void deallocate_image(Image* u) {
  for(int i = 0; i < u->m; i++)
    free(u->image_data[i]);
  free(u->image_data);
  u->m = 0;
  u->n = 0;
}

/* the array image_chars are assumed to be (m x n) */
void convert_jpeg_to_image(const unsigned char* image_chars, Image* u) {
  for (int i = 0; i < u->m; i++) {
    for (int j = 0; j < u->n; j++) {
      u->image_data[i][j] = (float) image_chars[u->n * i + j];
    }
  }
}

void convert_image_to_jpeg(const Image *u, unsigned char* image_chars) {
  for (int i = 0; i < u->m; i++) {
    for (int j = 0; j < u->n; j++) {
      image_chars[u->n * i + j] = u->image_data[i][j];
    }
  }
}

void iso_diffusion_denoising(Image* u, Image* u_bar, float kappa, int iters) {
  float** A = u->image_data; /* used for shorthand */
  float** B = u_bar->image_data;

  /* first copy boundry values from u=A to u_bar=B */
  for (int i = 0; i < u->m; i++) {
    B[i][0] = A[i][0];
    B[i][u->n-1] = A[i][u->n-1];
  }
  for (int j = 1; j < u->n-1; j++) {
    B[0][j] = A[0][j];
    B[u->m-1][j] = A[u->m-1][j];
  }
  
  for (int counter = 0; counter < iters; counter++) {
    /* perform one iteration of the transformation
     * note that only inner pixels are modified */
    for (int i = 1; i < u->m-1; i++) {
      for (int j = 1; j < u->n-1; j++) {
        B[i][j] = A[i][j] + kappa * (A[i-1][j] + A[i][j-1]
				     - 4*A[i][j]
				     + A[i+1][j] + A[i][j+1] );
      }
    }

    /* update for a new iteration */
    for (int i = 1; i < u->m; i++) 
      for (int j = 1; j < u->n; j++) 
	u->image_data[i][j] = u_bar->image_data[i][j];
  }  
}

