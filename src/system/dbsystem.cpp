#include <filesystem>
#include <fstream>
#include <cstring>
#include <tuple>

#include "../common/error.hpp"
#include "../common/const.hpp"
// #include "../field/field.hpp"
#include "dbsystem.hpp"

namespace fs = std::filesystem;

int read_val(BufType &p)
{
    int rst = (*p);
    p++;
    return rst;
}

std::tuple<int, std::string> read_string_with_len(BufType &p)
{
    uint32_t len = read_val(p);
    std::string rst;
    rst.assign(reinterpret_cast<char *>(p), len);
    p += (len + 3) >> 2;
    return std::make_tuple(len, rst);
}

void write_val(BufType &p, int val)
{
    memcpy(p, &val, sizeof(int32_t));
    p++;
}

void write_val(BufType &p, float val)
{
    memcpy(p, &val, sizeof(float));
    p++;
}

void write_val(BufType &p, uint32_t val)
{
    memcpy(p, &val, sizeof(uint32_t));
    p++;
}

void write_string_with_len(BufType &p, std::string val, int len)
{
    write_val(p, len);
    memcpy(reinterpret_cast<char *>(p), val.c_str(), val.length());
    // 向上取整
    p += (len + 3) >> 2;
}

// 保存表元信息到文件
void DatabaseSystem::saveTableMetadataToFile(const TableMetadata &metadata, const std::string &filepath)
{
    fm->createFile(filepath.c_str());
    int fileID;
    fm->openFile(filepath.c_str(), fileID);
    int index;
    BufType b = bpm->allocPage(fileID, 0, index, false);
    // 写入表名
    int32_t tableNameLength = table_entry().table_name_len;
    b += RESERVED_EVERY_PAGE;
    write_string_with_len(b, metadata.tableName, tableNameLength);
    // 写入行数
    write_val(b, metadata.row_num);

    // 写入字段数量
    int32_t numFields = metadata.fields.size();
    write_val(b, numFields);

    // 逐个写入字段信息
    for (const auto &field : metadata.fields)
    {
        // 写入字段类型
        write_val(b, field.type);
        // 对于 VARCHAR 类型，写入长度
        if (field.type == TableField::VARCHAR)
        {
            write_val(b, field.length);
        }

        // 写入字段名
        uint32_t fieldNameLength = FIELD_NAME_LEN;
        write_string_with_len(b, field.name, fieldNameLength);
    }

    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(fileID);
    std::cout << "Table metadata saved to file: " << filepath << std::endl;
}

// 从文件加载表元信息
TableMetadata DatabaseSystem::loadTableMetadataFromFile(const std::string &filepath)
{
    TableMetadata metadata("");
    int fileID;
    fm->openFile(filepath.c_str(), fileID);
    int index;
    BufType b = bpm->getPage(fileID, 0, index);
    b += RESERVED_EVERY_PAGE;
    // 读取表名
    auto [tableNameLength, tableName] = read_string_with_len(b);

    DbError().throw_info("loaded table name: " + tableName + " fileID: " + std::to_string(fileID));

    // 读取行数
    metadata.row_num = read_val(b);

    // 读取字段数量
    uint32_t numFields;
    numFields = read_val(b);
    // TODO:bugfix
    // // 逐个读取字段信息
    for (size_t i = 0; i < numFields; ++i)
    {

        uint32_t fieldType;
        fieldType = read_val(b);

        bool isVarchar = (fieldType == static_cast<uint32_t>(TableField::VARCHAR));

        // 对于 VARCHAR 类型，读取长度
        if (isVarchar)
        {
            uint32_t fieldLength;
            fieldLength = read_val(b);
        }

        auto [fieldNameLength, fieldName] = read_string_with_len(b);

        // 添加字段到表元信息
        if (isVarchar)
        {
            TableField field(fieldName, fieldNameLength);
            metadata.addField(field);
        }
        else
        {
            TableField field(static_cast<TableField::Type>(fieldType), fieldName);
            metadata.addField(field);
        }
    }

    bpm->release(index);
    fm->closeFile(fileID);
    std::cout << "Table metadata loaded from file: " << filepath << std::endl;
    return metadata;
}

// 定义一个访问者，根据变体的类型执行不同的代码
struct VariantVisitor
{
    int operator()(int value) const
    {
        std::cout << "Visited an int: " << value << std::endl;
        return 0;
    }

    int operator()(float value) const
    {
        std::cout << "Visited a float: " << value << std::endl;
        return 1;
    }

    int operator()(const std::string &value) const
    {
        std::cout << "Visited a string: " << value << std::endl;
        return 2;
    }
};

