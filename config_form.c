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
#include <string.h>
#include <bsd/string.h>


#include "libmooshak.h"

const char *
get_line(char *dst, size_t s) {
    const char *r = fgets(dst, 1024, stdin);
    int len = strlen(dst);
    if (len > 0)
        dst[len-1] = '\0'; /* strip \n */
    return r;
}

int
config_form(mooshak_ctx_t **ctx) {
    char inbuff[256], outbuff[1024];
    FILE *cfgfile;

    *inbuff = '\0'; *outbuff = '\0';

    cfgfile = fopen("mooshak.conf", "w");

    fprintf(cfgfile, "# mooshak-cli configuration file - generated\n");

    /* Endpoint URL */
    printf("Scheme [https(://)]: ");
    if (*get_line(inbuff, 1024) != '\0')
        strlcat(outbuff, inbuff, 1024);
    else
        strlcat(outbuff, "https", 1024);
    strlcat(outbuff, "://", 1024);

    printf("Hostname [mooshak.inf.um.es]: ");
    if (*get_line(inbuff, 1024) != '\0')
        strlcat(outbuff, inbuff, 1024);
    else
        strlcat(outbuff, "mooshak.inf.um.es", 1024);
    
    fprintf(cfgfile, "endpoint=%s\n", outbuff);


    


    /* Contest */
    printf("Contest: ");
    get_line(inbuff, 1024);
    fprintf(cfgfile, "contest=%s\n", inbuff);

    /* User */
    printf("User: ");
    get_line(inbuff, 1024);
    fprintf(cfgfile, "user=%s\n", inbuff);

    /* Contest */
    printf("Password: ");
    get_line(inbuff, 1024);
    fprintf(cfgfile, "password=%s\n", inbuff);

    fclose(cfgfile);
    
    return 0;
}

