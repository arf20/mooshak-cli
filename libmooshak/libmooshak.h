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

#ifndef _LIBMOOSHAK_H
#define _LIBMOOSHAK_H

/* Opaque context object */
typedef struct mooshak_ctx_s mooshak_ctx_t;

/* libmooshak initilization - must call before anything else */
mooshak_ctx_t *mooshak_init(const char *baseurl);

int mooshak_isinit(const mooshak_ctx_t *ctx);

void mooshak_deinit(mooshak_ctx_t *ctx);

const char *mooshak_getlasterror(mooshak_ctx_t *ctx);

/* before login */
char **mooshak_getcontests(mooshak_ctx_t *ctx);

int mooshak_login_contest(mooshak_ctx_t *ctx, const char *user,
    const char *password, char *contest);


#endif /* _LIBMOOSHAK_H */
