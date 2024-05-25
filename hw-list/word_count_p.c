/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
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

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) {
  
  list_init(&wclist->lst);
  pthread_mutex_init(&wclist->lock, NULL);
  // // Have to do type cast here
  // wclist->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER; 
}

size_t len_words(word_count_list_t* wclist) {
  // Other threads maybe updating the list, so to prevent 
  size_t list_length = list_size(&wclist->lst);
  return list_length;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  struct list_elem* e;

  // List Iterator
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    // Find the wrapper struct
    word_count_t* wrap = list_entry(e, word_count_t, elem);
    if (strcmp(wrap -> word, word) == 0) {
      return wrap;
    }
  }

  return NULL;
}

word_count_t* add_word_with_count(word_count_list_t* wclist, char* word, int count) {
  word_count_t* foundStruct = find_word(wclist, word);
  if (foundStruct != NULL) {
    (foundStruct -> count)++;
    return foundStruct;
  }

  word_count_t* newWord = (word_count_t *) malloc(sizeof(word_count_t));
  newWord -> word = (char*) malloc(strlen(word) + 1);
  if (newWord -> word == NULL) {
    fprintf(stderr, "Malloc Failed!");
    return NULL;
  }
  strcpy(newWord -> word, word);
  newWord->count = 1;
  list_push_front(&wclist->lst, &newWord->elem);
  return newWord;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  pthread_mutex_lock(&wclist->lock);
  word_count_t* wc = add_word_with_count(wclist, word, 1);
  pthread_mutex_unlock(&wclist->lock);
  return wc;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {
  struct list_elem* e;

  // List Iterator
  for (e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
    // Find the wrapper struct
    word_count_t* wrap = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%i\t%s\n", wrap->count, wrap->word);
  }
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  bool (*less)(const word_count_t *, const word_count_t *) = aux;
  word_count_t* wrap1 = list_entry(ewc1, word_count_t, elem);
  word_count_t* wrap2 = list_entry(ewc2, word_count_t, elem);

  // Could also be (*less)(wrap1, wrap2)
  return less(wrap1, wrap2); 
}


void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  list_sort(&wclist->lst, less_list, less);
}
