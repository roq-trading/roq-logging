/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

#include <cstddef>
#include <string_view>

namespace roq {

// constexpr string utilities

constexpr std::size_t static_strlen(const char *s) {
  size_t res = 0;
  for (auto iter = s; *iter != '\0'; ++iter, ++res) {
    // empty
  }
  return res;
}
constexpr const char *static_strrchr(const char *s, char c) {
  const char *res = s;
  for (auto iter = s; *iter != '\0'; ++iter)
    if (*iter == c)
      res = iter;
  return res;
}
constexpr const char *static_basename(const char *path) {
  auto res = static_strrchr(path, '/');
  while (*res == '/')
    ++res;
  return res;
}

// inspired by
// https://github.com/Jiwan/meta_crush_saga/blob/master/constexpr_string.hpp

template <size_t N>
class static_string final {
 public:
  constexpr explicit static_string(const char (&text)[N]) : data_{}, size_(N - 1) {
    for (size_t i = 0; i < N; ++i)
      data_[i] = text[i];
  }
  constexpr static_string(const char *text, size_t size) : data_{}, size_(size) {
    for (size_t i = 0; i < N && i < size; ++i)
      data_[i] = text[i];
  }
  template <size_t M>
  constexpr static_string(const static_string<M> &rhs, size_t size) : data_{}, size_(size) {
    for (size_t i = 0; i < rhs.size(); ++i)
      data_[i] = rhs[i];
  }

  constexpr const char *data() const { return data_; }

  constexpr size_t size() const { return size_; }

  constexpr char &operator[](size_t n) { return data_[n]; }

  constexpr const char &operator[](size_t n) const { return data_[n]; }

  template <size_t M>
  constexpr auto append(const static_string<M> &rhs) {
    static_string<N + M> res(*this, size() + rhs.size());
    for (size_t i = 0; i < rhs.size(); ++i)
      res[size() + i] = rhs[i];
    return res;
  }

  constexpr auto basename() const {
    size_t index = 0;
    for (size_t i = 0; i < size(); ++i)
      if (data_[i] == '/')
        index = i;
    while (index < size() && data_[index] == '/')
      ++index;
    auto length = size_ - index;
    return static_string<N>(data_ + index, length);
  }

  constexpr bool compare(const std::string_view &rhs) {
    return -rhs.compare(std::string_view(data(), size()));
  }

 private:
  char data_[N];
  size_t size_;
};

// XXX workaround to manage gcc build issues

template <size_t N>
class static_basename_string final {
 public:
  constexpr explicit static_basename_string(const char (&text)[N]) : data_{}, size_(N - 1) {
    size_t index = 0;
    for (size_t i = 0; i < N; ++i)
      if (text[i] == '/')
        index = i;
    while (index < N && text[index] == '/')
      ++index;
    size_ -= index;
    for (size_t i = 0; i < (size_ + 1); ++i)
      data_[i] = text[index + i];  // will include null terminator
  }

  constexpr const char *data() const { return data_; }

  constexpr size_t size() const { return size_; }

  constexpr char &operator[](size_t n) { return data_[n]; }

  constexpr const char &operator[](size_t n) const { return data_[n]; }

  template <size_t M>
  constexpr auto append(const static_string<M> &rhs) {
    static_string<N + M> res(data(), size() + rhs.size());
    for (size_t i = 0; i < rhs.size(); ++i)
      res[size() + i] = rhs[i];
    return res;
  }

  constexpr bool compare(const std::string_view &rhs) {
    return -rhs.compare(std::string_view(data(), size()));
  }

 private:
  char data_[N];
  size_t size_;
};

static_assert(static_basename_string("").compare("") == 0);
static_assert(static_basename_string("abc").compare("abc") == 0);
static_assert(static_basename_string("abc/def").compare("def") == 0);
static_assert(static_basename_string("abc/def/ghi").compare("ghi") == 0);

static_assert(
    static_basename_string("abc/def").append(static_string(":123")).compare("def:123") == 0);

}  // namespace roq
