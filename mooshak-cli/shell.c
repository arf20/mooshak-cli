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
#include <stdlib.h>
#include <string.h>

int
tokenize(char *str, char **tok, int size) {
    int i = 0;
    while (str) {
        tok[i] = str;
        str = strchr(str, ' ');
        if (!str) {
            i++;
            break;
        }
        *str = '\0';
        str++;
        i++;
    }
    tok[i] = 0;
    return i;
}

void
free_submissions(mooshak_submission_t *subs, int n) {
    for (int i = 0; i < n; i++) {
        free(subs[i].time);
        free(subs[i].country);
        free(subs[i].team);
        free(subs[i].problem);
        free(subs[i].language);
        free(subs[i].result);
        free(subs[i].state);
    }
    free(subs);
}

int
shell(mooshak_ctx_t *ctx) {
    char cmd[1024];
    char *args[16];

    printf("Type `help` for a list of commands\n");

    while (1) {
        printf("mooshak> ");
        if (*get_line(cmd, 1024, stdin) == '\0') continue;

        tokenize(cmd, args, 16);

        if (strcmp(args[0], "h") == 0 || strcmp(args[0], "help") == 0) {
            printf(
                "  h|help:           print this message\n"
                "  l|listsub:        list submissions\n"
                "  q|quit:           logoff and quit\n"
                
            );
        } else if (strcmp(args[0], "l") == 0 || strcmp(args[0], "listsub") == 0) {
            int n_next = 0, n = 15, page = 0;
            for (char **tok = args + 1; *tok != NULL; tok++) {
                if (n_next) {
                    n = atoi(*tok);
                    n_next = 0;
                    continue;
                }

                if (strcmp(*tok, "-n") == 0) {
                    n_next = 1;
                    continue;
                }
                else n_next = 0;

                page = atoi(*tok);
            }

            if (mooshak_set_sublist_params(ctx, n, page) < 0) {
                fprintf(stderr, "Error setting listing parameters: %s\n",
                    mooshak_getlasterror(ctx));
            }

            mooshak_submission_t *subs = NULL;
            if ((n = mooshak_fetch_sublist(ctx, 1, &subs)) < 0) {
                fprintf(stderr, "Error fetching list: %s\n",
                    mooshak_getlasterror(ctx));
            }

            printf(
                "   sid time        country team                               "
                " problem language    attempt result              state\n"
                "--------------------------------------------------------------"
                "------------------------------------------------------\n");

            for (int i = 0; i < n; i++) {
                printf("%6d %11s %-8s%-36s%-8s%-12s%7d %-20s%-5s\n", subs[i].id,
                    subs[i].time, subs[i].country, subs[i].team,
                    subs[i].problem, subs[i].language, subs[i].attempt,
                    subs[i].result, subs[i].state);
            }

            free_submissions(subs, n);
        } else if (strcmp(args[0], "q") == 0 || strcmp(args[0], "quit") == 0) {
            return 0;
        }  else {
            printf("?Unrecognized command\n");
        }
    }

    return 0;
}
