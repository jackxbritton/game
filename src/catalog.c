#include "catalog.h"
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include "misc.h"

void catalog_init(Catalog *c) {

    assert(c != NULL);

    // Open file descriptor.
    c->fd = inotify_init1(IN_NONBLOCK);
    if (c->fd == -1) {
        DEBUG("inotify_init1 failed.");
        return;
    }

    // Allocate table.
    c->table_allocated = 4;
    c->table = malloc(c->table_allocated * sizeof(CatalogEntry));
    if (c->table == NULL) {
        close(c->fd);
        DEBUG("malloc failed.");
        return;
    }
    c->table_count = 0;
}

void catalog_destroy(Catalog *c) {
    int i;
    assert(c != NULL);
    for (i = 0; i < c->table_count; i++) {
        inotify_rm_watch(c->fd, c->table[i].wd);
    }
    free(c->table);
    close(c->fd);
}

void catalog_add(Catalog *c, const char *filename, void (*callback)(), void *callback_arg) {

    assert(c != NULL);
    assert(callback != NULL);

    // Resize array?
    if (c->table_count == c->table_allocated) {
        c->table_allocated *= 2;
        errno = 0;
        c->table = realloc(c->table, c->table_allocated*sizeof(CatalogEntry));
        if (errno == ENOMEM) {
            DEBUG("realloc failed.");
            return;
        }
    }

    CatalogEntry *entry = &c->table[c->table_count];
    c->table_count++;

    entry->wd = inotify_add_watch(c->fd, filename, IN_CLOSE_WRITE);
    if (entry->wd == -1) {
        DEBUG("inotify_add_watch failed for '%s'.", filename);
        return;
    }

    entry->callback     = callback;
    entry->callback_arg = callback_arg;

}

void catalog_service(Catalog *c) {

    char buffer[256 * sizeof(struct inotify_event)];
    ssize_t i, len;
    int j;
    struct inotify_event *event;

    assert(c != NULL);

    len = read(c->fd, buffer, 256 * sizeof(struct inotify_event));
    if (len == -1 && errno != EAGAIN) {
        DEBUG("read failed.");
        return;
    }

    for (i = 0; i < len; i += sizeof(struct inotify_event) + event->len) {

        event = (struct inotify_event *) &buffer[i];

        // Find the entry.
        for (j = 0; j < c->table_count; j++) {
            if (c->table[j].wd != event->wd) continue;

            // Found.
            c->table[j].callback(c->table[j].callback_arg);

            break;
        }

    }

}
