#include <cstdio>
#include <cinttypes>
#include <stdlib.h>
#include <png.h>

typedef struct {
  uint8_t r, g, b, a;
} Pixels32;

typedef struct {
  unsigned int width, height;
  Pixels32 *pixels;
} Image32;


int save_image32_as_ppm(Image32 *image, const char* filename) {
  FILE *f = fopen(filename, "wb");
  if(!f) {
    return -1;
  }

  // * Create a PPM file format
  fprintf(f,
          "P6\n"
          "%d %d\n"
          "255\n",
          (int)image->width,
          (int)image->height);

  for (int row = 0; row < (int)image->height; ++row) {
    for (int col = 0; col < (int)image->width; ++col) {
      int index = ((int)image->width * row) + col;
      Pixels32 p = *(image->pixels + index);
      // printf("%c\n", x);
      fputc(p.r, f);
      fputc(p.g, f);
      fputc(p.b, f);
    }
  }

  fclose(f);
  return 0;
}

int main(int argc, char* argv[]) {
  if(argc < 2) {
    fprintf(stderr, "Usage: ./libpng <image.png>\n");
  }

  const char* filepath = argv[1];

  png_image image;
  image.opaque = NULL;
  image.version = PNG_IMAGE_VERSION;

  // * Adds the metadata about image in the structure (png_image)
  png_image_begin_read_from_file(&image, filepath);
  if(image.warning_or_error) {
    fprintf(stderr, "could not read file %s. %s\n", filepath, image.message);
    exit(1);
  }
  
  image.format = PNG_FORMAT_RGBA;
  // * Allocate the buffer for image
  Pixels32 *buffer = (Pixels32 *)malloc(sizeof(Pixels32) * image.width * image.height);

  // * Finish reading the image into the supplied buffer (color-map into your buffers.)
  png_int_32 row_stride = 0;
  png_image_finish_read(&image, NULL, buffer, row_stride, NULL);

  // * For Debugging
  // for(unsigned int y = 0; y <image.height; ++y) {
  //   for(unsigned int x = 0; x < image.width; ++x) {
  //     Pixels32 pixel = buffer[y * image.width + x];
  //     printf("{ .r = %d, .g = %d, b = %d, a = %d }\n", pixel.r, pixel.g, pixel.b, pixel.a);
  //   }
  // }

  // * Create an Image32 object
  Image32 surface = {
      .width = image.width,
      .height = image.height,
      .pixels = buffer,
  };

  // * clean up the png_image structure.
  png_image_free(&image);
  
  // * create the ppm image from Image32 surface
  save_image32_as_ppm(&surface, "output.ppm");
  return 0;
}