#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>

class task {
 public:
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  struct promise_type {
    task get_return_object() { return task(handle_type::from_promise(*this)); }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };

  handle_type h_;

  task(handle_type h) : h_(h) {}
};

auto switch_to_new_thread(std::jthread& out) {
  class awaitable {
   public:
    std::jthread* p_out;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> h) {
      std::jthread& out = *p_out;
      if (out.joinable())
        throw std::runtime_error("Output jthread paramter not empty");
      out = std::jthread([h] { h.resume(); });

      std::cout << "New thread ID: " << out.get_id() << std::endl;
    }
    void await_resume() {}

    awaitable(std::jthread* out) : p_out(out) {}
  };

  return awaitable(&out);
}

task resuming_on_new_thread(std::jthread& out) {
  std::cout << "Coroutine started on thread: " << std::this_thread::get_id()
            << std::endl;
  co_await switch_to_new_thread(out);
  std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id()
            << std::endl;
}

auto yield(unsigned i) {
  struct awaitable {
    unsigned value;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> h) {}
    void await_resume() { std::cout << "resume: " << value << std::endl; }
  };

  return awaitable{i};
}

task multi_resume(unsigned num) {
  for (unsigned i = 0; i < num; i++) co_await yield(i);

  co_return;
}

int main() {
  std::jthread out;
  resuming_on_new_thread(out);

  unsigned num = 5;
  auto a = multi_resume(num);
  auto handle = a.h_;

  for (unsigned i = 0; i < num; i++) handle.resume();
}
