#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

#include "furinadb.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QTableWidget *dbTable;          // 显示数据库列表
    QLineEdit *SQLStatement;        // 输入框
    QTableWidget *textTable;
    QPushButton *createsqlButton;   // 执行sql按钮
    QPushButton *logoutButton;      // 退出按钮

    void displayDatabases();        // 显示数据库的方法

private slots:
    void onLogoutClicked();         // 退出按钮的槽函数
    void onCellClicked(int row, int column);  // 处理单元格点击事件
    void onExcuteClicked();
};

#endif // MAINWINDOW_H
