#include "MainWindow.h"
#include "DatabaseDetailWindow.h"
#include <QVBoxLayout>
#include <QMessageBox>

#include "furinadb.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("FurinaDB");
    resize(800, 800);

    // 创建控件
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel *label1 = new QLabel("Databases", this);

    dbTable = new QTableWidget(this);
    dbTable->setColumnCount(1);  // 设置列数为1
    dbTable->setHorizontalHeaderLabels({" "});  // 设置表头
    dbTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 自动调整列宽

    QLabel* label2 = new QLabel("Result", this);
    label1->setAlignment(Qt::AlignCenter);
    label2->setAlignment(Qt::AlignCenter);

    textTable = new QTableWidget(this);
    textTable->setColumnCount(1);  // 设置列数为1
    textTable->setHorizontalHeaderLabels({" "});  // 设置表头
    textTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 自动调整列宽

    SQLStatement = new QLineEdit(this);
    SQLStatement->setPlaceholderText("Enter SQL Statement");

    createsqlButton = new QPushButton("Execute", this);
    logoutButton = new QPushButton("Exit", this);

    mainLayout->addWidget(label1);
    mainLayout->addWidget(dbTable);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(textTable);
    mainLayout->addWidget(SQLStatement);
    mainLayout->addWidget(createsqlButton);
    mainLayout->addWidget(logoutButton);

    setLayout(mainLayout);  // 设置布局

    // 显示数据库列表
    displayDatabases();

    // 连接信号与槽
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    connect(dbTable, &QTableWidget::cellClicked, this, &MainWindow::onCellClicked);  // 连接单元格点击事件
    connect(createsqlButton, &QPushButton::clicked, this, &MainWindow::onExcuteClicked);
}

void MainWindow::displayDatabases() {
    dbTable->clearContents();  // 清除表格内容
    dbTable->setRowCount(0);  // 清除当前行数
    FurinaDB::ptr db = FurinaDB::createDB();
    RecType r = db->execute("SHOW DATABASES;");
    QStringList databases;
    if (r.valid.isValid) {
        for (int i = 0; i < r.num_rows; i++) {
            databases.push_back(std::get<std::string>(r.data[i][0]).c_str());
        }
    }
    else {
        textTable->insertRow(0);
        textTable->setItem(0, 0, new QTableWidgetItem(r.valid.information.c_str()));
    }

    // 模拟填充数据库数据（在实际应用中，可以从文件或数据库获取数据）

    for (int i = 0; i < databases.size(); ++i) {
        dbTable->insertRow(i);  // 插入行
        dbTable->setItem(i, 0, new QTableWidgetItem(databases.at(i)));  // 设置每个数据库名称
    }
}

// 处理单元格点击事件
void MainWindow::onCellClicked(int row, int column) {
    // 获取用户点击的数据库名称
    QString databaseName = dbTable->item(row, column)->text();

    // 创建并显示详细窗口
    DatabaseDetailWindow *detailWindow = new DatabaseDetailWindow(databaseName, this);
    detailWindow->show();
}

// 退出按钮的槽函数实现
void MainWindow::onLogoutClicked() {
    close();  // 退出应用
}

void MainWindow::onExcuteClicked() {
    QString sql_ = SQLStatement->text();
    std::string sql = sql_.toStdString();
    SQLStatement->clear();
    textTable->clear();
    textTable->clearContents();  // 清除表格内容
    textTable->setRowCount(0);  // 清除当前行数
    QStringList message;
    FurinaDB::ptr db = FurinaDB::createDB();
    RecType r = db->execute(sql);
    if (r.valid.isValid) {
        textTable->setColumnCount(r.num_cols);

        textTable->insertRow(0);  // 插入行
        textTable->setItem(0, 0, new QTableWidgetItem("OK"));

        QStringList headers;
        for (int k = 0; k < r.num_cols; k++) {
            headers.push_back(r.col_names[k].c_str());
        }
        textTable->insertRow(1);
        for (int k = 0; k < headers.size(); ++k) {
            textTable->setItem(1, k, new QTableWidgetItem(headers.at(k)));
        }
        textTable->insertRow(2);
        for (int k = 0; k < headers.size(); ++k) {
            textTable->setItem(2, k, new QTableWidgetItem("--------------------------------"));
        }

        for (int i = 0; i < r.num_rows; i++) {
            message.clear();
            for (int j = 0; j < r.num_cols; j++) {
                std::visit([&](auto&& arg) {
                	if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) {
                		int content1 = std::get<int>(r.data[i][j]);
                        message.push_back(std::to_string(content1).c_str());
                	}
                	else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
                		double content2 = std::get<double>(r.data[i][j]);
                        message.push_back(std::to_string(content2).c_str());
                	}
                	else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                		std::string content3 = std::get<std::string>(r.data[i][j]);
                        message.push_back(content3.c_str());
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::monostate>) {
                        message.push_back("NULL");
                    }
                }, r.data[i][j]);
            }
            textTable->insertRow(i + 3);
            for (int k = 0; k < message.size(); ++k) { 
                textTable->setItem(i + 3, k, new QTableWidgetItem(message.at(k)));
            }
        }

    }
    else {
        textTable->insertRow(0);
        textTable->setItem(0, 0, new QTableWidgetItem(r.valid.information.c_str()));
    }

    displayDatabases();
}
