#ifndef __nnc_util_HPP__
#define __nnc_util_HPP__

namespace nnc {
  // From https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/
  template<size_t N>
  struct NncTemplateString {
      constexpr NncTemplateString(const char (&str)[N]) {
          std::copy_n(str, N, value);
      }

      char value[N];
  };
}

#endif
