#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "POP/BGDATA.h"
#include "POP_external/Level.h"

void add(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y) {
  SDL_Rect dst_rect = {x, y};
  SDL_QueryTexture(texture, NULL, NULL, &dst_rect.w, &dst_rect.h);

  dst_rect.h *= 4;
  dst_rect.w *= 4;
  SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
}

void drawa(SDL_Renderer *renderer, SDL_Texture *textures[], SDL_Rect dst_rect,
           int col, int row, int objid) {

  int texture_idx = piecea[objid];

  if (texture_idx == 0)
    return;
  texture_idx = (texture_idx - 1);

  add(renderer, textures[texture_idx], col * 128, row * 64);
}

void drawb(SDL_Renderer *renderer, SDL_Texture *textures[], SDL_Rect dst_rect,
           int col, int row, int objid) {
  int texture_idx = pieceb[objid];

  if (texture_idx == 0)
    return;
  texture_idx = (texture_idx - 1) % 128;

  add(renderer, textures[texture_idx], col * 128, row * 64);
}

void drawc(SDL_Renderer *renderer, SDL_Texture *textures[], SDL_Rect dst_rect,
           int col, int row, int objid) {

  int texture_idx = piecec[objid];

  if (texture_idx == 0)
    return;
  texture_idx = (texture_idx - 1) % 128;

  add(renderer, textures[texture_idx], col * 128, row * 64);
}

void drawd(SDL_Renderer *renderer, SDL_Texture *textures[], SDL_Rect dst_rect,
           int col, int row, int objid) {

  int texture_idx = pieced[objid];

  if (texture_idx == 0)
    return;
  texture_idx = (texture_idx - 1) % 128;

  add(renderer, textures[texture_idx], col * 128, row * 64);
}

int main() {

  Level lvl;

  int r = loadLevel(&lvl, "LEVEL/LEVEL0");
  if (r) {
    printf("Err %d durint level init\n", r);
    return -1;
  }

  if (SDL_Init(SDL_INIT_VIDEO))
    return -1;

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Quit();
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Prince Of Persia", 0, 0, 1300, 300,
                                        0); // SDL_WINDOW_MAXIMIZED
  if (window == NULL) {
    IMG_Quit();
    SDL_Quit();
    return -1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return -1;
  }

#define TEX_COUNT 127
#define TEX_COUNT2 50
  SDL_Texture *textures[TEX_COUNT + TEX_COUNT2];

  for (int i = 0; i < TEX_COUNT; i++) {
    char fname[64];
    sprintf(fname, "res/IMG.BGTAB1.PAL/%i.png", i);
    SDL_Texture *texture = IMG_LoadTexture(renderer, fname);
    if (texture == NULL) {
      printf("Unable to read texture: %d\n", i);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      IMG_Quit();
      SDL_Quit();
      return -1;
    }
    textures[i] = texture;
  }

  for (int i = 0; i < TEX_COUNT2; i++) {
    char fname[64];
    sprintf(fname, "res/IMG.BGTAB2.PAL/%i.png", i);
    SDL_Texture *texture = IMG_LoadTexture(renderer, fname);
    if (texture == NULL) {
      printf("Unable to read texture2: %d\n", i);
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      IMG_Quit();
      SDL_Quit();
      return -1;
    }
    textures[TEX_COUNT + i] = texture;
  }

  int selected_tex = 0;
  SDL_Rect dst_rect = {100, 120, 128, 64};
  // SDL_QueryTexture(textures[selected_tex], NULL, NULL, &dst_rect.w,
  // &dst_rect.h);

  SDL_Keycode current_key;

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

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    for (int row = 0; row < 3; row++)
      for (int col = 0; col < 10; col++) {
        int objid = lvl.blue_type[row * 10 + col] &
                    0x1F; // 0x14: un bloc ABCD = { 0x00, 0x84, 0x85, 0x86 }

        drawa(renderer, textures, dst_rect, col, row, objid);
        drawb(renderer, textures, dst_rect, col, row, objid);
        drawd(renderer, textures, dst_rect, col, row, objid);
        drawc(renderer, textures, dst_rect, col, row, objid);
      }

    SDL_RenderPresent(renderer);
  }

  for (int i = 0; i < TEX_COUNT; i++)
    SDL_DestroyTexture(textures[i]);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}
