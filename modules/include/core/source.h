#pragma once

#include <core/string.h>

#define MP_SOURCE_PEEK_EOF -1

struct mp_source_repo;

struct mp_source_pos
{
};

struct mp_source_repo* mp_create_source_repo(struct mp_string filename);
void mp_destroy_source_repo(struct mp_source_repo** repo);
void mp_source_insert(struct mp_source_repo* repo, size_t where, struct mp_string what);
void mp_source_delete(struct mp_source_repo* repo, size_t where, size_t count);
int mp_source_peek(struct mp_source_repo* repo, size_t index);
void mp_source_read(struct mp_source_repo* repo, size_t offset, char* dest, size_t count);
struct mp_source_pos mp_source_pos_from_index(struct mp_source_repo* repo, size_t index);
