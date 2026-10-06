#include "user.h"

#include "forum.h"

#include <memory>
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

void user::setLogin(std::string login) {
  login_ = std::move(login);
}

std::string user::createMessage(const std::shared_ptr<forum>& uforum,
                                const std::string& text) const {
  if (!uforum) {
    return "No forum to send the message to";
  }
  if (text.empty()) {
    return "Message is empty";
  }
  uforum->addMessage(user_id_, text);
  return "Message is send";
}