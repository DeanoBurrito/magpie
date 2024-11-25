#pragma once

#include <core/string.h>

enum mp_plugin_class
{
    mp_plugin_class_binman,
    mp_plugin_class_emit,
};

void mp_load_plugins();
void* mp_find_plugin_function(enum mp_plugin_class plug_class, struct mp_string plugin_name, struct mp_string func_name);

