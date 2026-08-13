#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* lzw_encode(int * base, unsigned n, int *input, unsigned n_input, unsigned *n_output);

int main() {
    int base[] = {0, 1, 2, 3, 4, 5};
    int data[] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1};

    unsigned n;
    int * output = lzw_encode(base, 6, data, 15, &n);

    for(unsigned i = 0; i < n; i++) {
        printf("%d\n", output[i]);
    }
    return 0;
}

typedef struct TableNode{
    unsigned n;
    unsigned elem_n;
    int *elem;
    struct TableNode *next;
} TableNode;

TableNode * table_Add(TableNode * root, int * elem, unsigned elem_n) {
    TableNode * new = malloc(sizeof(TableNode));
    new->elem = elem;
    new->elem_n = elem_n;
    new->n = 0;
    new->next = NULL;

    if (root) {
        for (;root->next;root = root->next) {
        }
        new->n = root->n + 1;

        root->next = new;
    }

    return new;
}

int * table_Get(TableNode * root, int n, unsigned *elem_n) {
    for(;root;root = root->next) {
        if (root->n == n) {
            *elem_n = root->elem_n;
            return root->elem;
        }
    }

    return NULL;
}

void table_Delete(TableNode * root) {
}

typedef struct CodeNode{
    unsigned n;
    int elem;
    struct CodeNode *next;
} CodeNode;

CodeNode * code_Add(CodeNode * root, int elem) {
    CodeNode * new = malloc(sizeof(CodeNode));
    new->elem = elem;
    new->n = 0;
    new->next = NULL;

    if (root) {
        for (;root->next;root = root->next) {
        }
        new->n = root->n + 1;

        root->next = new;
    } 

    return new;
}

int* code_Print(CodeNode* root, unsigned *n) {
    CodeNode * node = root;
    for (;node->next;node = node->next) {
    }
    *n = node->n + 1;

    int * out = malloc(*n * sizeof(int));
    for(int i = 0; root; root = root->next) {
        out[i++] = root->elem; 
    }

    return out;
}

int* lzw_encode(int * base, unsigned n, int *input, unsigned n_input, unsigned *n_output) {
    TableNode* table = table_Add(NULL, base, 1);
    for(unsigned i = 1; i < n; i++) {
        table_Add(table, base + i, 1);
    }

    CodeNode * code = code_Add(NULL, 0);

    CodeNode * pair = code_Add(NULL, input[0]);
    for (unsigned i = 1; i < n_input; i++) {
        int ch = input[i];
        code_Add(pair, ch);

        char equal;
        unsigned t = 0;
        for(TableNode* node = table; node; node = node->next) {
            unsigned n = node->elem_n;
            int * elem = node->elem;

            equal = 1;
            unsigned i = 0;
            CodeNode * this = pair;
            for(; this; this = this->next) {
                if (i == n) {
                    t = node->n;
                    equal = 0;
                    break;
                }

                if (elem[i] != this->elem) {
                    equal = 0;
                    break;
                } 
                i++;
            }

            this = pair;
            for(; this; this = this->next) {
            }

            if(this) {
                equal = 0;
            }

            if(equal) {
                break;
            }
        }

        if (equal) {
            continue;
        } else {
            code_Add(code, t);

            int * new = code_Print(pair, &n);
            table_Add(table, new, n);

            for(; pair->next; pair = pair->next) {
            }
            pair->n = 0;
        }
        
    }      

    return code_Print(code, n_output);
}