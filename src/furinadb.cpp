#include "furinadb.h"

FurinaDB::ptr FurinaDB::m_ptr = nullptr;

RecType FurinaDB::execute(const std::string& sql) {
    sqlog(sql, getCurrentTime());
    RecType r;
    std::optional<std::vector<Token>> tokens;
    try {
        tokens = sqlmanager.compile(sql);
    }
    catch (const std::exception& e) {
        r.valid.isValid = false;
        r.valid.reason = SYNTAX_ERROR;
        r.valid.information = "SYNTAX ERROR";
        return r;
    }

    if (!tokens.has_value()) {

		r.valid.isValid = false;
		r.valid.reason = SYNTAX_ERROR;
		r.valid.information = "SYNTAX ERROR";
        return r;
	}
	std::vector<Token> token = tokens.value();
	int pos = 0;

    Token temp = token[pos++];

    std::string old_table_name;
    std::string new_table_name;
    std::string path;
    std::string path2;
    switch (temp.type) {
    case TokenType::USE:

        temp = token[pos++];

        if (database != nullptr) {
            if (database->getBaseName() == temp.value) {
                r.valid.isValid = true;
                r.valid.reason = NOT_FAIL;
                r.valid.information = " ";
                return r;
            }
            else {
                database->close();
            }
        }
        database = std::make_shared<DataBase>(temp.value);
        database->load();
        r.valid.isValid = true;
        r.valid.reason = NOT_FAIL;
        r.valid.information = " ";
        return r;

        break;
    case TokenType::CREATE:

        temp = token[pos++];
        if (temp.type == TokenType::DATABASE) {
            temp = token[pos++];
            if (database != nullptr) {
                database->close();
            }

            std::string root_path = std::format("{}/db", Config::creat()->getConfig(ROOT_PATH));
            for (const auto& entry : std::filesystem::directory_iterator(root_path)) {
                if (std::filesystem::is_directory(entry)) {
                    if (entry.path().filename().string() == temp.value) {
                        r.valid.isValid = false;
                        r.valid.reason = EXIST;
                        r.valid.information = std::format("database {} already exist", temp.value);
                        return r;
                    }
                }
            }



            database = std::make_shared<DataBase>(temp.value);
            database->createNew();

            r.valid.isValid = true;
            r.valid.reason = NOT_FAIL;
            r.valid.information = " ";
            return r;
        }
        else if (temp.type == TokenType::TABLE) {

            if (database == nullptr) {
                r.valid.isValid = false;
                r.valid.reason = DATABASE_CLOSED;
                r.valid.information = "databasae closed";
                return r;
            }

            temp = token[pos++];
            std::string table_name = temp.value;

            for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
                if ((*iter).getName() == table_name) {
                    r.valid.isValid = false;
                    r.valid.reason = EXIST;
                    r.valid.information = std::format("table {} already exist", temp.value);
                    return r;
                }
            }


            std::list<TableMode> modes;
            temp = token[pos++];
            TableMode modee = { "","",0,0,0,1 };
            while (temp.type != TokenType::RIGHT_PAREN) {
                strcpy(modee.sFieldName, temp.value.c_str());
                temp = token[pos++];
                strcpy(modee.sType, temp.value.c_str());
                temp = token[pos++];
                modee.iSize = std::stoi(temp.value);
                temp = token[pos++];
                modee.bKey = std::stoi(temp.value);
                temp = token[pos++];
                modee.bNullFlag = std::stoi(temp.value);
                temp = token[pos++];
                modee.bValidFlag = 1;
                modes.push_back(modee);
            }
            database->createTable(table_name, modes);

            r.valid.isValid = true;
            r.valid.reason = NOT_FAIL;
            r.valid.information = " ";
            return r;
        }
        break;
    case TokenType::DROP:
        // 要删除数据表和索引表
        temp = token[pos++];
        if (temp.type == TokenType::TABLE) {
            temp = token[pos++];
            std::string root_path = std::format("{}/db/{}", Config::creat()->getConfig(ROOT_PATH), database->getBaseName());

            for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
                if ((*iter).getName() == temp.value) {
                    database->getTables().erase(iter);
                    for (const auto& entry : std::filesystem::directory_iterator(root_path)) {
                        if (std::filesystem::is_directory(entry)) {
                            if (entry.path().filename().string().starts_with(temp.value)) {
                                r.valid.isValid = true;
                                r.valid.reason = NOT_FAIL;
                                r.valid.information = " ";
                                std::filesystem::remove_all(entry.path());
                            }
                        }
                    }
                    return r;
                }
            }
            r.valid.isValid = false;
            r.valid.reason = NO_DATABASE;
            r.valid.information = std::format("no table {}", temp.value);
            return r;
        }
        else if (temp.type == TokenType::DATABASE) {
            temp = token[pos++];
            std::string root_path = std::format("{}/db", Config::creat()->getConfig(ROOT_PATH));
            for (const auto& entry : std::filesystem::directory_iterator(root_path)) {
                if (std::filesystem::is_directory(entry)) {
                    if (entry.path().filename().string() == temp.value) {
                        r.valid.isValid = true;
                        r.valid.reason = NOT_FAIL;
                        r.valid.information = " ";

                        if (database != nullptr) {
                            if (temp.value == database->getBaseName()) {
                                database->close();
                                database = nullptr;
                            }
                        }

                        std::filesystem::remove_all(std::format("{}/{}", root_path, temp.value));
                        return r;
                    }
                }
            }
            r.valid.isValid = false;
            r.valid.reason = NO_DATABASE;
            r.valid.information = std::format("no database {}", temp.value);
            return r;
        }

        break;
    case TokenType::SHOW:

        r.valid.isValid = true;
        r.valid.reason = NOT_FAIL;
        r.valid.information = " ";


        temp = token[pos++];
        if (temp.type == TokenType::TABLES) {
            if (database == nullptr) {
                r.valid.isValid = false;
                r.valid.reason = DATABASE_CLOSED;
                r.valid.information = "databasae closed";
                return r;
            }
            r.col_names.emplace_back("table_name");
            r.num_cols = 1;
            r.num_rows = database->getTableNum();
            for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
                std::vector<std::variant<int, double, std::string, std::monostate>> temp;
                temp.push_back((*iter).getName());
                r.data.push_back(temp);
            }
            return r;
        }
        else if (temp.type == TokenType::DATABASES) {
            r.col_names.emplace_back("database_name");
            r.num_cols = 1;
            r.num_rows = 0;
            std::string root_path = std::format("{}/db", Config::creat()->getConfig(ROOT_PATH));
            for (const auto& entry : std::filesystem::directory_iterator(root_path)) {
                if (std::filesystem::is_directory(entry)) {
                    r.num_rows++;
                    std::vector<std::variant<int, double, std::string, std::monostate>> temp;
                    temp.push_back(entry.path().filename().string());
                    r.data.push_back(temp);
                }
            }
            return r;
        }

        break;
    case TokenType::DESC:

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];

        for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
            if ((*iter).getName() == temp.value) {
                r.col_names = { "Field_Name",  "Type", "iSize",  "KEY_Flag", "NULL_Flag" };
                r.num_cols = 5;
                r.num_rows = 0;
                for (auto mode_it = (*iter).getModes().begin(); mode_it != (*iter).getModes().end(); mode_it++) {
                    if (mode_it->bValidFlag) {
                        r.data.push_back({ std::string(mode_it->sFieldName),
                                          std::string(mode_it->sType),
                                          std::to_string(mode_it->iSize),
                                           mode_it->bKey == 0 ? "not key" : "key",
                                           mode_it->bNullFlag == 0 ? "not null" : "can be null" });
                        r.num_rows++;
                    }
                }
                r.valid.isValid = true;
                r.valid.reason = NOT_FAIL;
                r.valid.information = " ";
                return r;
            }
        }

        r.valid.isValid = false;
        r.valid.reason = NO_DATABASE;
        r.valid.information = "no such table";
        return r;


        break;
    case TokenType::RENAME:

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];
        temp = token[pos++];
        old_table_name = temp.value;
        temp = token[pos++];
        new_table_name = temp.value;

        //先改数据库中的表的名字
        for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
            if ((*iter).getName() == old_table_name) {
                iter->resetName(new_table_name);
            }
        }
        //再改bufferpool中所有相关页的信息
        BufferPool::getBufferPool()->resetPageName(old_table_name, new_table_name);

        //要改所有文件的名字
        path = std::format("{}/db/{}", Config::creat()->getConfig(ROOT_PATH), database->getBaseName());
        namespace fs = std::filesystem;
        for (const auto& folder_entry : fs::directory_iterator(path)) {
            if (folder_entry.is_directory()) {
                std::string folder_name = folder_entry.path().filename().string();

                // 如果文件夹的名字以旧的前缀开头，则进行处理
                if (folder_name.rfind(old_table_name, 0) == 0) {
                    // 1. 重命名文件夹
                    fs::path new_folder_path = folder_entry.path().parent_path() / (new_table_name + folder_name.substr(old_table_name.length()));
                    fs::rename(folder_entry.path(), new_folder_path); // 重命名文件夹

                    // 2. 遍历文件夹中的文件，将文件名前缀修改为新前缀
                    for (const auto& file_entry : fs::directory_iterator(new_folder_path)) {
                        if (file_entry.is_regular_file()) {
                            std::string file_name = file_entry.path().filename().string();
                            // 如果文件名以旧前缀开头，则修改文件名
                            if (file_name.rfind(folder_name, 0) == 0) {
                                // 新的文件路径，修改前缀
                                fs::path new_file_path = file_entry.path().parent_path() / (new_table_name + file_name.substr(folder_name.length()));
                                fs::rename(file_entry.path(), new_file_path); // 重命名文件
                            }
                        }
                    }
                }
            }
        }

        r.valid.isValid = true;
        r.valid.reason = NOT_FAIL;
        r.valid.information = " ";
        return r;

        break;
    case TokenType::EDIT:
    {

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }


        temp = token[pos++];
        std::string table_name = temp.value;
        temp = token[pos++];
        std::string new_filed_name = temp.value;
        temp = token[pos++];
        temp = token[pos++];
        temp = token[pos++];
        std::string key_flag = temp.value;
        temp = token[pos++];
        temp = token[pos++];
        std::string field_name = temp.value;


        for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
            if ((*iter).getName() == table_name) {
                for (auto mode_it = (*iter).getModes().begin(); mode_it != (*iter).getModes().end(); mode_it++) {
                    if (!strcmp(field_name.c_str(), mode_it->sFieldName)) {
                        strcpy(mode_it->sFieldName, new_filed_name.c_str());
                        if (mode_it->bKey == 0 && key_flag[0] == '1') {
                            (*iter).createIndex(new_filed_name.c_str());
                        }
                        mode_it->bKey = std::stoi(key_flag);
                    }
                }
                r.valid.isValid = true;
                r.valid.reason = NOT_FAIL;
                r.valid.information = " ";
                return r;
            }
        }

        r.valid.isValid = false;
        r.valid.reason = NO_DATABASE;
        r.valid.information = "no such table or colum";
        return r;
    }
    break;
    case TokenType::ADD:
    {

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];
        std::string table_name = temp.value;
        temp = token[pos++];
        std::string new_filed_name = temp.value;
        temp = token[pos++];
        std::string new_type = temp.value;
        temp = token[pos++];
        std::string new_size = temp.value;
        temp = token[pos++];
        std::string key_flag = temp.value;
        temp = token[pos++];
        std::string null_flag = temp.value;

        if (std::stoi(null_flag) == 0) {
            r.valid.isValid = false;
            r.valid.information = "new col is null";
            return r;
        }
        if (key_flag == "1" && null_flag == "1") {
            r.valid.isValid = false;
            r.valid.information = "key cant be null";
            return r;
        }

        TableMode mode;
        strcpy(mode.sFieldName, new_filed_name.c_str());
        strcpy(mode.sType, new_type.c_str());
        mode.iSize = std::stoi(new_size);
        mode.bKey = std::stoi(key_flag);
        mode.bNullFlag = std::stoi(null_flag);
        mode.bValidFlag = 1;

        for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
            if ((*iter).getName() == table_name) {
                iter->addMode(mode);
            }
        }
        r.valid.isValid = true;
        r.valid.reason = NOT_FAIL;
        r.valid.information = " ";
        return r;
    }
    break;
    case TokenType::INSERT:
    {

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];
        std::string table_name = temp.value;
        std::list<TableMode> modes;
        bool has_table = false;

        auto table_iter = database->getTables().begin();
        for (; table_iter != database->getTables().end(); table_iter++) {
            if ((*table_iter).getName() == table_name) {
                modes = table_iter->getModes();
                has_table = true;
                break;
            }
        }
        if (!has_table) {
            r.valid.isValid = false;
            r.valid.information = "no such table";
            return r;
        }

        size_t num_modes = modes.size();

        temp = token[pos++];
        std::vector<int>indexes;
        std::vector<std::string>types;
        std::vector<std::variant<int, double, std::string, std::monostate>> data(num_modes, std::monostate());
        while (temp.type != TokenType::VALUES) {
            size_t bias = getIndex(modes, temp.value);
            if (bias == SIZE_MAX) {
                r.valid.isValid = false;
                r.valid.information = "no such colonm";
                return r;
            }
            indexes.push_back(bias);
            auto iter = modes.begin();
            std::advance(iter, bias);
            types.push_back(std::string(iter->sFieldName));
            temp = token[pos++];
        }
        temp = token[pos++];
        size_t index = 0;
        while (temp.type != TokenType::SEMICOLON) {
            if (types[index] == "int") {
                data[indexes[index]] = std::stoi(temp.value);
            }
            else if (types[index] == "double")
                data[indexes[index]] = std::stod(temp.value);
            else {
                data[indexes[index]] = temp.value;
            }
            index++;
            temp = token[pos++];
        }
        bool flag = true;
        // 检查空值
        for (int i = 0; i < modes.size(); i++) {
            auto iter = modes.begin();
            std::advance(iter, i);
            std::visit([&iter, &flag](auto&& arg) {
                if (iter->bNullFlag == 0 && iter->bValidFlag && std::is_same_v<std::monostate, std::decay_t<decltype(arg)>>) {
                    flag = false;
                }
                }, data[i]);
        }
        if (flag) {

            table_iter->insert(data);

            r.valid.isValid = true;
            r.valid.reason = NOT_FAIL;
            r.valid.information = " ";
            return r;
        }
        else {
            r.valid.isValid = false;
            r.valid.information = "null colounm";
            return r;
        }
    }
    break;
    case TokenType::UPDATE:
    {

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];
        std::string table_name = temp.value;

        std::list<TableMode> modes;
        bool has_table = false;

        //得到表
        auto table_iter = database->getTables().begin();
        for (; table_iter != database->getTables().end(); table_iter++) {
            if ((*table_iter).getName() == table_name) {
                modes = table_iter->getModes();
                has_table = true;
                break;
            }
        }
        if (!has_table) {
            r.valid.isValid = false;
            r.valid.information = "no such table";
            return r;
        }

        std::vector<int> update_index;
        std::vector<std::variant<int, double, std::string, std::monostate>> values;

        temp = token[pos++]; //set
        temp = token[pos++];
        while (temp.type != WHERE) {
            size_t poss = temp.value.find('=');
            std::string field_name = temp.value.substr(0, poss);
            std::string value = temp.value.substr(poss + 1);

            size_t bias = getIndex(modes, field_name);
            if (bias == SIZE_MAX) {
                r.valid.isValid = false;
                r.valid.information = "no such coloumn";
                return r;
            }
            auto mode_iter = modes.begin();
            std::advance(mode_iter, bias);
            update_index.push_back(bias);
            try {

                if (value == "NULL") {
                    values.push_back(std::monostate());
                }
                else if (!strcmp(mode_iter->sType, "int")) {
                    values.push_back(std::stoi(value));
                }
                else if (!strcmp(mode_iter->sType, "char")) {
                    values.push_back(value);
                }
                else if (!strcmp(mode_iter->sType, "double")) {
                    values.push_back(std::stod(value));
                }

            }
            catch (const std::exception& e) {
                r.valid.isValid = false;
                r.valid.information = "invalid value";
                return r;
            }
            temp = token[pos++];
        }

        table_iter->reset();
        while (true) {
            auto iter = table_iter->getNext();
            if (!iter.has_value())break;
            auto t = condition(pos, token, modes, iter.value());
            if (!t.has_value()) {
                r.valid.isValid = false;
                r.valid.information = "no such coloumn";
                return r;
            }
            if (t.value()) {
                for (int i = 0; i < update_index.size(); i++) {
                    (*(iter.value()))[update_index[i]] = values[i];
                }
            }
        }

        r.valid.isValid = true;
        r.valid.reason = NOT_FAIL;
        r.valid.information = " ";
        return r;

    }
        break;
    case TokenType::DELETE:

        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];
        if (temp.type == TokenType::COL) {
            temp = token[pos++];
            std::string table_name = temp.value;
            temp = token[pos++];
            std::string col_name = temp.value;

            for (auto iter = database->getTables().begin(); iter != database->getTables().end(); iter++) {
                if ((*iter).getName() == table_name) {
                    for (auto mode_it = (*iter).getModes().begin(); mode_it != (*iter).getModes().end(); mode_it++) {
                        if (!strcmp(mode_it->sFieldName, col_name.c_str())) {
                            mode_it->bValidFlag = 0;
                            break;
                        }
                    }
                    r.valid.isValid = true;
                    r.valid.reason = NOT_FAIL;
                    r.valid.information = " ";
                    return r;
                }
            }

        }
        else if (temp.type == TokenType::FROM) {
            temp = token[pos++];
            std::string table_name = temp.value;

            std::list<TableMode> modes;
            bool has_table = false;

            //得到表
            auto table_iter = database->getTables().begin();
            for (; table_iter != database->getTables().end(); table_iter++) {
                if ((*table_iter).getName() == table_name) {
                    modes = table_iter->getModes();
                    has_table = true;
                    break;
                }
            }
            if (!has_table) {
                r.valid.isValid = false;
                r.valid.information = "no such table";
                return r;
            }

            table_iter->reset();
            while (true) {
                auto iter = table_iter->getNext();
                if (!iter.has_value())break;
                auto t = condition(pos, token, modes, iter.value());
                if (!t.has_value()) {
                    r.valid.isValid = false;
                    r.valid.information = "no such coloumn";
                }
                if (t.value()) {
                    table_iter->deleteOne(iter.value());
                    table_iter->backStep();
                }
            }
            r.valid.isValid = true;
            r.valid.reason = NOT_FAIL;
            r.valid.information = " ";
            return r;


        }
        break;
    case TokenType::SELECT:
    {
        if (database == nullptr) {
            r.valid.isValid = false;
            r.valid.reason = DATABASE_CLOSED;
            r.valid.information = "databasae closed";
            return r;
        }

        temp = token[pos++];
        std::vector<int>indexes;
        std::vector<std::string> field_names;
        bool star_flag = true;
        if (temp.type == ASTERISK) {
            star_flag = true;
            temp = token[pos++];
        }
        else {
            star_flag = false;
            while (temp.type != FROM) {
                field_names.push_back(temp.value);
                temp = token[pos++];
            }
        }

        temp = token[pos++]; // 表名
        std::string table_name = temp.value;
        std::list<TableMode> modes;
        bool has_table = false;

        //得到表
        auto table_iter = database->getTables().begin();
        for (; table_iter != database->getTables().end(); table_iter++) {
            if ((*table_iter).getName() == table_name) {
                modes = table_iter->getModes();
                has_table = true;
                break;
            }
        }
        if (!has_table) {
            r.valid.isValid = false;
            r.valid.information = "no such table";
            return r;
        }
        bool join_flag = false;
        //取得需要列的index和表头名字
        size_t num_modes = modes.size();
        if (star_flag) {
            for (int i = 0; i < num_modes; i++) {
                
                auto iter = modes.begin();
                std::advance(iter, i);
                if (iter->bValidFlag) {
                    indexes.push_back(i);
                    r.col_names.push_back(std::string(iter->sFieldName));
                }
            }
        }
        else {
            for (int i = 0; i < field_names.size(); i++) {
                size_t bias = getIndex(modes, field_names[i]);
                if (bias == SIZE_MAX) {
                    r.valid.isValid = false;
                    r.valid.information = "no such colonm";
                    join_flag = true;
                }
                else {
                    indexes.push_back(bias);
                    auto iter = modes.begin();
                    std::advance(iter, bias);
                    r.col_names.push_back(std::string(iter->sFieldName));
                }
            }
        }

        temp = token[pos++];
        table_iter->reset();
        if (temp.type == SEMICOLON) {
            if (join_flag)return r;
            r.num_cols = indexes.size();
            while (true) {
                auto iter = table_iter->getNext();
                std::vector<std::variant<int, double, std::string, std::monostate>> temp;
                if (!iter.has_value())break;
                for (int i = 0; i < indexes.size(); i++) {
                    auto it = iter.value()->begin();
                    temp.push_back(*(it + indexes[i]));

                }
                r.data.push_back(temp);
                r.num_rows++;
            }
            r.valid.isValid = true;
            return r;
        }

        if (temp.type == JOIN) {
           

            TempTable table;

            //处理左表表头
            table.lhs_mode.splice(table.lhs_mode.end(), modes);
            for (auto it = table.lhs_mode.begin(); it != table.lhs_mode.end(); it++) {
                std::string temp = table_name + '.' + it->sFieldName;
                strcpy(it->sFieldName, temp.c_str());
            }
            //加入左表数据
            while (true) {
                auto iter = table_iter->getNext();
                if (!iter.has_value())break;
                std::vector<std::variant<int, double, std::string, std::monostate>> vec3(iter.value()->begin(), iter.value()->end());
                table.lhs.emplace_back(std::move(vec3));
            }



            while (temp.type != WHERE && temp.type != SEMICOLON) {
                temp = token[pos++];
                std::string r_table_name = temp.value;

                std::list<TableMode> r_modes;
                bool r_has_table = false;

                //得到右表
                auto r_table_iter = database->getTables().begin();
                for (; r_table_iter != database->getTables().end(); r_table_iter++) {
                    if ((*r_table_iter).getName() == r_table_name) {
                        r_modes = r_table_iter->getModes();
                        r_has_table = true;
                        break;
                    }
                }
                if (!r_has_table) {
                    r.valid.isValid = false;
                    r.valid.information = "no such table";
                    return r;
                }
                //处理右表表头
                table.rhs_mode.splice(table.rhs_mode.end(), r_modes);
                for (auto it = table.rhs_mode.begin(); it != table.rhs_mode.end(); it++) {
                    std::string temp = r_table_name + '.' + it->sFieldName;
                    strcpy(it->sFieldName, temp.c_str());
                }
                r_table_iter->reset();
                //得到右表数据
                while (true) {
                    auto iter = r_table_iter->getNext();
                    if (!iter.has_value())break;
                    std::vector<std::variant<int, double, std::string, std::monostate>> vec3(iter.value()->begin(), iter.value()->end());
                    table.rhs.emplace_back(std::move(vec3));
                }
                temp = token[pos++];//on

                //取得两边的列号
                temp = token[pos++];
                size_t equal_pos = temp.value.find('=');
                std::string left = temp.value.substr(0, equal_pos);
                std::string right = temp.value.substr(equal_pos + 1);

                size_t l_index = getIndex(table.lhs_mode, left);
                if (l_index == SIZE_MAX) {
                    r.valid.isValid = false;
                    r.valid.information = "no such coloumn";
                    return r;
                }
                size_t r_index = getIndex(table.rhs_mode, right);
                if (r_index == SIZE_MAX) {
                    r.valid.isValid = false;
                    r.valid.information = "no such coloumn";
                    return r;
                }

                //合并两张表
                try {
                    for (int i = 0; i < table.lhs.size(); i++) {
                        for (int j = 0; j < table.rhs.size(); j++) {
                            if (table.lhs[i][l_index] == table.rhs[j][r_index]) {
                                std::vector<std::variant<int, double, std::string, std::monostate>> vec3(table.lhs[i].begin(), table.lhs[i].end());
                                vec3.insert(vec3.end(), table.rhs[j].begin(), table.rhs[j].end());
                                table.t.emplace_back(vec3);
                            }
                        }
                    }
                }
                catch (const std::exception& e) {
                    r.valid.isValid = false;
                    r.valid.information = "invalid compare";
                    return r;
                }

                //将合并后的表转移到左表
                table.lhs = std::move(table.t);
                table.lhs_mode.splice(table.lhs_mode.end(), table.rhs_mode);

                temp = token[pos++];
            }

            //设置要输出的索引
            indexes.clear();
            r.col_names.clear();
            r.data.clear();
            r.num_cols = 0;
            r.num_rows = 0;
            size_t num_modes = table.lhs_mode.size();
            if (star_flag) {
                for (int i = 0; i < num_modes; i++) {
                    
                    auto iter = table.lhs_mode.begin();
                    std::advance(iter, i);
                    if (iter->bValidFlag) {
                        indexes.push_back(i);
                        r.col_names.push_back(std::string(iter->sFieldName));
                    }
                }
            }
            else {
                for (int i = 0; i < field_names.size(); i++) {
                    size_t bias = getIndex(table.lhs_mode, field_names[i]);
                    if (bias == SIZE_MAX) {
                        r.valid.isValid = false;
                        r.valid.information = "no such colonm";
                        return r;
                    }
                    indexes.push_back(bias);
                    auto iter = table.lhs_mode.begin();
                    std::advance(iter, bias);
                    r.col_names.push_back(std::string(iter->sFieldName));
                }
            }
            if (temp.type == WHERE) {
                r.num_cols = indexes.size();
                while (true) {
                    auto iter = table.getNext();
                    if (!iter.has_value())break;
                    auto t = condition(pos, token, table.lhs_mode, iter.value());
                    if (!t.has_value()) {
                        r.valid.isValid = false;
                        r.valid.information = "no such coloumn";
                    }
                    if (t.value()) {
                        std::vector<std::variant<int, double, std::string, std::monostate>> temp;
                        for (int i = 0; i < indexes.size(); i++) {
                            auto it = iter.value()->begin();
                            temp.push_back(*(it + indexes[i]));
                        }
                        r.data.push_back(temp);
                        r.num_rows++;
                    }
                }
                r.valid.isValid = true;
                return r;
            }
            else {
                r.num_cols = indexes.size();
                while (true) {
                    auto iter = table.getNext();
                    std::vector<std::variant<int, double, std::string, std::monostate>> temp;
                    if (!iter.has_value())break;
                    for (int i = 0; i < indexes.size(); i++) {
                        auto it = iter.value()->begin();
                        temp.push_back(*(it + indexes[i]));

                    }
                    r.data.push_back(temp);
                    r.num_rows++;
                }
                r.valid.isValid = true;
                return r;
            }

        }
        

            r.num_cols = indexes.size();
            while (true) {
                auto iter = table_iter->getNext();
                if (!iter.has_value())break;
                auto t = condition(pos, token, modes, iter.value());
                if (!t.has_value()) {
                    r.valid.isValid = false;
                    r.valid.information = "no such coloumn";
                }
                if (t.value()) {
                    std::vector<std::variant<int, double, std::string, std::monostate>> temp;
                    for (int i = 0; i < indexes.size(); i++) {
                        auto it = iter.value()->begin();
                        temp.push_back(*(it + indexes[i]));
                    }
                    r.data.push_back(temp);
                    r.num_rows++;
                }
            }
            r.valid.isValid = true;
            return r;
        }
        


    
        break;
    case ALTER:
    {
        temp = token[pos++];
        std::string old_password = temp.value;
        temp = token[pos++];
        std::string new_password = temp.value;
        temp = token[pos++];
        if (new_password != temp.value) {
            r.valid.isValid = false;
            r.valid.information = "two password differernt";
            return r;
        }

        MD5 md5(old_password);
        if (md5.md5() != Config::creat()->getConfig(PASSWORD)) {
            r.valid.isValid = false;
            r.valid.information = "wrong password";
            return r;
        }

        md5 = MD5(new_password);
        Config::creat()->updatePassword(md5.md5());

        r.valid.isValid = true;
        r.valid.information = " ";
        return r;
    }

        break;
    default:
        r.valid.isValid = false;
        r.valid.reason = SYNTAX_ERROR;
        r.valid.information = "SYNTAX ERROR";
        return r;
        break;
    }

}



