#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    return 1;
  }

  if (SDL_Init(0) != 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
    SDL_Quit();
    return 1;
  }

  FILE *fp = fopen(argv[1], "rb");
  if (!fp) {
    perror("fopen");
    return 1;
  }

  if (fseek(fp, 0, SEEK_END) != 0) {
    perror("fseek");
    fclose(fp);
    return 1;
  }

  long size = ftell(fp);
  if (size < 0) {
    perror("ftell");
    fclose(fp);
    return 1;
  }

  rewind(fp);

  unsigned char *data = malloc((size_t)size);
  if (!data) {
    perror("malloc");
    fclose(fp);
    return 1;
  }

  if (fread(data, 1, (size_t)size, fp) != (size_t)size) {
    perror("fread");
    free(data);
    fclose(fp);
    return 1;
  }

  fclose(fp);

  char *file_name = argv[1];

  { // Get the file name from the path
    char *p1 = strrchr(file_name, '/');
    char *p2 = strrchr(file_name, '\\');

    char *last = p1 > p2 ? p1 : p2;

    if (last)
      file_name = last + 1;
  }

  char surface_name[256];
  sprintf(surface_name, "out/%s", file_name);
  mkdir(surface_name, 0700);

  unsigned char images_count = data[0];
  printf("Images count: %d\n", images_count);

  for (int i = 0; i < images_count; i++) {
    unsigned short offset =
        (((unsigned short)data[2 * i + 2] << 8) |
         (unsigned short)data[2 * i + 1]) -
        0x6000; // 0x6000 = the absolute image's position on ram

    unsigned char bytes_per_line = data[offset++], lines_count = data[offset++];

#define PPB 7
    printf("Image %d Offset: %d ; w=%d; h=%d\n", i, offset,
           PPB * bytes_per_line, lines_count);

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, PPB * bytes_per_line, lines_count, 32, SDL_PIXELFORMAT_BGRA32);

    if (!surface) {
      fprintf(stderr, "Surface creation failed: %s\n", SDL_GetError());
      break;
    }

    SDL_LockSurface(surface);
    Uint32 *pixels = (Uint32 *)surface->pixels;

    for (int y = 0; y < lines_count; y++) {
      for (int x = 0; x < bytes_per_line; x++) {
        unsigned char byte = data[offset++];
        for (int bit = 0; bit < PPB; bit++) {
          // printf((byte & (1 << bit)) ? "#" : ".");
          int b = ((byte >> bit) & 1);
          pixels[(lines_count - y - 1) * PPB * bytes_per_line + PPB * x + bit] =
              (byte >> bit) & 1 ? 0xFFFFFFFF : 0xFF000000;
        }
      }
      // printf("\n");
    }

    SDL_UnlockSurface(surface);

    sprintf(surface_name, "out/%s/%d.png", file_name, i);

    if (IMG_SavePNG(surface, surface_name) != 0) {
      fprintf(stderr, "PNG save failed: %s\n", IMG_GetError());
      break;
    }

    SDL_FreeSurface(surface);
  }

  free(data);

  IMG_Quit();
  SDL_Quit();

  return 0;
}
