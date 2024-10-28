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

*/

#ifndef _HTML_PARSER_H
#define _HTML_PARSER_H

#include <stddef.h>

void html_preprocess(char *html);
char *html_ingest_starttag(char *html, char *tagbuf, size_t tagbufsize);
char *html_skip_whole_tag(char *html);
char *html_ingest_attribute(char *html, char *keybuf, size_t keybufsize,
    char *valbuf, size_t valbufsize);
char *html_ingest_contents(char *html, char *contbuf, size_t contbufsize);
char *html_ingest_contents_toend(char *html, char *contbuf, char *tag,
    size_t contbufsize);

#endif /* _HTML_PARSER_H */

