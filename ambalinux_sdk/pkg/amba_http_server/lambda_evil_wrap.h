#pragma once

//// Because the c++ 11 does not support capture unique_ptr (need c++ 14)
// NOTE: it's UNSAFE, use wrap_move with little performance lost but SAFE.
//template<typename T > struct lambda_evil_wrap {
//  mutable T ptr_;
//  lambda_evil_wrap(T&& ptr) : ptr_(std::forward< T>(ptr)) {}
//  lambda_evil_wrap(lambda_evil_wrap const& other) : ptr_(std::move(other.ptr_)) {}
//  lambda_evil_wrap & operator=(lambda_evil_wrap& other) = delete;
//};

// Because asio handler want copyable
// "However, handler types are still required to be copy constructible."
// So I can not capture a unique_ptr in asio handler
//template<class T, class...Args >
//std::shared_ptr<std::unique_ptr<T>> make_shared_unique(Args&&... args) {
//  return std::make_shared<std::unique_ptr<T>>(
//    std::make_unique<T>(std::forward<Args>(args)...));
//}

// gcc 4.8 does not support std::make_unique and not support validic template
template<class T, class Args0 >
std::shared_ptr<std::unique_ptr<T>> make_shared_unique(Args0&& args0) {
  return std::make_shared<std::unique_ptr<T>>(new std::unique_ptr<T>(
    new T(std::forward<Args0>(args0))));
}

template<class T, class Args0, class Args1 >
std::shared_ptr<std::unique_ptr<T>> make_shared_unique(Args0&& args0,
  Args1&& args1) {
  return std::make_shared<std::unique_ptr<T>>(new std::unique_ptr<T>(
    new T(std::forward<Args0>(args0), std::forward<Args1>(args1))));
}

template<class T>
std::shared_ptr<T> wrap_move(T&& p) {
  return std::make_shared<T>(std::move(p));
}

// another more safe hack
//template <typename F>
//struct move_wrapper : F {
//  move_wrapper(F&& f) : F(std::move(f)) {}
//
//  move_wrapper(move_wrapper&&) = default;
//  move_wrapper& operator=(move_wrapper&&) = default;
//
//  move_wrapper(const move_wrapper&);
//  move_wrapper& operator=(const move_wrapper&);
//};
//
//template <typename T>
//auto move_handler(T&& t) -> move_wrapper<typename std::decay<T>::type> {
//  return std::move(t);
//}

