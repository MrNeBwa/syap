#pragma once

#include "message.h"
#include <list>
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

  [[nodiscard]] const std::string& name() const noexcept;
  [[nodiscard]] const std::list<message>& getMessages() const noexcept;

  void addMessage(std::uint32_t userId, std::string text);

private:
  std::string forumName;
  std::list<message> messages;
};