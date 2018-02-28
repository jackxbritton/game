#ifndef CATALOG_H
#define CATALOG_H

#include <sys/inotify.h>
#include <unistd.h>

struct CatalogEntry {
    int wd;
    void (*callback)();
    void *callback_arg;
};
typedef struct CatalogEntry CatalogEntry;

struct Catalog {

    int fd;

    CatalogEntry *table;
    size_t table_allocated,
           table_count;

};
typedef struct Catalog Catalog;

void catalog_init(Catalog *catalog);
void catalog_destroy(Catalog *catalog);

void catalog_add(Catalog *catalog, const char *filename, void (*callback)(), void *callback_arg);
void catalog_service(Catalog *catalog);

#endif
