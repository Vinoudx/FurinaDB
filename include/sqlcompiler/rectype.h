#ifndef _FURINADB_SQL_RECTYPE_
#define _FURINADB_SQL_RECTYPE_

#include <vector>
#include <string>
#include <variant>

enum Fail_Reason {
	NOT_FAIL,
	NO_COL,
	NO_ROW,
	INVALID_VALUE, //不能将输入的数据转换成字段的类型
	CANT_JOIN,
	DATABASE_CLOSED,
	NO_DATABASE,
	EXIST,
	SYNTAX_ERROR
};

struct Valid {
	bool isValid = true;														 // 是否成功
	Fail_Reason reason;													 // 不成功原因
	std::string information = "";											 // 不成功位置
};

class RecType {
public:
	size_t num_cols = 0;														 // 记录数
	size_t num_rows = 0;														 // 列数
	std::vector<std::string> col_names;										 // 字段名
	std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>> data;	 // 返回数据
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
