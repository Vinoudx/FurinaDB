#include "Table.h"
#include <QVBoxLayout>

Table1::Table1(const QString &dbName, const QString& tName, QWidget *parent)
    : QWidget(parent), databaseName(dbName), tableName(tName) {
    setWindowTitle("Database Details: " + databaseName);
    resize(800, 800);
    setWindowOpacity(1.0);
    setStyleSheet("background-color: rgba(255, 255, 255, 255);");
    // 创建控件
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel* label1 = new QLabel(std::format("Database: {}, Table: {}, 内容",databaseName.toStdString(), tableName.toStdString()).c_str(), this);

    dbTable = new QTableWidget(this);
    dbTable->setColumnCount(1);  // 设置列数为1
    dbTable->setHorizontalHeaderLabels({" "});  // 设置表头
    dbTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 自动调整列宽

    QLabel* label2 = new QLabel(std::format("Database: {}, Table: {}, 结构", databaseName.toStdString(), tableName.toStdString()).c_str(), this);
    label1->setAlignment(Qt::AlignCenter);
    label2->setAlignment(Qt::AlignCenter);

    structure = new QTableWidget(this);
    structure->setColumnCount(1);  // 设置列数为1
    structure->setHorizontalHeaderLabels({" "});  // 设置表头
    structure->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 自动调整列宽

    textTable = new QTableWidget(this);
    textTable->setColumnCount(1);  // 设置列数为1
    textTable->setHorizontalHeaderLabels({"Result"});  // 设置表头
    textTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 自动调整列宽

    SQLStatement = new QLineEdit(this);
    SQLStatement->setPlaceholderText("Enter SQL Statement");

    createsqlButton = new QPushButton("Execute", this);
    logoutButton = new QPushButton("Back", this);


    mainLayout->addWidget(label1);
    mainLayout->addWidget(dbTable);
    mainLayout->addWidget(label2);
    mainLayout->addWidget(structure);
    mainLayout->addWidget(textTable);
    mainLayout->addWidget(SQLStatement);
    mainLayout->addWidget(createsqlButton);
    mainLayout->addWidget(logoutButton);

    setLayout(mainLayout);  // 设置布局

    // 显示数据库详情
    displayDatabases();

    // 连接退出按钮
    connect(logoutButton, &QPushButton::clicked, this, &Table1::close);
    connect(createsqlButton, &QPushButton::clicked, this, &Table1::onExcuteClicked);
}

void Table1::displayDatabases() {
    dbTable->clear();
    dbTable->clearContents();  // 清除表格内容
    dbTable->setRowCount(0);  // 清除当前行数
    structure->clearContents();  // 清除表格内容
    structure->setRowCount(0);  // 清除当前行数
    QStringList message;
    FurinaDB::ptr db = FurinaDB::createDB();
    db->execute(std::format("USE {};", databaseName.toStdString()));
    RecType r = db->execute(std::format("SELECT * FROM {};", tableName.toStdString()));
    if (r.valid.isValid) {
        dbTable->setColumnCount(r.num_cols);

        QStringList headers;
        for (int k = 0; k < r.num_cols; k++) {
            headers.push_back(r.col_names[k].c_str());
        }
        dbTable->insertRow(0);
        for (int k = 0; k < headers.size(); ++k) {
            dbTable->setItem(0, k, new QTableWidgetItem(headers.at(k)));
        }
        dbTable->insertRow(1);
        for (int k = 0; k < headers.size(); ++k) {
            dbTable->setItem(1, k, new QTableWidgetItem("--------------------------------"));
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
            dbTable->insertRow(i + 2);
            for (int k = 0; k < message.size(); ++k) {
                dbTable->setItem(i + 2, k, new QTableWidgetItem(message.at(k)));
            }
        }
    }
    else {
        dbTable->insertRow(0);
        dbTable->setItem(0, 0, new QTableWidgetItem(r.valid.information.c_str()));
    }


    structure->clear();
    QStringList message1;
    RecType r1 = db->execute(std::format("DESC {};", tableName.toStdString()));
    if (r1.valid.isValid) {
        structure->setColumnCount(r1.num_cols);

        QStringList headers;
        for (int k = 0; k < r1.num_cols; k++) {
            headers.push_back(r1.col_names[k].c_str());
        }
        structure->insertRow(0);
        for (int k = 0; k < headers.size(); ++k) {
            structure->setItem(0, k, new QTableWidgetItem(headers.at(k)));
        }
        structure->insertRow(1);
        for (int k = 0; k < headers.size(); ++k) {
            structure->setItem(1, k, new QTableWidgetItem("--------------------------------"));
        }

        for (int i = 0; i < r1.num_rows; i++) {
            message1.clear();
            for (int j = 0; j < r1.num_cols; j++) {
                std::visit([&](auto&& arg) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) {
                        int content1 = std::get<int>(r1.data[i][j]);
                        message1.push_back(std::to_string(content1).c_str());
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
                        double content2 = std::get<double>(r1.data[i][j]);
                        message1.push_back(std::to_string(content2).c_str());
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                        std::string content3 = std::get<std::string>(r1.data[i][j]);
                        message1.push_back(content3.c_str());
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::monostate>) {
                        message1.push_back("NULL");
                    }
                    }, r1.data[i][j]);
            }
            structure->insertRow(i + 2);
            for (int k = 0; k < message1.size(); ++k) {
                structure->setItem(i + 2, k, new QTableWidgetItem(message1.at(k)));
            }
        }
    }
    else {
        structure->insertRow(0);
        structure->setItem(0, 0, new QTableWidgetItem(r1.valid.information.c_str()));
    }

}



// 退出按钮的槽函数实现
void Table1::onLogoutClicked() {
    close();  // 退出应用
}

void Table1::onExcuteClicked() {
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
            textTable->setItem(0, k, new QTableWidgetItem(headers.at(k)));
        }
        textTable->insertRow(2);
        for (int k = 0; k < headers.size(); ++k) {
            textTable->setItem(1, k, new QTableWidgetItem("--------------------------------"));
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
