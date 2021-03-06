#include <assert.h>
#include <type_traits>

template <class X, class Y, typename Enabler = void>
struct isa_impl {
  static inline bool impl(const Y* ptr) {
    assert(ptr && "isa<> called on null pointer");
    return X::classof(ptr);
  }
};

template <class X, class Y>
struct isa_impl<X, Y,
                typename std::enable_if<std::is_base_of<X, Y>::value>::type> {
  static inline bool impl(const Y* ptr) {
    assert(ptr && "isa<> called on null pointer");
    return true;
  }
};

template <class X, class Y>
inline bool isa(const Y* ptr) {
  return isa_impl<X, Y>::impl(ptr);
}
