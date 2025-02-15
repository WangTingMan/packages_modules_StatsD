/*
 * Copyright (C) 2023, The Android Open Source Project
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

#pragma once

#include <stddef.h>
#include <stdint.h>
#if __has_include(<sys/cdefs.h>)
#include <sys/cdefs.h>
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

int64_t get_elapsed_realtime_ns();

int toSocketLossError(int errno_code);

__END_DECLS
