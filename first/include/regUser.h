#pragma once
#include "user.h"
#include <string>
class regUser : user {
public:
  regUser();
  regUser(regUser&&) = default;
  regUser(const regUser&) = default;
  regUser& operator=(regUser&&) = default;
  regUser& operator=(const regUser&) = default;
  ~regUser();

  bool login(std::string login, uint32_t password);
  bool changeLogin(std::string newLogin);

private:
  std::string _login;
};
