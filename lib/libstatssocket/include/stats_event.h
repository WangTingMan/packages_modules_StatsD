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

#ifndef ANDROID_STATS_LOG_STATS_EVENT_H
#define ANDROID_STATS_LOG_STATS_EVENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libstatssocket_export.h"

/*
 * Functionality to build and store the buffer sent over the statsd socket.
 * This code defines and encapsulates the socket protocol.
 *
 * Usage:
 *      AStatsEvent* event = AStatsEvent_obtain();
 *
 *      AStatsEvent_setAtomId(event, atomId);
 *      AStatsEvent_addBoolAnnotation(event, 5, false); // atom-level annotation
 *      AStatsEvent_writeInt32(event, 24);
 *      AStatsEvent_addBoolAnnotation(event, 1, true); // annotation for preceding atom field
 *      AStatsEvent_addInt32Annotation(event, 2, 128);
 *      AStatsEvent_writeFloat(event, 2.0);
 *
 *      AStatsEvent_write(event);
 *      AStatsEvent_release(event);
 *
 * Note that calls to add atom fields and annotations should be made in the
 * order that they are defined in the atom.
 */

#ifdef __cplusplus
extern "C" {
#endif  // __CPLUSPLUS

/**
 * Opaque struct use to represent a StatsEvent. It builds and stores the data that is sent to
 * statsd.
 */
struct AStatsEvent;
typedef struct AStatsEvent AStatsEvent;

/**
 * Returns a new AStatsEvent. If you call this function, you must call AStatsEvent_release to free
 * the allocated memory.
 */
LIBSTATSSOCKET_API AStatsEvent* AStatsEvent_obtain();

/**
 * Builds and finalizes the AStatsEvent for a pulled event.
 * This should only be called for pulled AStatsEvents.
 *
 * After this function, the StatsEvent must not be modified in any way other than calling release or
 * write.
 *
 * Build can be called multiple times without error.
 * If the event has been built before, this function is a no-op.
 */
LIBSTATSSOCKET_API void AStatsEvent_build(AStatsEvent* event);

/**
 * Writes the StatsEvent to the stats log.
 * For all UIDs except system server:
 * - Returns number of bytes written into the socket, or socket error code.
 * For the system_server the write is done via intermediate queue:
 * - Returns 1 if event was added into the queue, 0 otherwise.
 *
 * After calling this, AStatsEvent_release must be called,
 * and is the only function that can be safely called.
 */
LIBSTATSSOCKET_API int AStatsEvent_write(AStatsEvent* event);

/**
 * Frees the memory held by this StatsEvent.
 *
 * After calling this, the StatsEvent must not be used or modified in any way.
 */
LIBSTATSSOCKET_API void AStatsEvent_release(AStatsEvent* event);

/**
 * Sets the atom id for this StatsEvent.
 *
 * This function should be called immediately after AStatsEvent_obtain. It may
 * be called additional times as well, but subsequent calls will have no effect.
 **/
LIBSTATSSOCKET_API void AStatsEvent_setAtomId(AStatsEvent* event, uint32_t atomId);

/**
 * Writes an int32_t field to this StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeInt32(AStatsEvent* event, int32_t value);

/**
 * Writes an int64_t field to this StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeInt64(AStatsEvent* event, int64_t value);

/**
 * Writes a float field to this StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeFloat(AStatsEvent* event, float value);

/**
 * Write a bool field to this StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeBool(AStatsEvent* event, bool value);

/**
 * Write a byte array field to this StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeByteArray(AStatsEvent* event, const uint8_t* buf,
                                                   size_t numBytes);

/**
 * Write a string field to this StatsEvent.
 *
 * The string must be null-terminated.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeString(AStatsEvent* event, const char* value);

/**
 * Write an attribution chain field to this StatsEvent.
 *
 * The sizes of uids and tags must be equal. The AttributionNode at position i is
 * made up of uids[i] and tags[i].
 *
 * \param uids array of uids in the attribution chain.
 * \param tags array of tags in the attribution chain. Each tag must be null-terminated.
 * \param numNodes the number of AttributionNodes in the attribution chain. This is the length of
 *                 the uids and the tags.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeAttributionChain(AStatsEvent* event, const uint32_t* uids,
                                       const char* const* tags, uint8_t numNodes);

/**
 * Write a int32 array field to this StatsEvent.
 *
 * Max size of array is 127. If exceeded, array is not written and ERROR_LIST_TOO_LONG is appended
 * to StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeInt32Array(AStatsEvent* event, const int32_t* elements,
                                                    size_t numElements);

/**
 * Write a int64 array field to this StatsEvent.
 *
 * Max size of array is 127. If exceeded, array is not written and ERROR_LIST_TOO_LONG is appended
 * to StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeInt64Array(AStatsEvent* event, const int64_t* elements,
                                                    size_t numElements);

/**
 * Write a float array field to this StatsEvent.
 *
 * Max size of array is 127. If exceeded, array is not written and ERROR_LIST_TOO_LONG is appended
 * to StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeFloatArray(AStatsEvent* event, const float* elements,
                                                    size_t numElements);

/**
 * Write a bool array field to this StatsEvent.
 *
 * Max size of array is 127. If exceeded, array is not written and ERROR_LIST_TOO_LONG is appended
 * to StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeBoolArray(AStatsEvent* event, const bool* elements,
                                                   size_t numElements);

/**
 * Write a string array field to this StatsEvent.
 *
 * String array encoding is UTF8.
 *
 * Strings must be null terminated. Max size of array is 127. If exceeded, array is not written and
 * ERROR_LIST_TOO_LONG is appended to StatsEvent.
 **/
LIBSTATSSOCKET_API void AStatsEvent_writeStringArray(AStatsEvent* event,
                                                     const char* const* elements,
                                  size_t numElements);

/**
 * Write a bool annotation for the previous field written.
 **/
LIBSTATSSOCKET_API void AStatsEvent_addBoolAnnotation(AStatsEvent* event, uint8_t annotationId,
                                                      bool value);

/**
 * Write an integer annotation for the previous field written.
 **/
LIBSTATSSOCKET_API void AStatsEvent_addInt32Annotation(AStatsEvent* event, uint8_t annotationId,
                                                       int32_t value);

// Internal/test APIs. Should not be exposed outside of the APEX.
LIBSTATSSOCKET_API void AStatsEvent_overwriteTimestamp(AStatsEvent* event, uint64_t timestampNs);
LIBSTATSSOCKET_API uint32_t AStatsEvent_getAtomId(AStatsEvent* event);
// Size is an output parameter.
LIBSTATSSOCKET_API uint8_t* AStatsEvent_getBuffer(AStatsEvent* event, size_t* size);
LIBSTATSSOCKET_API uint32_t AStatsEvent_getErrors(AStatsEvent* event);

#ifdef __cplusplus
}
#endif  // __CPLUSPLUS

#endif  // ANDROID_STATS_LOG_STATS_EVENT_H
