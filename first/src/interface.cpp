#include "interface.h"

#include "regUser.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

interface::interface(QWidget* parent) : QMainWindow(parent) {
  forum_ = std::make_shared<forum>("General");


  #pragma region layout init
  auto* main = new QWidget(this);
  auto* root = new QVBoxLayout(main);
  auto* top = new QHBoxLayout(main);
  auto* foot = new QVBoxLayout(main);
  stack = new QStackedWidget(main);
  
  #pragma endregion

  

  #pragma region buttons
  QWidget* buttons = new QWidget();
  auto* buttonsPage = new QVBoxLayout(buttons);
  auto* createMessage = new QPushButton("Создать сообщение", main);
  auto* logininto = new QPushButton("Зайти в аккаунт", main);
  auto* getMessageList = new QPushButton("Получить список сообщений", main);
  auto* deleteMessage = new QPushButton("Delete Message", main);
  auto* changeLogin = new QPushButton("Change login", main);
  auto* addUser = new QPushButton("Add user", main);
  buttonsPage->addWidget(logininto);
  buttonsPage->addWidget(createMessage);
  buttonsPage->addWidget(getMessageList);
  buttonsPage->addWidget(deleteMessage);
  stack->addWidget(buttons);
  stack->setCurrentWidget(buttons);
  stackTrace.push_back(buttons);
  #pragma endregion
  
  #pragma region loginPage add
  QWidget* login = new QWidget();
  auto* loginRow = new QHBoxLayout(login);
  loginEdit_ = new QLineEdit(main);
  loginEdit_->setPlaceholderText("login");
  passwordEdit_ = new QLineEdit(main);
  passwordEdit_->setPlaceholderText("password");
  passwordEdit_->setEchoMode(QLineEdit::Password);
  auto* loginButton = new QPushButton("Login", main);
  loginRow->addWidget(loginEdit_);
  loginRow->addWidget(passwordEdit_);
  loginRow->addWidget(loginButton);
  stack->addWidget(login);
  stackTrace.push_back(login);
  #pragma endregion


  #pragma region askPage
  QWidget* askpage = new QWidget();
  asklayout = new QVBoxLayout(askpage);
  stack->addWidget(askpage);
  stackTrace.push_back(askpage);

  #pragma endregion


  auto* content = new QHBoxLayout();
  messageView_ = new QTextEdit(main);
  messageView_->setReadOnly(true);
  content->addWidget(messageView_, 1);

  top->addLayout(content, 1);
  top->addWidget(stack);
  
  auto* sendRow = new QHBoxLayout();
  messageEdit_ = new QLineEdit(main);
  messageEdit_->setPlaceholderText("message");
  auto* sendButton = new QPushButton("Send", main);
  sendRow->addWidget(messageEdit_, 1);
  sendRow->addWidget(sendButton);
  foot->addLayout(sendRow);

  statusLabel_ = new QLabel(main);
  foot->addWidget(statusLabel_);

  #pragma region drawing

  scene = new QGraphicsScene();
  QGraphicsView *view = new QGraphicsView(scene);
  QGraphicsRectItem *block =
      scene->addRect(50, 50, 120, 70);
  scene_obj_.insert(block);

  block->setFlag(QGraphicsItem::ItemIsMovable);
  block->setBrush(QColor("#ab4242"));
  #pragma endregion


  foot->addWidget(view);
  root->addLayout(top);
  root->addLayout(foot);
  setCentralWidget(main);
  setWindowTitle("Laba1");
  resize(1000, 500);

  connect(loginButton, &QPushButton::clicked, this, &interface::login);
  connect(sendButton, &QPushButton::clicked, this, &interface::sendMessage);
  connect(messageEdit_, &QLineEdit::returnPressed, this,
          &interface::sendMessage);
  connect(logininto, &QPushButton::clicked, this, [this](){changeBottom(std::move(1));});
  connect(deleteMessage, &QPushButton::clicked, this, &interface::deleteMessageFun);
  setStatus("Not logged in");
  refreshMessages();
}

void interface::deleteMessageFun(){
  ask(2);
}

std::vector<std::string> interface::ask(
    const int& number
){
  //create that many enters with
  std::vector<QLineEdit*> textFields (number, nullptr);
  for (size_t i = 0; i < number; ++i){
    auto *askEdit_ = new QLineEdit();
    asklayout->addWidget(askEdit_);
    textFields[i] = askEdit_;
  }
  changeBottom(2);
  return {};
}


void interface::changeBottom(const int&& a){
  stack->setCurrentWidget(stackTrace[a]);
} 


void interface::login() {
  const std::string name = loginEdit_->text().toStdString();
  const std::string password = passwordEdit_->text().toStdString();

  if (!db_.login(name, password)) {
    currentUser_.reset();
    setStatus(QString("Wrong login or password: %1").arg(
        QString::fromStdString(name)));
    return;
  }

  currentUser_ = std::make_shared<regUser>(nextUserId_++, name, password);
  this->changeBottom(std::move(0));
  loginEdit_->clear();
  passwordEdit_->clear();
  setStatus(QString("Logged in as %1").arg(QString::fromStdString(name)));
}



void interface::sendMessage() {
  const QString text = messageEdit_->text();
  if (!currentUser_) {
    setStatus("Log in first");
    return;
  }
  if (auto forumPtr = currentForum()) {
    const std::string result =
        currentUser_->createMessage(forumPtr, text.toStdString());
    setStatus(QString::fromStdString(result));
    if (result == "Message is send") {
      messageEdit_->clear();
      refreshMessages();
    }
  }
}

void interface::refreshMessages() {
  messageView_->clear();
  auto forumPtr = currentForum();
  if (!forumPtr) {
    return;
  }

  QString header = QString("Forum: %1").arg(
      QString::fromStdString(forumPtr->name()));
  messageView_->setPlainText(header + "\n\n");

  for (const auto& msg : forumPtr->getMessages()) {
    messageView_->append(
        QString("#%1: %2")
            .arg(msg.userID())
            .arg(QString::fromStdString(msg.text())));
  }
}

void interface::setStatus(const QString& text) {
  statusLabel_->setText(text);
}

std::shared_ptr<forum> interface::currentForum() const {
  return forum_;
}
