#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct {
  float** image_data; /* the (m x n) matrix */
  int m; /* pixels on y axis */
  int n; /* pixels on x axis */
} Image;
  

void allocate_image(Image *u, int m, int n);
void deallocate_image(Image *u);
void convert_jpeg_to_image(const unsigned char* image_chars, Image *u);
void convert_image_to_jpeg(const Image *u, unsigned char* image_chars);
void iso_diffusion_denoising(Image *u, Image *u_bar, float kappa, int iters);

#endif
