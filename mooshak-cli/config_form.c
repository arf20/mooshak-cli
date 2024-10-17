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

    config_form.c: Configuration form and generator

*/

#include "config_form.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bsd/string.h>


#include "libmooshak.h"

#include "utils.h"


int
config_form(mooshak_ctx_t **ctx) {
    char inbuff[256], outbuff[1024];
    FILE *cfgfile;

    *inbuff = '\0'; *outbuff = '\0';

    cfgfile = fopen("mooshak.conf", "w");

    fprintf(cfgfile, "# mooshak-cli configuration file - generated\n");

    /* Endpoint URL */
    printf("Scheme [https(://)]: ");
    if (*get_line(inbuff, 1024, stdin) != '\0')
        strlcat(outbuff, inbuff, 1024);
    else
        strlcat(outbuff, "https", 1024);
    strlcat(outbuff, "://", 1024);

    printf("Hostname [mooshak.inf.um.es]: ");
    if (*get_line(inbuff, 1024, stdin) != '\0')
        strlcat(outbuff, inbuff, 1024);
    else
        strlcat(outbuff, "mooshak.inf.um.es", 1024);
    
    fprintf(cfgfile, "baseurl=%s\n", outbuff);


    *ctx = mooshak_init(outbuff);
    if (!mooshak_isinit(*ctx)) {
        fprintf(stderr, "%s\n", mooshak_getlasterror(*ctx));
        return -1;
    }

    /* Contest */
    char **contests = mooshak_getcontests(*ctx);
    
    printf("Contests:\n");
    for (int i = 0; contests[i] != NULL; i++)
        printf("  #%d: %s\n", i, contests[i]);
    printf("Contest #: ");
    get_line(inbuff, 1024, stdin);
    int contestidx = atoi(inbuff);
    fprintf(cfgfile, "contest=%s\n", contests[contestidx]);

    /* User */
    printf("User: ");
    get_line(inbuff, 1024, stdin);
    char *user = strdup(inbuff);
    fprintf(cfgfile, "user=%s\n", user);

    /* Password */
    printf("Password: ");
    get_line(inbuff, 1024, stdin);
    char *password = strdup(inbuff);
    fprintf(cfgfile, "password=%s\n", password);


    /* Validate */
    if (mooshak_login_contest(*ctx, user, password, contests[contestidx]) < 0) {
        fprintf(stderr, "Error performing login\n");
        return -1;
    }

    /* clean up */
    fclose(cfgfile);

    free(user);
    free(password);

    for (int i = 0; contests[i] != NULL; i++)
        free(contests[i]);
    free(contests);
    
    return 0;
}

