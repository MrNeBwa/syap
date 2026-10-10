#include "interface.h"

#include "regUser.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QStackedWidget>

namespace {
const QColor kUserDBColor("#2f6fbd");
const QColor kForumColor("#3f9a52");
const QColor kUserColor("#d97a26");
const QString kUserDBKey = "userDB";
} // namespace

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
  buttonsPage->addWidget(changeLogin);
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

  graph_.ensureEntity(kUserDBKey, "userDB", kUserDBColor);
  graph_.ensureEntity(forumKey(), forumLabel(), kForumColor);
  graph_.beginAction();

  #pragma endregion


  foot->addWidget(graph_.view());
  root->addLayout(top);
  root->addLayout(foot);
  setCentralWidget(main);
  setWindowTitle("Laba1");
  resize(1000, 500);

  connect(loginButton, &QPushButton::clicked, this, &interface::login);
  connect(sendButton, &QPushButton::clicked, this, &interface::sendMessage);
  connect(messageEdit_, &QLineEdit::returnPressed, this,
          &interface::sendMessage);
  connect(logininto, &QPushButton::clicked, this, [this]() { changeBottom(1); });
  connect(deleteMessage, &QPushButton::clicked, this, [this]() {
    ask(1, [this](std::vector<QLineEdit*> args) { deleteMessageFun(args); });
  });
  connect(changeLogin, &QPushButton::clicked, this, [this]() {
    ask(1, [this](std::vector<QLineEdit*> args) { changeLoginFun(args); });
  });
  connect(addUser, &QPushButton::clicked, this, [this]() {
    ask(2, [this](std::vector<QLineEdit*> args) { createAccoutFun(args); });
  });
  connect(createMessage, &QPushButton::clicked, this,
          [this]() { messageEdit_->setFocus(); });
  connect(getMessageList, &QPushButton::clicked, this, &interface::getMessages);
  setStatus("Not logged in");
  refreshMessages();
}

void interface::deleteMessageFun(std::vector<QLineEdit*> args)
{
  graph_.beginAction();
  graph_.ensureEntity(forumKey(), forumLabel(), kForumColor);

  const bool removed = forum_->deleteMessage(args[0]->text().toStdString());

  if (currentUser_) {
    graph_.ensureEntity(userKey(), userNameLabel(), kUserColor);
    graph_.addRelation(userKey(), forumKey(),
                       removed ? "deleteMessage()" : "deleteMessage() ✗");
  } else {
    graph_.addRelation(forumKey(), forumKey(),
                       removed ? "deleteMessage()" : "deleteMessage() ✗");
  }

  setStatus(removed ? QString("Message deleted")
                    : QString("Message not found: %1").arg(args[0]->text()));
  refreshMessages();
  changeBottom(0);
}

void interface::createAccoutFun(std::vector<QLineEdit*> args){
  if (args.size() < 2) {
    return;
  }
  const std::string name = args[0]->text().toStdString();
  const std::string password = args[1]->text().toStdString();

  graph_.beginAction();
  graph_.ensureEntity(kUserDBKey, "userDB", kUserDBColor);

  const std::uint32_t newId = nextUserId_;
  const QString newKey = QString("user:%1").arg(newId);
  graph_.ensureEntity(newKey,
                      QString("User: %1").arg(QString::fromStdString(name)),
                      kUserColor);
  graph_.addRelation(newKey, kUserDBKey, "register()");

  const bool added = db_.add(name, password);
  currentRegUser_ = std::make_shared<regUser>(nextUserId_++, name, password);
  currentUser_ = currentRegUser_;

  graph_.ensureEntity(userKey(), userNameLabel(), kUserColor);
  graph_.addRelation(kUserDBKey, userKey(), added ? "created" : "exists");
  graph_.addPersistentRelation(userKey(), kUserDBKey, "stored");

  changeBottom(0);
  loginEdit_->clear();
  passwordEdit_->clear();
  setStatus(QString("Logged in as %1").arg(args[0]->text()));
}

void interface::changeLoginFun(std::vector<QLineEdit*> args){
  graph_.beginAction();
  graph_.ensureEntity(kUserDBKey, "userDB", kUserDBColor);

  if (!currentRegUser_) {
    graph_.addRelation(kUserDBKey, kUserDBKey, "changeLogin() denied");
    setStatus("Log in first");
    changeBottom(0);
    return;
  }

  const QString newLogin = args[0]->text();
  const bool changed = currentRegUser_->changeLogin(newLogin.toStdString());

  graph_.ensureEntity(userKey(), userNameLabel(), kUserColor);
  graph_.addRelation(userKey(), userKey(),
                     changed ? "changeLogin()" : "changeLogin() ✗");
  graph_.addRelation(userKey(), kUserDBKey, "update()");

  setStatus(changed ? QString("Login changed to %1").arg(newLogin)
                    : QString("Login unchanged"));
  changeBottom(0);
}

