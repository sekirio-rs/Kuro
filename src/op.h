#include <iostream>
#include <coroutine>
#include <stdexcept>

template<typename T>
class Op {
 public:
  unsigned token;
  
  // Awaitable needed
  bool await_ready();
  void await_suspend(std::coroutine_handle<> h);
  auto await_resume();
  
  Op(T val);

 private:
  T value;
};
