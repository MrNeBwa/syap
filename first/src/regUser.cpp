#include "regUser.h"
#include <iostream>
#include <string>

regUser::regUser() {
  regUser::_login = "Basic";
  std::cout << _login << std::endl;
}
regUser::~regUser() = default;
bool regUser::login(std::string login, uint32_t password) {
  // logic
  return true;
}
