#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <inttypes.h>

#define MAX_DICT_SIZE 1024

typedef struct {
    char * version;
} GIFHeader;

typedef struct {
    size_t _size;
    uint16_t width;
    uint16_t height;

    // packed field
    uint8_t packed;
} GIFLogicalScreenDescriptor;

typedef struct {
    size_t _size;
    uint32_t* color;
} GIFColorTable;

typedef struct {
    size_t _size;
    uint8_t extension_introducer; // always 0x21
    uint8_t graphic_conrtol_label; // always 0xf9
    uint8_t byte_size;

    uint8_t packed;

    uint16_t delay_time;
    uint8_t transparent_color_index;
    uint8_t block_terminator; // always 0x00
} GIFGraphicsConrtolExtension;

typedef struct {
    size_t _size;
    uint8_t image_seperator; // always 0x2c
    uint16_t image_left;
    uint16_t image_top;
    uint16_t image_width;
    uint16_t image_height;

    //packed field
    uint8_t packed;
} GIFImageDescriptor;

typedef struct {
    uint8_t extension_introducer; //always 0x21
    uint8_t comment_extension_label; //always 0xfe
    void * subblocks;
    uint8_t block_terminator; //always 0x00
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

typedef struct {
    int code;
    unsigned width;
} LZWCode;

static LZWCode* lzw_encode(const int * base, unsigned n, const int *input, unsigned n_input, unsigned *n_output);
int gif_encode(GIFConfig * config, int fd);

int main() {


    GIFConfig config = (GIFConfig){
        
    };

    return 0;
}

int gif_encode(GIFConfig * config, int fd) {

    char bitmap[4][4] = {
        {0, 1, 0, 0},
        {1, 0, 1, 0},
        {0, 1, 0, 0},
        {1, 0, 1, 0},
    };

    // HEADER
    char header[] = {47, 49, 46, 38, 39, 61};
    write(fd, header, sizeof(header));

    // LGC
    int32_t h = 4;
    int32_t w = 4;

    GIFLogicalScreenDescriptor LGC = (GIFLogicalScreenDescriptor) {
        ._size = 7,
        .height = 4,
        .width = 4,
        .packed = 0x0101,
    };

    void * buf = calloc(1, LGC._size);

    memcpy(buf, &LGC.height, sizeof(LGC.height));
    memcpy(buf + sizeof(LGC.height), &LGC.width, sizeof(LGC.width));
    memcpy(buf + sizeof(LGC.height) + sizeof(LGC.width), &LGC.packed, sizeof(LGC.packed));
    uint8_t _k = 0x00;
    memcpy(buf + sizeof(LGC.height) + sizeof(LGC.width) + sizeof(LGC.packed), &_k, 1);

    write(fd, buf, LGC._size);
    free(buf);

    //GCT

    uint32_t * colors = calloc(4, sizeof(*colors));
    colors[0] = 0x00ffffff;
    colors[1] = 0x00000000;
    colors[2] = 0x0000ff00;
    colors[3] = 0x00ff0000;


    GIFColorTable GCT = (GIFColorTable){
        ._size = 4 * sizeof(*colors),
        .color = colors,
    };

    buf = calloc(1, GCT._size);
    memcpy(buf, GCT.color, GCT._size);

    write(fd, buf, GCT._size);

    free(buf);
    free(colors);

    //GCE

    GIFGraphicsConrtolExtension GCE = (GIFGraphicsConrtolExtension){
        ._size = 8,
        .extension_introducer = 0x21,
        .graphic_conrtol_label = 0xf9,
        .byte_size = 0x04,
        .packed = 0x00,
        .delay_time = 0x0000,
        .transparent_color_index = 0x00,
        .block_terminator = 0x00,
    };

    void * buf = calloc(1, GCE._size);
    off_t off = 0;

    memcpy(buf + off, &GCE.extension_introducer, sizeof(GCE.extension_introducer));
    off += sizeof(GCE.extension_introducer);
    memcpy(buf + off, &GCE.graphic_conrtol_label, sizeof(GCE.graphic_conrtol_label));
    off += sizeof(GCE.graphic_conrtol_label);
    memcpy(buf + off, &GCE.byte_size, sizeof(GCE.byte_size));
    off += sizeof(GCE.byte_size);
    memcpy(buf + off, &GCE.packed, sizeof(GCE.packed));
    off += sizeof(GCE.packed);
    memcpy(buf + off, &GCE.delay_time, sizeof(GCE.delay_time));
    off += sizeof(GCE.delay_time);
    memcpy(buf + off, &GCE.transparent_color_index, sizeof(GCE.transparent_color_index));
    off += sizeof(GCE.transparent_color_index);
    memcpy(buf + off, &GCE.block_terminator, sizeof(GCE.block_terminator));

    write(fd, buf, GCE._size);
    free(buf);

    //Image Descriptor

    GIFImageDescriptor ID = (GIFImageDescriptor){
        ._size = 11,
        .image_seperator = 0x2c, 
        .image_left = 0x0000,
        .image_top = 0x0000,
        .image_width = 0x000a,
        .image_height = 0x000a,
        .packed = 0x00,
    };

    void * buf = calloc(1, ID._size);
    off_t off = 0;

    memcpy(buf + off, &ID.image_seperator, sizeof(ID.image_seperator));
    off += sizeof(ID.image_seperator);
    memcpy(buf + off, &ID.image_left, sizeof(ID.image_left));
    off += sizeof(ID.image_left);
    memcpy(buf + off, &ID.image_top, sizeof(ID.image_top));
    off += sizeof(ID.image_top);
    memcpy(buf + off, &ID.image_width, sizeof(ID.image_width));
    off += sizeof(ID.image_width);
    memcpy(buf + off, &ID.image_height, sizeof(ID.image_height));
    off += sizof(ID.image_height);
    memcpy(buf + off, &ID.packed, sizeof(ID.packed));

    write(fd, buf, ID._size);
    free(buf);

    // image data

    GIFImageData image_data = (GIFImageData) {

    };
    //lzw_encode();

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

typedef struct {
    LZWCode *data;
    unsigned size;
    unsigned capacity;
} LZWOutput;

enum {
    LZW_OUTPUT_SIZE = 1<<16,
};

int lzw_emit(LZWOutput *output, int code, unsigned width) {
    if(output -> size >= output->capacity) {
        unsigned new_cap;
        if(output->capacity) {
            new_cap = output->capacity * 2;
        } else {
            new_cap = LZW_OUTPUT_SIZE;
        }

        LZWCode *tmp = realloc(
            output->data,
            new_cap * sizeof(*tmp)
        );

        if (!tmp) {
            return -1;
        }

        output->data = tmp;
        output->capacity = new_cap;
    }
    
    output->data[output->size].code = code;
    output->data[output->size].width = width;
    output->size++;

    return 0;
}

LZWCode* lzw_encode(const int * base, unsigned n, const int *input, unsigned n_input, unsigned *n_output) {
    *n_output = 0;

    LZWOutput output = {0};
    Dictionary dict = {0};

    for(unsigned i = 0; i < n; i++) {
        dictionary_add(&dict, -1, base[i]);
    }

    unsigned clear_code = n;
    unsigned end_code = n + 1;

    dictionary_add(&dict, -2, -2);
    dictionary_add(&dict, -2, -3);

    unsigned width = 1;
    while((1u << width) <= end_code) {
        width++;
    }

    unsigned init_width = width;

    if(lzw_emit(&output, clear_code, width)) {
        return NULL;
    }

    if (n_input == 0) {
        if (lzw_emit(&output, end_code, width) != 0) {
            free(output.data);
            return NULL;
        }

        *n_output = output.size;
        return output.data;
    }

    int prefix = dictionary_find(&dict, -1, input[0]);
    if (prefix < 0) {
         free(output.data);
            return NULL;
    }

    for(unsigned i = 1; i< n_input; i++) {
        int ch = input[i];

        int find = dictionary_find(&dict, prefix, ch);

        if (find != -1) {
            prefix = find;
            continue;
        } 

        if(lzw_emit(&output, prefix, width) != 0) {
            free(output.data);
            return NULL;
        }
        
        if(dict.size < MAX_DICT_SIZE) {
            dictionary_add(&dict, prefix, ch);

            if(dict.size > (1u << width) && width < 12) {
                width++;
            }
        } else {
            if (lzw_emit(&output, clear_code, width)) {
                free(output.data);
                return NULL;
            }

            dict.size = 0;

            for(unsigned j = 0; j < n; j++) {
                dictionary_add(&dict, -1, base[j]);
            }

            dictionary_add(&dict, -2, -2);
            dictionary_add(&dict, -2, -3);

            width = init_width;
        }

        prefix = dictionary_find(&dict, -1, ch);

        if (prefix < 0) {
            free(output.data);
            return NULL;
        }
    }

    if(lzw_emit(&output, prefix, width)) {
        free(output.data);
        return NULL;
    }

    if(lzw_emit(&output, end_code, width)) {
        free(output.data);
        return NULL;
    }

    *n_output = output.size;
    return output.data;
}