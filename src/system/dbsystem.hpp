#include <set>
#include <filesystem>
#include <filesystem/bufmanager/BufPageManager.h>
#include <filesystem/fileio/FileManager.h>

const std::string base_dir = "./data";
const std::string db_description_file = "tables.bin"; // db description file

void createFolder(std::string name, bool exist_ok);

class dbsystem {
public:
    dbsystem() {
        MyBitMap::initConst();
        fm = new FileManager();
        bpm = new BufPageManager(fm);
        createFolder(base_dir, true);
        for (auto &file: std::filesystem::directory_iterator(base_dir)) {
            if (std::filesystem::is_directory(file)) {
                databases.insert(std::filesystem::path(file).filename());
            }
        }
    }
    ~dbsystem() {}
    void createDatabase(std::string db_name);
    void useDatabase(std::string db_name);
    void closeDatabase(std::string db_name);
    void createTable(std::string table_name);

private:
    std::set<std::string> databases;
    std::string current_db;
    FileManager* fm;
    BufPageManager* bpm;

};