// insertRow 函数的修改
void DatabaseSystem::insertRow(TableMetadata &metadata, const TableRow &row, const std::string &table_name)
{
    std::string file_path = base_dir + "/" + current_db + "/" + table_name + ".data";
    int fileID;
    fm->openFile(file_path.c_str(), fileID);
    int index;
    // TODO:bug。这里没有考虑到多页的情况
    BufType b = bpm->getPage(fileID, 0, index);
    b += RESERVED_EVERY_PAGE;
    uint32_t slot_size = metadata.row_size + metadata.padding_size;
    b += slot_size * metadata.row_num >> 2;
    // memset(b, 0, slot_size);
    for (int i = 0; i < row.values.size(); i++)
    {
        int type = std::visit(VariantVisitor(), row.values[i]);
        if (type == INT32)
        {
            write_val(b, std::get<int>(row.values[i]));
        }
        else if (type == FLOAT32)
        {
            write_val(b, std::get<float>(row.values[i]));
        }
        else if (type == VARCHAR)
        {
            uint32_t len = metadata.fields[i].length;
            DbError().throw_info("visit varchar, len: " + std::to_string(len));
            write_string_with_len(b, std::get<std::string>(row.values[i]), len);
        }
    }
    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(fileID);

    std::cout << "Row inserted into table: " << table_name << std::endl;
}

void createFolder(std::string name, bool exist_ok)
{
    if (exist_ok)
    {
        if (fs::exists(name))
        {
            if (fs::is_directory(name) == false)
            {
                DbError().throw_error(ErrorTypeEnum::SemanticError, "file exists but is not a directory");
            }
        }
        else
            fs::create_directory(name);
    }
    else
    {
        if (fs::exists(name))
        {
            DbError().throw_error(ErrorTypeEnum::SemanticError, "file exists");
        }
        else
        {
            fs::create_directory(name);
        }
    }
}

void DatabaseSystem::createDatabase(std::string db_name)
{
    // create folder
    if (databases.find(db_name) != databases.end())
    {
        DbError().throw_error(ErrorTypeEnum::SemanticError, "database already exists");
    }
    databases.insert(db_name);
    createFolder(base_dir + "/" + db_name, false);
    // create file
    std::string file_dir = base_dir + "/" + db_name + "/" + db_description_file;
    fm->createFile(file_dir.c_str());
    int fileID;
    fm->openFile(file_dir.c_str(), fileID);
    int index;
    BufType b = bpm->allocPage(fileID, 0, index, false);
    memset(b, 0, PAGE_SIZE);
    std::cerr << "create database, table num:" << b[0] << "index:" << index << "\n";
    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(fileID);
    std::cout << "create database success\n";
}

void DatabaseSystem::dropDatabase(std::string db_name)
{
    if (databases.find(db_name) == databases.end())
    {
        DbError().throw_error(ErrorTypeEnum::SemanticError, "database not exists");
    }
    databases.erase(db_name);
    std::string file_dir = base_dir + "/" + db_name;
    fs::remove_all(file_dir);
    std::cout << "drop database success\n";
    DbError().throw_warning("drop database is written by ggx, not tested,error may occur");
}

void write_table_entry(BufType &p, table_entry entry)
{
    memset(p, 0, table_entry_size);
    std::cerr << "write table entry, table name:" << entry.table_name << "table id:" << entry.table_id << "table name len:" << entry.table_name_len << "\n";
    write_val(p, entry.table_id);
    write_string_with_len(p, entry.table_name, entry.table_name_len);
}

table_entry *read_table_entry(BufType &p)
{
    table_entry *entry = new table_entry();
    entry->table_id = read_val(p);
    auto [name_len, name] = read_string_with_len(p);
    strcpy(entry->table_name, name.c_str());
    return entry;
}

void DatabaseSystem::useDatabase(std::string db_name)
{
    if (databases.find(db_name) == databases.end())
    {
        DbError().throw_error(ErrorTypeEnum::SemanticError, "database not exists");
    }
    if (on_use)
        closeDatabase();
    on_use = true;
    current_db = db_name;
    // open description file
    std::string file_dir = base_dir + "/" + db_name + "/" + db_description_file;
    fm->openFile(file_dir.c_str(), db_description_fileID);
    // read database information
    int index;
    BufType b = bpm->getPage(db_description_fileID, 0, index);
    b += 3;
    table_num = read_val(b);
    int table_id, table_name_len;
    std::string table_name;
    for (int i = 0; i < table_num; i++)
    {
        table_entry *entry = read_table_entry(b);
        table_id = entry->table_id;
        table_name = entry->table_name;
        tables[table_name] = table_id;
    }
    std::cout << "table num:" << table_num << "\n";
    for (auto &table : tables)
    {
        std::cout << table.first << "\n";
    }
}

void DatabaseSystem::closeDatabase()
{
    // write back to db description
    int index;
    BufType b = bpm->getPage(db_description_fileID, 0, index);
    on_use = false;
    current_db = "";
    tables.clear();
    table_num = 0;
}

void DatabaseSystem::showDatabases()
{
    for (auto &db : databases)
    {
        std::cout << db << "\n";
    }
}

int DatabaseSystem::nextTableID()
{
    int id = 0;
    // check if id is in tables.values()
    bool find = true;
    while (find)
    {
        find = false;
        for (auto &table : tables)
        {
            if (table.second == id)
            {
                id++;
                find = true;
                break;
            }
        }
    }
    return id;
}

