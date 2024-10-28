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

#include "html_parser.h"


typedef struct {
    char *memory;
    size_t size;
    size_t off;
} buff_t;


struct mooshak_ctx_s {
    int init;           /* Init success => nonzero */
    char *endpoint;     /* Discovered endpoint with the numbery */
    CURL *curl;         /* CURL context */
    char *lasterr;      /* Last lib error str */
    CURLcode laststat;  /* Last CURL return val */
    buff_t resbuff;     /* Response buffer */
};



static size_t
mem_write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    buff_t *mem = (buff_t*)userp;
    
    if (mem->off + realsize >= mem->size) {
        mem->size += realsize + 1;
        mem->memory = realloc(mem->memory, mem->size);
    }
        
    if (!mem->memory) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(mem->memory + mem->off, contents, realsize);
    mem->off += realsize;
    mem->memory[mem->off] = 0;
    
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

char *
append_args(char *dst, const char *src) {
    size_t dstlen = strlen(dst);
    const char *args = strchr(src, '?');
    size_t argslen = strlen(args);
    dst = realloc(dst, dstlen + argslen + 1);
    strncpy(dst + dstlen, args, argslen);
    dst[dstlen + argslen] = '\0';
    return dst;
}

void
rstrip(char *s) {
    char *e = s + strlen(s) - 1;
    while (e > s) {
        if (*e != ' ') break;
        *e = '\0';
        e--;
    }
}

void
strreplace(char *s, char f, char t) {
    while (*s) {
        if (*s == f)
            *s = t;
        s++;
    }
}

int
config_request(mooshak_ctx_t *ctx) {
    char buff[1024];
    snprintf(buff, 1024, "%s?config+language+en", ctx->endpoint);
    curl_easy_setopt(ctx->curl, CURLOPT_URL, buff);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    #ifdef _DEBUG_
    long http_code;
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", buff, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing config query";
        return -1;
    }

    if (strstr(ctx->resbuff.memory, "Session expired")) {
        ctx->lasterr = "Mooshak: Session expired";
        return -1;
    }

    return 0;
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
    curl_easy_setopt(ctx->curl, CURLOPT_COOKIEJAR, "cookies.txt");

    /* set libmooshak useragent */
    curl_easy_setopt(ctx->curl, CURLOPT_USERAGENT, "libmooshak/0.0.1");


    /* ===== Service discovery request */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, baseurl);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", baseurl, http_code);
    #endif
    
    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing service discovery";
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

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0;

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", disc_url, http_code);
    #endif 

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing endpoint discovery";
        return ctx;
    }

    /* get endpoint (3xx redirect Location header) */
    char *endpoint_tmp = NULL;
    curl_easy_getinfo(ctx->curl, CURLINFO_REDIRECT_URL, &endpoint_tmp);
    if (!endpoint_tmp) {
        ctx->lasterr = "Error getting redirect location";
        return ctx;
    }

    ctx->endpoint = strdup(endpoint_tmp);

    #ifdef _DEBUG_
    printf("==endpoint: %s\n", ctx->endpoint);
    #endif    



    /* ===== config */
    /* index request */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, ctx->endpoint);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0;

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", ctx->endpoint, http_code);
    #endif

    /* get config endpoint (refresh url) */
    char *config_url = get_refresh_url(ctx->resbuff.memory);
    if (!config_url) {
        ctx->lasterr = "Error getting login refresh url";
        return NULL;
    }

    if (config_url[0] != 'h') {
        char *tmp = strdup(ctx->endpoint);
        tmp = append_args(tmp, config_url);
        free(config_url);
        config_url = tmp;
    }

    #ifdef _DEBUG_
    printf("==config url: %s\n", config_url);
    #endif

    /* ===== config query  */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, config_url);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    #ifdef _DEBUG_
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", config_url, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing config query";
        free(config_url);
        return NULL;
    }

    /* done */

    free(disc_url);
    free(config_url);

    ctx->init = 1;
    return ctx;
}

int
mooshak_isinit(const mooshak_ctx_t *ctx) {
    return ctx->init;
}

void
mooshak_deinit(mooshak_ctx_t *ctx) {
    if (!ctx) return;
    if (ctx->endpoint)
        free(ctx->endpoint);
    if (ctx->curl)
        curl_easy_cleanup(ctx->curl);
    if (ctx->resbuff.memory)
        free(ctx->resbuff.memory);
    free(ctx);
    curl_global_cleanup();
}

const char *
mooshak_getlasterror(mooshak_ctx_t *ctx) {
    return ctx->lasterr;
}

char **
mooshak_getcontests(mooshak_ctx_t *ctx) {
    /* ===== login page request  */
    curl_easy_setopt(ctx->curl, CURLOPT_URL, ctx->endpoint);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    #ifdef _DEBUG_
    long http_code;
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", ctx->endpoint, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing login page request";
        return NULL;
    }

    char *html = ctx->resbuff.memory;

    html_preprocess(html);

    char tag[16], attrkey[16], value[256];
    char contbuf[256];
    int selectcount = 0, optioncount = 0;

    char **contests = malloc(sizeof(char*)*2);
    int contestsize = 2, contestcount = 0;

    while ((html = html_ingest_starttag(html, tag, 16))) {
        if (strlen(tag) > 0) {
            if (strcmp(tag, "select") == 0) {
                selectcount++;
                optioncount = 0;
            }
            else if (strcmp(tag, "option") == 0) {
                optioncount++;
                if ((selectcount > 2) && (optioncount > 1)) {
                    html = html_ingest_attribute(html, attrkey, 16, value, 256);
                    html = html_ingest_contents(html, contbuf, 256);

                    if (contestcount + 1 > contestsize - 1) {
                        contests = realloc(contests, 
                            (sizeof(char*) * (contestcount + 2)));
                        contestsize = contestcount;
                    }

                    contests[contestcount] = strdup(value);
                    //rstrip(contests[contestcount]);
                    contestcount++;
                }
            }
        }
    }

    contests[contestcount] = NULL;

    return contests;
}

