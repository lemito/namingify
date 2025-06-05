#include <concepts>
#include <iostream>
#include <type_traits>

constexpr size_t CNT = 10;

/**
 * x_n+1 = x_n*(2-D*x_n)
 * n-> inf => x_n -> 1/D
 */
template <typename T>
concept Integer = requires(T t) {
  std::is_integral<decltype(t)>::value == true &&
      std::is_same_v<decltype(t), bool> == false;
};

template <Integer T> constexpr T newton_div(T div) {
  if (div == T(0)) {
    return T(0);
  }

  T x{};
  T abs_div = div < 0 ? -div : div;

  if (abs_div < T(1)) {
    x = div < 0 ? T(-1) : T(1);
  } else if (abs_div < T(10)) {
    x = div < 0 ? T(-0.1) : T(0.1);
  } else {
    x = div < 0 ? T(-0.01) : T(0.01);
  }

  for (size_t i = 0; i < CNT; i++) {
    x = x * (T{2} - div * x);
  }

  return x;
};

int main() {
  constexpr double div = 3;
  constexpr double res = newton_div<double>(div);
  std::cout << res << '\n';
  return 0;
}
