/*
 * Copyright (C) 2017 The Android Open Source Project
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

#ifndef ART_RUNTIME_BASE_SAFE_COPY_H_
#define ART_RUNTIME_BASE_SAFE_COPY_H_

#include <museum/8.1.0/bionic/libc/sys/types.h>

namespace facebook { namespace museum { namespace MUSEUM_VERSION { namespace art {

// Safely dereference a pointer.
// Returns -1 if safe copy isn't implemented on the platform, or if the transfer is too large.
// Returns 0 if src is unreadable.
ssize_t SafeCopy(void *dst, const void *src, size_t len);

} } } } // namespace facebook::museum::MUSEUM_VERSION::art

#endif  // ART_RUNTIME_BASE_SAFE_COPY_H_
