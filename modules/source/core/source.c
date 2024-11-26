#include <core/source.h>
#include <core/queue.h>
#include <core/alloc.h>

#define MP_SOURCE_PEEK_DEFAULT_CHAR ' '

struct mp_source_segment
{
    LIST_ENTRY(mp_source_segment) list;
    size_t length;
    char* data;
    bool should_free;
};

struct mp_source_repo
{
    LIST_HEAD(, mp_source_segment) segments;
    size_t length;
    struct mp_string original_source;
};

struct mp_source_repo* mp_create_source_repo(struct mp_string filename)
{
    struct mp_source_repo* repo = mp_try_alloc(sizeof(*repo));
    if (NULL == repo)
        return NULL;

    repo->length = 0;

    repo->original_source = mp_host_open_file(filename.text); //TODO: what if length < null terminator?
    repo->length = repo->original_source.length;
    if (0 == repo->original_source.length)
    {
        mp_free(repo, sizeof(*repo));
        return NULL;
    }

    struct mp_source_segment* seg = mp_try_alloc(sizeof(*seg));
    if (NULL == seg)
    {
        mp_host_close_file(repo->original_source);
        mp_free(repo, sizeof(*repo));
        return NULL;
    }

    seg->data = (char*)repo->original_source.text;
    seg->length = repo->original_source.length;
    seg->should_free = false;
    LIST_INSERT_HEAD(&repo->segments, seg, list);

    return repo;
}

void mp_destroy_source_repo(struct mp_source_repo** repo)
{
    if (NULL == repo || NULL == *repo)
        return;

    while (!LIST_EMPTY(&(**repo).segments))
    {
        struct mp_source_segment* seg = LIST_FIRST(&(**repo).segments);
        LIST_REMOVE_HEAD(&(**repo).segments, list);
        
        if (seg->should_free)
            mp_free(seg->data, seg->length);
        mp_free(seg, sizeof(*seg));
    }

    mp_host_close_file((**repo).original_source);
    mp_free(*repo, sizeof(**repo));
    *repo = NULL;
}

void mp_source_insert(struct mp_source_repo* repo, size_t where, struct mp_string what)
{}

void mp_source_delete(struct mp_source_repo* repo, size_t where, size_t count)
{}

int mp_source_peek(struct mp_source_repo* repo, size_t index)
{
    if (NULL == repo)
        return MP_SOURCE_PEEK_EOF;
    if (index >= repo->length)
        return MP_SOURCE_PEEK_EOF;

    struct mp_source_segment* it = NULL;
    LIST_FOREACH(it, &repo->segments, list)
    {
        if (index >= it->length)
        {
            index -= it->length;
            continue;
        }

        return it->data[index];
    }

    return MP_SOURCE_PEEK_DEFAULT_CHAR;
}

void mp_source_read(struct mp_source_repo* repo, size_t offset, char* dest, size_t count)
{
    if (NULL == repo || NULL == dest)
        return;

    count = mp_min(count, repo->length - (offset + count));
    if (0 == count)
        return;
    
    size_t copied = 0;
    struct mp_source_segment* it = NULL;
    LIST_FOREACH(it, &repo->segments, list)
    {
        if (offset >= it->length)
        {
            offset -= it->length;
            continue;
        }

        const size_t copy_len = mp_min(count, it->length - offset);
        mp_memcpy(dest + copied, it->data + offset, copy_len);
        copied += copy_len;
        offset = 0;

        if (copied == count)
            return;
    }
}

struct mp_source_pos mp_source_pos_from_index(struct mp_source_repo* repo, size_t index)
{
}
