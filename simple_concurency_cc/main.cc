#include <algorithm>
#include <atomic>
#include <barrier>
#include <future>
// #include <generator>
#include <iostream>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

// Пример с promise - future

template <typename T>
  requires requires(T a, T b) { a + b; }
T bin_op(T a, T b) {
  return a + b;
}

void do_op(std::vector<int>::iterator first, std::vector<int>::iterator last,
           std::promise<int> pr) {
  int res = std::accumulate(first, last, 0, bin_op<int>);
  pr.set_value(res);
}

// простенький пример с общей инфой и мбютексом

int32_t some_cnt = 0;
std::mutex m;

void just_code_with_mutex() {
  std::lock_guard l(m);
  ++some_cnt;
}

// condvar

std::mutex mcv;
std::condition_variable condvar;
bool cvpred = false;

void cv_test() {
  {
    std::lock_guard l(mcv);
    cvpred = true;
  }
  condvar.notify_one();
}

void cv_worker() {
  std::unique_lock l(mcv);
  condvar.wait(l, []() { return cvpred; });
  std::cout << "Done!\n";
}

// примерчик с атомиком

/**
 * https://en.cppreference.com/w/cpp/atomic/memory_order.html
 */

std::atomic<int> cnt(0);
int tmp = 1;

void with_atomic() {
  cnt.fetch_add(1, std::memory_order_relaxed);

  while (cnt.compare_exchange_weak(tmp, 2, std::memory_order_relaxed)) {
    tmp = 0;
  }
  std::cout << "Cnt update to " << cnt.load() << std::endl;
}

// барьерчик

std::barrier bar(5);

void bar_worker() {
  std::cout << "Something before barrier\n";
  bar.arrive_and_wait(); // действие барьера
  std::cout << "Something after barrier\n";
}

/// корутины

// генератор

// std::generator<int> fib(int n) {
//   int a = 0, b = 1;

//   if (n >= 1) {
//     co_yield a;
//   }
//   if (n >= 2) {
//     co_yield b;
//   }

//   for (int i = 2; i < n; ++i) {
//     int c = a + b;
//     co_yield c;
//     a = b;
//     b = c;
//   }
// }

int main() {
  std::cout << "=======================\n";
  {
    std::vector<int> inpt = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::promise<int> pr;
    std::future<int> ft = pr.get_future();
    std::jthread work_thread(do_op, inpt.begin(), inpt.end(), std::move(pr));

    std::cout << "res=" << ft.get() << '\n';
  }

  std::cout << "=======================\n";

  {
    std::vector<std::jthread> threads;
    for (size_t i = 0; i < 10; i++) {
      threads.emplace_back(just_code_with_mutex);
    }
  }

  std::cout << "res with mutex=" << some_cnt << '\n';

  std::cout << "=======================\n";

  {
    std::jthread cvt1(cv_test);
    std::jthread cvt2(cv_worker);
  }

  std::cout << "=======================\n";

  {
    std::vector<std::jthread> threads;
    for (size_t i = 0; i < 10; i++) {
      threads.emplace_back(with_atomic);
    }
  }

  std::cout << "res with atomic=" << cnt << '\n';

  std::cout << "=======================\n";

  {
    std::vector<std::jthread> threads;
    for (size_t i = 0; i < 5; i++) {
      threads.emplace_back(bar_worker);
    }
  }

  std::cout << "=======================\n";

  //   for (auto num : fib(6)) {
  //     std::cout << num << " ";
  //   }

  //   std::cout << "\n=======================\n";

  return 0;
}