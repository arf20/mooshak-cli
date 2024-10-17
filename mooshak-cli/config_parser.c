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

    config_parser.c: Configuration reader parser

*/

#include "config_parser.h"

#include "utils.h"

#include <string.h>
#include <stdlib.h>

int
config_read_parse(const char *path, config_t *cfg) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char linebuf[1024], *sep;
    while (get_line(linebuf, 1024, f)) {
        if (*linebuf == '#') continue;
        sep = strchr(linebuf, '=');
        if (!sep) {
            fclose(f);
            return 1;
        }
        *sep = '\0';

        /* assume key start in char 0 */
        if (strcmp(linebuf, "baseurl") == 0) {
            cfg->baseurl = strdup(sep + 1);
        } else if (strcmp(linebuf, "user") == 0) {
            cfg->user = strdup(sep + 1);
        } else if (strcmp(linebuf, "password") == 0) {
            cfg->password = strdup(sep + 1);
        } else if (strcmp(linebuf, "contest") == 0) {
            cfg->contest = strdup(sep + 1);
        }
    }

    if (!(cfg->baseurl && cfg->user && cfg->password && cfg->contest)) {
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}

void
config_free(const config_t *cfg) {
    free(cfg->baseurl);
    free(cfg->user);
    free(cfg->password);
    free(cfg->contest);
}
