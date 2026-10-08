#pragma once

#include "message.h"
#include <list>
#include <cstdint>
#include <string>

class forum {
public:
  forum();
  explicit forum(std::string name);

  forum(const forum&) = default;
  forum(forum&&) = default;
  forum& operator=(const forum&) = default;
  forum& operator=(forum&&) = default;
  ~forum();

  [[nodiscard]] bool deleteMessage(std::uint32_t ID);
  [[nodiscard]] bool deleteMessage(std::string text);
  [[nodiscard]] const std::string& name() const noexcept;
  [[nodiscard]] const std::list<message>& getMessages() const noexcept;

  void addMessage(std::uint32_t userId, std::string text);

private:
  std::uint32_t nextMessageID = 0;
  std::string forumName;
  std::list<message> messages;
};