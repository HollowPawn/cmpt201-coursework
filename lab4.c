#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXTRA_SIZE 256
#define BLOCK_SIZE 128
#define BUF_SIZE 128

// Header structure for each memory block
struct header {
  uint64_t size;
  struct header *next;
};

// Error handler
static void handle_error(const char *msg) {
  perror(msg);
  _exit(1);
}

// Print helper — uses stack buffer + write() (avoids malloc)
static void print_out(const char *format, const void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len;

  if (data_size == sizeof(uint64_t)) {
    len = snprintf(buf, BUF_SIZE, format, *(const uint64_t *)data);
  } else {
    len = snprintf(buf, BUF_SIZE, format, *(void *const *)data);
  }

  if (len < 0)
    handle_error("snprintf");

  if (write(STDOUT_FILENO, buf, (size_t)len) < 0)
    handle_error("write");
}

int main(void) {
  // Step 1: Increase heap size by 256 bytes
  void *start = sbrk(EXTRA_SIZE);
  if (start == (void *)-1)
    handle_error("sbrk");

  // Step 2: Create two equal-size blocks of 128 bytes
  struct header *first_block = (struct header *)start;
  struct header *second_block = (struct header *)((char *)start + BLOCK_SIZE);

  // Step 3: Initialize headers
  first_block->size = BLOCK_SIZE;
  first_block->next = NULL;

  second_block->size = BLOCK_SIZE;
  second_block->next = first_block;

  // Step 4: Initialize data sections
  memset((char *)first_block + sizeof(struct header), 0,
         BLOCK_SIZE - sizeof(struct header));
  memset((char *)second_block + sizeof(struct header), 1,
         BLOCK_SIZE - sizeof(struct header));

  // Step 5: Print header information
  print_out("first block:      %p\n", &first_block, sizeof(first_block));
  print_out("second block:     %p\n", &second_block, sizeof(second_block));

  print_out("first block size: %lu\n", &first_block->size,
            sizeof(first_block->size));
  print_out("first block next: %p\n", &first_block->next,
            sizeof(first_block->next));

  print_out("second block size: %lu\n", &second_block->size,
            sizeof(second_block->size));
  print_out("second block next: %p\n", &second_block->next,
            sizeof(second_block->next));

  // Step 6: Print block data
  unsigned char *first_data =
      (unsigned char *)first_block + sizeof(struct header);
  unsigned char *second_data =
      (unsigned char *)second_block + sizeof(struct header);

  for (int i = 0; i < (int)(BLOCK_SIZE - sizeof(struct header)); i++) {
    dprintf(STDOUT_FILENO, "%d\n", first_data[i]);
  }
  for (int i = 0; i < (int)(BLOCK_SIZE - sizeof(struct header)); i++) {
    dprintf(STDOUT_FILENO, "%d\n", second_data[i]);
  }

  return 0;
}
