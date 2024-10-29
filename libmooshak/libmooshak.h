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

/**
 * @file libmooshak.h
 * @brief libmooshak library header interface
 */

#ifndef _LIBMOOSHAK_H
#define _LIBMOOSHAK_H

#include <time.h>

/**
 * libmooshak opaque context type to which all calls refer to
 */
typedef struct mooshak_ctx_s mooshak_ctx_t;

/**
 * Submission list item type
 */
typedef struct {
    int  id;            /**< Sequential ID */
    char *time;         /**< Submission time (n)H:MM:SS */
    char *country;      /**< Team country */
    char *team;         /**< Team name */
    char *problem;      /**< Problem code */
    char *language;     /**< Submission language */
    int  attempt;       /**< Attempt number for problem by team */
    char *result;       /**< Test result */
    char *state;        /**< Submission state */
} mooshak_submission_t;

/**
 * @brief libmooshak initilization: must call before anything else
 * @param baseurl URL of mooshak server (schema + hostname + mooshak root)
 * @returns new libmooshak context (must be freed when unused with mooshak_deinit)
 */
mooshak_ctx_t *mooshak_init(const char *baseurl);

/**
 * @brief Check if libmooshak context is initialized
 * @param ctx libmooshak context
 * @returns non-zero on init, zero on no init
 */
int mooshak_isinit(const mooshak_ctx_t *ctx);

/**
 * @brief Free libmooshak context
 * @param ctx libmooshak context
 */
void mooshak_deinit(mooshak_ctx_t *ctx);

/**
 * @brief Get last libmooshak error string
 * @param ctx libmooshak context
 * @returns inmutable C-string describing the last lib error
 */
const char *mooshak_getlasterror(mooshak_ctx_t *ctx);

/**
 * @brief Get available contests from server, before login
 * @param ctx libmooshak context
 * @returns NULL-terminated array of NUL-terminated C-strings containing
 * the names of the active available contests
 */
char **mooshak_getcontests(mooshak_ctx_t *ctx);

/**
 * @brief Login into a contest server with credentials
 * @param ctx libmooshak context
 * @param user login username
 * @param password login password
 * @param contest login into a contest (get with mooshak_getcontests)
 * @returns non-zero on error
 */
int mooshak_login_contest(mooshak_ctx_t *ctx, const char *user,
    const char *password, const char *contest);

/**
 * @brief Logout of server, prevents hanging sessions in server
 * @param ctx libmooshak context
 * @returns non-zero on error
 */
int mooshak_logoff(mooshak_ctx_t *ctx);

/**
 * @brief Set submission listing parameters
 * @param ctx libmooshak context
 * @param n number of submissions to fetch
 * @param n page index to fetch (starts at 0)
 * @returns non-zero on error
 */
int mooshak_set_sublist_params(mooshak_ctx_t *ctx, int n, int page);

/**
 * @brief Fetch submission list
 * @param ctx libmooshak context
 * @param page listing page of n size (mooshak_set_sublist_params)
 * from newest to oldest
 */
int mooshak_fetch_sublist(mooshak_ctx_t *ctx, int page,
    mooshak_submission_t **list);

#endif /* _LIBMOOSHAK_H */
