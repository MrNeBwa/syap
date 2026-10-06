#pragma once

#include "forum.h"
#include "user.h"
#include "userDB.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <cstdint>
#include <memory>
#include <string>

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

private:
  void changeBottom(const int&& a);
  void refreshMessages();
  void setStatus(const QString& text);
  [[nodiscard]] std::shared_ptr<forum> currentForum() const;
  std::vector<QWidget*> stackTrace;
  userDB db_;
  std::shared_ptr<forum> forum_;
  std::shared_ptr<user> currentUser_;
  std::uint32_t nextUserId_ = 100;
  
  QLineEdit* loginEdit_ = nullptr;
  QLineEdit* passwordEdit_ = nullptr;
  
  QStackedWidget* stack;
  QLabel* statusLabel_ = nullptr;
  QTextEdit* messageView_ = nullptr;
  QLineEdit* messageEdit_ = nullptr;

};
