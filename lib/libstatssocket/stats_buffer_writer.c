/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "include/stats_buffer_writer.h"

#include <errno.h>
#ifdef _MSC_VER
#include <utils/Timers.h>
#else
#include <sys/time.h>
#include <sys/uio.h>
#endif

#include "stats_buffer_writer_impl.h"
#include "stats_buffer_writer_queue.h"
#include "statsd_writer.h"

static const uint32_t kStatsEventTag = 1937006964;

#ifdef _MSC_VER
struct iovec {
    void* iov_base;
    size_t iov_len;
};
#endif

extern struct android_log_transport_write statsdLoggerWrite;

static int __write_to_statsd_init(struct iovec* vec, size_t nr);
static int (*__write_to_statsd)(struct iovec* vec, size_t nr) = __write_to_statsd_init;

/**
 * @brief Logs the error code associated with atom loss
 *
 * @param error To distinguish source of error, the errno code values must be negative,
 *              while the libstatssocket internal error codes are positive
 */
void note_log_drop(int error, int atomId) {
    statsdLoggerWrite.noteDrop(error, atomId);
}

void stats_log_close() {
    statsd_writer_init_lock();
    __write_to_statsd = __write_to_statsd_init;
    if (statsdLoggerWrite.close) {
        (*statsdLoggerWrite.close)();
    }
    statsd_writer_init_unlock();
}

int stats_log_is_closed() {
    return statsdLoggerWrite.isClosed && (*statsdLoggerWrite.isClosed)();
}

int write_buffer_to_statsd(void* buffer, size_t size, uint32_t atomId) {
    const int kQueueOverflowErrorCode = 1;
    if (should_write_via_queue(atomId)) {
        const bool ret = write_buffer_to_statsd_queue(buffer, size, atomId);
        if (!ret) {
            // to account on the loss, note atom drop with predefined internal error code
            note_log_drop(kQueueOverflowErrorCode, atomId);
        }
        return ret;
    }
    return write_buffer_to_statsd_impl(buffer, size, atomId, true);
}

int write_buffer_to_statsd_impl(void* buffer, size_t size, uint32_t atomId, bool doNoteDrop) {
    int ret = 1;

    struct iovec vecs[2];
    vecs[0].iov_base = (void*)&kStatsEventTag;
    vecs[0].iov_len = sizeof(kStatsEventTag);
    vecs[1].iov_base = buffer;
    vecs[1].iov_len = size;

    ret = __write_to_statsd(vecs, 2);

    if (ret < 0 && doNoteDrop) {
        note_log_drop(ret, atomId);
    }

    return ret;
}

static int __write_to_stats_daemon(struct iovec* vec, size_t nr) {
    int save_errno;
    struct timespec ts;
    size_t len, i;

    for (len = i = 0; i < nr; ++i) {
        len += vec[i].iov_len;
    }
    if (!len) {
        return -EINVAL;
    }

    save_errno = errno;
#if defined(__ANDROID__)
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#endif

    int ret = (int)(*statsdLoggerWrite.write)(&ts, vec, nr);
    errno = save_errno;
    return ret;
}

static int __write_to_statsd_initialize_locked() {
    if (!statsdLoggerWrite.open || ((*statsdLoggerWrite.open)() < 0)) {
        if (statsdLoggerWrite.close) {
            (*statsdLoggerWrite.close)();
            return -ENODEV;
        }
    }
    return 1;
}

static int __write_to_statsd_init(struct iovec* vec, size_t nr) {
    int ret, save_errno = errno;

    statsd_writer_init_lock();

    if (__write_to_statsd == __write_to_statsd_init) {
        ret = __write_to_statsd_initialize_locked();
        if (ret < 0) {
            statsd_writer_init_unlock();
            errno = save_errno;
            return ret;
        }

        __write_to_statsd = __write_to_stats_daemon;
    }

    statsd_writer_init_unlock();

    ret = __write_to_statsd(vec, nr);
    errno = save_errno;
    return ret;
}
