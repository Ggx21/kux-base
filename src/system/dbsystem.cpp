#include <system/dbsystem.hpp>
#include <filesystem>

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

void dbsystem::useDatabase(std::string db_name) {
    current_db = db_name;
    // open description file
    std::string file_dir = base_dir + "/" + db_name + "/" + db_description_file;
    int fileID;
    fm->openFile(file_dir.c_str(), fileID);
    // read dababase information
    int index;
    BufType b = bpm->getPage(fileID, 0, index);
    int table_num = b[0];
    for (int i=0; i<table_num; i++) {
        // read table name & id

    }
    std::cout << table_num << "\n";
}

void dbsystem::closeDatabase(std::string db_name) {

}

void dbsystem::createTable(std::string table_name) {

}