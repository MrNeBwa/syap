#pragma once

#include <cstdint>
#include <string>

class user {
public:
  user();
  explicit user(std::uint32_t id, std::string login);

  user(const user&) = default;
  user(user&&) = default;
  user& operator=(const user&) = default;
  user& operator=(user&&) = default;
  ~user();

  [[nodiscard]] std::uint32_t id() const noexcept;
  [[nodiscard]] const std::string& login() const noexcept;
  [[nodiscard]] std::string createMessage() const;

private:
  std::uint32_t user_id_ = 0;
  std::string login_;
};
