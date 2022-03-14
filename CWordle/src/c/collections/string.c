#include "collections/string.h"
#include "util.h"

#include <ctype.h>
#include <string.h>

#define DEFAULT_CAP 16

string_t string_new(void)
{
    return string_with_capacity(DEFAULT_CAP);
}

string_t string_with_capacity(size_t cap)
{
    string_t string = {
        0,
        cap + 1,
        calloc(cap + 1, sizeof(*string.buf))
    };
    return string;
}

string_t string_from(char const *str)
{
    size_t str_len = strlen(str);

    size_t buf_len = max(str_len, DEFAULT_CAP) + 1;
    char *buf = calloc(buf_len, sizeof(*buf));
    strcpy(buf, str);

    string_t string = {
        str_len,
        buf_len,
        buf
    };
    return string;
}

string_t string_clone(string_t const *string)
{
    if (string == NULL) {
        return string_with_capacity(0);
    }

    string_t str = {
        string->len,
        string->buf_sz,
        malloc(string->buf_sz * sizeof(*str.buf))
    };
    memcpy(str.buf, string->buf, str.buf_sz * sizeof(*str.buf));
    return str;
}

void string_drop(string_t *string)
{
    if (string == NULL) {
        return;
    }
    free(string->buf);
    memset(string, 0, sizeof(*string));
}

void string_reserve(string_t *string, size_t additional)
{
    if (string == NULL) {
        return;
    }

    if (string->len + additional >= string->buf_sz) {
        string->buf_sz = max(string->len + additional + 1, (string->buf_sz << 1) - 1);
        string->buf = realloc(string->buf, string->buf_sz * sizeof(*string->buf));
    }
}

int string_is_empty(string_t const *string)
{
    return string->len == 0;
}

void string_push_str(string_t *string, char const *str)
{
    if (string == NULL || str == NULL) {
        return;
    }

    size_t str_len = strlen(str);
    string_reserve(string, str_len);

    strcpy(string->buf + string->len, str);
    string->len += str_len;
}

void string_clear(string_t *string)
{
    string->len = 0;
    *string->buf = '\0';
}

void string_to_lowercase(string_t *string)
{
    if (string == NULL) {
        return;
    }

    char *str_ptr = string->buf;
    for (; *str_ptr != 0; ++str_ptr) {
        *str_ptr = tolower(*str_ptr);
    }
}

void string_to_uppercase(string_t *string)
{
    if (string == NULL) {
        return;
    }

    char *str_ptr = string->buf;
    for (; *str_ptr != 0; ++str_ptr) {
        *str_ptr = toupper(*str_ptr);
    }
}

void string_trim(string_t *string)
{
    string_trim_leading(string);
    string_trim_trailing(string);
}

void string_trim_leading(string_t *string)
{
    if (string == NULL) {
        return;
    }

    char *str_ptr = string->buf;
    for (; isspace(*str_ptr); ++str_ptr) {}
    size_t str_ptr_offset = str_ptr - string->buf;
    if (str_ptr_offset > 0) {
        memmove(string->buf, str_ptr, string->buf_sz - str_ptr_offset);
        string->len -= str_ptr_offset;
    }
}

void string_trim_trailing(string_t *string)
{
    if (string == NULL) {
        return;
    }

    char *str_ptr = string->buf + string->len - 1;
    for (; isspace(*str_ptr); --str_ptr) {}
    *(str_ptr + 1) = '\0';
    string->len = str_ptr - string->buf;
}

void string_trim_newline(string_t *string)
{
    if (string == NULL) {
        return;
    }

    char *str_ptr = string->buf + string->len - 1;
    for (; *str_ptr == '\n' || *str_ptr == '\r'; --str_ptr) {}
    *(str_ptr + 1) = '\0';
    string->len = str_ptr - string->buf;
}

static int string_compare(string_t const *string, string_t const *other)
{
    return strcmp(string->buf, other->buf);
}

static int string_hash(string_t const *string)
{
    size_t sum = 0;
    for (int i = 0; i < string->len; ++i) {
        sum += string->buf[i];
    }
    return sum;
}

static type_info_t const type_info = {
    sizeof(string_t),
    string_drop,
    string_compare,
    string_hash
};

type_info_t string_type_info(void)
{
    return type_info;
}
