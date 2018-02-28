#include "misc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

char *load_entire_file(const char *filename) {

    assert(filename != NULL);

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        DEBUG("fopen failed for '%s'.", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);

    char *out = malloc(len + 1);
    if (out == NULL) {
        DEBUG("malloc failed for '%s', len=%lu.", filename, len);
        fclose(f);
        return NULL;
    }

    fread(out, len, 1, f);
    fclose(f);
    out[len] = '\0';

    return out;

}

char *copy_string(const char *str) {

    assert(str != NULL);

    char *out = strdup(str);
    if (out == NULL) {
        DEBUG("strdup failed for '%s'.", str);
        return NULL;
    }

    return out;
}
