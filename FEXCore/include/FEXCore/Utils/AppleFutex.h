// SPDX-License-Identifier: MIT
#pragma once
#ifdef __APPLE__
#include <mach/mach_time.h>
#include <os/os_sync_wait_on_address.h>

#include <cerrno>
#include <cstdint>
#include <ctime>

namespace FEXCore::AppleFutex {
// Futex wait: sleeps while *Addr == Expected. Returns false on timeout.
// os_sync has no wake bitsets; callers that relied on them see extra wake-ups, which a futex user must tolerate anyway.
inline bool Wait(uint32_t* Addr, uint32_t Expected, const struct timespec* Timeout = nullptr) {
  int Result;
  if (Timeout) {
    // The timeout is in mach absolute time units, not nanoseconds (24 MHz ticks on Apple silicon).
    static const mach_timebase_info_data_t Base = [] {
      mach_timebase_info_data_t B {};
      mach_timebase_info(&B);
      return B;
    }();
    const uint64_t NS = static_cast<uint64_t>(Timeout->tv_sec) * 1000000000ull + static_cast<uint64_t>(Timeout->tv_nsec);
    const uint64_t Ticks = NS * Base.denom / Base.numer;
    Result = os_sync_wait_on_address_with_timeout(Addr, Expected, sizeof(uint32_t), OS_SYNC_WAIT_ON_ADDRESS_NONE, OS_CLOCK_MACH_ABSOLUTE_TIME,
                                                  Ticks ? Ticks : 1);
  } else {
    Result = os_sync_wait_on_address(Addr, Expected, sizeof(uint32_t), OS_SYNC_WAIT_ON_ADDRESS_NONE);
  }
  return !(Result < 0 && errno == ETIMEDOUT);
}

inline void WakeOne(uint32_t* Addr) {
  os_sync_wake_by_address_any(Addr, sizeof(uint32_t), OS_SYNC_WAKE_BY_ADDRESS_NONE);
}

inline void WakeAll(uint32_t* Addr) {
  os_sync_wake_by_address_all(Addr, sizeof(uint32_t), OS_SYNC_WAKE_BY_ADDRESS_NONE);
}
} // namespace FEXCore::AppleFutex
#endif
