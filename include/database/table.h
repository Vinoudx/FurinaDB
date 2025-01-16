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

	void firstCreate();// ��һ�δ���һ�ű���Ҫ���������ļ�,�͵�һ��ҳ
	void save();//��Ԫ���ݱ��浽.dat�ļ���
	void load();//��.dat�ļ���ͷ���������

	void createIndex(const char* index_name); //����������
	void insertIndex(const std::variant<int,double,std::string, std::monostate>& index_value, int page_id);
	size_t getPgIdFromIndex() {};

	void insert(const std::vector<std::variant<int, double, std::string, std::monostate>>& data);

	//cursor����
	std::optional<std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator> getNext();
	void deleteOne(std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter);
	//ɾ��һ����¼��Ҫ����backstep��Ϊvector��ɾ��һ�������ļ�¼��ǰ��
	//���Ե�ǰ�е��к�Ӧ�þ���ɾ�������е��к�
	void backStep() {
		//����ǵ�һ���Ͳ�����
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
	//cursor����
	std::optional<std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator> getNext();

	size_t m_current_record = 0;
	std::list<TableMode> lhs_mode;
	std::list<TableMode> rhs_mode;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> lhs;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> rhs;
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> t;
};





#endif
