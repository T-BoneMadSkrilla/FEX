// SPDX-License-Identifier: MIT
#pragma once
#include <cstddef>

namespace FEXCore::Utils {
// FEX assumes an operating page size of 4096
// To work around build systems that build on a 16k/64k page size, define our page size here
// Don't use the system provided PAGE_SIZE define because of this.
constexpr size_t FEX_PAGE_SIZE = 4096;
constexpr size_t FEX_PAGE_SHIFT = 12;
constexpr size_t FEX_PAGE_MASK = ~(FEX_PAGE_SIZE - 1);

// The host's own page, for memory FEX protects itself (guard pages, the interrupt fault page).
// Guest pages stay FEX_PAGE_SIZE; Apple silicon's kernel maps in 16 KiB pages.
#if defined(__APPLE__) && defined(__aarch64__)
constexpr size_t FEX_HOST_PAGE_SIZE = 16384;
#else
constexpr size_t FEX_HOST_PAGE_SIZE = FEX_PAGE_SIZE;
#endif
} // namespace FEXCore::Utils
