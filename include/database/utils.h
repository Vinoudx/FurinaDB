#ifndef _FURINADB_UTILS_
#define _FURINADB_UTILS_
#define _CRT_SECURE_NO_WARNINGS

#include <chrono>

struct TableMode {
	char sFieldName[128];  //�ֶ���
	char sType[8];  //�ֶ�����
	int iSize;  //�ֳ�
	char bKey;  //���ֶ��Ƿ�ΪKEY��
	char bNullFlag;  //���ֶ��Ƿ�����Ϊ��
	char bValidFlag;  //���ֶ��Ƿ���Ч���������Ժ�Ա��и��ֶε�ɾ��
};


// ��ȡ��ǰʱ��
inline time_t getCurrentTime() {
	auto now = std::chrono::system_clock::now();
	return std::chrono::system_clock::to_time_t(now);
}

//ͨ��cursorʵ��һ��һ�ж�ȡ����
class Cursor {

};

#endif
