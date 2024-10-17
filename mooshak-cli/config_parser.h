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

*/

#ifndef _CONFIG_PARSER_H
#define _CONFIG_PARSER_H

#include <stdio.h>

/* Config - from file */
typedef struct {
    char *baseurl;
    char *user;
    char *password;
    char *contest;
} config_t;

int config_read_parse(const char *path, config_t *cfg);
void config_free(const config_t *cfg);

#endif
