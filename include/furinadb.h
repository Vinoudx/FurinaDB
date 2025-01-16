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

    //����ȡ��һ����¼
    //���ÿһ�����Ӧ�ڵڼ��У�Ҫ��cursor�к��б�ͷ��Ϣ,��ʵ��getnext��reset��
    size_t getIndex(const std::list<TableMode>& modes, const std::string& field_name);

    //˳�����������
    //�����identifier
    //���������ÿһ�����һ��identifier�����д���õ�true/false trueΪ1��falseΪ0
    //���identifier�Ậ�� fieldname=123  ��������ʽ�����԰��յȺŷֳ��������֣�������ֶ���
    //������getIndex���index����cursor���һ����¼�������Ϳ��Ի�ü�¼ֵ���ұ߱Ƚϡ�
    //����Ƕ���ѯ�������tablename.fieldname��������ʽ,��������Ǹ��Ӷȿ��Զ���ü��θ���modes
    int processItem(const Token& token, const std::list<TableMode>& modes, 
        std::vector<std::vector<std::variant<int, double, std::string, std::monostate>>>::iterator iter);

    //������������������
    //����뵽�������������� Ϊvetcor<int>
    //false = 0; true = 1;    
    //   LEFT_PAREN = 2,
    //   RIGHT_PAREN = 3,
    //   OR = 4,
    //   AND = 5,
    //   NOT = 6,

    //���õ�����׺���ʽתΪ��׺���ʽ
    std::vector<int> in2post(const std::vector<int>& in_fix);

    //����õ��ĺ�׺���ʽ
    bool calculate(const std::vector<int>& post_fix);

    //��������������
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
