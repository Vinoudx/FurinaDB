#ifndef _FURINADB_BUFFER_POOL_
#define _FURINADB_BUFFER_POOL_

#include <memory>
#include <vector>
#include <optional>
#include <list>

#include "page.h"
#include "config.h"
#include "utils.h"


class BufferPoolItem {
public:
	typedef std::shared_ptr<BufferPoolItem> ptr;
	size_t getIndex() { return m_page.get()->getIndex(); };

	BufferPoolItem(DataPage::ptr page) : m_page(page) { setFactor(); };

	void setFactor() { m_LRU_factor = getCurrentTime(); }

	~BufferPoolItem() = default;
public:
	time_t m_LRU_factor;
	bool isEdited = false;
	DataPage::ptr m_page;
};


class BufferPool {
public:

	struct ReturnType {
		DataPage::ptr page;
		bool flag;
	};

	typedef std::shared_ptr<BufferPool> ptr;

	static BufferPool::ptr getBufferPool();
	BufferPool(size_t size) :m_size(size) { m_pool.resize(m_size); };
	//先调用getPAge若没有则调用loadpage
	DataPage::ptr loadPage(size_t index, std::string base_name, std::string table_name, const std::list<TableMode>& modes, const char* type);
	
	ReturnType getPageFromPool(size_t index, const std::string& base_name, const std::string& table_name, const char* type);


	DataPage::ptr addPage(DataPage::ptr page); //加入pool中
	std::vector<BufferPoolItem::ptr>::iterator tirePage();//按lru替换page 返回可插入位置
	
	//当缓存池析构时自动将所有页都保存
	void savePages();

	~BufferPool() = default;
	//~BufferPool() { savePages(); }

	void resetPageName(const std::string& old_name, const std::string& new_name);

private:
	
	std::vector<BufferPoolItem::ptr> m_pool;
	static std::shared_ptr<BufferPool> m_ptr;
	size_t m_size;
	size_t m_empty_pos = 0;
};

#endif
