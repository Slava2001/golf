#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define LOG_LVL_DEBUG 1
#define LOG_LVL_INFO 2
#define LOG_LVL_ERROR 3
#define LOG_LVL_CRITICAL 4

#ifndef LOG_LVL
#define LOG_LVL LOG_LVL_ERROR
#endif

// log macros

#define log(lvl, fmt, ...)  do { \
                                if (lvl >= LOG_LVL) { \
                                    (void)printf(fmt, ## __VA_ARGS__ ); \
                                    fflush(stdout); \
                                } \
                            } while(0)

#define log_raw_lvl(lvl, fmt, ...) log(lvl, fmt, __VA_ARGS__)
#define log_rawd(fmt, ...) log(LOG_LVL_DEBUG, fmt, __VA_ARGS__)
#define log_rawi(fmt, ...) log(LOG_LVL_INFO, fmt, __VA_ARGS__)
#define log_rawe(fmt, ...) log(LOG_LVL_ERROR, fmt, __VA_ARGS__)
#define log_rawc(fmt, ...) log(LOG_LVL_CRITICAL, fmt, __VA_ARGS__)

#define log_lvl(lvl, fmt, ...) log_raw_lvl(lvl, "%s(): "fmt"\n", __func__, ## __VA_ARGS__)
#define logd(fmt, ...) log_raw_lvl(LOG_LVL_DEBUG, "%s(): "fmt"\n", __func__, ## __VA_ARGS__)
#define logi(fmt, ...) log_raw_lvl(LOG_LVL_INFO, "%s(): "fmt"\n", __func__, ## __VA_ARGS__)
#define loge(fmt, ...) log_raw_lvl(LOG_LVL_ERROR, "%s(): "fmt"\n", __func__, ## __VA_ARGS__)
#define logc(fmt, ...) log_raw_lvl(LOG_LVL_CRITICAL, "%s(): "fmt"\n", __func__, ## __VA_ARGS__)

#define log_enter() logd("Enter")

#define FLOAT_FMT "%6.3f"
#define log_arr(name, fmt, ptr, cnt) \
        do { \
            log_rawi("%s(): "name": [", __func__); \
            for (unsigned i = 0; i < (cnt); i++) { \
                log_rawi(fmt" ", ptr[i]); \
            } \
            log_rawi("]\n"); \
        } while(0)

// check status macros

#define check_true(cond) err_if(!(cond), "Failed to check \"%s\". It is false", # cond)
#define err_if(cond, msg, ...)  do { if (cond) { loge(msg, ## __VA_ARGS__ ); return -1; } }while(0)
#define try_do(code) err_if(code, "Failed to %s", # code)
#define ext_if(cond, msg, ...)  do { if (cond) { loge(msg, ## __VA_ARGS__ ); exit(-1); } }while(0)
#define try_do_exit(code) ext_if(code, "Failed to %s", # code)

// true if a equal b, else false
#define FLOAT_CMP(a, b, d) (fabs(a - b) < d)

#define SWAP(type, a, b) do { type tmp = a; a = b; b = tmp;  } while(0)
#define MIN(a, b) (((a) < (b))?(a):(b))
#define MAX(a, b) (((a) > (b))?(a):(b))

#endif // INCLUDE_UTIL_H
