#ifndef _FURINADB_DATABASE_
#define _FURINADB_DATABASE_

#include <string>
#include <fstream>
#include <list>
#include <filesystem>
#include <memory>
#include <format>

#include "table.h"
#include "config.h"

class DataBase {
public:
	typedef std::shared_ptr<DataBase> ptr;
	DataBase(const std::string& base_name):m_base_name(base_name){
		
	}

	//创建一个新的数据库
	void createNew();
	//载入已有数据库
	void load();
	//创建一张表
	void createTable(const std::string& table_name, const std::list<TableMode>& modes);
	//关闭数据库，将相应数据写回
	void close();

	std::string getBaseName() { return m_base_name; }
	int getTableNum() { return m_tables.size(); }
	std::list<Table>& getTables() { return m_tables; }

	~DataBase() = default;


private:
	bool m_isEdited = false; // 如果没有对数据库进行修改，则不需要重新保存
	std::string m_base_name;
	size_t m_num_tables = 0;
	std::list<Table> m_tables;

};


#endif 
