#include "table.h"

void Table::swap(Table& rhs) {
	std::swap(this->m_base_name, rhs.m_base_name);
	std::swap(this->m_table_name, rhs.m_table_name);
	std::swap(this->m_num_pages, rhs.m_num_pages);
	std::swap(this->m_num_modes, rhs.m_num_modes);
	std::swap(this->m_modes, rhs.m_modes);
	std::swap(this->m_current_page, rhs.m_current_page);
	std::swap(this->m_current_record, rhs.m_current_record);
}

Table& Table::addMode(const TableMode& mode) {
	//��ȡ����ҳ
	for (int i = 0; i < m_num_pages; i++) {
		BufferPool::ReturnType rec = BufferPool::getBufferPool()->getPageFromPool(i, m_base_name, m_table_name, m_type);

		DataPage::ptr page;

		if (rec.flag) {
			page = rec.page;
		}
		else {
			page = BufferPool::getBufferPool()->loadPage(i, m_base_name, m_table_name, m_modes, m_type);
		}
		page->addMode(mode);
	}

	m_modes.push_back(mode);
	m_num_modes++;
	return *this;
}

void Table::firstCreate() {
	mylog(__FILE__, __LINE__, std::format("create table {}", m_table_name), LogLevel::Level::INFO);

	m_num_pages = 0;

	//�Ƚ�����Ӧ�ļ���
	std::string table_root_dir = std::format("{}/db/{}/{}", Config::creat()->getConfig(ROOT_PATH),
		m_base_name, m_table_name);

	if (!std::filesystem::create_directory(table_root_dir)) {
		mylog(__FILE__, __LINE__, std::format("unable to create table {}", m_table_name), LogLevel::Level::FATAL);
	}

	// ���������ļ�
	std::ofstream file;
	std::string table_path = std::format("{}/{}_{}.dat", table_root_dir, m_table_name, m_type);
	file.open(table_path, std::ios::binary);
	if (!file) {
		mylog(__FILE__, __LINE__, std::format("unable to create table {}", m_table_name), LogLevel::Level::FATAL);
	}

	file.close();

	//������һ��ҳ
	DataPage::ptr page(new DataPage(0, m_base_name, m_table_name, m_modes, m_type));
	m_num_pages++;
	page->create();
	BufferPool::getBufferPool()->addPage(page);

	//��������
	for (auto iter = m_modes.begin(); iter != m_modes.end(); iter++) {
		if ((*iter).bKey == 1) {
			createIndex((*iter).sFieldName);
		}
	}

}

