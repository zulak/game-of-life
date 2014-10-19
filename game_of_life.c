#include <assert.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_main.h>
#include <SDL/SDL_image.h>
#include "game_of_life.h"

static Uint32 next_time;

int main(int argc, char* argv[]) {
  SDL_Surface* screen;
  SDL_Event event;

  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    return 1;
  }
  SDL_WM_SetCaption("Game of Life", NULL);

  screen = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, 32, SDL_SWSURFACE);
  Uint32 bg_color = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
  Uint32 live_color = SDL_MapRGB(screen->format, 0x33, 0x33, 0x33);

  int board1[BOARD_SIZE];
  int board2[BOARD_SIZE];

  initialize_board(board1);
  memcpy(board2, board1, BOARD_SIZE * sizeof(int));

  Uint64 generation = 0;

  next_time = SDL_GetTicks() + TICK_INTERVAL;
  while(1) {
    int quit = 0;
    int* current;
    int* next;

    if (generation++ % 2) {
      current = board2;
      next = board1;
    } else {
      current = board1;
      next = board2;
    }

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          if (event.button.y > (ROWS * CELL_SIZE)) break;
          int tx = floor(event.button.x / CELL_SIZE);
          int ty = floor(event.button.y / CELL_SIZE);
          add_glider(current, tx, ty);
        }
        break;
      default: continue;
      }
    }

    if (quit) {
      break;
    }

    draw_board(screen, current, bg_color, live_color);
    SDL_Delay(time_left());
    next_time += TICK_INTERVAL;
    tick(current, next);
  }

  SDL_Quit();
  return 0;
}

void add_glider(int* board, int x, int y) {
  int x_minus = (x == 0) ? COLUMNS - 1 : x - 1;
  int y_minus = (y == 0) ? ROWS - 1 : y - 1;
  int x_plus = (x == COLUMNS - 1) ? 0 : x + 1;
  int y_plus = (y == ROWS - 1) ? 0 : y + 1;

  board[CELL(x, y_minus)] = 1;
  board[CELL(x, y_plus)] = 1;
  board[CELL(x_plus, y)] = 1;
  board[CELL(x_plus, y_plus)] = 1;
  board[CELL(x_minus, y_plus)] = 1;
}


void draw_line(SDL_Surface* screen,
               int start_x,
               int start_y,
               int end_x,
               int end_y,
               Uint32 color) {
  SDL_Rect r;
  int dx = end_x - start_x;
  int dy = end_y - start_y;

  if (dy == 0) {
    r.x = start_x;
    r.y = start_y;
    r.h = 1;
    r.w = dx;
  }

  if (dx == 0) {
    r.x = start_x;
    r.y = start_y;
    r.h = dy;
    r.w = 1;
  }

  SDL_FillRect(screen, &r, color);
  return;
}

void fill_cell(SDL_Surface* screen, int column, int row, Uint32 color) {
  assert(column < COLUMNS);
  assert(row < ROWS);

  SDL_Rect rect;
  rect.x = CELL_SIZE * column;
  rect.y = CELL_SIZE * row;
  rect.w = CELL_SIZE;
  rect.h = CELL_SIZE;

  SDL_FillRect(screen, &rect, color);
}

int count_neighbors(int* board, int x, int y) {
  int live_cells = 0;

  int x_minus = (x == 0) ? COLUMNS - 1 : x - 1;
  int y_minus = (y == 0) ? ROWS - 1 : y - 1;
  int x_plus = (x == COLUMNS - 1) ? 0 : x + 1;
  int y_plus = (y == ROWS - 1) ? 0 : y + 1;

  if (board[CELL(x_minus, y_minus)]) {
    ++live_cells;
  }

  if (board[CELL(x_minus, y)]) {
    ++live_cells;
  }

  if (board[CELL(x_minus, y_plus)]) {
    ++live_cells;
  }

  if (board[CELL(x, y_minus)]) {
    ++live_cells;
  }

  if (board[CELL(x, y_plus)]) {
    ++live_cells;
  }

  if (board[CELL(x_plus, y_minus)]) {
    ++live_cells;
  }

  if (board[CELL(x_plus, y)]) {
    ++live_cells;
  }

  if (board[CELL(x_plus, y_plus)]) {
    ++live_cells;
  }

  return live_cells;
}

void tick(int* current, int* next) {
  clear_board(next);
  int x,y;
  for (y = 0; y < ROWS; ++y) {
    for (x = 0; x < COLUMNS; ++x) {
      int is_live = current[CELL(x,y)];
      int n = count_neighbors(current, x, y);

      if (is_live && (n < 2  || n > 3)) {
        next[CELL(x,y)] = 0;
        continue;
      } else if (is_live) {
        next[CELL(x,y)] = 1;
        continue;
      }

      if (!is_live && n == 3) {
        next[CELL(x,y)] = 1;
        continue;
      }
    }
  }
}

void initialize_board(int* board) {
  int x,y;
  for (y = 0; y < ROWS; ++y) {
    for (x = 0; x < COLUMNS; ++x) {
      if (rand() % 4 == 0) {
        board[CELL(x,y)] = 1;
      } else {
        board[CELL(x,y)] = 0;
      }
    }
  }
}

void clear_board(int* board) {
  int i;
  for (i = 0; i < BOARD_SIZE; ++i) {
    board[i] = 0;
  }
}

void draw_board(SDL_Surface* screen, int* board, Uint32 bg_color, Uint32 cell_color) {
  SDL_FillRect(screen, &(screen->clip_rect), bg_color);

  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLUMNS; ++x) {
      if (board[CELL(x,y)]) {
        fill_cell(screen, x, y, cell_color);
      }
    }
  }

  draw_line(screen, 0, 0, SCR_WIDTH - 1, 0, bg_color);
  draw_line(screen, 0, 0, 0, SCR_HEIGHT - 1, bg_color);
  draw_line(screen, 0, SCR_HEIGHT - 1, SCR_WIDTH - 1, SCR_HEIGHT - 1, bg_color);
  draw_line(screen, SCR_WIDTH - 1, 0, SCR_WIDTH - 1, SCR_HEIGHT - 1, bg_color);

  for (int x = CELL_SIZE; x < SCR_WIDTH; x += CELL_SIZE) {
      draw_line(screen, x, 0, x, SCR_HEIGHT - 1, bg_color);
  }

  for (int y = CELL_SIZE; y < SCR_HEIGHT; y += CELL_SIZE) {
    draw_line(screen, 0, y, SCR_WIDTH - 1, y, bg_color);
  }

  SDL_Flip(screen);
}

Uint32 time_left(void){
  Uint32 now;
  now = SDL_GetTicks();
  if(next_time <= now) {
    return 0;
  } else {
    return next_time - now;
  }
}
