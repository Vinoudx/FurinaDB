#ifndef _FURINADB_UTILS_
#define _FURINADB_UTILS_
#define _CRT_SECURE_NO_WARNINGS

#include <chrono>

struct TableMode {
	char sFieldName[128];  //字段名
	char sType[8];  //字段类型
	int iSize;  //字长
	char bKey;  //该字段是否为KEY键
	char bNullFlag;  //该字段是否允许为空
	char bValidFlag;  //该字段是否有效，可用于以后对表中该字段的删除
};


// 获取当前时间
inline time_t getCurrentTime() {
	auto now = std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(now);
}

//通过cursor实现一行一行读取数据
class Cursor {

};

#endif
