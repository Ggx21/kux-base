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
    std::string file_dir = base_dir + "/" + db_name + "/" + table_file;
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