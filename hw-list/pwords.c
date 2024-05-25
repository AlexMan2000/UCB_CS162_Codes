/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"


// Shared resources on the static memory
word_count_list_t word_counts;


// struct to hold thread arguments
typedef struct targs {
  long threadid;
  char* infile;
} thread_args_t;



// typedef struct targs {
//   long threadid;
//   char* infile;
//   word_count_list_t* word_count_list;
// } thread_args_t;


void* threadfun(void* args) {
  thread_args_t* t_args = (thread_args_t*) args;
  char* filename = t_args -> infile;
  FILE* infile = fopen(filename, "r");
  
  if (infile == NULL) {
      fprintf(stderr, "Error occurred during file opening!");
      fclose(infile);
      return NULL;
  }

  // printf("Thread %lx", t_args->threadid);
  count_words(&word_counts, infile);
  fclose(infile);
  return NULL;
}



/*
 * main - handle command line, spawning one thread per file.
 */
int main(int argc, char* argv[]) {
  /* Create the empty data structure. */
  init_words(&word_counts);
  
  int rc; // Detect error when creating threads
  long t;

  char* filename = NULL;

  if (argc <= 1) {
    /* Process stdin in a single thread. */
    count_words(&word_counts, stdin);
  } else {
    pthread_t threads[argc - 1];
    for (t = 1; t < argc; t++ ) {
      filename = argv[t];
      thread_args_t t_args = {t, filename};
      rc = pthread_create(&threads[t - 1], NULL, threadfun, (void *) &t_args);
      if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }

      for (t = 1; t < argc; t++) {
        pthread_join(threads[t - 1], NULL);
      }
    }
  }
  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  return 0;
}
