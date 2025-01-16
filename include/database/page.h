#ifndef _FURINADB_PAGE_
#define _FURINADB_PAGE_

#include <tuple>
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <variant>
#include <algorithm>

#include "logger.h"
#include "config.h"
#include "utils.h"

class DataPage {
public:
	typedef std::shared_ptr<DataPage> ptr;
	DataPage() = default;
	DataPage(DataPage&& rhs) { swap(rhs); }
	DataPage(int page_no, const std::string& base_name, const std::string& table_name, const std::list<TableMode>& mode, const char* kind) :
		m_page_no(page_no), m_base_name(base_name), m_table_name(table_name), m_modes(mode) {
		strcpy(m_type, kind);
	};

	size_t getIndex() { return m_page_no; }
	std::string getTableName() { return m_table_name; }
	std::string getBaseName() { return m_base_name; }
	char* getType(){ return m_type; }
	bool isFull() { return m_num_records == m_max_records; }
	auto& getDatas() { return m_data; }
	bool isEmpty() { return m_num_records == 0; }
	size_t& getNumRecs() { return m_num_records; }
	void resetName(const std::string& new_name) { m_table_name = new_name; }


	void swap(DataPage& rhs);

	void load();
	// 表的第一次创建
	void create();
	void save();
	void addMode(const TableMode& mode);

	void addRecode(const std::vector<std::variant<int, double, std::string, std::monostate>>& data) {
		m_data.push_back(data);
		m_num_records++;
	}
	void getRecode();


private:

	size_t m_page_no = 0;         // 页号
	size_t m_num_records = 0;	  // 记录条数
	size_t m_max_records = 0;     // 最大记录条数
	std::string m_table_name; // 数据表名
	std::string m_base_name;  // 数据库名
	size_t m_num_col = 0;		  // 列数
	char m_type[5];			  // 种类
	size_t m_num_null = 0;        //null值数量
	std::vector<std::pair<size_t, size_t>> m_null;
	
	std::list<TableMode> m_modes;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> m_data;
};

class PageAppender { // 一般由buffer pool调用 在调入时调用in，调出时调用out
public:
	typedef std::shared_ptr<PageAppender> ptr;
	PageAppender(size_t index, std::string base_name, std::string table_name,const std::list<TableMode>& modes, const char* type) :m_index(index), m_base_name(base_name), m_table_name(table_name),m_modes(modes) {
		strcpy(m_type, type);
	};
	
	DataPage::ptr loadPage() {
		DataPage::ptr page = std::make_shared<DataPage>(m_index, m_base_name, m_table_name, m_modes, m_type);
		page->load();
		return page;
	};

private:
	std::string m_table_name; // 表文件名
	std::string m_base_name;  // 库文件名
	char m_type[5];
	size_t m_index;
	std::list<TableMode> m_modes;
};

#endif