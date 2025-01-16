#ifndef _FURINADB_CONFIG_
#define _FURINADB_CONFIG_

#include <fstream>
#include <unordered_map>
#include <string>
#include <memory>

#define CONFIG_PATH "D:/work/furinadb/furinadb/config.txt"


enum INT_CONFIG{
	PAGE_SIZE,
	ZERO_PAGE_SIZE,
	ONE_PAGE_SIZE,
	BUFFER_POOL_SIZE,
	MAX_NAME_LENGTH,
	MAX_STRING_TYPE_RECORD_LENGTH
};

enum STRING_CONFIG {
	ROOT_PATH,
	USER_NAME,
	PASSWORD
};

class Config {
private:
	static std::shared_ptr<Config> self_ptr;
	std::fstream f;
	std::unordered_map<std::string, std::string> umap;

public:
	Config();
	Config(const Config& other) = delete;
	Config& operator=(const Config& other) = delete;
	~Config() { f.close(); self_ptr = nullptr; printf("config decostruct\n"); }

	void updatePassword(const std::string& new_md5);
	static std::shared_ptr<Config> creat();
	int getConfig(enum INT_CONFIG type);
	std::string getConfig(enum STRING_CONFIG type);
};

#endif