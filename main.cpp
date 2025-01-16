#include <QApplication>
#include "LoginWindow.h"
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    LoginWindow loginWindow;
    MainWindow mainWindow;

    // 连接登录成功信号到显示主窗口
    QObject::connect(&loginWindow, &LoginWindow::loginSuccessful, &mainWindow, &MainWindow::show);

    loginWindow.show();

    return a.exec();
}
