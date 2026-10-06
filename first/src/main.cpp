#include "interface.h"

#include <QApplication>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  interface window;
  window.show();
  return app.exec();
}
