#pragma once
#include <string>
#include <vector>
#include <variant>
#include <map> //TODO: 用unordered_map代替map

class TableField
{
public:
    enum Type
    {
        INT32,
        FLOAT32,
        VARCHAR
    };

    Type type;
    std::string name;
    uint32_t length; // 用于 VARCHAR 类型

    // 构造函数，用于 INT32 和 FLOAT32 类型
    TableField(Type fieldType, const std::string &fieldName)
        : type(fieldType), name(fieldName), length(0) {}

    // 构造函数，用于 VARCHAR 类型
    TableField(const std::string &fieldName, size_t varcharLength)
        : type(VARCHAR), name(fieldName), length(varcharLength) {}
};

class IndexEntry
{
public:
    // 可以用来存储指向具体行的指针或者是行号
    std::vector<int> rowIds; // 指向行的ID数组
};

class TableIndex
{
public:
    std::string indexName; // 索引名称
    // std::vector<std::string> columnNames; // 索引涉及的列名称,这是联合索引的情况
    std::string columnName; // 索引涉及的列名称
    // 根据列值存储行号，多列时用tuple或自定义的结构体作为map的key, 用vector<int>存储行号
    // TODO: 用unordered_map代替map, 使用variant的vector实现联合索引
    std::map<std::variant<int, float, std::string>, IndexEntry> entries;

    TableIndex(const std::string &name, const std::string &cols)
        : indexName(name), columnName(cols) {}

    // 添加索引项
    void addEntry(const std::variant<int, float, std::string> &key, int rowId)
    {
        if (entries.find(key) == entries.end())
        {
            entries[key] = IndexEntry();
        }
        entries[key].rowIds.push_back(rowId);
    }

    // 新建索引时，将已有的数据加入索引
    void addEntries(const std::vector<std::variant<int, float, std::string>> &keys, const std::vector<int> &rowIds)
    {
        for (int i = 0; i < keys.size(); i++)
        {
            addEntry(keys[i], rowIds[i]);
        }
    }

    // 删除索引项
    void deleteEntry(const std::variant<int, float, std::string> &key, int rowId)
    {
        if (entries.find(key) != entries.end())
        {
            auto &rowIds = entries[key].rowIds;
            for (int i = 0; i < rowIds.size(); i++)
            {
                if (rowIds[i] == rowId)
                {
                    rowIds.erase(rowIds.begin() + i);
                    break;
                }
            }
        }
    }

    // 更新索引项
    void updateEntry(const std::variant<int, float, std::string> &oldKey, const std::variant<int, float, std::string> &newKey, int rowId)
    {
        deleteEntry(oldKey, rowId);
        addEntry(newKey, rowId);
    }

};

class TableMetadata
{
public:
    std::vector<TableIndex> indexes;
    int row_size = 0;     // row_size,字节单位
    int padding_size = 0; // padding_size,字节单位

    //---------------------saved in file---------------------
    std::string tableName;
    uint32_t row_num = 0; // 行数
    std::vector<TableField> fields;

    TableMetadata(const std::string &name) : tableName(name) {}
    TableMetadata(const std::string &name, const std::vector<TableField> &fields)
        : tableName(name), fields(fields)
    {
        row_size = get_row_size_();
        padding_size = 16 - row_size % 16;
    }

    // 添加字段到表元信息
    void addField(const TableField &field)
    {
        fields.push_back(field);
        if (field.type == TableField::INT32)
        {
            row_size += 4;
        }
        else if (field.type == TableField::FLOAT32)
        {
            row_size += 4;
        }
        else if (field.type == TableField::VARCHAR)
        {
            row_size += field.length + 4;
        }
        padding_size = 16 - row_size % 16;
    }

    int get_field_num()
    {
        return fields.size();
    }

    int get_row_size_()
    {
        int size = 0;
        for (auto field : fields)
        {
            if (field.type == TableField::INT32)
            {
                size += 4;
            }
            else if (field.type == TableField::FLOAT32)
            {
                size += 4;
            }
            else if (field.type == TableField::VARCHAR)
            {
                size += field.length + 4;
            }
        }
        return size;
    }

    int get_row_size()
    {
        return row_size;
    }
};

class TableRow
{
public:
    std::vector<std::variant<int, float, std::string>> values = {};

    // 构造函数，初始化行
    TableRow() = default;
};
