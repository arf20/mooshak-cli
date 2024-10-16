/*

    libmooshak: Mooshak API-ish interface libray
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

    html_parser.c: 

*/

#include "html_parser.h"

#include <string.h>

void
html_preprocess(char *html) {
    while (*html) {
        if (*html == '\n')
            *html = ' ';
        html++;
    }
}

char *
html_ingest_starttag(char *html, char *tagbuf, size_t tagbufsize) {
    *tagbuf = '\0';

    char *start = strchr(html, '<');
    if (start == NULL) return NULL;

    if (start[1] == '/')
        return strchr(start, '>') + 1;

    char *attr = strchr(start, ' ');

    char *end = strchr(start, '>');
    if (end == NULL) return NULL;

    
    int len;
    if (attr && attr < end)
        len = attr - start - 1;
    else
        len = end - start - 1;

    if (len >= tagbufsize - 1)
        len = end - start - 3;

    strncpy(tagbuf, start + 1, len);
    tagbuf[len] = '\0';

    return end + 1;
}

char *
html_ingest_contents(char *html, char *contbuf, size_t contbufsize) {
    *contbuf = '\0';

    char *end = strchr(html, '<');
    if (end == NULL) return NULL;

    int len = end - html;
    if (len + 1 >= contbufsize)
        len = contbufsize - 1;
    strncpy(contbuf, html, len);
    contbuf[len] = '\0';

    return html;
}
