#include "regUser.h"
#include "user.h"
#include <iostream>
int main() {
  const user anonymous;
  const user u{7, "nebwa"};
  const regUser a{};
  std::cout << anonymous.createMessage() << '\n';
  std::cout << u.createMessage() << '\n';
  std::cout << "id=" << u.id() << " login=" << u.login() << '\n';
  return 0;
}
