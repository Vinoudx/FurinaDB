#include "page.h"

void DataPage::swap(DataPage& rhs) {
	std::swap(this->m_page_no, rhs.m_page_no);
	std::swap(this->m_num_records, rhs.m_num_records);
	std::swap(this->m_max_records, rhs.m_max_records);
	std::swap(this->m_table_name, rhs.m_table_name);
	std::swap(this->m_base_name, rhs.m_base_name);
	std::swap(this->m_num_col, rhs.m_num_col);
	std::swap(this->m_data, rhs.m_data);
}

void DataPage::create() {
	mylog(__FILE__, __LINE__, std::format("create page {} in {}", m_page_no, m_table_name), LogLevel::Level::INFO);

	int num_int = 0;
	int num_double = 0;
	int num_string = 0;

	for (auto iter = m_modes.begin(); iter != m_modes.end(); iter++) {
		if (strcmp("int", (*iter).sType) == 0) { num_int++; }
		else if (strcmp("double", (*iter).sType) == 0) { num_double++; }
		else { num_string++; }
	}

	size_t total_size = num_int * 4 + num_double * 8 + num_string * Config::creat()->getConfig(MAX_STRING_TYPE_RECORD_LENGTH);

	m_num_records = 0;
	m_num_col = m_modes.size();
	m_max_records = ((Config::creat()->getConfig(PAGE_SIZE)
		- 4 * 8
		- Config::creat()->getConfig(MAX_NAME_LENGTH) * 2
		- 8 /*这个是冗余*/)
		/ total_size);
					

}

void DataPage::save() {
	
	mylog(__FILE__, __LINE__, std::format("save page {} in {}", m_page_no, m_table_name), LogLevel::Level::INFO);

	std::fstream file;
	std::string root_path = Config::creat()->getConfig(ROOT_PATH);
	std::string table_path = std::format("{}/db/{}/{}/{}_{}.dat", root_path,
		m_base_name, m_table_name, m_table_name, m_type);
	file.open(table_path, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
	if (!file) {
		mylog(__FILE__, __LINE__, std::format("page {} in {} not exsist", m_page_no, m_table_name), LogLevel::Level::WARNING);
		file.open(table_path, std::ios::binary | std::ios::out | std::ios::ate);
		file.close();
		file.open(table_path, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
	}
	//找到自己的位置
	size_t size = Config::creat()->getConfig(PAGE_SIZE);
	size_t bias = 2 * sizeof(size_t) + sizeof(char) * 10 + 11 + m_page_no * size;
	file.seekp(bias, std::ios_base::beg);
	//写页号
	file.write((const char*)&m_page_no, sizeof(m_page_no));
	//写记录条数
	file.write((const char*)&m_num_records, sizeof(m_num_records));
	//写最大记录条数
	file.write((const char*)&m_max_records, sizeof(m_max_records));
	//写列数
	file.write((const char*)&m_num_col, sizeof(m_num_col));
	//写种类
	file.write(m_type, sizeof(char) * 5);
	//写null数量
	file.write((const char*)&m_num_null, sizeof(m_num_null));


	//写null位置
	for (int i = 0; i < m_num_null; i++) {
		file.write((const char*)&m_null[i].first, sizeof(m_null[i].first));
		file.write((const char*)&m_null[i].second, sizeof(m_null[i].second));
	}

	//monostata就不写了
	for (int outer = 0; outer < m_num_records; outer++) {
		for (int inner = 0; inner < m_num_col; inner++) {
			// 写记录
			std::visit([&](auto&& arg) {
				if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>) {
					// std::cout << "Integer: " << arg << std::endl;
					int content1 = std::get<int>(m_data[outer][inner]);
					file.write((const char*)&content1, sizeof(content1));
				}
				else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
					// std::cout << "Float: " << arg << std::endl;
					double content2 = std::get<double>(m_data[outer][inner]);
					file.write((const char*)&content2, sizeof(content2));
				}
				else if constexpr(std::is_same_v<std::decay_t<decltype(arg)>, std::string>){
					// std::cout << "String: " << arg << std::endl;
					std::string content3 = std::get<std::string>(m_data[outer][inner]);
					size_t length = Config::creat()->getConfig(MAX_STRING_TYPE_RECORD_LENGTH);
					file.write(content3.c_str(), sizeof(char) * length);
				}
				}, m_data[outer][inner]);
		}
	}

	
	file.close();
}

