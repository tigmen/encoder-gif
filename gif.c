#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 1024
#define LZW_OUTPUT_SIZE 128

int* lzw_encode(const int * base, unsigned n, const int *input, unsigned n_input, unsigned *n_output);

int main() {
    int base[] = {0, 1, 2, 3, 4, 5};
    int data[] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1};

    unsigned n;
    int * output = lzw_encode(base, 6, data, 15, &n);

    for(unsigned i = 0; i < n; i++) {
        printf("%d\n", output[i]);
    }

    free(output);

    return 0;
}

typedef struct {
    int prefix;
    int elem;
} dictEntry;

typedef struct {
    unsigned size;
    dictEntry entry[MAX_DICT_SIZE];
} Dictionary;

void dictionary_add(Dictionary * dict, int prefix, int elem) {
    if (dict->size < MAX_DICT_SIZE) {
        dict->entry[dict->size].prefix = prefix;
        dict->entry[dict->size].elem = elem;
        dict->size++;
    }
}

int dictionary_find(Dictionary * dict, int prefix, int elem) {
    for (unsigned i = 0; i < dict->size; i++) {
        if (dict->entry[i].prefix == prefix) {
            if(dict->entry[i].elem == elem) {
                return i;
            }
        }
    }
    return -1;
}

int* lzw_encode(const int * base, unsigned n, const int *input, unsigned n_input, unsigned *n_output) {
    unsigned cap_output = LZW_OUTPUT_SIZE;
    *n_output = 0;
    int* output = malloc(sizeof(int) * cap_output);
    if (!output) {
        return NULL;
    }

    Dictionary dict;
    dict.size = 0;

    for(unsigned i = 0; i < n; i++) {
        dictionary_add(&dict, -1, base[i]);
    }

    int prefix = dictionary_find(&dict, -1, input[0]);

    for(unsigned i = 1; i< n_input; i++) {
        int ch = input[i];

        int find = dictionary_find(&dict, prefix, ch);

        if (find != -1) {
            prefix = find;
            continue;
        } else {
            if (*n_output >= cap_output) {
                cap_output *= 2;
                int* tmp = realloc(output, sizeof(int) * cap_output);
                if (!tmp) {
                    free(output);
                    return NULL;
                }
                output = tmp;
            }
            output[*n_output] = prefix;
            (*n_output)++;

            dictionary_add(&dict, prefix, ch);
            prefix = dictionary_find(&dict, -1, ch);
        }
    }

    if (prefix != -1) {
        if (*n_output >= cap_output) {
            cap_output *= 2;
            int* tmp = realloc(output, sizeof(int) * cap_output);
            if (!tmp) {
                free(output);
                return NULL;
            }
            output = tmp;
        }
        output[*n_output] = prefix;
        (*n_output)++;
    }

    return output;
}