#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_SCORE 7       // total wrong guesses allowed
#define WORD_COUNT 1022

const int hanger_x = 0;
const int hanger_y = 3;
const int parts_off[][2] = {{10, 3}, {10, 4}, {9, 4}, {11, 4}, {10, 5}, {9, 6}, {11, 6}};
const char *parts_shape = "O|/\\|/\\";

typedef struct {
  char *current_word;
  char *display_word;     //contains _ for unknown locations
  size_t word_len;
  unsigned int score;

  char *guesses;          // past guesses
  unsigned int guesses_made;
  FILE *word_file;
} Game;

typedef enum {
  GAME_OVER,
  GAME_WON,
  GAME_RUNNING,
} GameState;

void clear_stdin() {
  int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void clear_console() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

int is_alphabet(char ch) {
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}

int str_contains(char *str, char c) {
  assert(str != 0);
  for(int i = 0; i < strlen(str); ++i)
    if(str[i] == c) return 1;
  return 0;
}

char char_lower(char ch) {
  return (ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A') : ch;
}

void str_lower(char *str){
  assert(str != 0);
  for(int i = 0; i < strlen(str); ++i)
    str[i] = char_lower(str[i]);
}

void read_random_word(FILE *f, char **dst) {
  unsigned int index = (unsigned int)(((float)rand()) * (WORD_COUNT) / RAND_MAX);
  unsigned int count = 0;

  fseek(f, 0, SEEK_SET);
  char ch;
  while (count < index) {
    ch = fgetc(f);
    if(ch == ',') count++;
  }
  if(index != 0) ch = fgetc(f);

  size_t word_len = 1;
  while ((ch = fgetc(f)) != ',') word_len++;

  if(*dst) free(*dst);
  *dst = (char*)calloc(word_len, sizeof(char));
  fseek(f, -word_len, SEEK_CUR);
  fread(*dst, sizeof(char), word_len - 1, f);
}

Game game_init(FILE *word_file) {
  Game game = { 0 };
  
  game.word_file = word_file;
  read_random_word(game.word_file, &game.current_word);
  game.word_len = strlen(game.current_word);
  game.display_word = (char*)calloc(game.word_len, sizeof(char));
  for(int i = 0; i < game.word_len; ++i)
    game.display_word[i] = '_';

  game.guesses = (char*)calloc(game.word_len + MAX_SCORE, sizeof(char));
  game.score = MAX_SCORE;
  game.guesses_made = 0;

  return game;
}

GameState game_update(Game *game, char in) {
  if(str_contains(game->guesses, in)) return GAME_RUNNING;
  game->guesses[game->guesses_made++] = in;
  
  int match = 0;
  for(int i = 0; i < game->word_len; ++i) {
    if(game->current_word[i] == char_lower(in)){
      match = 1;
      game->display_word[i] = char_lower(in);
    }
  }

  if(!match) {
    game->score--;
    if(game->score < 1) return GAME_OVER;
    return GAME_RUNNING;
  }

  if(strcmp(game->current_word, game->display_word) == 0) return GAME_WON;
  return GAME_RUNNING;
}

void game_destroy(Game *game) {
  free(game->current_word);
  free(game->display_word);
  free(game->guesses);
}

char get_input() {
  char ch = 0;
  printf("Enter your guess: ");
  scanf("%c", &ch);
  clear_stdin();

  while(!is_alphabet(ch)) {
    printf("Invalid input. Enter again: ");
    scanf("%c", &ch);
    clear_stdin();
  }
  
  return ch;
}

void draw(Game *game) {
  clear_console();
  printf("%s \t Score: %u \t Your guesses: ", game->display_word, game->score);
  for(int i = 0; i < game->guesses_made; ++i)
    printf("%c ", game->guesses[i]);
  printf("\033[%d;%dH", hanger_y, hanger_x);
  printf("---------+---------\n"
	 "||       |\n"
	 "||       |\n"
	 "||\n"
	 "||\n"
	 "||\n"
	 "||\n"
	 "||\n"
	 "||\n"
	 "||\n"
	 "||\n"
	 "||_________________\n\n\n\n");

  int parts_to_draw = MAX_SCORE - game->score;
  for (int i = 0; i < parts_to_draw; ++i) {
    printf("\033[%d;%dH", hanger_y + parts_off[i][1], hanger_x + parts_off[i][0]);
    printf("%c", parts_shape[i]);
  }
  printf("\033[%d;%dH", hanger_y + 15, 0);
}	 

int main() {
  srand(time(0));
  FILE *file = fopen("words.txt", "r");
  if(!file) {
    printf("Cannot find word file.\n");
    return 1;
  }

  char choice;
  do {
    Game game = game_init(file);
    GameState state = GAME_RUNNING;
    
    while(state == GAME_RUNNING) {
      draw(&game);
      char in = get_input();
      state = game_update(&game, in);
    }

    draw(&game);
    if(state == GAME_OVER) printf("You lost. The word was %s\n", game.current_word);
    else printf("You won!! Score: %u\n", game.score);

    printf("Play again (y or n)? ");
    scanf("%c", &choice);
    clear_stdin();
    game_destroy(&game);
  } while(char_lower(choice) == 'y');

  return 0;
}
