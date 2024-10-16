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

mooshak_ctx_t *
mooshak_init(const char *baseurl) {
    mooshak_ctx_t *ctx = malloc(sizeof(struct mooshak_ctx_s));
    memset(ctx, 0, sizeof(struct mooshak_ctx_s));

    long http_code;

    ctx->curl = curl_easy_init();
    if (!ctx->curl) {
        ctx->lasterr = "Error curl_easy_init\n";
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
    printf("===GET %s %ld\n", baseurl, http_code);
    #endif
    
    if (res != CURLE_OK) {
        ctx->lasterr = "Error performing service discovery\n";
        ctx->laststat = res;
        return ctx;
    }

    /*   extract URL= field */
    const char *disc_url_beg = NULL, *disc_url_end = NULL;
    if (!(disc_url_beg = strstr(ctx->resbuff.memory, "URL="))) {
        ctx->lasterr = "Service discovery failed, URL= not found\n";
        return ctx;
    }

    disc_url_beg += 4;

    if (!(disc_url_end = strchr(disc_url_beg, '"'))) {
        ctx->lasterr = "Service discovery failed, URL= invalid\n";
        return ctx;
    }

    int disc_url_len = disc_url_end - disc_url_beg;

    char *disc_url = malloc(disc_url_len + 1);
    strncpy(disc_url, disc_url_beg, disc_url_len);

    #ifdef _DEBUG_
    printf("==discovered url: %s\n", disc_url);
    #endif


    /* ===== Endpoint discovery request */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, disc_url);

    res = curl_easy_perform(ctx->curl);

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s %ld\n", disc_url, http_code);
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


