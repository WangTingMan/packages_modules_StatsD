/*
 * Copyright (C) 2018, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_STATS_LOG_STATS_WRITER_H
#define ANDROID_STATS_LOG_STATS_WRITER_H

#ifdef _MSC_VER
#include <stdint.h>
#else
#include <pthread.h>
#include <stdatomic.h>
#include <sys/socket.h>
#endif

#ifdef __cplusplus
#ifndef __BEGIN_DECLS
#define __BEGIN_DECLS extern "C" {
#endif
#else
#define __BEGIN_DECLS
#endif

#ifdef __cplusplus
#ifndef __END_DECLS
#define __END_DECLS }
#endif
#else
#define __END_DECLS
#endif

__BEGIN_DECLS

/**
 * Internal lock should not be exposed. This is bad design.
 * TODO: rewrite it in c++ code and encapsulate the functionality in a
 * StatsdWriter class.
 */
void statsd_writer_init_lock();
int statsd_writer_init_trylock();
void statsd_writer_init_unlock();

struct android_log_transport_write {
    const char* name; /* human name to describe the transport */
    #ifdef _MSC_VER
    int sock;
    #else
    atomic_int sock;
    #endif
    int (*available)(); /* Does not cause resources to be taken */
    int (*open)();      /* can be called multiple times, reusing current resources */
    void (*close)();    /* free up resources */
    /* write log to transport, returns number of bytes propagated, or -errno */
    int (*write)(struct timespec* ts, struct iovec* vec, size_t nr);
    /* note one log drop */
    void (*noteDrop)(int error, int tag);
    /* checks if the socket is closed */
    int (*isClosed)();
};

__END_DECLS

#endif  // ANDROID_STATS_LOG_STATS_WRITER_H
