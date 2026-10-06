#include "forum.h"

#include <cstdint>
#include <string>
#include <utility>

forum::forum() : forumName("default") {}

forum::forum(std::string name) : forumName(std::move(name)) {}

forum::~forum() = default;

const std::string& forum::name() const noexcept {
  return forumName;
}

const std::list<message>& forum::getMessages() const noexcept {
  return messages;
}

void forum::addMessage(std::uint32_t userId, std::string text) {
  messages.emplace_back(userId, std::move(text));
}