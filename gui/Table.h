#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>

#include "furinadb.h"

class Table1 : public QWidget {
    Q_OBJECT

public:
    explicit Table1(const QString &dbName, const QString& tName, QWidget *parent = nullptr);

private:
    QTableWidget *dbTable;          // 显示表列表
    QTableWidget *structure;
    QLineEdit *SQLStatement;        // 输入框
    QTableWidget *textTable;
    QString databaseName;
    QString tableName;
    QPushButton *createsqlButton;   // 执行sql按钮
    QPushButton *logoutButton;      // 退出按钮

    void displayDatabases();        // 显示数据库的方法
    void onExcuteClicked();

private slots:
    void onLogoutClicked();         // 退出按钮的槽函数

};

#endif // TABLE_H
