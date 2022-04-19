#include <coroutine>
#include <functional>

template <typename T>
class Future {
 public:
  // Awaitable in C++, Future in Rust
  virtual bool await_ready();
  virtual void await_suspend(std::coroutine_handle<> h);
  virtual T await_resume();
};

template <typename T, typename U>
concept Awaitable = std::is_base_of<Future<U>, T>::value;

template <typename T, Awaitable<T> U, typename R>
class Map : Future<R> {
 public:
  U future;
  std::function<R(T)> func;

  bool await_ready() { return future.await_ready(); }
  void await_suspend(std::coroutine_handle<> h) {
    return future.await_suspend(h);
  }
  R await_resume() { return func(future.await_resume()); }

  Map(U&& future, std::function<R(T)> func) : future(future), func(func) {}
};
