#ifndef _FURINADB_TABLE_
#define _FURINADB_TABLE_

#include <string>
#include <list>
#include <fstream>
#include <format>
#include <memory>
#include <filesystem>

#include "page.h"
#include "config.h"
#include "utils.h"
#include "bufferpool.h"





class Table {
	friend class Cursor;
public:
	typedef std::shared_ptr<Table> ptr;
	Table(const std::string& base_name,const std::string& table_name, const std::list<TableMode>& modes) :
		m_table_name(table_name),m_base_name(base_name), m_modes(modes){
		m_num_modes = modes.size();
	}

	Table(const char* type, const std::string& base_name, const std::string& table_name, const std::list<TableMode>& modes) :
		m_table_name(table_name), m_base_name(base_name), m_modes(modes){
		strcpy(m_type, type);
		m_num_modes = modes.size();
	}

	Table(Table&& rhs){swap(rhs);}
	void swap(Table& rhs);

	std::string getName() { return m_table_name; }
	size_t getModeNum() { return m_num_modes; }
	size_t getPageNum() { return m_num_pages; }
	std::list<TableMode>& getModes() { return m_modes; }
	void resetName(const std::string& new_name) { m_table_name = new_name; }

	Table& addMode(const TableMode& mode);

	void firstCreate();// 第一次创建一张表，需要建立数据文件,和第一个页
	void save();//将元数据保存到.dat文件中
	void load();//从.dat文件的头部获得数据

	void createIndex(const char* index_name); //创建索引表
	void insertIndex(const std::variant<int,double,std::string, std::monostate>& index_value, int page_id);
	size_t getPgIdFromIndex() {};

	void insert(const std::vector<std::variant<int, double, std::string, std::monostate>>& data);

	//cursor操作
	std::optional<std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator> getNext();
	void deleteOne(std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter);
	//删除一条记录后要调用backstep因为vector中删除一条后后面的记录会前移
	//所以当前行的行号应该就是删除的那行的行号
	void backStep() {
		//如果是第一条就不用退
		if (m_current_record == 0)return;
		m_current_record--;
	}

	void reset() {
		m_current_page = 0;
		m_current_record = 0;
	}

private:
	std::string m_table_name;
	std::string m_base_name;
	char m_type[5] = "data";
	size_t m_num_pages = 0;
	size_t m_num_modes = 0;
	std::list<TableMode> m_modes;

	size_t m_current_page = 0;
	size_t m_current_record = 0;

};


class TempTable {
public:
	//cursor操作
	std::optional<std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator> getNext();

	size_t m_current_record = 0;
	std::list<TableMode> lhs_mode;
	std::list<TableMode> rhs_mode;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> lhs;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> rhs;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> t;
};





#endif