std::vector<int> FurinaDB::in2post(const std::vector<int>& in_fix) {
    std::stack<int> fuhao;
    std::vector<int>post;
    for (int i = 0; i < in_fix.size(); i++) {
        if (in_fix[i] == 0 || in_fix[i] == 1) {
            post.push_back(in_fix[i]);
        }
        else if (in_fix[i] == TokenType::LEFT_PAREN) {
            fuhao.push(in_fix[i]);
        }
        else if (in_fix[i] == TokenType::RIGHT_PAREN) {
            while (!fuhao.empty() && fuhao.top() != TokenType::LEFT_PAREN) {
                post.push_back(fuhao.top());
                fuhao.pop();
            }
        }
        else {
            while (!fuhao.empty() && fuhao.top() != TokenType::LEFT_PAREN && fuhao.top() > in_fix[i]) {
                post.push_back(fuhao.top());
                fuhao.pop();
            }
            fuhao.push(in_fix[i]);
        }
    }
    while (!fuhao.empty()) {
        if (fuhao.top() != LEFT_PAREN)post.push_back(fuhao.top());
        fuhao.pop();
    }

    return post;
}

bool FurinaDB::calculate(const std::vector<int>& post_fix) {
    std::stack<int> stk;
    for (int i = 0; i < post_fix.size(); i++) {
        if (post_fix[i] == 0 || post_fix[i] == 1) {
            stk.push(post_fix[i]);
        }
        else if (post_fix[i] == 4) {
            int a, b;
            a = stk.top();
            stk.pop();
            b = stk.top();
            stk.pop();
            stk.push(a | b);
        }
        else if (post_fix[i] == 5) {
            int a, b;
            a = stk.top();
            stk.pop();
            b = stk.top();
            stk.pop();
            stk.push(a & b);
        }
        else if (post_fix[i] == 6) {
            int a, b;
            a = stk.top();
            stk.pop();
            stk.push(~a);
        }
    }
    return stk.top() == 1;
}

