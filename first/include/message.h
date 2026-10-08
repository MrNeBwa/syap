#pragma once

#include <cstdint>
#include <string>

class message {
public:
  message() = default;
  message(std::uint32_t userId, std::uint32_t ID, std::string text)
      : userID_(userId), text_(std::move(text)), ID(ID) {}

  [[nodiscard]] std::uint32_t userID() const noexcept {
    return userID_;
  }
  [[nodiscard]] const std::string& text() const noexcept {
    return text_;
  }
  [[nodiscard]] const std::string& imagePATH() const noexcept {
    return imagePATH_;
  }
  std::uint32_t ID;
  std::string text_;
private:
  std::uint32_t userID_ = 0;
  std::string imagePATH_;
};