#include "players/players.h"
#include "players/database.h"

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

    database_entry_t entry;
    char str[] = "String Test: test";
    database_entry_from_line(&entry, str, str_to_owned, string_type_info());
    printf("%s\n%s\n", entry.name.buf, ((string_t *)entry.value)->buf);
    database_entry_drop(&entry);
}
