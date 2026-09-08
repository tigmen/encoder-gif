#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 1024
#define LZW_OUTPUT_SIZE 128

typedef struct {
    char * version;
} GIFHeader;

typedef struct {
    unsigned width;
    unsigned height;

    // packed field
    char global_color_table_flag; // 1 bit
    char color_resulution; // 3 bit
    char sort_flag; // 1 bit
    char global_color_table_size; // 3 bit
} GIFLogicalScreenDescriptor;

typedef struct {
    int* color;
} GIFColorTable;

typedef struct {
    char extension_introducer; // always 0x21
    char graphic_conrtol_label; // always 0xf9
    char byte_size;

    //packed field
    char reserved_for_future; // 3 bit
    char disposal_method; // 3 bit
    char user_input_flag; // 1 bit
    char transparent_color_flag; // 1 bit

    char transparent_color_index;
    char block_terminator; // always 0x00
} GIFGraphicsConrtolExtension;

typedef struct {
    char image_seperator; // always 0x2c
    short image_left;
    short image_top;
    short image_width;
    short image_height;

    //packed field
    char local_color_table_flag; // 1 bit
    char interlace_flag; // 1 bit
    char sort_flag; // 1 bit
    char reserved_for_future; // 2 bit
    char local_color_table_size; // 3 bit
} GIFImageDescriptor;

typedef struct {
    char extension_introducer; //always 0x21
    char comment_extension_label; //always 0xfe
    void * subblocks;
    char block_terminator; //always 0x00
} GIFImageData;

typedef struct {
    GIFHeader * header;
    GIFLogicalScreenDescriptor * screen_descriptor;
    GIFColorTable * global_color_table;
    GIFGraphicsConrtolExtension * graphics_control_extension;
    GIFImageDescriptor * image_desriptor;
    GIFColorTable * local_color_table;
    GIFImageData * image_data;
} GIFConfig;

int* lzw_encode(const int * base, unsigned n, const int *input, unsigned n_input, unsigned *n_output);

int main() {
    GIFConfig config;

    return 0;
}

int gif_encode(GIFConfig * config) {
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