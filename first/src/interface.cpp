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
const QString kUserKey = "user";
const QString kForumKey = "forum";
const QString kRegUserKey = "regUser";
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
  auto* clearDiagram = new QPushButton("Clear diagram", main);
  buttonsPage->addWidget(logininto);
  buttonsPage->addWidget(createMessage);
  buttonsPage->addWidget(getMessageList);
  buttonsPage->addWidget(changeLogin);
  buttonsPage->addWidget(deleteMessage);
  buttonsPage->addWidget(addUser);
  buttonsPage->addWidget(clearDiagram);
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

  resetDiagram();

  #pragma endregion


  foot->addWidget(diagram_.view());
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
  connect(clearDiagram, &QPushButton::clicked, this,
          [this]() { resetDiagram(); });
  connect(createMessage, &QPushButton::clicked, this,
          [this]() { messageEdit_->setFocus(); });
  connect(getMessageList, &QPushButton::clicked, this, &interface::getMessages);
  setStatus("Not logged in");
  refreshMessages();
}

void interface::deleteMessageFun(std::vector<QLineEdit*> args) {
  if (args.empty()) {
    return;
  }
  const QString text = args[0]->text();
  const bool removed = forum_->deleteMessage(text.toStdString());

  drawForumCall("deleteMessage()", removed ? "removed" : "not found");

  setStatus(removed ? QString("Message deleted")
                    : QString("Message not found: %1").arg(text));
  refreshMessages();
  changeBottom(0);
}

void interface::createAccoutFun(std::vector<QLineEdit*> args) {
  if (args.size() < 2) {
    return;
  }
  const std::string name = args[0]->text().toStdString();
  const std::string password = args[1]->text().toStdString();

  // Request to add (sent by whoever is using the app) ...
  const bool added = db_.add(name, password);
  if (!added) {
    diagram_.addCall(kUserKey, kUserDBKey, "addUser()", "exists");
    changeBottom(0);
    setStatus(QString("Cannot add user: %1").arg(args[0]->text()));
    return;
  }

  // ... the base receives it, creates the registered user and stores it.
  const std::uint32_t newId = nextUserId_++;
  const QString newKey = QString("%1:%2").arg(kRegUserKey).arg(newId);
  diagram_.ensureParticipant(
      newKey, QString("regUser: %1").arg(args[0]->text()), kUserColor);
  diagram_.addMessage(kUserKey, kUserDBKey, "addUser()");
  diagram_.addMessage(kUserDBKey, kUserDBKey, "receiveRequest()");
  diagram_.addMessage(kUserDBKey, newKey, "createUser()",
                      seq::MessageKind::Create);
  diagram_.addMessage(kUserDBKey, kUserDBKey, "addToDatabase()");

  changeBottom(0);
  loginEdit_->clear();
  passwordEdit_->clear();
  setStatus(QString("User added: %1").arg(args[0]->text()));
}

void interface::changeLoginFun(std::vector<QLineEdit*> args) {
  if (args.empty()) {
    return;
  }

  diagram_.ensureParticipant(kUserKey, userNameLabel(), kUserColor);

  if (!currentRegUser_) {
    diagram_.addCall(kUserKey, kUserDBKey, "changeLogin()", "denied");
    setStatus("Log in first");
    changeBottom(0);
    return;
  }

  const QString newLogin = args[0]->text();
  const bool changed = currentRegUser_->changeLogin(newLogin.toStdString());

  diagram_.ensureParticipant(kUserKey, userNameLabel(), kUserColor);
  diagram_.addMessage(kUserKey, kUserKey,
                      changed ? "changeLogin()" : "changeLogin() ✗");
  if (changed) {
    diagram_.addCall(kUserKey, kUserDBKey, "update()", "ok");
  }

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
  const bool accepted = db_.login(name, password);

  diagram_.ensureParticipant(
      kUserKey, QString("User: %1").arg(QString::fromStdString(name)),
      kUserColor);
  diagram_.addCall(kUserKey, kUserDBKey, "login()",
                   accepted ? "ok" : "denied");

  if (!accepted) {
    currentUser_.reset();
    currentRegUser_.reset();
    setStatus(QString("Wrong login or password: %1").arg(
        QString::fromStdString(name)));
    return;
  }

  currentRegUser_ = std::make_shared<regUser>(nextUserId_++, name, password);
  currentUser_ = currentRegUser_;
  changeBottom(0);
  loginEdit_->clear();
  passwordEdit_->clear();
  setStatus(QString("Logged in as %1").arg(QString::fromStdString(name)));
}

void interface::getMessages() {
  drawForumCall("getMessages()", "messages");
  refreshMessages();
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
    const bool sent = result == "Message is send";

    drawForumCall(sent ? "createMessage()" : "createMessage() ✗",
                  sent ? QString("message") : QString::fromStdString(result));
    setStatus(QString::fromStdString(result));
    if (sent) {
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

void interface::resetDiagram() {
  // Fixed columns, left to right; created users are appended on the right.
  diagram_.beginScenario();
  diagram_.ensureParticipant(kUserKey, userNameLabel(), kUserColor);
  diagram_.ensureParticipant(kForumKey, forumLabel(), kForumColor);
  diagram_.ensureParticipant(kUserDBKey, "userDB", kUserDBColor);
}

void interface::drawForumCall(const QString& method, const QString& reply) {
  diagram_.ensureParticipant(kUserKey, userNameLabel(), kUserColor);
  if (currentUser_) {
    // A logged in user asks the forum and gets an answer back.
    diagram_.addCall(kUserKey, kForumKey, method, reply);
  } else {
    // Nobody is logged in: the forum acts on its own (self message).
    diagram_.addMessage(kForumKey, kForumKey, method);
  }
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