size_t FurinaDB::getIndex(const std::list<TableMode>& modes, const std::string& field_name) {
    for (auto iter = modes.begin(); iter != modes.end(); iter++) {
        if (!strcmp(iter->sFieldName, field_name.c_str()) && iter->bValidFlag) {
            return std::distance(modes.begin(), iter);
        }
    }
    return SIZE_MAX;
}


std::optional<bool> FurinaDB::condition(int pos, const std::vector<Token>& token, const std::list<TableMode>& modes,
  std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter) {

    Token temp;
    std::vector<int> in_fix;
    temp = token[pos++];
    while (temp.type != SEMICOLON) {
        if (temp.value == "NOT") {
            in_fix.push_back(6);
        }
        else if (temp.value == "AND") {
            in_fix.push_back(5);
        }
        else if (temp.value == "OR") {
            in_fix.push_back(4);
        }
        else if (temp.type == LEFT_PAREN) {
            in_fix.push_back(2);
        }
        else if (temp.type == RIGHT_PAREN) {
            in_fix.push_back(3);
        }
        else if (temp.type == IDENTIFIER) {
            int a = processItem(temp, modes, iter);
            if (a == -1)return std::nullopt;
            in_fix.push_back(a);
        }
        temp = token[pos++];
    }
    return calculate(in2post(in_fix));

}

