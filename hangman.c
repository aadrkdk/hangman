#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_SCORE 5 // total wrong guesses allowed
#define WORD_COUNT 1022

typedef struct {
  char *current_word;
  char *display_word;     //contains _ for unknown locations
  size_t word_len;
  unsigned int score;

  char *guesses;       // past guesses
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

  char ch;
  while (count < index) {
    ch = fgetc(f);
    if(ch == ',') count++;
  }
  ch = fgetc(f);

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
  game.display_word = (char*)malloc(game.word_len);
  for(int i = 0; i < game.word_len; ++i)
    game.display_word[i] = '_';

  game.guesses = (char*)calloc(game.word_len + MAX_SCORE, sizeof(char));
  game.score = MAX_SCORE;
  game.guesses_made = 0;

  return game;
}

GameState game_update(Game *game, char in) {
  if(str_contains(game->guesses, in)) {
    printf("You have already guessed this.\n");
    return GAME_RUNNING;
  }
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

int main() {
  srand(time(0));
  FILE *file = fopen("words.txt", "r");
  if(!file) {
    printf("Cannot find word file.\n");
    return 1;
  }

  char *word = 0;
  read_random_word(file, &word);
  printf("%s\n", word);

  Game game = game_init(file);
  GameState state = GAME_RUNNING;

  while(state == GAME_RUNNING) {
    clear_console();
    printf("%s \t Score: %u\n", game.display_word, game.score);
    char in = get_input();
    state = game_update(&game, in);
  }

  clear_console();
  if(state == GAME_OVER) printf("You lost. The word was %s\n", game.current_word);
  else printf("You won!! Score: %u\n", game.score);

  return 0;
}