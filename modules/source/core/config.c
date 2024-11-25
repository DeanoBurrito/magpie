#include <core/config.h>
#include <core/alloc.h>
#include <core/queue.h>
#include <core/diagnostics.h>

struct mp_config_element
{
    LIST_ENTRY(mp_config_element) list;
    struct mp_string key;
    struct mp_string value;
};

struct mp_config_store
{
    struct mp_string backing_string; //local copy of the string the config elements refer to.
    LIST_HEAD(, mp_config_element) elements;
};

struct mp_config_store* mp_create_config_store()
{
    struct mp_config_store* store = mp_try_alloc(sizeof(struct mp_config_store));
    if (NULL == store)
        return NULL;

    mp_memset(store, 0, sizeof(*store));

    return store;
}

void mp_destroy_config_store(struct mp_config_store** store)
{
    if (NULL == store || NULL == *store)
        return;

    mp_free(*store, sizeof(**store));
    *store = NULL;
}

void mp_config_from_string(struct mp_config_store* store, struct mp_string str)
{
    if (NULL == store)
        return;
    if (store->backing_string.length != 0)
    {
        LOG_ERROR("config store already in use");
        return;
    }

    char* backing_str = mp_try_alloc(str.length + 1);
    if (NULL == backing_str)
    {
        LOG_ERROR("failed to allocate for local string copy");
        return;
    }

    mp_memcpy(backing_str, str.text, str.length + 1);
    store->backing_string.length = str.length;
    store->backing_string.text = backing_str;

    struct mp_config_element* elem = NULL;
    bool in_value = false;
    for (size_t i = 0; i < str.length; i++)
    {
        if (mp_isspace(str.text[i]))
        {
            in_value = false;
            elem = NULL;
            continue;
        }

        if (NULL != elem && '=' == str.text[i])
        {
            in_value = true;
            continue;
        }

        if (NULL == elem)
        {
            elem = mp_try_alloc(sizeof(*elem));
            if (NULL == elem)
                return;

            elem->value.length = 0;
            elem->value.text = NULL;
            elem->key.length = 0;
            elem->key.text = &str.text[i];
            LIST_INSERT_HEAD(&store->elements, elem, list);
        }

        if (in_value)
        {
            if (0 == elem->value.length)
                elem->value.text = &str.text[i];
            elem->value.length++;
        }
        else
            elem->key.length++;
    }
}

bool mp_get_config(struct mp_config_store* store, struct mp_string key, struct mp_string* value)
{
    if (NULL == store)
        return false;
    if (key.length == 0)
        return false;

    struct mp_config_element* it = NULL;
    LIST_FOREACH(it, &store->elements, list)
    {
        if (it->key.length != key.length)
            continue;

        if (0 != mp_memcmp(it->key.text, key.text, it->key.length))
            continue;

        *value = it->value;
        return true;
    }

    return false;
}