int FurinaDB::processItem(const Token& token, const std::list<TableMode>& modes,
    std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter) {

    //分别找=><^
    size_t pos = 0;
    char op = ' ';
    if (token.value.find('=') != std::string::npos) {
        pos = token.value.find('=');
        op = '=';
    }
    else if (token.value.find('>') != std::string::npos) {
        pos = token.value.find('>');
        op = '>';
    }
    else if (token.value.find('<') != std::string::npos) {
        pos = token.value.find('<');
        op = '<';
    }
    else if (token.value.find('^') != std::string::npos) {
        pos = token.value.find('^');
        op = '^';
    }

    std::string field_name = token.value.substr(0, pos);
    std::string value = token.value.substr(pos+1);
    size_t bias = getIndex(modes, field_name);
    if (bias == SIZE_MAX) {
        return -1;
    }
    auto it = modes.begin();
    std::advance(it, bias);
    std::string type = it->sType;
    auto itt = iter->begin() + bias;
    bool flag = false;

    try {
        if (std::get_if<int>(&(*itt))) {
            if (op == '=') {
                return std::get<int>(*itt) == std::stoi(value);
            }
            else if (op == '>') {
                return std::get<int>(*itt) > std::stoi(value);
            }
            else if (op == '<') {
                return std::get<int>(*itt) < std::stoi(value);
            }
            else if (op == '^') {
                return std::get<int>(*itt) != std::stoi(value);
            }
        }
        else if (std::get_if<double>(&(*itt))) {
            if (op == '=') {
                return std::get<double>(*itt) == std::stod(value);
            }
            else if (op == '>') {
                return std::get<double>(*itt) > std::stod(value);
            }
            else if (op == '<') {
                return std::get<double>(*itt) < std::stod(value);
            }
            else if (op == '^') {
                return std::get<double>(*itt) != std::stod(value);
            }
        }
        else if (std::get_if<std::string>(&(*itt))) {
            if (op == '=') {
                return std::get<std::string>(*itt) == value;
            }
            else if (op == '>') {
                return std::get<std::string>(*itt) > value;
            }
            else if (op == '<') {
                return std::get<std::string>(*itt) < value;
            }
            else if (op == '^') {
                return std::get<std::string>(*itt) != value;
            }
        }
        else if (std::is_same_v<std::monostate, std::decay_t<decltype(*itt)>>) {
            return -1;
        }
    }
    catch (const std::exception& e) {
        return -1;
    }
}

