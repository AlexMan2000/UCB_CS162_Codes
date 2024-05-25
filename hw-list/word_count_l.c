/*
 * Implementation of the word_count interface using Pintos lists.
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


/* Since in the makefile, we set up a compiler flag -DPINTOS_LIST, so
*  1. word_count_list_t would be struct list defined in list.h
*  2. word_count_t would depend on list_elem defined in list.h 
*/

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_l.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) {
/* Calls the list_init in list.c 
 * since word_count_list_t here is just struct list
 */
  list_init(wclist);   
}

size_t len_words(word_count_list_t* wclist) {
  return list_size(wclist);
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  struct list_elem* e;

  // List Iterator
  for (e = list_begin(wclist); e != list_end(wclist); e = list_next(e)) {
    // Find the wrapper struct
    word_count_t* wrap = list_entry(e, word_count_t, elem);
    if (strcmp(wrap -> word, word) == 0) {
      return wrap;
    }
  }

  return NULL;
}


word_count_t* add_word(word_count_list_t* wclist, char* word) {

  word_count_t* foundStruct = find_word(wclist, word);
  if (foundStruct != NULL) {
    foundStruct -> count++;
    return foundStruct;
  }

  // If we allocate the new nodes on the stack, it will be freed after the functione exits
  // So we have to allocate the newNode on the heap.(Very important)
  word_count_t* newWord = (word_count_t *) malloc(sizeof(word_count_t));
  newWord -> word = (char *) malloc(strlen(word) + 1);
  if (newWord -> word == NULL) {
    fprintf(stderr, "Malloc Failed!");
    return NULL;
  }
  strcpy(newWord->word, word); // We don't need to terminate assuming that word is '\0' terminated
  newWord->count = 1;
  list_push_front(wclist, &newWord->elem);
  return newWord;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {

  struct list_elem* e;

  // List Iterator
  for (e = list_begin(wclist); e != list_end(wclist); e = list_next(e)) {
    // Find the wrapper struct
    word_count_t* wrap = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%i\t%s\n", wrap->count, wrap->word);
  }
}

// less_list is used to compare two list_elem, but in order to compare, we have to
// use less() function, which compares between two word_count_t(which contains list_elem as members)
static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  
  // The aux field is just the function pointer passed in by the wordcount_sort()
  // which is defined in word_helper.h

  // Here we cast void* to get the function pointer
  bool (*less) (const word_count_t*, const word_count_t*) = aux;
  word_count_t* wrap1 = list_entry(ewc1, word_count_t, elem);
  word_count_t* wrap2 = list_entry(ewc2, word_count_t, elem);

  // Could also be (*less)(wrap1, wrap2)
  return less(wrap1, wrap2); 
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  list_sort(wclist, less_list, less);
}
