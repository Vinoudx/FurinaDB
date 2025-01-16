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

	//����һ���µ����ݿ�
	void createNew();
	//�����������ݿ�
	void load();
	//����һ�ű�
	void createTable(const std::string& table_name, const std::list<TableMode>& modes);
	//�ر����ݿ⣬����Ӧ����д��
	void close();

	std::string getBaseName() { return m_base_name; }
	int getTableNum() { return m_tables.size(); }
	std::list<Table>& getTables() { return m_tables; }

	~DataBase() = default;


private:
	bool m_isEdited = false; // ���û�ж����ݿ�����޸ģ�����Ҫ���±���
	std::string m_base_name;
	size_t m_num_tables = 0;
	std::list<Table> m_tables;

};


#endif 