void Table::save() {
	std::fstream file;
	mylog(__FILE__, __LINE__, std::format("save table {}", m_table_name), LogLevel::Level::INFO);

	std::string table_path = std::format("{}/db/{}/{}/{}_{}.dat", Config::creat()->getConfig(ROOT_PATH),
		m_base_name, m_table_name, m_table_name, m_type);
	file.open(table_path, std::ios::binary | std::ios::in | std::ios::out |  std::ios::ate);


	if (!file) {
		mylog(__FILE__, __LINE__, std::format("table {} not exsist", m_table_name), LogLevel::Level::WARNING);
		file.open(table_path, std::ios::binary | std::ios::out | std::ios::ate);
		file.close();
		file.open(table_path, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
	}

	//д��Ԫ����
	file.write((const char*)&m_num_pages, sizeof(m_num_pages));
	file.write(m_type, 10 * sizeof(char));

	file.close();
}

void Table::load() {
	std::fstream file;
	mylog(__FILE__, __LINE__, std::format("load table {}", m_table_name), LogLevel::Level::INFO);

	std::string table_path = std::format("{}/db/{}/{}/{}_{}.dat", Config::creat()->getConfig(ROOT_PATH),
		m_base_name, m_table_name, m_table_name, m_type);
	file.open(table_path, std::ios::binary | std::ios::in);

	if (!file) {
		mylog(__FILE__, __LINE__, std::format("unable to load table {}", m_table_name), LogLevel::Level::FATAL);
	}
	//��ȡԪ����
	file.read((char*)&m_num_pages, sizeof(size_t));
	file.read(m_type, 10 * sizeof(char));

	file.close();

}

void Table::insert(const std::vector<std::variant<int, double, std::string, std::monostate>>& data) {
	mylog(__FILE__, __LINE__, std::format("insert into table {} in page {}", m_table_name, m_num_pages), LogLevel::Level::INFO);
	//����������sql�����м�����ݺϷ���
	//���ҵ����һҳ
	BufferPool::ReturnType rec = BufferPool::getBufferPool()->getPageFromPool(m_num_pages - 1, m_base_name, m_table_name, m_type);

	DataPage::ptr page;

	if (rec.flag) {
		page = rec.page;
	}
	else {
		page = BufferPool::getBufferPool()->loadPage(m_num_pages - 1, m_base_name, m_table_name, m_modes, m_type);
	}

	//�ж����һҳ��û����
	if (!page->isFull()) {
		page->addRecode(data);
	}
	else {
		//���˾��½�һҳ
		m_num_pages++;
		DataPage::ptr dp(new DataPage(m_num_pages - 1, m_base_name, m_table_name, m_modes, m_type));
		dp->addRecode(data);
		BufferPool::getBufferPool()->addPage(dp);
	}

	//��������������
	int row = 0;
	for (auto iter = m_modes.begin(); iter != m_modes.end(); iter++) {
		if ((*iter).bKey) {
			std::list<TableMode> modes;
			modes.push_back({ "index_item", "char", 100, 0, 0 });
			modes.push_back({ "is_leaf", "int", 100, 0, 0 });
			modes.push_back({ "page_id", "int", 100, 0, 0 });
			Table::ptr index_table = std::make_shared<Table>("inde", m_base_name, std::format("{}_{}", m_table_name, (*iter).sFieldName), modes);
			index_table->load();
			index_table->insertIndex(data[row], m_num_pages);
		}
		row++;
	}


}

void Table::createIndex(const char* index_name) {
	mylog(__FILE__, __LINE__, std::format("create index {} table {}", index_name, m_table_name), LogLevel::Level::INFO);
	std::list<TableMode> modes;
	modes.push_back({ "index_item", "char", 100, 0, 0 });
	modes.push_back({ "is_leaf", "int", 100, 0, 0 });
	modes.push_back({ "page_id", "int", 100, 0, 0 });
	Table::ptr index_table(new Table("inde", m_base_name, std::format("{}_{}", m_table_name, index_name), modes));
	index_table->firstCreate();
	index_table->save();

}

void Table::insertIndex(const std::variant<int, double, std::string, std::monostate>& index_value, int page_id) {

	//�ӵ�һҳ��ʼ��
	size_t current_page = 0;
	bool flag = true;

	auto tar_value = index_value;
	int tar_page = page_id;

	while (flag) {

		BufferPool::ReturnType rec = BufferPool::getBufferPool()->getPageFromPool(m_num_pages - 1, m_base_name, m_table_name, m_type);

		DataPage::ptr page;

		if (rec.flag) {
			page = rec.page;
		}
		else {
			page = BufferPool::getBufferPool()->loadPage(m_num_pages - 1, m_base_name, m_table_name, m_modes, m_type);
		}
		//��һ������ֵ���뵽��һҳ
		if (page->isEmpty()) {
			std::vector<std::variant<int, double, std::string, std::monostate>> data;
			data.push_back(tar_value);
			data.push_back(1);
			data.push_back(tar_page);
			page->addRecode(data);
			return;
		}
		auto& datas = page->getDatas();
		for (auto iter = datas.begin(); iter != datas.end(); iter++) {
			//������������Ļ�Ҫ��
			if (iter == datas.end() - 1 && tar_value > (*iter)[0]) {
				std::swap(tar_value, (*iter)[0]);
				int temp = std::get<int>((*iter)[2]);
				tar_page = std::get<int>((*iter)[2]);
				(*iter)[2] = tar_page;
				break;
			}
			if (tar_value > (*iter)[0])continue;

			//�������Ҷ�ӽڵ�
			if (std::get<int>((*iter)[1]) == 0) {
				current_page = std::get<int>((*iter)[2]);
				break;
			}
			else {
				//�����Ҷ�ӽڵ�
				std::vector<std::variant<int, double, std::string, std::monostate>> data;
				data.push_back(tar_value);
				data.push_back(1);
				data.push_back(tar_page);
				datas.insert(iter, data);
				page->getNumRecs()++;
				flag = false;
				break;
			}

		}
		//�����������˾ͷ���
		if (page->isFull()) {
			std::list<TableMode> modes;
			modes.push_back({ "index_item", "char", 100, 0, 0 });
			modes.push_back({ "is_leaf", "int", 100, 0, 0 });
			modes.push_back({ "page_id", "int", 100, 0, 0 });
			m_num_pages++;
			DataPage::ptr dp1(new DataPage(m_num_pages, m_base_name, m_table_name, modes, m_type));
			auto& data1 = dp1->getDatas();
			m_num_pages++;
			DataPage::ptr dp2(new DataPage(m_num_pages, m_base_name, m_table_name, modes, m_type));
			auto& data2 = dp1->getDatas();

			data1 = std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>(page->getDatas().begin(), page->getDatas().begin() + page->getNumRecs() / 2);
			data2 = std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>(page->getDatas().begin() + page->getNumRecs() / 2, page->getDatas().end());
			page->getDatas().clear();
			page->getDatas().push_back(*(data1.end() - 1));
			page->getDatas().push_back(*(data2.end() - 1));
			page->getDatas()[0][1] = 1;
			page->getDatas()[1][1] = 1;

			BufferPool::getBufferPool()->addPage(dp1);
			BufferPool::getBufferPool()->addPage(dp2);
		}

	}
	

}


std::optional<std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator> Table::getNext() {
	//�ȴ���һҳ
	if (m_current_page == m_num_pages) { return std::nullopt; }

	BufferPool::ReturnType rec = BufferPool::getBufferPool()->getPageFromPool(m_current_page, m_base_name, m_table_name, m_type);

	DataPage::ptr page;

	if (rec.flag) {
		page = rec.page;
	}
	else {
		page = BufferPool::getBufferPool()->loadPage(m_current_page, m_base_name, m_table_name, m_modes, m_type);
	}
	//Ȼ��ȡ��¼
	if (page->getNumRecs() == 0) {return std::nullopt; } // ���������û�м�¼ʱ��һҳû�м�¼
	auto rtn = page->getDatas().begin();
	std::advance(rtn, m_current_record);
	//�ж��ǲ���ȡ�꣬ȡ���˾ͽ�ָ��ָ����һҳ�ĵ�һ��
	m_current_record++;
	if (m_current_record == page->getNumRecs()) {
		m_current_record = 0;
		m_current_page++;
	}
	return rtn;
}

void Table::deleteOne(std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter){
	size_t pageNumber = m_current_record == 0 ? m_current_page - 1 : m_current_page;
	BufferPool::ReturnType rec = BufferPool::getBufferPool()->getPageFromPool(pageNumber, m_base_name, m_table_name, m_type);

	DataPage::ptr page;

	if (rec.flag) {
		page = rec.page;
	}
	else {
		page = BufferPool::getBufferPool()->loadPage(pageNumber, m_base_name, m_table_name, m_modes, m_type);
	}

	page->getDatas().erase(iter);
	page->getNumRecs()--;
}


std::optional<std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator> TempTable::getNext() {

	m_current_record++;
	if (m_current_record > lhs.size())return std::nullopt;
	return lhs.begin() + m_current_record - 1;

}
