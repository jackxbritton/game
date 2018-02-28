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

    array_init(&c->table, 4, sizeof(CatalogEntry));
}

void catalog_destroy(Catalog *c) {
    int i;
    assert(c != NULL);
    for (i = 0; i < c->table.count; i++) {
        CatalogEntry *entry = array_get(&c->table, i);
        inotify_rm_watch(c->fd, entry->wd);
    }
    array_destroy(&c->table);
    close(c->fd);
}

void catalog_add(Catalog *c, const char *filename, void (*callback)(), void *callback_arg) {

    assert(c != NULL);
    assert(callback != NULL);

    CatalogEntry entry;
    entry.wd = inotify_add_watch(c->fd, filename, IN_CLOSE_WRITE);
    if (entry.wd == -1) {
        DEBUG("inotify_add_watch failed for '%s'.", filename);
        return;
    }
    entry.callback     = callback;
    entry.callback_arg = callback_arg;

    array_add(&c->table, &entry);

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
        for (j = 0; j < c->table.count; j++) {

            CatalogEntry *entry = array_get(&c->table, j);
            if (event->wd != entry->wd) continue;

            // Found.
            entry->callback(entry->callback_arg);

            break;
        }

    }

}
