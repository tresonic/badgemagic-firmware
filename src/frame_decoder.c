#include "frame_decoder.h"

#include "heatshrink/heatshrink_decoder.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


static int poll_decoder(frame_decoder* fd, uint8_t *frame);

int init_frame_decoder(frame_decoder *fd, uint8_t *in, size_t len, size_t bytesPerFrame) {
    fd->in = in;
    fd->in_len = len;
    fd->in_idx = 0;

    fd->bytes_per_frame = bytesPerFrame;
    heatshrink_decoder_reset(&fd->hsd);
    fd->poll_res = HSDR_POLL_EMPTY;

    return 0;
}


int get_frame(frame_decoder *fd, uint8_t *frame) {
    size_t count = 0;

    DBG("\ngetting frame\n");

    while (fd->in_idx < fd->in_len) {
        assert(heatshrink_decoder_sink(&fd->hsd, &fd->in[fd->in_idx], fd->in_len - fd->in_idx,
                    &count) >= 0);
        fd->in_idx += count;
        DBG("sunk %zd\n", count);

        if (poll_decoder(fd, frame) == 0) {
            return 0;
        }
    }

    DBG("reached idx >= len\n");
    int ret = poll_decoder(fd, frame);
    if (ret == 0) {
        return 0;
    } else if (fd->polled >= fd->bytes_per_frame) {
        DBG("last polbuf %zd >= frame bytes\n", fd->polled);
        memcpy(frame, fd->dec_buf, fd->bytes_per_frame);
        memcpy(fd->dec_buf, &fd->dec_buf[fd->bytes_per_frame], DECMP_SIZE - fd->bytes_per_frame);
        fd->polled -= fd->bytes_per_frame;
        return 0;
    }

    DBG("stop with %zd left\n", fd->polled);

    return -1;
}



static int poll_decoder(frame_decoder* fd, uint8_t *frame) {
    size_t count = 0;
    do {
        fd->poll_res = heatshrink_decoder_poll(&fd->hsd, &fd->dec_buf[fd->polled],
                DECMP_SIZE - fd->polled, &count);
        assert(fd->poll_res >= 0);
        fd->polled += count;
        DBG("polled %zd\n", count);

        if (fd->polled >= fd->bytes_per_frame) {
            DBG("polbuf %zd >= frame bytes\n", fd->polled);
            memcpy(frame, fd->dec_buf, fd->bytes_per_frame);
            memcpy(fd->dec_buf, &fd->dec_buf[fd->bytes_per_frame], DECMP_SIZE - fd->bytes_per_frame);
            fd->polled -= fd->bytes_per_frame;
            return 0;
        }
    } while (fd->poll_res == HSDR_POLL_MORE);
    return -1;
}
