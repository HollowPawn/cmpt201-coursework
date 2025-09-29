#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 5

// Function to print history buffer
void print_history(char *history[], int index, int count) {
  int start = (index - count + HISTORY_SIZE) % HISTORY_SIZE;
  for (int i = 0; i < count; i++) {
    int pos = (start + i) % HISTORY_SIZE;
    printf("%s", history[pos]); // line already has newline from getline
  }
}

int main(void) {
  char *history[HISTORY_SIZE] = {NULL}; // circular buffer
  int index = 0;                        // next insert position
  int count = 0;                        // number of valid entries

  char *line = NULL;
  size_t len = 0;

  while (1) {
    printf("Enter input: ");
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) {
      break; // EOF or Ctrl+D
    }

    // Free old memory if overwriting
    if (history[index] != NULL) {
      free(history[index]);
    }

    // Store new line
    history[index] = strdup(line);
    index = (index + 1) % HISTORY_SIZE;
    if (count < HISTORY_SIZE) {
      count++;
    }

    // Check for "print\n"
    if (strcmp(line, "print\n") == 0) {
      print_history(history, index, count);
    }
  }

  // Cleanup
  for (int i = 0; i < HISTORY_SIZE; i++) {
    free(history[i]);
  }
  free(line);

  return 0;
}
