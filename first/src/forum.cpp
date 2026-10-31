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

bool forum::deleteMessage(std::uint32_t ID){
  for (auto it = messages.begin(); it != messages.end(); ++it){
    if (it->ID == ID){
      messages.erase(it);
      return true;
    }
  }
  return false;
}
bool forum::deleteMessage(std::string text){
  bool removed = false;
  for (auto it = messages.begin(); it != messages.end(); ++it){
    if (it->text_ == text){
      removed = true;
      messages.erase(it);
      return true;
    }
  }
  return removed;
}


void forum::addMessage(std::uint32_t userId, std::string text) {
  messages.emplace_back(userId, nextMessageID++, std::move(text));
}