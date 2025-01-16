#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPainter>   // 用于绘制背景图像
#include <QPixmap>    // 用于加载图片
#include <QResizeEvent>  // 用于处理窗口大小变化事件（可选）

#include <string>

#include "config.h"
#include "md5.h"

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);

signals:
    void loginSuccessful();  // 登录成功时发射的信号

protected:
    void paintEvent(QPaintEvent *event) override;  // 重写paintEvent以绘制背景图像

private:
    QLineEdit *usernameLineEdit;  // 用户名输入框
    QLineEdit *passwordLineEdit;  // 密码输入框
    QPushButton *loginButton;     // 登录按钮

private slots:
    void onLoginClicked();  // 登录按钮的槽函数

};

#endif // LOGINWINDOW_H
