#ifndef _IV_OS_ALLOCATOR_H_
#define _IV_OS_ALLOCATOR_H_
#include "platform.h"
namespace iv {
namespace core {

class OSAllocator {
 public:
  static void* Allocate(std::size_t bytes);
  static void Deallocate(void* address, std::size_t bytes);
  static void Commit(void* address, std::size_t bytes);
  static void Decommit(void* addr, std::size_t bytes);
};

} }  // namespace iv::core
#ifdef OS_WIN
#include "windows.h"
#include "os_allocator_win.h"
#else
#include "os_allocator_posix.h"
#endif  // OS_WIN
#endif  // _IV_OS_ALLOCATOR_H_