#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#define TICK_INTERVAL 100
#define CELL_SIZE 6
#define ROWS 100
#define COLUMNS 200
#define BOARD_SIZE ROWS * COLUMNS
#define SCR_HEIGHT CELL_SIZE * ROWS + 1
#define SCR_WIDTH CELL_SIZE * COLUMNS + 1
#define CELL(x,y) (ROWS * (y) + (x))

void initialize_board(int* board);
void clear_board(int* board);
int count_neighbors(int* board, int x, int y);
void tick(int* current, int* next);
void add_glider(int* board, int x, int y);

Uint32 time_left(void);
void draw_board(SDL_Surface* screen, int* board, Uint32 bg_color, Uint32 cell_color);
void fill_cell(SDL_Surface* screen, int column, int row, Uint32 color);
void draw_line(SDL_Surface* screen, int start_x, int start_y,
                 int end_x, int end_y, Uint32 color);

#endif
