#include "logger.h"
#include <iostream>

std::string LogLevel::toString(LogLevel::Level level) {
	switch (level){
#define S(name) \
	case name:\
		return #name;\
		break;
	S(DEBUG);
	S(INFO);
	S(WARNING);
	S(ERROR);
	S(FATAL);
	S(DATA);
#undef S
	default:
		break;
	}
}

std::string ProgramLogEvent::format() {
	return std::format("{},at {}, in line {}, {}", m_time, m_filename, m_line, m_context);
}

std::string DataLogEvent::format() {
	return std::format("{} {}", m_time, m_sql);
}

Logger& Logger::addAppender(LogAppender::ptr appender) {
	m_appenders.emplace_back(appender);
	return *this;
}

Logger& Logger::delAppender(LogAppender::ptr appender) {
	m_appenders.remove(appender);
	return *this;
}

void Logger::log(LogLevel::Level level, LogEvent::ptr logevent) {
	if (level >= m_level) {
		for (auto iter = m_appenders.begin(); iter != m_appenders.end(); iter++) {
			(*iter)->log(level, logevent);
		}
	}
}

void StdAppender::log(LogLevel::Level level, LogEvent::ptr logevent) {
	if (level >= m_level) {
		std::cout << std::format("{}, {} \n", LogLevel::toString(level), logevent.get()->format());
	}
}

void FileAppender::setPath(FileAppender::FILE_DEST dest) {
	switch (dest){
	case FileAppender::NORMAL:
		m_path = Config::creat()->getConfig(ROOT_PATH).append(std::string("/log/normal.txt"));
		break;
	case FileAppender::UNDO:
		m_path = Config::creat()->getConfig(ROOT_PATH).append(std::string("/log/undo.txt"));
		break;
	case FileAppender::REDO:
		m_path = Config::creat()->getConfig(ROOT_PATH).append(std::string("/log/redo.txt"));
		break;
	default:
		break;
	}
}

bool FileAppender::reopen() {
	if (m_file.is_open()) {
		m_file.close();
	}
	m_file.open(m_path, std::ios::in | std::ios::out | std::ios::app);
	return !!m_file;
}

void FileAppender::log(LogLevel::Level level, LogEvent::ptr logevent) {
	if (level >= m_level) {
		reopen();
		m_file << std::format("{} {} \n", LogLevel::toString(level), logevent.get()->format());
	}
}