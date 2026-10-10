#pragma once

#include "entityGraph.h"
#include "forum.h"
#include "regUser.h"
#include "user.h"
#include "userDB.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class QLabel;
class QLineEdit;
class QTextEdit;

class interface : public QMainWindow {
  Q_OBJECT

public:
  explicit interface(QWidget* parent = nullptr);

private slots:
  void login();
  void sendMessage();
  void getMessages();

private:
  void ask(int number,
    std::function<void(std::vector<QLineEdit*>)> forward);
  void clearLayout(QLayout* layout);
  void createAccoutFun(std::vector<QLineEdit*> args);
  void changeLoginFun(std::vector<QLineEdit*> args);
  void deleteMessageFun(std::vector<QLineEdit*> args);
  void changeBottom(const int&& a);
  void refreshMessages();
  void setStatus(const QString& text);
  [[nodiscard]] std::shared_ptr<forum> currentForum() const;
  [[nodiscard]] QString userKey() const;
  [[nodiscard]] QString forumKey() const;
  [[nodiscard]] QString forumLabel() const;
  [[nodiscard]] QString userNameLabel() const;

  std::vector<QWidget*> stackTrace;
  userDB db_;
  std::shared_ptr<forum> forum_;
  std::shared_ptr<user> currentUser_;
  std::shared_ptr<regUser> currentRegUser_;
  std::uint32_t nextUserId_ = 100;

  QLayout* asklayout = nullptr;
  QLineEdit* loginEdit_ = nullptr;
  QLineEdit* passwordEdit_ = nullptr;

  entityGraph graph_;
  QStackedWidget* stack = nullptr;
  QLabel* statusLabel_ = nullptr;
  QTextEdit* messageView_ = nullptr;
  QLineEdit* messageEdit_ = nullptr;

};
