#include "LoginWindow.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QResizeEvent>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("FurinaDB");
    resize(800, 400);

    // 创建组件
    QLabel *usernameLabel = new QLabel("用户名:", this);
    QLabel *passwordLabel = new QLabel("密码:", this);
    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login", this);

    //修改字体大小
    usernameLabel->setStyleSheet("font-size: 24px;");
    passwordLabel->setStyleSheet("font-size: 24px;");
    usernameLineEdit->setStyleSheet("font-size:24px;");
    passwordLineEdit->setStyleSheet("font-size: 24px;");
    loginButton->setStyleSheet("font-size: 24px;");

    //设置颜色为半透明
    usernameLineEdit->setStyleSheet("background-color: rgba(255, 255, 255, 128);"
                                    "font-size: 24px;");
    passwordLineEdit->setStyleSheet("background-color: rgba(255, 255, 255, 128);"
                                    "font-size: 24px;");
    loginButton->setStyleSheet("background-color: rgba(255, 255, 255, 128);"
                               "font-size: 24px;");


    // Position the widgets
    usernameLabel->move(100, 80);
    usernameLineEdit->move(300, 80);
    passwordLabel->move(100, 183);
    passwordLineEdit->move(300, 183);
    loginButton->move(300, 322);

    // 连接信号和槽
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

// Override the paintEvent method to draw the background image
void LoginWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Load the pixmap and stretch it to fill the widget
    QPixmap pixmap("D:/work/furinadb/furinadb/gui/furina.jpg");
    painter.drawPixmap(0, 0, width(), height(), pixmap);

    // Call the base class's paintEvent to handle normal painting
    QWidget::paintEvent(event);
}

void LoginWindow::onLoginClicked() {
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();
    MD5 md5(password.toStdString());
    std::string pwd_md5 = md5.md5();

    // Simple username and password validation
    if (username == Config::creat()->getConfig(USER_NAME) && pwd_md5 == Config::creat()->getConfig(PASSWORD)) {
        emit loginSuccessful();  // Emit the login success signal
        close();  // Close the login window
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password.");
    }
}
