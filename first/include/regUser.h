#pragma once

#include "user.h"

#include <cstdint>
#include <string>

class regUser : public user {
public:
  regUser();
  explicit regUser(std::uint32_t id, std::string login, std::string password);

  regUser(const regUser&) = default;
  regUser(regUser&&) = default;
  regUser& operator=(const regUser&) = default;
  regUser& operator=(regUser&&) = default;
  ~regUser() override;

  using user::login;
  bool login(const std::string& login, const std::string& password) const;
  bool changeLogin(std::string newLogin);

private:
  std::string password_;
};