int
mooshak_login_contest(mooshak_ctx_t *ctx, const char *user,
    const char *password, const char *contest)
{
    char loginquery[1024];

    snprintf(loginquery, 1024,
        "command=relogin&arguments=&contest=%s&user=%s&password=%s",
        contest, user, password);

    curl_easy_setopt(ctx->curl, CURLOPT_POST, 1);
    curl_easy_setopt(ctx->curl, CURLOPT_URL, ctx->endpoint);
    curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDS, loginquery);
    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    /* reset */
    curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(ctx->curl, CURLOPT_HTTPGET, 1);

    #ifdef _DEBUG_
    long http_code;
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===POST %s | %s -> %ld\n", ctx->endpoint, loginquery, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing login query POST request";
        return -1;
    }

    if (strstr(ctx->resbuff.memory, "Invalid authentication")) {
        ctx->lasterr = "Got 'Invalid authentication' message from server";
        return -1;
    }

    if (strstr(ctx->resbuff.memory, "enough open sessions")) {
        ctx->lasterr = "Got 'enough open sessions' message from server";
        return -1;
    }

    /* It's probably a success I think */

    return 0;
}

int
mooshak_logoff(mooshak_ctx_t *ctx) {
    char query[1024];
    snprintf(query, 1024, "%s?logout", ctx->endpoint);
    curl_easy_setopt(ctx->curl, CURLOPT_URL, query);
    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0;

    #ifdef _DEBUG_
    long http_code;
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", query, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing logout request";
        return -1;
    }

    return 0;
}

int
mooshak_set_sublist_params(mooshak_ctx_t *ctx, int n) {
    char params[1024];
    snprintf(params, 1024,
        "all_problems=true&page=0&problem=P004&type=submissions"
        "&all_teams=true&lines=%d&time=5&command=listing",
        n);

    curl_easy_setopt(ctx->curl, CURLOPT_POST, 1);
    curl_easy_setopt(ctx->curl, CURLOPT_URL, ctx->endpoint);
    curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDS, params);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDS, NULL);
    curl_easy_setopt(ctx->curl, CURLOPT_HTTPGET, 1);

    #ifdef _DEBUG_
    long http_code;
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===POST %s | %s -> %ld\n", ctx->endpoint, params, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing sublist params query";
        return -1;
    }

    return 0;
}

int
mooshak_fetch_sublist(mooshak_ctx_t *ctx, int page, mooshak_submission_t **list)
{
    char listquery[1024];
    snprintf(listquery, 1024,
        "%s?listing",
        ctx->endpoint);

    curl_easy_setopt(ctx->curl, CURLOPT_URL, listquery);

    ctx->laststat = curl_easy_perform(ctx->curl);
    ctx->resbuff.off = 0; /* reset buff offset pointer */

    #ifdef _DEBUG_
    long http_code;
    curl_easy_getinfo(ctx->curl, CURLINFO_RESPONSE_CODE, &http_code);
    printf("===GET %s -> %ld\n", listquery, http_code);
    #endif

    if (ctx->laststat != CURLE_OK) {
        ctx->lasterr = "Error performing sublist params query";
        return -1;
    }

    char *html = ctx->resbuff.memory;

    html_preprocess(html);

    char tag[16];
    char contbuf[256];
    int trcount = 0, tdcount = 0;

    mooshak_submission_t *submissions = malloc(sizeof(mooshak_submission_t)*2);
    int subsize = 2, subcount = 0;

    while ((html = html_ingest_starttag(html, tag, 16))) {
        if (strlen(tag) > 0) {
            if (strcmp(tag, "tr") == 0) {
                if (trcount == 0) {
                    trcount++;
                    tdcount = 0;
                    continue;
                }

                /* allocate */
                if (subcount + 1 > subsize - 1) {
                    submissions = realloc(submissions, 
                        sizeof(mooshak_submission_t) * (subcount + 1));
                    subsize = subcount + 1;
                }

                trcount++;
                subcount++;
                tdcount = 0;
            }
            else if (strcmp(tag, "td") == 0) {
                if (trcount < 2) continue;

                html = html_skip_whole_tag(html);

                switch (tdcount) {
                    case 0:
                        html = html_skip_whole_tag(html);
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].id = atoi(contbuf);
                    break;
                    case 1:
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].time = strdup(contbuf);
                    break;
                    case 2: 
                        submissions[subcount-1].country = ""; //fixme
                    break;
                    case 3:
                        html = html_skip_whole_tag(html);
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].team = strdup(contbuf);
                    break;
                    case 4:
                        html = html_skip_whole_tag(html);
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].problem = strdup(contbuf);
                    break;
                    case 5:
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].language = strdup(contbuf);
                    break;
                    case 6: {
                        html = html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].attempt = atoi(contbuf);
                        html = html_skip_whole_tag(html);
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].result = strdup(contbuf);
                    } break;
                    case 7:
                        html = html_skip_whole_tag(html);
                        html_ingest_contents(html, contbuf, 256);
                        submissions[subcount-1].state = strdup(contbuf);
                    break;
                }

                tdcount++;
            }
        }
    }

    *list = submissions;
    return subcount;
}
