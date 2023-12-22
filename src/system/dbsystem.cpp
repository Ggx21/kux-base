#include <filesystem>
#include <cstring>
#include "../common/error.hpp"
#include "../common/const.hpp"
#include "dbsystem.hpp"

namespace fs = std::filesystem;

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
    b[0] = 0;
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

int read_val(BufType &p)
{
    int rst = (*p);
    p++;
    return rst;
}

std::string read_string(BufType &p, int len)
{
    std::string rst;
    rst.assign(reinterpret_cast<char *>(p), len);
    p += (len / 4);
    return rst;
}

void wirte_val(BufType &p, int val)
{
    *p = val;
    p++;
}

void wirte_string(BufType &p, std::string val)
{
    memcpy(reinterpret_cast<char *>(p), val.c_str(), val.length());
    //向上取整
    p += (val.length() + 3) >> 2;
}

void write_table_entry(BufType &p, table_entry entry)
{
    std::cerr<<"write table entry, table name:"<<entry.table_name<<"table id:"<<entry.table_id<<"table name len:"<<entry.table_name_len<<"\n";
    wirte_val(p, entry.table_id);
    wirte_val(p, entry.table_name_len);
    wirte_string(p, entry.table_name);
}

const table_entry &read_table_entry(BufType &p)
{
    table_entry entry;
    entry.table_id = read_val(p);
    std::string table_name = read_string(p, entry.table_name_len);
    strcpy(entry.table_name, table_name.c_str());
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
    // read dababase information
    int index;
    BufType b = bpm->getPage(db_description_fileID, 0, index);
    table_num = read_val(b);
    int table_id, table_name_len;
    std::string table_name;
    for (int i = 0; i < table_num; i++)
    {
        // // read table info
        // table_id = read_val(b);
        // table_name_len = read_val(b);
        // table_name = read_string(b, table_name_len);
        table_entry entry = read_table_entry(b);
        table_id = entry.table_id;
        table_name = entry.table_name;
        tables[table_name] = table_id;
    }
    std::cout << table_num << "\n";
}

void DatabaseSystem::closeDatabase()
{
    // write back to db description
    int index;
    BufType b = bpm->getPage(db_description_fileID, 0, index);
    b[0] = table_num;
    int table_id, table_name_len;
    std::string table_name;
    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(db_description_fileID);
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

void DatabaseSystem::createTable(std::string table_name)
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
    b[0] = table_num;
    b += 4;
    b += table_entry_size * (table_num - 1) >> 2;
    table_entry entry;
    entry.table_id = table_id;
    strcpy(entry.table_name, table_name.c_str());
    write_table_entry(b, entry);
    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(fileID);
    // create new data file
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
    DbError().throw_unimplemented("desc table");
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
