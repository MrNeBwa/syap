#include "regUser.h"

#include <cstdint>
#include <string>
#include <utility>

regUser::regUser() : user(0, "Basic") {}

regUser::regUser(std::uint32_t id, std::string login, std::string password)
    : user(id, std::move(login)), password_(std::move(password)) {}

regUser::~regUser() = default;

bool regUser::login(const std::string& login,
                    const std::string& password) const {
  return !login.empty() && this->login() == login &&
         this->password_ == password;
}

bool regUser::changeLogin(std::string newLogin) {
  if (newLogin.empty()) {
    return false;
  }
  setLogin(std::move(newLogin));
  return true;
}