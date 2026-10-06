#pragma once

#include "user.h"

#include <cstdint>
#include <map>
#include <string>

class userDB {
public:
  userDB();
  userDB(const userDB&) = default;
  userDB(userDB&&) = default;
  userDB& operator=(const userDB&) = default;
  userDB& operator=(userDB&&) = default;
  ~userDB();

  bool add(std::string name, std::string password);
  bool del(std::string name);
  bool login(const std::string& name, const std::string& password);
  static bool checkAdmin(const std::string& name);

private:
  std::map<std::string, std::uint32_t> database;
  void addDefault();
  bool addToDataBase(const std::string& name, const std::string& password);
  bool deleteFromDataBase(const std::string& nameToDelete);
};