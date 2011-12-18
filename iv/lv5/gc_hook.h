#ifndef IV_LV5_GC_HOOK_H_
#define IV_LV5_GC_HOOK_H_
#include <gc/gc.h>
#include <iv/detail/unique_ptr.h>
namespace iv {
namespace lv5 {

// GC hook object
template<typename T>
class GCHook {
 public:
  explicit GCHook(T* delegate)
    : target_(Initialize(this)),
      delegate_(delegate) {
  }

  ~GCHook() {
    GC_REGISTER_FINALIZER_NO_ORDER(*target_, NULL, NULL, NULL, NULL);
  }

 private:
  static void Main(void* ptr, void* data) {
    GCHook* hook = *reinterpret_cast<GCHook**>(ptr);
    hook->delegate_->OnGarbageCollect();
    GC_REGISTER_FINALIZER_NO_ORDER(ptr, &Main, data, NULL, NULL);
  }

  static GCHook*** Initialize(GCHook* target) {
    GCHook** data =
        reinterpret_cast<GCHook**>(GC_MALLOC_ATOMIC(sizeof(target)));
    data[0] = target;
    GC_REGISTER_FINALIZER_NO_ORDER(
        reinterpret_cast<void*>(data), &Main, NULL, NULL, NULL);
    return new GCHook**(data);
  }

  core::unique_ptr<GCHook**> target_;
  T* delegate_;
};

} }  // namespace iv::lv5
#endif  // IV_LV5_GC_HOOK_H_
