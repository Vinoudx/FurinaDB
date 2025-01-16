#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>

#include "config.h"
#include "logger.h"
#include "utils.h"
#include "table.h"
#include "bufferpool.h"
#include "page.h"
#include "database.h"

#include "furinadb.h"
#include "md5.h"

#include <stack>

using namespace std;



int main(){

	LogEvent::ptr logevent(new ProgramLogEvent(__FILE__, __LINE__,"hello", getCurrentTime()));
	Logger::ptr logger(new Logger);
	logger->addAppender(LogAppender::ptr(new StdAppender))
		.addAppender(LogAppender::ptr(new FileAppender));
	logger->log(LogLevel::Level::WARNING, logevent);

	FurinaDB::ptr db = FurinaDB::createDB();


	//db->execute("CREATE DATABASE hym;");
	db->execute("USE tt;");
	//db->execute("CREATE TABLE table2 ('aa' char 10 0 0 , 'bb' char 10 0 0 , 'cc' char 10 0 0 , 'dd' char 10 0 0);");
	//db->execute("ADD COL table1 ( e char 10 1 1 );");
	//db->execute("DELETE COL table1 b;");
	
	//RecType r = db->execute("INSERT INTO table2 ( aa bb cc dd ) VALUES ( aa cccc dddd ee );");

	RecType r3 = db->execute("SHOW TABLES;");
	if (r3.valid.isValid) {
		for (int i = 0; i < r3.num_rows; i++) {
			for (int j = 0; j < r3.num_cols; j++) {
				std::visit([&](auto&& arg) {
					if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) {
						// std::cout << "Integer: " << arg << std::endl;
						int content1 = std::get<int>(r3.data[i][j]);
						cout << content1 << ' ';
					}
					else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
						// std::cout << "Float: " << arg << std::endl;
						double content2 = std::get<double>(r3.data[i][j]);
						cout << content2 << ' ';
					}
					else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
						// std::cout << "String: " << arg << std::endl;
						std::string content3 = std::get<std::string>(r3.data[i][j]);
						cout << content3 << ' ';
					}
					}, r3.data[i][j]);
			}
			cout << endl;
		}
	}

	//RecType r2 = db->execute("UPDATE table1 SET c=ac WHERE d=ee;");
	//if (!r2.valid.isValid) {
	//	mylog(__FILE__, __LINE__, r2.valid.information , LogLevel::Level::INFO);
	//}


	RecType r4 = db->execute("SELECT table1.a table2.bb table1.b FROM table1 JOIN table2 ON table1.a=table2.aa;");
	if (r4.valid.isValid) {
		for (int i = 0; i < r4.num_rows; i++) {
			for (int j = 0; j < r4.num_cols; j++) {
				std::visit([&](auto&& arg) {
					if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) {
						// std::cout << "Integer: " << arg << std::endl;
						int content1 = std::get<int>(r4.data[i][j]);
						cout << content1 << ' ';
					}
					else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
						// std::cout << "Float: " << arg << std::endl;
						double content2 = std::get<double>(r4.data[i][j]);
						cout << content2 << ' ';
					}
					else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
						// std::cout << "String: " << arg << std::endl;
						std::string content3 = std::get<std::string>(r4.data[i][j]);
						cout << content3 << ' ';
					}
					}, r4.data[i][j]);
			}
			cout << endl;
		}
	}
	else {
		mylog(__FILE__, __LINE__, r4.valid.information, LogLevel::Level::WARNING);
	}


	

	//db.close();

	//std::fstream f;
	//f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary| std::ios::in | std::ios::out | std::ios::ate);
	//if (!f.is_open()) {
	//	cout << 123 << endl;
	//	f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary | std::ios::out | std::ios::ate);
	//	f.close();
	//	f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
	//}

	//f.seekp(0, std::ios::beg);
	////size_t a = 3;
	//size_t b = 5;
	//char a[2] = { 'a', 'b' };
	////f.write(reinterpret_cast<const char*>(&a), sizeof(a));
	//f.write((const char*)&b, sizeof(b));
	//f.write(a, sizeof(char) * 2);
	////f.write((const char*)&a, sizeof(a));

	//f.close();

	//f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary | std::ios::in  | std::ios::out | std::ios::ate);
	//if (!f.is_open()) {
	//	cout << 123 << endl;
	//	f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary | std::ios::out | std::ios::ate);
	//	f.close();
	//	f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
	//}

	//f.seekp(sizeof(size_t) + 2 * sizeof(char), std::ios::beg);
	//size_t c = 4;
	//char d[2] = { 'a', 'b' };
	//size_t h = 6;
	//f.write((const char*)(&c), sizeof(c));
	//f.write(d, sizeof(char) * 2);
	//f.write((const char*)(&h), sizeof(h));

	//f.close();


	//f.open("c:\\users\\a\\desktop\\1.txt", std::ios::binary | std::ios::in);
	//f.seekg(10, std::ios::beg);
	//size_t e = 0;
	//size_t g = 0;
	//f.read((char*)&e, sizeof(size_t));
	//char buffer[1024];
	//f.read(buffer, 2 * sizeof(char));
	//f.read((char*)&g, sizeof(size_t));
	//cout << std::format("{}/{}/{}", e, g, buffer);

	//f.close();


	//std::string str = "root";

	//MD5 md5(str);
	//string result = md5.md5();
	//cout << "abc md5 is " << result << endl;


	return 0;
}

