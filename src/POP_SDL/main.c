#include <SDL2/SDL.h>
#include <stdlib.h>

#include "POP_SDL/bgdata.h"
#include "POP_SDL/frameadv.h"
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

int loadLevel(Level *level, char *filename) {
  FILE *f = fopen(filename, "r");

  if (f == NULL)
    return 1;

  size_t size = fread(level, sizeof(Level), 1, f);
  printf("s: %d %d\n", size, sizeof(Level));

  if (size != 1)
    return size;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 10; col++)
      printf("%02X:%02X ", level->blue_type[row * 10 + col] & ~0x1F,
             level->blue_type[row * 10 + col] & 0x1F);
    printf("\n");
  }

  return 0;
}

static inline float clamp(float v, float mn, float mx) {
  return v < mn ? mn : (v > mx ? mx : v);
}

void renderColoredScreen(SDL_Surface *surface) {
  SDL_LockSurface(surface);
  uint32_t *pixels = (uint32_t *)surface->pixels;

  static const float w[7] = {0.05f, 0.10f, 0.20f, 0.30f, 0.20f, 0.10f, 0.05f};
  // static const float w[5] = {0.10f, 0.20f, 0.40f, 0.20f, 0.10f};

  for (int j = 0; j < 192; j++) {
    const int row_off = (j * 320) / 8;
    float sig[286] = {0};
    int pal[286] = {0};

    for (int i = 0; i < 40; i++) {
      uint8_t b = screen[row_off + i];
      int p = (b >> 7) & 1;
      for (int k = 0; k < 7; k++) {
        int idx = i * 7 + k + 3;
        // int idx = i * 7 + k + 2;
        sig[idx] = (b >> k) & 1 ? 1.0f : 0.0f;
        pal[idx] = p;
      }
    }

    for (int x = 0; x < 280; x++) {
      float Y = 0.0f, I = 0.0f, Q = 0.0f;
      for (int t = 0; t < 7; t++) {
        // for (int t = 0; t < 5; t++) {
        int idx = x + t;
        int col = x + t - 3;
        float s = sig[idx];
        float sgn = (col & 1) ? -1.0f : 1.0f;
        float ci = pal[idx] ? -0.70711f : 0.70711f;
        Y += w[t] * s;
        I += w[t] * s * sgn * ci;
        Q += w[t] * s * sgn * 0.70711f;
      }

      I *= 2.0f;
      Q *= 2.0f;

      float R = clamp(Y + 0.9563f * I + 0.6210f * Q, 0, 1);
      float G = clamp(Y - 0.2721f * I - 0.6474f * Q, 0, 1);
      float B = clamp(Y - 1.1070f * I + 1.7046f * Q, 0, 1);

      pixels[j * 280 + x] = 0xFF000000 | ((uint32_t)(R * 255.0f + 0.5f) << 16) |
                            ((uint32_t)(G * 255.0f + 0.5f) << 8) |
                            (uint32_t)(B * 255.0f + 0.5f);
    }
  }
  SDL_UnlockSurface(surface);
}

void renderBnWScreen(SDL_Surface *surface) {
  SDL_LockSurface(surface);
  uint32_t *pixels = surface->pixels;
  for (int j = 0; j < 192; j++)
    for (int i = 0; i < 280 / 7; i++)
      for (int k = 0; k < 7; k++)
        pixels[j * 280 + i * 7 + k] =
            screen[(j * 320) / 8 + i] & (1 << (k)) ? 0xFFFFFFFF : 0xFF000000;
  SDL_UnlockSurface(surface);
}

void updateScreen(SDL_Surface *apple_screen_surface,
                  SDL_Surface *screen_surface, SDL_Rect screen_rect, Level *lvl,
                  int screen_idx) {
  for (int row = 2; row >= 0; row--)
    for (int col = 0; col < 10; col++)
      drawBlock(lvl, screen_idx, col, row);

  // drawTest();

  renderColoredScreen(apple_screen_surface);

  SDL_BlitScaled(apple_screen_surface, NULL, screen_surface, &screen_rect);
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

  SDL_Window *window =
      SDL_CreateWindow("Prince Of Persia", 0, 0, 280 * 4, 192 * 4,
                       0); // SDL_WINDOW_MAXIMIZED
  if (window == NULL) {
    SDL_Quit();
    return -1;
  }

  int screen_idx = 0;
  SDL_Rect screen_rect = {0, 0, 280 * 4, 192 * 4};

  SDL_Surface *screen_surface = SDL_GetWindowSurface(window);

  SDL_Surface *apple_screen_surface =
      SDL_CreateRGBSurfaceWithFormat(0, 280, 192, 32, SDL_PIXELFORMAT_BGRA32);
  SDL_FillRect(apple_screen_surface, NULL,
               SDL_MapRGB(apple_screen_surface->format, 255, 100, 150));

  SDL_Keycode current_key;

  for (int i = 0; i < sizeof(screen); i++)
    screen[i] = 0;

  updateScreen(apple_screen_surface, screen_surface, screen_rect, &lvl,
               screen_idx);

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

        struct map_entry map = lvl.map[screen_idx];
        int screen_goal = current_key == SDLK_LEFT    ? map.left
                          : current_key == SDLK_RIGHT ? map.right
                          : current_key == SDLK_DOWN  ? map.down
                          : current_key == SDLK_UP    ? map.up
                                                      : 0;

        if (screen_goal > 0) {
          memset(screen, 0, sizeof(screen));
          screen_idx = screen_goal - 1;
          printf("Goto %d\n", screen_idx);
          updateScreen(apple_screen_surface, screen_surface, screen_rect, &lvl,
                       screen_idx);
        }

        break;
      }
    }

    SDL_UpdateWindowSurface(window);
    SDL_Delay(50);
  }

  SDL_FreeSurface(apple_screen_surface);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return EXIT_SUCCESS;
}