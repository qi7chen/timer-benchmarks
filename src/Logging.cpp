// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// https://developers.google.com/protocol-buffers/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Logging.h"
#include <cstdio>
#include <cstring>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#define snprintf _snprintf
#endif

using std::mutex;
using std::lock_guard;

namespace internal {

void DefaultLogHandler(LogLevel level, const char* filename, int line,
                       const std::string& message) 
{
    static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };
    const char* sep = strrchr(filename, '\\');
    if (sep)
    {
        filename = sep;
    }
    char buffer[1024] = {};
    snprintf(buffer, 1023, "[%s %s:%d] %s\n", level_names[level], filename,
        line, message.c_str());
#if defined(_WIN32)
    OutputDebugStringA(buffer);
#else
    fprintf(stderr, "%s", buffer);
#endif
}

void NullLogHandler(LogLevel /* level */, const char* /* filename */,
                    int /* line */, const std::string& /* message */) 
{
    // Nothing.
}

static LogHandler* log_handler_ = &DefaultLogHandler;
static int log_silencer_count_ = 0;
static mutex log_silencer_count_mutex_;

void LogMessage::Finish() {
    bool suppress = false;

    if (level_ != LOGLEVEL_FATAL) {
        lock_guard<mutex> lock(log_silencer_count_mutex_);
        suppress = log_silencer_count_ > 0;
    }

    if (!suppress) {
        log_handler_(level_, filename_, line_, message_);
    }

    if (level_ == LOGLEVEL_FATAL) {
        abort();
    }
}

void LogFinisher::operator=(LogMessage& other) {
    other.Finish();
}

} // namespace internal


LogHandler* SetLogHandler(LogHandler* new_func) {
    LogHandler* old = internal::log_handler_;
    if (old == &internal::NullLogHandler) {
        old = NULL;
    }
    if (new_func == NULL) {
        internal::log_handler_ = &internal::NullLogHandler;
    }
    else {
        internal::log_handler_ = new_func;
    }
    return old;
}
