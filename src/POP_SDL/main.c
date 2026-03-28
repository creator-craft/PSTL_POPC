#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "POP_SDL/bgdata.h"
#include "POP_SDL/level.h"

uint8_t screen[320 * 192 / 8];

uint8_t BGTAB1[16384], BGTAB2[16384];

size_t loadImages() {
  FILE *f = fopen("original_res/images/IMG.BGTAB1.DUN", "rb");
  if (f == NULL)
    return 0;

  size_t size = fread(BGTAB1, 1, sizeof(BGTAB1), f);

  fclose(f);
  if (size <= 0)
    return 0;

  f = fopen("original_res/images/IMG.BGTAB2.DUN", "rb");
  if (f == NULL)
    return 0;

  size_t size2 = fread(BGTAB2, 1, sizeof(BGTAB2) - size, f);

  fclose(f);
  if (size2 <= 0)
    return 0;

  return size + size2;
}

#define STA 1
#define OR 2

void add(int tex_id, int x, int y, int op) {
  uint8_t *images = tex_id >= 128 ? BGTAB2 : BGTAB1;
  tex_id %= 128;

  unsigned short offset = (((unsigned short)images[2 * tex_id + 2] << 8) |
                           (unsigned short)images[2 * tex_id + 1]) -
                          0x6000;

  unsigned char bytes_per_line = images[offset++],
                lines_count = images[offset++];

  for (int j = 0; j < lines_count; j++)
    for (int i = 0; i < bytes_per_line; i++)
      if ((y * 320 + j * 320 + x) / 8 + i >= 0 &&
          (y * 320 + j * 320 + x) / 8 + i < sizeof(screen))
        if (op == STA)
          screen[(y * 320 + j * 320 + x) / 8 + i] = images[offset++];
        else if (op == OR)
          screen[(y * 320 + j * 320 + x) / 8 + i] |= images[offset++];

  // for (int j = 0; j < 64; j++)
  //   for (int i = 0; i < 32; i++)
  //     screen[(y * 320 * 64 + j * 320 + x * 32 + i) / 8] |= 1 << ((i) %
  //     8);
}

void drawa(int col, int row, int objid) {
  int texture_idx = piecea[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  int y_off = pieceay[objid];
  add(texture_idx, col * 32, row * 64 + y_off, OR);
}

void drawb(int col, int row, int objid) {
  int texture_idx = pieceb[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  int y_off = pieceby[objid];
  add(texture_idx, col * 32, row * 64 + y_off, OR);
}

void drawc(int col, int row, int objid) {
  int texture_idx = piecec[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  add(texture_idx, col * 32, row * 64, OR);
}

void drawd(int col, int row, int objid) {
  int texture_idx = pieced[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  add(texture_idx, col * 32, row * 64, OR);
}

void drawfront(int col, int row, int objid) {

  int texture_idx = fronti[objid];

  if (texture_idx == 0)
    return;
  texture_idx = texture_idx - 1;

  int x_off = frontx[objid];
  int y_off = fronty[objid];
  add(texture_idx, col * 32 + x_off, row * 64 + y_off, OR);
}

int main() {
  Level lvl;

  int r = loadLevel(&lvl, "original_res/levels/LEVEL1");
  if (r) {
    printf("Err %d durint level init\n", r);
    return -1;
  }

  size_t images_size = loadImages();
  if (images_size == 0) {
    printf("Error while loading images\n");
    return -1;
  }

  if (SDL_Init(SDL_INIT_VIDEO))
    return -1;

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Quit();
    return -1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Prince Of Persia", 0, 0, 280 * 4, 192 * 4,
                       0); // SDL_WINDOW_MAXIMIZED
  if (window == NULL) {
    IMG_Quit();
    SDL_Quit();
    return -1;
  }

  int selected_tex = 0;
  SDL_Rect screen_rect = {0, 0, 280 * 4, 192 * 4};

  SDL_Surface *screen_surface = SDL_GetWindowSurface(window);

  SDL_Surface *apple_screen_surface =
      SDL_CreateRGBSurfaceWithFormat(0, 280, 192, 32, SDL_PIXELFORMAT_BGRA32);
  SDL_FillRect(apple_screen_surface, NULL,
               SDL_MapRGB(apple_screen_surface->format, 255, 100, 150));

  SDL_Keycode current_key;

  for (int i = 0; i < sizeof(screen); i++)
    screen[i] = 0;

  int running = 1;
  while (running) {
    SDL_Event event;

    SDL_PumpEvents();
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
        break;
      case SDL_KEYDOWN:
        current_key = event.key.keysym.sym;
        if (current_key == SDLK_ESCAPE)
          running = 0;
        break;
      }
    }

    for (int row = 0; row < 3; row++)
      for (int col = 0; col < 10; col++) {
        int objid = lvl.blue_type[row * 10 + col] &
                    0x1F; // 0x14: un bloc ABCD = { 0x00, 0x84, 0x85, 0x86 }

        drawc(col, row, objid);
        drawb(col, row, objid);
        drawd(col, row, objid);
        drawa(col, row, objid);
        drawfront(col, row, objid);
      }

    SDL_LockSurface(apple_screen_surface);
    uint32_t *pixels = apple_screen_surface->pixels;
    for (int j = 0; j < 192; j++)
      for (int i = 0; i < 280 / 7; i++)
        for (int k = 0; k < 7; k++)
          pixels[j * 280 + i * 7 + k] =
              screen[(j * 320) / 8 + i] & (1 << (k)) ? 0xFFFFFFFF : 0xFF000000;
    SDL_UnlockSurface(apple_screen_surface);

    // SDL_FillRect(screen_surface, NULL, 0xFF00FFFF);
    SDL_BlitScaled(apple_screen_surface, NULL, screen_surface, &screen_rect);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(50);
  }

  SDL_FreeSurface(apple_screen_surface);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}