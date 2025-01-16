#ifndef _FURINADB_FURINADB_
#define _FURINADB_FURINADB_

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <memory>

#include "sqlmanager.h"
#include "database.h"
#include "rectype.h"
#include "bufferpool.h"
#include "md5.h"

class FurinaDB {
public:
    typedef std::shared_ptr<FurinaDB> ptr;

    static FurinaDB::ptr createDB() {
        if (m_ptr == nullptr) {
            m_ptr = std::make_shared<FurinaDB>();
        }
        return m_ptr;
    }

	FurinaDB():database(nullptr){
        if (m_pool == nullptr) {
            m_pool = BufferPool::getBufferPool();
        }
    }

	RecType execute(const std::string& sql);

    void close() {
        if (database != nullptr) {
            database->close();
        }
    }


    std::list<TableMode> getModes(const std::string& table_name);

    //首先取来一条记录
    //获得每一个项对应在第几列（要求cursor中含有表头信息,并实现getnext和reset）
    size_t getIndex(const std::list<TableMode>& modes, const std::string& field_name);

    //顺序处理条件语句
    //如果是identifier
    //条件语句中每一个项都是一个identifier，进行处理得到true/false true为1，false为0
    //这个identifier会含有 fieldname=123  这样的形式，所以按照等号分成两个部分，左边是字段名
    //从上面getIndex获得index，从cursor获得一条记录，这样就可以获得记录值和右边比较。
    //如果是多表查询还会出现tablename.fieldname这样的形式,如果不考虑复杂度可以多调用几次个体modes
    int processItem(const Token& token, const std::list<TableMode>& modes, 
        std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter);

    //如果是运算符或者括号
    //则插入到条件语句的序列中 为vetcor<int>
    //false = 0; true = 1;    
    //   LEFT_PAREN = 2,
    //   RIGHT_PAREN = 3,
    //   OR = 4,
    //   AND = 5,
    //   NOT = 6,

    //将得到的中缀表达式转为后缀表达式
    std::vector<int> in2post(const std::vector<int>& in_fix);

    //计算得到的后缀表达式
    bool calculate(const std::vector<int>& post_fix);

    //条件处理的总入口
    std::optional<bool> condition(int pos, const std::vector<Token>& tokens, const std::list<TableMode>& modes,
        std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter);

    ~FurinaDB() {
        m_pool->savePages();
        close();
    }

private:
    static std::shared_ptr<FurinaDB> m_ptr;
	SQLManager sqlmanager;
	DataBase::ptr database;
    BufferPool::ptr m_pool;
};


#endif 
