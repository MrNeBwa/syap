#include "userDB.h"

#include <cstdint>
#include <functional>
#include <string>
#include <utility>

namespace {
std::uint32_t hashOf(const std::string& value) {
  return static_cast<std::uint32_t>(std::hash<std::string>{}(value));
}
} // namespace

userDB::userDB() {
  addDefault();
}

userDB::~userDB() = default;

void userDB::addDefault() {
  database["admin"] = hashOf("admin");
  database["user"] = hashOf("user");
}

bool userDB::addToDataBase(const std::string& name,
                           const std::string& password) {
  if (name.empty() || database.contains(name)) {
    return false;
  }
  database[name] = hashOf(password);
  return true;
}

bool userDB::add(std::string name, std::string password) {
  return addToDataBase(std::move(name), std::move(password));
}

bool userDB::deleteFromDataBase(const std::string& nameToDelete) {
  if (checkAdmin(nameToDelete)) {
    return false;
  }
  return database.erase(nameToDelete) != 0;
}

bool userDB::del(std::string name) {
  return deleteFromDataBase(std::move(name));
}

bool userDB::checkAdmin(const std::string& name) {
  return name == "admin";
}

bool userDB::login(const std::string& name, const std::string& password) {
  auto it = database.find(name);
  return it != database.end() && it->second == hashOf(password);
}