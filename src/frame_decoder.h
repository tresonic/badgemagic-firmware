#pragma once

#include <stdint.h>
#include <string.h>

#include "heatshrink/heatshrink_decoder.h"

#ifdef FRAME_DECODER_DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif

/* #define DECMP_SIZE 88*2-1 */
#define DECMP_SIZE 256

typedef struct {
    uint8_t *in;
    size_t in_len;
    size_t in_idx;

    size_t bytes_per_frame;

    heatshrink_decoder hsd;
    HSD_poll_res poll_res;
    size_t polled;
    uint8_t dec_buf[DECMP_SIZE];
} frame_decoder;


int init_frame_decoder(frame_decoder *fd, uint8_t *in, size_t len, size_t bytesPerFrame);
int get_frame(frame_decoder *fd, uint8_t *frame);
