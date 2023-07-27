#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SCORE 5       // total guesses allowed + 1

typedef struct {
  char *mem;
  size_t len;
} Word;

typedef struct {
  Word current_word;
  unsigned int score;

  char guesses[5];       // past guesses
  unsigned int guesses_made;
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

int is_alphabet(char ch) {
  return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
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
  char guess = get_input();
  printf("Your guess is: %c\n", guess);

  return 0;
}