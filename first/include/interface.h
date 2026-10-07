#pragma once

#include "forum.h"
#include "user.h"
#include "userDB.h"

#include <QAbstractGraphicsShapeItem>
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QGraphicsScene>
#include <cstdint>
#include <memory>
#include <string>
#include <set>

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
  void ask(
    size_t size_,
    std::function<void(uint32_t, std::string)> op);
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
  
  QGraphicsScene *scene;
  std::set <QAbstractGraphicsShapeItem*> scene_obj_;
  QStackedWidget* stack;
  QLabel* statusLabel_ = nullptr;
  QTextEdit* messageView_ = nullptr;
  QLineEdit* messageEdit_ = nullptr;

};
