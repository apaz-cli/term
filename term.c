/* SDL Term.
 * A quick and dirty terminal emulaator written with SDL
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define TERM_TITLE "Tiny Terminal"
#define TERM_FONTFILE "assets/lemon.ttf"
#define TERM_BACKGROUND "assets/background.jpg"
#define TERM_FONTSIZE 15
#define TERM_WIDTH 80
#define TERM_HEIGHT 24
#define TERM_FPS 60

#define PANIC()                                                                \
  do {                                                                         \
    printf("Panic at %s:%d\nSDL Error: %s\n", __FILE__, __LINE__,              \
           SDL_GetError());                                                    \
    exit(1);                                                                   \
  } while (0)

static TTF_Font *font = NULL;
static void load_font(void) {
  font = TTF_OpenFont(TERM_FONTFILE, TERM_FONTSIZE);
  if (!font)
    PANIC();
}

static SDL_Window *window = NULL;
static void create_window(void) {
  // Create a window in the middle of the screen
  // It should be the correct size given font width and height.
  int width = 0, height = 0;
  if (TTF_SizeText(font, "a", &width, &height) != 0)
    PANIC();
  width *= TERM_WIDTH;
  height *= TERM_HEIGHT;

  window = SDL_CreateWindow(TERM_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, width, height, 0);
  if (!window)
    PANIC();
}

static SDL_Renderer *renderer = NULL;
static void create_renderer() {
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Error creating renderer: %s\n", SDL_GetError());
    exit(1);
  }
}

static SDL_Texture *background = NULL;
static void load_background(void) {
  SDL_Surface *surface = IMG_Load(TERM_BACKGROUND);
  if (!surface)
    PANIC();
  background = SDL_CreateTextureFromSurface(renderer, surface);
  if (!background)
    PANIC();
  SDL_FreeSurface(surface);
}

static void render_background() {
  SDL_RenderCopy(renderer, background, NULL, NULL);
}

static void render_text(SDL_Renderer *renderer, char *text) {
  if (!text || !*text)
    return;

  int window_width, window_height;
  SDL_GetWindowSize(window, &window_width, &window_height);

  // Render the text wrapped.
  SDL_Color color = {255, 0, 0, 255};
  SDL_Surface *surface =
      TTF_RenderText_Blended_Wrapped(font, text, color, window_width);
  if (!surface)
    PANIC();
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture)
    PANIC();
  SDL_Rect rect = {0, 0, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

int main() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    PANIC();
  if (TTF_Init() != 0)
    PANIC();

  load_font();
  create_window();
  create_renderer();
  load_background();
  render_background();

  // Main loop
  SDL_Event event;
  int quit = 0;
  char input[10000];
  memset(input, 0, sizeof(input));

  SDL_StartTextInput();
  while (!quit) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      printf("Event type: %d\n", event.type);
      if (event.type == SDL_QUIT) {
        quit = 1;
      } else if (event.type == SDL_TEXTINPUT) {
        strcat(input, event.text.text);
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(input) > 0) {
          input[strlen(input) - 1] = '\0';
        } else if (event.key.keysym.sym == SDLK_RETURN) {
          input[strlen(input)] = '\n';
        }
      }
    }

    // Render
    SDL_RenderClear(renderer);
    render_background();
    render_text(renderer, input);

    // Display
    SDL_RenderPresent(renderer);
    SDL_Delay(1000 / TERM_FPS);
  }

  // Cleanup
  SDL_DestroyTexture(background);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
