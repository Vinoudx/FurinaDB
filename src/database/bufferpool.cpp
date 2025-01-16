#include "bufferpool.h"

BufferPool::ptr BufferPool::m_ptr = nullptr;

BufferPool::ptr BufferPool::getBufferPool() {
	if (m_ptr == nullptr) {
		m_ptr = std::make_shared<BufferPool>(Config::creat()->getConfig(BUFFER_POOL_SIZE));
	}
	return m_ptr;
}

DataPage::ptr BufferPool::addPage(DataPage::ptr page) {
	if (m_empty_pos == m_size) {
		auto pos = tirePage();
		*pos = std::make_shared<BufferPoolItem>(page);
		//return DataPage::ptr((*pos)->m_page.get());
		return page;
	}
	else {
		m_pool.emplace(m_pool.begin() + m_empty_pos, std::make_shared<BufferPoolItem>(page));
		m_empty_pos++;
		//return DataPage::ptr((*(m_pool.begin() + m_empty_pos - 1))->m_page.get());
		return page;
	}
}

std::vector<BufferPoolItem::ptr>::iterator BufferPool::tirePage() {
	time_t earlest = LLONG_MAX;
	std::vector<BufferPoolItem::ptr>::iterator pos = m_pool.begin();
	for (auto iter = m_pool.begin(); iter != m_pool.end(); iter++) {
		if (iter->get()->m_LRU_factor < earlest) {
			earlest = iter->get()->m_LRU_factor;
			pos = iter;
		}
	}
	(*pos).get()->m_page->save();
	return pos;
}

BufferPool::ReturnType BufferPool::getPageFromPool(size_t index, const std::string& base_name,
	const std::string& table_name, const char* type) {
	//先找pool中有没有这一页
	for (int i = 0; i < m_empty_pos; i++) {
		if (m_pool[i]->getIndex() == index && m_pool[i]->m_page->getBaseName() == base_name && m_pool[i]->m_page->getTableName() == table_name && !strcmp(type, m_pool[i]->m_page->getType())) {
			m_pool[i]->setFactor();
			return BufferPool::ReturnType{ m_pool[i]->m_page, true };
		}
	}

	return BufferPool::ReturnType{ nullptr, false};
}

DataPage::ptr BufferPool::loadPage(size_t index, std::string base_name,
	std::string table_name,
	const std::list<TableMode>& modes, const char* type) {//从文件读取page ,调用pageappender
	//调用pageappender
	PageAppender::ptr appender(new PageAppender(index, base_name, table_name, modes, type));
	DataPage::ptr temp = appender->loadPage();
	DataPage::ptr a = addPage(temp);
	return a;
}

void BufferPool::savePages() {
	mylog(__FILE__, __LINE__, "saving all pages", LogLevel::Level::INFO);
	for (int i = 0; i < m_empty_pos; i++) {
		m_pool[i]->m_page->save();
	}
}

void BufferPool::resetPageName(const std::string& old_name, const std::string& new_name) {
	for (int i = 0; i < m_empty_pos; i++) {
		if (m_pool[i]->m_page->getTableName().starts_with(old_name)) {
			m_pool[i] ->m_page->resetName(new_name);
		}
	}
}