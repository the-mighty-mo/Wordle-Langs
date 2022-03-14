#include "players/players.h"
#include "players/database.h"

#include "collections/treeset.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int str_to_owned(char *str, void **value) {
    string_t *string = malloc(sizeof(string_t));
    *string = string_from(str);
    *value = string;
    return 0;
}

int main()
{
    player_info_t player = {0};
    player_info_from_file(&player, "player.txt");

    string_t player_stats = player_info_get_stats(&player);
    printf("%s\n", player_stats.buf);
    string_drop(&player_stats);

    player_info_write_to_file(&player, "player.txt");

    player_info_drop(&player);

    printf("\n");

    treeset_t treeset = treeset_new(string_type_info());
    string_t value = string_from("ben");
    treeset_insert(&treeset, &value);
    value = string_from("user");
    treeset_insert(&treeset, &value);
    value = string_from("trace");
    treeset_insert(&treeset, &value);
    value = string_from("insert");
    treeset_insert(&treeset, &value);
    value = string_from("crate");
    treeset_insert(&treeset, &value);
    value = string_from("funny");
    treeset_insert(&treeset, &value);
    value = string_from("fudge");
    treeset_insert(&treeset, &value);
    value = string_from("grace");
    treeset_insert(&treeset, &value);
    value = string_from("humorous");
    treeset_insert(&treeset, &value);
    value = string_from("lame");
    treeset_insert(&treeset, &value);
    value = string_from("pours");
    treeset_insert(&treeset, &value);

    string_t *elem = NULL;
    for (int i = 0; i < treeset.len; ++i) {
        elem = treeset_get_next(&treeset, elem);
        printf("%s\n", elem->buf);
    }

    treeset_drop(&treeset);
}
