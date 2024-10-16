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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char *memory;
    size_t size;
} buff_t;

struct mooshak_ctx_s {
    int init;
    const char *endpoint;
    CURL *curl;
    const char *lasterr;
    CURLcode laststat;
    buff_t resbuff;
};



static size_t
mem_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    buff_t *mem = (buff_t*)userp;
    
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    
    return realsize;
}

char *
get_refresh_url(const char *str) {
    const char *url_beg = NULL, *url_end = NULL;
    if (!(url_beg = strstr(str, "URL=")))
        return NULL;

    url_beg += 4;

    if (!(url_end = strchr(url_beg, '"')))
        return NULL;

    int url_len = url_end - url_beg;

    char *url = malloc(url_len + 1);
    strncpy(url, url_beg, url_len);
    url[url_len] = '\0';
    return url;
}

mooshak_ctx_t *
mooshak_init(const char *baseurl) {
    mooshak_ctx_t *ctx = malloc(sizeof(struct mooshak_ctx_s));
    memset(ctx, 0, sizeof(struct mooshak_ctx_s));

    long http_code;

    ctx->curl = curl_easy_init();
    if (!ctx->curl) {
        ctx->lasterr = "Error curl_easy_init";
        return ctx;
    }

    /* ===== One time stuff */
    /* set buffer stuff */
    ctx->resbuff.memory = malloc(1); /* grown as needed */
    ctx->resbuff.size = 0;
    curl_easy_setopt(ctx->curl, CURLOPT_WRITEFUNCTION, mem_write_callback);
    curl_easy_setopt(ctx->curl, CURLOPT_WRITEDATA, (void*)&ctx->resbuff);

    /* set libmooshak useragent */
    curl_easy_setopt(ctx->curl, CURLOPT_USERAGENT, "libmooshak/0.0.1");


    /* ===== Service discovery request */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, baseurl);

    CURLcode res = curl_easy_perform(ctx->curl);

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", baseurl, http_code);
    #endif
    
    if (res != CURLE_OK) {
        ctx->lasterr = "Error performing service discovery";
        ctx->laststat = res;
        return ctx;
    }

    /*   extract refresh URL field */
    char *disc_url = get_refresh_url(ctx->resbuff.memory);
    if (!disc_url) {
        ctx->lasterr = "Error discovering service url";
    }

    #ifdef _DEBUG_
    printf("==discovered url: %s\n", disc_url);
    #endif


    /* ===== Endpoint discovery request */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, disc_url);

    res = curl_easy_perform(ctx->curl);

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", disc_url, http_code);
    #endif 

    if (res != CURLE_OK) {
        ctx->lasterr = "Error performing endpoint discovery\n";
        ctx->laststat = res;
        return ctx;
    }

    /* get endpoint (3xx redirect Location header)*/
    curl_easy_getinfo(ctx->curl, CURLINFO_REDIRECT_URL, &ctx->endpoint);

    #ifdef _DEBUG_
    printf("==endpoint: %s\n", ctx->endpoint);
    #endif

    free(disc_url);
    return ctx;
}

int
mooshak_isinit(const mooshak_ctx_t *ctx) {
    return ctx->init;
}

char **
mooshak_getcontest(mooshak_ctx_t *ctx) {
    /* ===== login page - contest discovery */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, ctx->endpoint);

    CURLcode res = curl_easy_perform(ctx->curl);

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", disc_url, http_code);
    #endif 

    if (res != CURLE_OK) {
        ctx->lasterr = "Error performing endpoint discovery\n";
        ctx->laststat = res;
        return ctx;
    }

}

void
mooshak_deinit(mooshak_ctx_t *ctx) {
    curl_easy_cleanup(ctx->curl);
    free(ctx);
    curl_global_cleanup();
}

const char *
mooshak_getlasterror(mooshak_ctx_t *ctx) {
    return ctx->lasterr;
}