void DatabaseSystem::createTable(std::string table_name, const std::vector<TableField> &fields)
{
    if (!on_use)
    {
        // cannot create table w/o selecting database, throw error
    }
    int table_id = nextTableID();
    tables[table_name] = table_id;
    // store table info to db_description file
    std::string file_dir = base_dir + "/" + current_db + "/" + db_description_file;
    int fileID;
    fm->openFile(file_dir.c_str(), fileID);
    int index;
    BufType b = bpm->getPage(fileID, 0, index);

    // store info to meta file
    table_num++;
    b[3] = table_num;
    b += RESERVED_EVERY_PAGE;
    b += table_entry_size * (table_num - 1) >> 2;
    table_entry entry;
    entry.table_id = table_id;
    strcpy(entry.table_name, table_name.c_str());
    write_table_entry(b, entry);
    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(fileID);

    DbError().throw_info("create table, table name:" + table_name + "table id:" + std::to_string(table_id));
    DbError().throw_info("start to save table metadata to file");

    // create new data file
    // 创建表元信息
    TableMetadata metadata(table_name, fields);
    string filepath = base_dir + "/" + current_db + "/" + table_name + ".meta";
    // 保存表元信息到文件
    saveTableMetadataToFile(metadata, filepath);
    // 创建空表
    string datafilepath = base_dir + "/" + current_db + "/" + table_name + ".data";
    fm->createFile(datafilepath.c_str());
    int datafileID;
    fm->openFile(datafilepath.c_str(), datafileID);
    int dataindex;
    BufType datab = bpm->allocPage(datafileID, 0, dataindex, false);
    memset(datab, 0, PAGE_SIZE);
    bpm->markDirty(dataindex);
    bpm->writeBack(dataindex);
    fm->closeFile(datafileID);
    std::cout << "create table success\n";
}

void DatabaseSystem::dropTable(std::string table_name)
{
    if (!on_use)
    {
        // cannot create table w/o selecting database, throw error
        DbError().throw_error(ErrorTypeEnum::SemanticError, "no database selected");
    }
    if (tables.find(table_name) == tables.end())
    {
        // table not exists, throw error
        DbError().throw_error(ErrorTypeEnum::SemanticError, "table not exists");
    }
    int table_id = tables[table_name];
    tables.erase(table_name);
    // store table info to db_description file
    std::string file_dir = base_dir + "/" + current_db + "/" + db_description_file;
    int fileID;
    fm->openFile(file_dir.c_str(), fileID);
    int index;
    BufType b = bpm->getPage(fileID, 0, index);

    // store info to meta file

    // create new data file
}

void DatabaseSystem::descTable(std::string table_name)
{
    if (!on_use)
    {
        // cannot create table w/o selecting database, throw error
        DbError().throw_error(ErrorTypeEnum::SemanticError, "no database selected");
    }
    if (tables.find(table_name) == tables.end())
    {
        // table not exists, throw error
        DbError().throw_error(ErrorTypeEnum::SemanticError, "table not exists");
    }
    int table_id = tables[table_name];
    // store table info to db_description file
    std::string file_dir = base_dir + "/" + current_db + "/" + table_name + ".meta";
    // 从文件加载表元信息
    TableMetadata loadedMetadata = loadTableMetadataFromFile(file_dir);
    DbError().throw_info("loaded table metadata from file: " + file_dir);
    // 输出加载的表元信息
    std::cout << "Loaded Table Metadata:\n";
    std::cout << "Table Name: " << loadedMetadata.tableName << "\n";
    std::cout << "Fields:\n";
    for (const auto &field : loadedMetadata.fields)
    {
        std::cout << "  Name: " << field.name << ", Type: " << field.type;
        if (field.type == TableField::VARCHAR)
        {
            std::cout << ", Length: " << field.length;
        }
        std::cout << "\n";
    }
}

void DatabaseSystem::showTables()
{
    if (!on_use)
    {
        // cannot create table w/o selecting database, throw error
        DbError().throw_error(ErrorTypeEnum::SemanticError, "no database selected");
    }
    for (auto &table : tables)
    {
        std::cout << table.first << "\n";
    }
}

void DatabaseSystem::showIndexes()
{
    DbError().throw_unimplemented("show indexes");
}

void DatabaseSystem::createIndex(std::string index_name, std::string table_name, std::string field_name)
{
    DbError().throw_unimplemented("create index");
}

void DatabaseSystem::dropIndex(std::string index_name)
{
    DbError().throw_unimplemented("drop index");
}

void DatabaseSystem::insertIntoTable(std::string table_name, const TableRow &row)
{
    auto metadata = loadTableMetadataFromFile(base_dir + "/" + current_db + "/" + table_name + ".meta");
    DbError().throw_info("insertRow into table: " + table_name);
    insertRow(metadata, row, table_name);
    metadata.row_num++;
    // TODO:bugfix,什么时候把修改信息写回文件？
    saveTableMetadataToFile(metadata, base_dir + "/" + current_db + "/" + table_name + ".meta");
    DbError().throw_info("insertRow into table: " + table_name + " success");
}