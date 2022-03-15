#pragma once

#include <stdio.h>
#include <string.h>

#include "type_info.h"

typedef struct _string_t {
    size_t len;
    size_t buf_sz;
    char *buf;
} string_t;

string_t string_new(void);
string_t string_with_capacity(size_t cap);
string_t string_from(char const *str);
string_t string_clone(string_t const *string);
void string_drop(string_t *string);
void string_reserve(string_t *string, size_t additional);

int string_is_empty(string_t const *string);
void string_push_str(string_t *string, char const *str);
void string_clear(string_t *string);

void string_to_lowercase(string_t *string);
void string_to_uppercase(string_t *string);
void string_trim(string_t *string);
void string_trim_leading(string_t *string);
void string_trim_trailing(string_t *string);
void string_trim_newline(string_t *string);

int file_read_to_string(string_t *string, FILE *file);
int file_read_line_to_string(string_t *file_contents, FILE *file);

type_info_t string_type_info(void);
