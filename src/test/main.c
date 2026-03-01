#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "POP_external/Level.h"

int main() {

  LEVEL *lvl = createLevel();
  if(lvl == NULL){
    printf("Err 1 : L'allocation mémoire n'a pas pu être effectué\n");
    return 1;
  }

  int r = initiateLevel(lvl, "LEVEL/LEVEL0");
  if (r > 0){
    printf("Err %d : La récupération des données du niveau n'a pu être effectué\n", r);
    return 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO)) {
    return -1;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    SDL_Quit();
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow("Prince Of Persia", 0, 0, 0, 0, SDL_WINDOW_MAXIMIZED);
  if (window == NULL) {
    IMG_Quit();
    SDL_Quit();
    return -1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer) {
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return -1;
  }

  #define TEX_COUNT 127
  SDL_Texture *textures[TEX_COUNT];

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

  int selected_tex = 0;
  SDL_Rect dst_rect = { 100, 120, 128, 64 };
  // SDL_QueryTexture(textures[selected_tex], NULL, NULL, &dst_rect.w, &dst_rect.h);

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
        else if (current_key == SDLK_LEFT) {
          selected_tex --;
          if (selected_tex < 0)
            selected_tex = TEX_COUNT - 1;
          // SDL_QueryTexture(textures[selected_tex], NULL, NULL, &dst_rect.w, &dst_rect.h);
          // dst_rect.w *= 8;
          // dst_rect.h *= 8;
        } else if (current_key == SDLK_RIGHT) {
          selected_tex ++;
          if (selected_tex >= TEX_COUNT)
            selected_tex = 0;
          // SDL_QueryTexture(textures[selected_tex], NULL, NULL, &dst_rect.w, &dst_rect.h);
          // dst_rect.w *= 8;
          // dst_rect.h *= 8;
        }
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    for (int row = 0; row < 3; row++)
      for (int col = 0; col < 10; col++) {
        dst_rect.x = col * 128;
        dst_rect.y = row * 64;
        SDL_RenderCopy(renderer, textures[lvl->blue_type[row * 10 + col] & 0x1F], NULL, &dst_rect);
      }

    SDL_RenderPresent(renderer);
  }

  for (int i = 0; i < TEX_COUNT; i++)
    SDL_DestroyTexture(textures[i]);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  destroyLevel(lvl);

  return EXIT_SUCCESS;
}

