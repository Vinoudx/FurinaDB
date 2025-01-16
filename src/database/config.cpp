#include <string>
#include "config.h"
#include "logger.h"

#define ERROR_POSITION

std::shared_ptr<Config> Config::self_ptr = nullptr;

std::shared_ptr<Config> Config::creat() {
	if (self_ptr == nullptr) {
		self_ptr = std::make_shared<Config>();
	}
	
	return self_ptr;
}

Config::Config() {
	printf("config init\n");
	f.open(CONFIG_PATH, std::ios::in);
	if (f.fail()) {
		ERROR_POSITION;
	}
	std::string line;
	std::string left_part, right_part;
	while (std::getline(f, line)) {
		if (line[0] == '/') {
			continue;
		}
		size_t pos = line.find('=');
		left_part = line.substr(0, pos);
		right_part = line.substr(pos + 1);
		umap.emplace(left_part, right_part);
	}
	f.close();
}

int Config::getConfig(enum INT_CONFIG type) {

	switch (type){
#define XX(name) case name:return std::stoi(umap[#name]); break;

		XX(PAGE_SIZE);
		XX(BUFFER_POOL_SIZE);
		XX(MAX_NAME_LENGTH);
		XX(MAX_STRING_TYPE_RECORD_LENGTH);

#undef XX //нч╣пак
	default:
		ERROR_POSITION;
		break;
	}
}

std::string Config::getConfig(enum STRING_CONFIG type) {
	switch (type) {
#define XX(name) case name:return std::string(umap[#name]); break;

		XX(USER_NAME);
		XX(PASSWORD);

#undef XX

	case ROOT_PATH:return umap.at("ROOT_PATH"); break;
	default:
		ERROR_POSITION;
		break;
	}
}

void Config::updatePassword(const std::string& new_md5) {
	f.open(CONFIG_PATH, std::ios::ate | std::ios::in | std::ios::out);
	f.seekp(0, std::ios::beg);
	if (f.fail()) {
		ERROR_POSITION;
	}
	f<<std::format("PASSWORD={}\n", new_md5);
	f.close();
}