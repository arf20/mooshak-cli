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

    libmooshak.c: Mooshak API-ish interface library implementation

*/

#include "libmooshak.h"

#include <curl/curl.h>

#include <string.h>

struct mooshak_ctx_s {
    int init;
    const char *s_endpoint;
    CURL *curl;
    const char *lasterr;
    CURLcode laststat;
};

mooshak_ctx_t *
mooshak_init(const char *baseurl) {
    mooshak_ctx_t *ctx = malloc(sizeof(struct mooshak_ctx_s));
    memset(ctx, 0, sizeof(struct mooshak_ctx_s));

    ctx->curl = curl_easy_init();
    if (!ctx->curl) {
        ctx->lasterr = "Error curl_easy_init\n";
        return ctx;
    }

    curl_easy_setopt(ctx->curl, CURLOPT_URL, baseurl);
    CURLcode res = curl_easy_perform(ctx->curl);
    if (res != CURLE_OK) {
        ctx->lasterr = "Error performing discovery\n";
        ctx->laststat = res;
        return ctx;
    }

    

    return ctx;
}

void
mooshak_deinit(mooshak_ctx_t *ctx) {

}

const char *
mooshak_getlasterror_str(mooshak_ctx_t *ctx) {

}

const char **
get_contests(mooshak_ctx_t *ctx) {

}

