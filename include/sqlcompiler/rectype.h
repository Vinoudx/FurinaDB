#ifndef _FURINADB_SQL_RECTYPE_
#define _FURINADB_SQL_RECTYPE_

#include <vector>
#include <string>
#include <variant>

enum Fail_Reason {
	NOT_FAIL,
	NO_COL,
	NO_ROW,
	INVALID_VALUE, //���ܽ����������ת�����ֶε�����
	CANT_JOIN,
	DATABASE_CLOSED,
	NO_DATABASE,
	EXIST,
	SYNTAX_ERROR
};

struct Valid {
	bool isValid = true;														 // �Ƿ�ɹ�
	Fail_Reason reason;													 // ���ɹ�ԭ��
	std::string information = "";											 // ���ɹ�λ��
};

class RecType {
public:
	size_t num_cols = 0;														 // ��¼��
	size_t num_rows = 0;														 // ����
	std::vector<std::string> col_names;										 // �ֶ���
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> data;	 // ��������
	Valid valid;

	RecType() = default;

	RecType(RecType&& rhs){
		std::swap(num_cols, rhs.num_cols);
		std::swap(num_rows, rhs.num_rows);
		std::swap(col_names, rhs.col_names);
		std::swap(num_rows, rhs.num_rows);
		std::swap(data, rhs.data);
		std::swap(valid, rhs.valid);
	}

};

#endif
