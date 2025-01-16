#ifndef DATABASEDETAILWINDOW_H
#define DATABASEDETAILWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel.h>

#include "furinadb.h"

class DatabaseDetailWindow : public QWidget {
    Q_OBJECT

public:
    explicit DatabaseDetailWindow(const QString &dbName, QWidget *parent = nullptr);

private:
    QTableWidget *dbTable;          // 显示数据库列表
    QLineEdit *SQLStatement;        // 输入框
    QPushButton *createsqlButton;   // 执行sql按钮
    QPushButton *logoutButton;      // 退出按钮
    QString databaseName;           // 数据库名称
    QTableWidget *textTable;

    void displayDatabaseDetails();  // 显示具体的数据库详情
    void onExcuteClicked();

private slots:
    void onLogoutClicked();         // 退出按钮的槽函数
    void onCellClicked(int row, int column);  // 处理单元格点击事件
};

#endif // DATABASEDETAILWINDOW_H
