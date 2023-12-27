#pragma once
#include <string>
#include <vector>
#include <variant>

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

class TableMetadata
{
public:
    std::vector<std::string> indexes;
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
