#include "user.h"

#include <utility>

user::user() = default;

user::user(std::uint32_t id, std::string login)
    : user_id_(id), login_(std::move(login)) {}

user::~user() = default;

std::uint32_t user::id() const noexcept {
  return user_id_;
}

const std::string& user::login() const noexcept {
  return login_;
}

std::string user::createMessage() const {
  if (login_.empty()) {
    return "Hello, anonymous user " + std::to_string(user_id_);
  }
  return "Hello, " + login_ + " (#" + std::to_string(user_id_) + ")";
}