void interface::ask(
    int number,
    std::function<void(std::vector<QLineEdit*>)> forward
)
{
    std::vector<QLineEdit*> textFields;
    clearLayout(asklayout);
    for (int i = 0; i < number; ++i)
    {
        auto* askEdit = new QLineEdit();

        asklayout->addWidget(askEdit);
        textFields.push_back(askEdit);
    }

    auto* submitBt = new QPushButton("Дальше");
    asklayout->addWidget(submitBt);

    changeBottom(2);

    connect(submitBt, &QPushButton::clicked,
            this,
            [textFields, forward]()
            {
                forward(textFields);
            });
}

void interface::clearLayout(QLayout* layout)
{
    if (!layout)
        return;

    while (QLayoutItem* item = layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        } else if (QLayout* childLayout = item->layout()) {
            clearLayout(childLayout);
            delete childLayout;
        }

        delete item;
    }
}


void interface::changeBottom(const int&& a){
  stack->setCurrentWidget(stackTrace[a]);
} 


void interface::login() {
  const std::string name = loginEdit_->text().toStdString();
  const std::string password = passwordEdit_->text().toStdString();

  graph_.beginAction();
  graph_.ensureEntity(kUserDBKey, "userDB", kUserDBColor);

  const std::uint32_t attemptId = nextUserId_;
  const QString attemptKey = QString("user:%1").arg(attemptId);
  graph_.ensureEntity(attemptKey,
                      QString("User: %1").arg(QString::fromStdString(name)),
                      kUserColor);
  graph_.addRelation(attemptKey, kUserDBKey, "login()");

  if (!db_.login(name, password)) {
    currentUser_.reset();
    currentRegUser_.reset();
    graph_.addRelation(kUserDBKey, attemptKey, "denied");
    setStatus(QString("Wrong login or password: %1").arg(
        QString::fromStdString(name)));
    return;
  }

  currentRegUser_ = std::make_shared<regUser>(nextUserId_++, name, password);
  currentUser_ = currentRegUser_;
  graph_.ensureEntity(userKey(), userNameLabel(), kUserColor);
  graph_.addRelation(kUserDBKey, userKey(), "user");
  graph_.addPersistentRelation(userKey(), kUserDBKey, "stored");
  this->changeBottom(0);
  loginEdit_->clear();
  passwordEdit_->clear();
  setStatus(QString("Logged in as %1").arg(QString::fromStdString(name)));
}

void interface::getMessages() {
  graph_.beginAction();
  graph_.ensureEntity(forumKey(), forumLabel(), kForumColor);

  if (currentUser_) {
    graph_.ensureEntity(userKey(), userNameLabel(), kUserColor);
    graph_.addRelation(userKey(), forumKey(), "getMessages()");
  } else {
    graph_.addRelation(forumKey(), forumKey(), "getMessages()");
  }

  refreshMessages();
}



void interface::sendMessage() {
  const QString text = messageEdit_->text();
  if (!currentUser_) {
    setStatus("Log in first");
    return;
  }
  if (auto forumPtr = currentForum()) {
    graph_.beginAction();
    graph_.ensureEntity(forumKey(), forumLabel(), kForumColor);
    graph_.ensureEntity(userKey(), userNameLabel(), kUserColor);

    const std::string result =
        currentUser_->createMessage(forumPtr, text.toStdString());
    graph_.addRelation(userKey(), forumKey(),
                       result == "Message is send" ? "createMessage()"
                                                   : "createMessage() ✗");
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
        QString("#%1-%2: %3")
            .arg(msg.userID())
            .arg(msg.ID)
            .arg(QString::fromStdString(msg.text())));
  }
}

void interface::setStatus(const QString& text) {
  statusLabel_->setText(text);
}

std::shared_ptr<forum> interface::currentForum() const {
  return forum_;
}

QString interface::userKey() const {
  if (!currentUser_) {
    return "user:none";
  }
  return QString("user:%1").arg(currentUser_->id());
}

QString interface::forumKey() const {
  return QString("forum:%1").arg(QString::fromStdString(forum_->name()));
}

QString interface::forumLabel() const {
  return QString("Forum: %1").arg(QString::fromStdString(forum_->name()));
}

QString interface::userNameLabel() const {
  if (!currentUser_) {
    return "User";
  }
  return QString("User: %1").arg(QString::fromStdString(currentUser_->login()));
}
