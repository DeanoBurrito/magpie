#pragma once

#include <core/string.h>
#include <stdbool.h>

struct mp_config_store;

struct mp_config_store* mp_create_config_store();
void mp_destroy_config_store(struct mp_config_store** store);
void mp_config_from_string(struct mp_config_store* store, struct mp_string str);
bool mp_get_config(struct mp_config_store* store, struct mp_string key, struct mp_string* value);
