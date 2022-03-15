#pragma once

#include "collections/hashset.h"
#include "collections/treeset.h"

#define USERNAMES_FILENAME "users.txt"

void run_console_app(hashset_t const *dictionary, treeset_t *usernames);
