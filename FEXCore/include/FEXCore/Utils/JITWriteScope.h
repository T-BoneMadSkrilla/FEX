// SPDX-License-Identifier: MIT
#pragma once

#if defined(__APPLE__) && defined(__aarch64__)
#include <pthread.h>
#endif

namespace FEXCore::Utils {
// Darwin on Apple silicon maps JIT memory (MAP_JIT) either writable or executable per thread, never both.
// Threads stay executable; code writes happen inside one of these. Nests (patching from within a compile).
class JITWriteScope final {
public:
#if defined(__APPLE__) && defined(__aarch64__)
  JITWriteScope() {
    if (Depth++ == 0) {
      pthread_jit_write_protect_np(0);
    }
  }
  ~JITWriteScope() {
    if (--Depth == 0) {
      pthread_jit_write_protect_np(1);
    }
  }
private:
  static inline thread_local unsigned Depth = 0;
#else
  JITWriteScope() = default;
#endif

public:
  JITWriteScope(const JITWriteScope&) = delete;
  JITWriteScope& operator=(const JITWriteScope&) = delete;
};
} // namespace FEXCore::Utils
