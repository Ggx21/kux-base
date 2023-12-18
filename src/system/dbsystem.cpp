#include <system/dbsystem.hpp>
#include <filesystem>
#include <cstring>

void createFolder(std::string name, bool exist_ok) {
    if (exist_ok) {
        if (std::filesystem::exists(name)) {
            if (std::filesystem::is_directory(name) == false) {
                // throw some error
            }
        }
        else 
            std::filesystem::create_directory(name);
    }
    else {
        if (std::filesystem::exists(name)) {
            // throw some error
        }
        else {
            std::filesystem::create_directory(name);
        }
    }
}

void dbsystem::createDatabase(std::string db_name) {
    // create folder
    if (databases.find(db_name) != databases.end()) {
        // database already exists, throw some error
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
    b[0] = 4;
    bpm->markDirty(index);
    bpm->writeBack(index);
    fm->closeFile(fileID);
    std::cout << "create database success\n";
}

int read_val(BufType p) {
    int rst = (*p);
    p++;
    return rst;
}

std::string read_string(BufType p, int len) {
    std::string rst;
    rst.assign(reinterpret_cast<char *>(p), len);
    p += (len/4);
    return rst;
}

void wirte_val(BufType p, int val) {
    *p = val;
    p++;
}

void wirte_string(BufType p, std::string val) {
    memcpy(reinterpret_cast<char *>(p), val.c_str(), val.length());
    p += (val.length()+3)/4;
}

void dbsystem::useDatabase(std::string db_name) {
    if (databases.find(db_name) == databases.end()) {
        // unknown database, throw error
    }
    if (on_use)
        closeDatabase();
    on_use = true;
    current_db = db_name;
    // open description file
    std::string file_dir = base_dir + "/" + db_name + "/" + db_description_file;
    fm->openFile(file_dir.c_str(), db_description_fd);
    // read dababase information
    int index;
    BufType b = bpm->getPage(db_description_fd, 0, index);
    table_num = b[0];
    int table_id, table_name_len;
    std::string table_name;
    for (int i=0; i<table_num; i++) {
        // read table if
        table_id = read_val(b);
        table_name_len = read_val(b);
        table_name = read_string(b, table_name_len);
        tables[table_name] = table_id;
    }
    for (int i=0; i<table_num; i++) {

    }
    std::cout << table_num << "\n";
}

void dbsystem::closeDatabase() {
    // write back to db description
    db_description_fd;
}

int dbsystem::nextTableID() {
    int id = 0;
    // while(tables.right.find(id) != tables.right.end()) {
    //     id++;
    // }
    return id;
}

void dbsystem::createTable(std::string table_name) {
    if (!on_use) {
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

    // create new data file
}