void DataPage::load() {
	mylog(__FILE__, __LINE__, std::format("load page {} in {}", m_page_no, m_table_name), LogLevel::Level::INFO);

	std::fstream file;
	std::string table_path = std::format("{}/db/{}/{}/{}_{}.dat", Config::creat()->getConfig(ROOT_PATH),
		m_base_name, m_table_name, m_table_name, m_type);
	file.open(table_path, std::ios::binary | std::ios::in);
	if (!file) {
		mylog(__FILE__, __LINE__, std::format("page {} in {} not exist", m_page_no, m_table_name), LogLevel::Level::FATAL);
	}

	//找到自己的位置
	size_t bias = 2 * sizeof(size_t) + sizeof(char) * 10 + 11 + m_page_no * Config::creat()->getConfig(PAGE_SIZE);
	file.seekg(bias, std::ios_base::beg);

	//读页号
	size_t temp = 0;
	file.read((char*)&temp, sizeof(size_t));
	m_page_no = temp;
	//读记录条数
	file.read((char*)&temp, sizeof(size_t));
	m_num_records = temp;
	//读最大记录条数
	file.read((char*)&temp, sizeof(size_t));
	m_max_records = temp;
	//读列数
	file.read((char*)&temp, sizeof(size_t));
	m_num_col = temp;
	//读种类
	file.read(m_type, sizeof(char) * 5);
	//读null数量
	file.read((char*)&temp, sizeof(size_t));
	m_num_null = temp;

	//读null位置
	for (int i = 0; i < m_num_null && !m_null.empty(); i++) {
		size_t x = 0, y = 0;
		file.read((char*)&x, sizeof(size_t));
		file.read((char*)&y, sizeof(size_t));
		m_null.emplace_back( x, y );
	}


	//读数据, 如果位置是null就加入一个monostate
	for (int i = 0; i < m_num_records; i++) {
		auto iter = m_modes.begin();
		std::vector<std::variant<int, double, std::string, std::monostate>> temp;
		for (int j = 0; j < m_num_col; j++) {

			auto it = std::find(m_null.begin(), m_null.end(), std::pair<size_t, size_t>(i, j));

			if (it != m_null.end()) {
				iter++;
				temp.push_back(std::monostate());
			}

			if (strcmp("int", (*iter).sType) == 0) {
				int t1 = 0;
				file.read((char*)&t1, sizeof(int));
				temp.emplace_back(t1);
			}
			else if (strcmp("double", (*iter).sType) == 0) {
				double t2 = 0;
				file.read((char*)&t2, sizeof(double));
				temp.emplace_back(t2);
			}
			else if(strcmp("char", (*iter).sType) == 0){
				char buffer[1024];
				file.read(buffer, Config::creat()->getConfig(MAX_STRING_TYPE_RECORD_LENGTH) * sizeof(char));
				temp.emplace_back(buffer);
			}
			iter++;
		}
		m_data.emplace_back(std::move(temp));
	}

	file.close();
}

void DataPage::addMode(const TableMode& mode) {
	mylog(__FILE__, __LINE__, std::format("add mode to page {} in {}", m_page_no, m_table_name), LogLevel::Level::INFO);

	m_modes.push_back(mode);
	for (int i = 0; i < m_num_records; i++) {
		m_data[i].push_back(std::monostate());
		m_num_null++;
		m_null.emplace_back(i, m_num_col);
	}

	m_num_col++;
}