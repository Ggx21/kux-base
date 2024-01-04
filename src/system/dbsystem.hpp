#pragma once

#include <set>
#include <filesystem>
#include <unordered_map>
#include <variant>

#include <filesystem/bufmanager/BufPageManager.h>
#include <filesystem/fileio/FileManager.h>

#include "../common/error.hpp"
#include "../common/const.hpp"
#include "../field/field.hpp"

void createFolder(std::string name, bool exist_ok);

class DatabaseSystem
{
public:
    static DatabaseSystem &getInstance()
    {
        static DatabaseSystem instance; // 声明一个静态实例
        return instance;
    }
    //------------------utils------------------
    void writeMeta();
    void saveTableMetadataToFile(const TableMetadata &metadata, const std::string &filepath);
    TableMetadata loadTableMetadataFromFile(const std::string &filepath);
    void insertRow(TableMetadata &metadata, const TableRow &row, const std::string &filename);

    //------------------dml visitor------------------
    void createDatabase(std::string db_name);
    void dropDatabase(std::string db_name);
    void useDatabase(std::string db_name);
    void closeDatabase();
    void showDatabases();
    void createTable(std::string table_name, const std::vector<TableField> &fields);
    void dropTable(std::string table_name);
    void descTable(std::string table_name);
    void showTables();
    void showIndexes();
    void createIndex(std::string table_name, std::string index_name, std::string field_name);
    void dropIndex(std::string table_name, std::string index_name);
    void insertIntoTable(std::string table_name, const TableRow &row);
    void selectFromTable(std::string table_name, std::vector<std::string> field_names, std::vector<std::string> values);
    void deleteFromTable(std::string table_name, std::vector<std::string> field_names, std::vector<std::string> values);
    void update(std::string table_name, std::vector<std::string> fields, std::vector<std::string> values);

private:
    std::set<std::string>
        databases;
    std::string current_db;
    FileManager *fm;
    BufPageManager *bpm;
    int32_t table_num;
    std::unordered_map<std::string, int> tables;

    bool on_use;
    int db_description_fileID; // fileID of db_description_file，在useDatabase时初始化

    std::unordered_map<int, int> table_meta_fileID;
    std::unordered_map<int, int> table_data_fileID;

    int nextTableID();

    DatabaseSystem()
    {
        std::cerr << "DatabaseSystem init\n";
        MyBitMap::initConst();
        fm = new FileManager();
        bpm = new BufPageManager(fm);
        createFolder(base_dir, true);
        for (auto &file : std::filesystem::directory_iterator(base_dir))
        {
            if (std::filesystem::is_directory(file))
            {
                databases.insert(std::filesystem::path(file).filename());
            }
        }
    }
    ~DatabaseSystem() {}
    DatabaseSystem(const DatabaseSystem &) = delete;
    DatabaseSystem &operator=(const DatabaseSystem &) = delete;
};