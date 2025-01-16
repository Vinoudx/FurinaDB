#include "database.h"

void DataBase::createNew() {
	m_num_tables = 0;
	//首先创建一个根文件夹 
	std::string root_path = Config::creat()->getConfig(ROOT_PATH);
	std::string database_root_dir = std::format("{}/db/{}", root_path, m_base_name);
	if (!std::filesystem::create_directory(database_root_dir)) {
		mylog(__FILE__, __LINE__, std::format("database {} already exsist", m_base_name), LogLevel::Level::FATAL);
	}
	//其次在根文件夹中创建一个库信息文件

	std::ofstream file;
	file.open(std::format("{}/{}.dbf", database_root_dir, m_base_name), std::ios::binary);

	if (!file) {
		mylog(__FILE__, __LINE__, std::format("unable to create database {}", m_base_name), LogLevel::Level::FATAL);
	}
	file.close();
	mylog(__FILE__, __LINE__, std::format("create database {}", m_base_name), LogLevel::Level::INFO);
}

void DataBase::createTable(const std::string& table_name, const std::list<TableMode>& modes) {
	Table table(m_base_name, table_name, modes);
	table.firstCreate();
	m_tables.emplace_back(std::move(table));
	m_num_tables++;
}

void DataBase::close() {
	std::ofstream file;
	std::string root_path = Config::creat()->getConfig(ROOT_PATH);
	std::string database_root_dir = std::format("{}/db/{}", root_path, m_base_name);
	file.open(std::format("{}/{}.dbf", database_root_dir, m_base_name), std::ios::binary);
	if (!file) {
		mylog(__FILE__, __LINE__, std::format("unable to save database {}", m_base_name), LogLevel::Level::FATAL);
	}
	
	//首先写表数量
	file.write((const char*)&m_num_tables, sizeof(size_t));

	//再写表头信息
	for (auto& table : m_tables) {
		//先写数据表名
		file.write(table.getName().c_str(), Config::creat()->getConfig(MAX_NAME_LENGTH) * sizeof(char));

		//先写字段数量
		size_t num_modes = table.getModeNum();
		file.write((const char*)&num_modes, sizeof(size_t));
		//再写字段类型
		std::list<TableMode> modes = table.getModes();
		for (auto& mode : modes) {
			file.write((const char*)&mode, sizeof(TableMode));
		}
		//关闭每张表
		table.save();
	}


	file.close();

	//m_pool->savePages();
	mylog(__FILE__, __LINE__, std::format("close database {}", m_base_name), LogLevel::Level::INFO);
}

void DataBase::load() {
	mylog(__FILE__, __LINE__, std::format("load database {}", m_base_name), LogLevel::Level::INFO);
	
	std::ifstream file;
	std::string root_path = Config::creat()->getConfig(ROOT_PATH);
	std::string database_root_dir = std::format("{}/db/{}", root_path, m_base_name);
	file.open(std::format("{}/{}.dbf", database_root_dir, m_base_name), std::ios::binary | std::ios::in);
	if (!file) {
		mylog(__FILE__, __LINE__, std::format("unable to load database {}", m_base_name), LogLevel::Level::FATAL);
	}


	//读数据表数量
	size_t num_table = 0;
	file.read((char*)&num_table, sizeof(size_t));
	m_num_tables = num_table;

	for (size_t i = 0; i < num_table; i++) {
		std::list<TableMode> modes;
		//读字段名
		char buffer1[1024] = { 0 };
		file.read(buffer1, Config::creat()->getConfig(MAX_NAME_LENGTH) * sizeof(char));
		std::string table_name(buffer1);
		//读字段数量
		size_t num_modes;
		file.read((char*)&num_modes, sizeof(size_t));
		//读字段类型
		for (size_t j = 0; j < num_modes; j++) {
			TableMode tm;
			file.read((char*)&tm, sizeof(TableMode));
			modes.push_back(tm);
		}
		//创建表
		Table table(m_base_name, table_name, modes);
		table.load();
		m_tables.push_back(std::move(table));
	}
	file.close();
	
}