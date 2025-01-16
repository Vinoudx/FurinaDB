#ifndef _FURINADB_LOGGER_
#define _FURINADB_LOGGER_

#include <string>
#include <memory>
#include <list>
#include <format>
#include <iostream>
#include <fstream>
#include <format>

#include "config.h"
#include "utils.h"


class LogEvent {
public:
	typedef std::shared_ptr<LogEvent> ptr;
	virtual std::string format() = 0;
	virtual ~LogEvent() = default;
};

class ProgramLogEvent : public LogEvent {
public:
	typedef std::shared_ptr<ProgramLogEvent> ptr;
	ProgramLogEvent(const char* filename, size_t line,std::string context, time_t time) :m_filename(filename), m_line(line), m_time(time),m_context(context) {};
	std::string format() override;
private:
	time_t m_time = 0;
	const char* m_filename = nullptr;
	size_t m_line = 0;
	std::string m_context = nullptr;
};


class DataLogEvent : public LogEvent {
public:
	typedef std::shared_ptr<DataLogEvent> ptr;
	DataLogEvent(const std::string& sql, time_t time) :m_sql(sql), m_time(time) {};
	std::string format();
private:
	std::string m_sql;
	time_t m_time = 0;
};

class LogLevel {
public:
	enum Level{
		DEBUG = 1,
		INFO = 2,
		WARNING = 3,
		ERROR = 4,
		FATAL = 5,
		DATA = 6
	};

	static std::string toString(LogLevel::Level level);
};

class LogAppender { //logÊä³öµØ
public:
	typedef std::shared_ptr<LogAppender> ptr;

	virtual void log(LogLevel::Level level, LogEvent::ptr logevent) = 0;

	LogAppender() = default;
	LogAppender(LogLevel::Level level) :m_level(level) {};
	virtual ~LogAppender() = default;
protected:
	LogLevel::Level m_level = LogLevel::Level::DEBUG;
};


class StdAppender : public LogAppender {
public:
	StdAppender() = default;
	StdAppender(LogLevel::Level level) :LogAppender(level) {};
	void log(LogLevel::Level level, LogEvent::ptr logevent) override;
};


class FileAppender : public LogAppender {
public:
	enum FILE_DEST {
		NORMAL,
		UNDO,
		REDO
	};

	FileAppender() { setPath(NORMAL); }
	FileAppender(LogLevel::Level level) :LogAppender(level) {};
	FileAppender(FileAppender::FILE_DEST dest) { setPath(dest); }
	FileAppender(FileAppender::FILE_DEST dest, LogLevel::Level level) :LogAppender(level) { setPath(dest); }

	void log(LogLevel::Level level, LogEvent::ptr logevent) override;
	void setPath(FileAppender::FILE_DEST dest);
	bool reopen();
private:
	std::fstream m_file;
	std::string m_path;
};

class Logger { // log
public:
	typedef std::shared_ptr<Logger> ptr;
	void log(LogLevel::Level level, LogEvent::ptr logevent);
	Logger& addAppender(LogAppender::ptr appender);
	Logger& delAppender(LogAppender::ptr appender);
	Logger() = default;
	Logger(LogLevel::Level level) :m_level(level) {};

private:
	std::list<LogAppender::ptr> m_appenders;
	LogLevel::Level m_level = LogLevel::Level::DEBUG;
};


inline void mylog(const char* file, size_t line, const std::string& message, LogLevel::Level level) {
	LogEvent::ptr event(new ProgramLogEvent(file, line, message, getCurrentTime()));
	Logger::ptr logger(new Logger);
	logger->addAppender(LogAppender::ptr(new StdAppender)).addAppender(LogAppender::ptr(new FileAppender(FileAppender::FILE_DEST::NORMAL)));
	logger->log(level, event);
}

inline void sqlog(const std::string& sql, time_t time) {
	LogEvent::ptr event(new DataLogEvent(sql, time));
	Logger::ptr logger(new Logger);
	logger->addAppender(LogAppender::ptr(new StdAppender)).addAppender(LogAppender::ptr(new FileAppender(FileAppender::FILE_DEST::REDO)));
	logger->log(LogLevel::Level::INFO, event);
}


#endif