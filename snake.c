#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<SDL.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

#define CELL_SIZE 20
#define LINE_WIDTH 1
#define LINE_HEIGHT 1
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define COLUMNS SCREEN_WIDTH / CELL_SIZE

#define COLOR_BLACK 0xff000000
#define COLOR_WHITE 0xffffffff
#define COLOR_GRAY 0x1f1f1f1f
#define COLOR_RED 0x00ff0000
#define COLOR_GREEN 0x0000ff00

typedef struct SnakeElement SnakeElement; 
struct SnakeElement *tail = NULL;

struct SnakeElement {
  int x, y;
  bool head;
  SnakeElement *pnext;
};

typedef struct {
  int x, y;
} Apple;

// * Only one value should be non zero
typedef struct {
  int dx, dy;
} Direction;

SnakeElement *createNode() {
  struct SnakeElement *temp;
  temp = (SnakeElement *)malloc(sizeof(struct SnakeElement));
  temp->pnext = NULL;                                       
  return temp;
}

void draw_grid(SDL_Surface *surface) {

  SDL_Rect row = (SDL_Rect){0, 0, SCREEN_WIDTH, LINE_HEIGHT};
  for(int i = 0; i < ROWS; ++i) {
    row.y = i * CELL_SIZE;
    SDL_FillRect(surface, &row, COLOR_GRAY);
  }

  SDL_Rect column = (SDL_Rect){0, 0, LINE_WIDTH, SCREEN_HEIGHT};
  for(int i = 0; i < COLUMNS; ++i) {
    column.x = i * CELL_SIZE;
    SDL_FillRect(surface, &column, COLOR_GRAY);
  }
}

void fill_cell(SDL_Surface *surface, int x, int y, Uint32 color) {
  SDL_Rect rect = (SDL_Rect){
      x * CELL_SIZE,
      y * CELL_SIZE,
      CELL_SIZE, CELL_SIZE};
  SDL_FillRect(surface, &rect, color);
}

void draw_snake(SDL_Surface *surface, SnakeElement *snake) {
  if (snake == NULL)
    return;
  // printf("S = %p\n", snake);
  // printf("x = %d, y = %d\n", snake->x, snake->y);

  SDL_Rect snake_pos = (SDL_Rect){
      snake->x * CELL_SIZE,
      snake->y * CELL_SIZE,
      CELL_SIZE, CELL_SIZE};

  Uint32 color = COLOR_WHITE;
  if(snake->head) {
    color = COLOR_GREEN;
  }
  SDL_FillRect(surface, &snake_pos, color);
  draw_snake(surface, snake->pnext);
}

void move_snake(SnakeElement *snake, Direction *direction) {
  // printf("%p\n", snake);
  if (direction->dx == 0 && direction->dy == 0)
    return;
  int x_new = 0, y_new = 0;
  SnakeElement *temp = snake;
  while(snake != NULL) {
    if(snake->head) {
      // * Only move the head
      snake->x += direction->dx;
      snake->y += direction->dy;

      x_new = snake->x, y_new = snake->y;
     
      // * Check if snake bit itself
      while(temp != NULL) {
        if (!temp->head && temp->x == x_new && temp->y == y_new) {
          printf("Snake Bit Itself\n");
          exit(1);
        }
        temp = temp->pnext;
      } 

      break;
    } else {
      // * Copy next node positions to current node
      snake->x = snake->pnext->x;
      snake->y = snake->pnext->y;
      snake = snake->pnext;
    }
  }
}

void draw_apple(SDL_Surface *surface, const Apple *apple) {
  fill_cell(surface, apple->x, apple->y, COLOR_RED);
}

void reset_apple(SnakeElement snake, Apple *apple) {
  // printf("Snake = %p \n", &snake);

  // * New positions for apple
  apple->x = (int)(SCREEN_WIDTH * ((double)rand() / RAND_MAX)) / CELL_SIZE;
  apple->y = (int)(SCREEN_HEIGHT * ((double)rand() / RAND_MAX)) / CELL_SIZE;

  // do {
  //   if(snake.x == apple->x && snake.y == apple->y) {
  //     reset_apple(snake, apple);
  //     break;
  //   } else {
  //     if(snake.pnext != NULL) {
  //       SnakeElement next = {
  //           .x = snake.x,
  //           .y = snake.y,
  //           .pnext = snake.pnext,
  //       };
  //       snake = next;
  //     }
  //   }
  // }
  // while(snake.pnext != NULL);
  // printf("New Apple Pos: %d %d\n", apple->x, apple->y);
}

int main() {
  printf("Hello Snake\n");

  SDL_Init(SDL_INIT_VIDEO);

  // * Create a window
  SDL_Window *window = SDL_CreateWindow(
      "Classic Snake",
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      SCREEN_WIDTH,
      SCREEN_HEIGHT,
      0);

  SDL_Surface *surface = SDL_GetWindowSurface(window);

  // * Initial Snake
  SnakeElement snake = {
    .x = 5,
    .y = 5,
    .head = true,
    .pnext = NULL
  };

  tail = &snake;

  // * Initial Direction
  Direction direction = {0, 0};
  // * Initial Apple
  Apple apple = {2, 3};

  SDL_Rect override_rect = (SDL_Rect){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

  // * Poll for events
  bool quit = false;
  while(!quit) {
  
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN: {
        direction = (Direction){0, 0};
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
          break;
        case SDLK_RIGHT: {
          direction.dx = 1;
        } break;
        case SDLK_LEFT: {
          direction.dx = -1;
        } break;
        case SDLK_UP: {
          direction.dy = -1;
        } break;
        case SDLK_DOWN: {
          direction.dy = 1;
        } break;
        case SDLK_a: {
          // * Add new block to tail of snake
          SnakeElement *new_tail = createNode();
          new_tail->x = tail->x;
          new_tail->y = tail->y;
          new_tail->head = false;
          new_tail->pnext = tail;

          tail = new_tail;

          SnakeElement *temp = tail;
          while(temp != NULL) {
            printf("%p %d -> ", (void *)temp, temp->head);
            temp = temp->pnext;
          }
          printf("\n--------------\n");
        } break;
        default:
          break;
        }

      } break;
      default:
        break;
      }
    }

    SDL_FillRect(surface, &override_rect, COLOR_BLACK);
    move_snake(tail, &direction);

    // * Snake Ate apple
    // printf("Snake = x: %d, y: %d\n", snake.x, snake.y);
    // printf("Apple = x: %d, y: %d\n", apple.x, apple.y);
    // if(snake.x == apple.x && snake.y == apple.y) {
    //   printf("Cur Tail: %p, next: %p\n", (void *)tail, (void *)tail->pnext);
    //   // SnakeElement *cur_tail = tail;
    //   // printf("cur_tail: %p\n", cur_tail);
    //   SnakeElement new_tail = {
    //       .x = tail->x,
    //       .y = tail->y,
    //       .pnext = tail};

    //   printf("new_tail: %p, new_next_tail: %p\n", (void *)&new_tail, (void *)new_tail.pnext);
    //   printf("----------------------\n");
    //   tail = &new_tail;
    //   reset_apple(*tail, &apple);
    // }
    draw_apple(surface, &apple);

    draw_snake(surface, tail);
    draw_grid(surface);

    SDL_UpdateWindowSurface(window);
    SDL_Delay(200);


  }
}

