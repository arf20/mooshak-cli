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

    main.c: Program entry point

*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "libmooshak.h"
#include "config_form.h"
#include "config_parser.h"
#include "shell.h"


void
usage(const char *self) {
    printf("usage: %s [options]\n\n  "
        "-c:   Interactive config generator\n", self);
}

int
main(int argc, char **argv) {
    printf("moonshak-cli Moonshak Command Line Interface 0.0.1 by arf20\n"
        "Copyright (C) 2024  Angel Ruiz Fernandez\n"
        "This program comes with ABSOLUTELY NO WARRANTY;\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under certain conditions; <http://gnu.org/licenses/gpl.html>\n\n");

    mooshak_ctx_t *ctx = NULL;
    config_t cfg = { 0 };

    if (argc == 1) {
        int r = config_read_parse("mooshak.conf", &cfg);
        if (r < 0) {
            fprintf(stderr, "Cannot read configuration: %s\n", strerror(errno));
            return 1;
        } else if (r > 0) {
            fprintf(stderr, "Invalid configuration\n");
            return 1;
        }

        config_free(&cfg);
    } else if (argc == 2 && strcmp(argv[1], "-c") == 0) {
        config_form(&ctx);
        shell(ctx);
    } else {
        usage(argv[0]);
        return 1;
    }

    mooshak_deinit(ctx);
    
    return 0;
}
