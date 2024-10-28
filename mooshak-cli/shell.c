/*

    mooshak-cli: A command-line interface for Mooshak
    Copyright (C) 2024 Angel Ruiz Fernandez <a.ruizfernandez@um.es>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    shell.c: Command shell & interpreter

*/

#include "shell.h"

#include "libmooshak.h"

#include "utils.h"

#include <stdio.h>
#include <string.h>

int
shell(mooshak_ctx_t *ctx) {
    char cmd[1024];

    printf("Type `help` for a list of commands\n");

    while (1) {
        printf("mooshak> ");
        get_line(cmd, 1024, stdin);

        if (strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0) {
            printf(
                "  h|help:           print this message\n"
                "  l|listsub:        list submissions\n"
                "  q|quit:           logoff and quit\n"
                
            );
        } else if (strcmp(cmd, "l") == 0 || strcmp(cmd, "listsub") == 0) {
            mooshak_submission_t *subs = NULL;
            int n = 0;
            if ((n = mooshak_fetch_sublist(ctx, 1, &subs)) < 0) {
                fprintf(stderr, "%s\n", mooshak_getlasterror(ctx));
            }

            printf(
                "sid\ttime\tcountry\tteam\tproblem\tlanguage\tattempt\tresult\tstate\n"
                "---\t----\t-------\t----\t-------\t--------\t-------\t------\t-----\n");
            for (int i = 0; i < n; i++) {
                printf("%d\t%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n", subs[i].id,
                    subs[i].time, subs[i].country, subs[i].team,
                    subs[i].problem, subs[i].language, subs[i].attempt,
                    subs[i].result, subs[i].state);
            }
        } else if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
            return 0;
        }  else {
            printf("?Unrecognized command\n");
        }
    }

    return 0;
}
