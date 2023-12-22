#include <set>
#include <filesystem>
#include <unordered_map>
#include <filesystem/bufmanager/BufPageManager.h>
#include <filesystem/fileio/FileManager.h>

const std::string base_dir = "./data";
const std::string db_description_file = "tables.bin";

void createFolder(std::string name, bool exist_ok);

class DatabaseSystem
{
public:
    static DatabaseSystem &getInstance()
    {
        static DatabaseSystem instance; // 声明一个静态实例
        return instance;
    }

    void createDatabase(std::string db_name);
    void dropDatabase(std::string db_name);
    void useDatabase(std::string db_name);
    void closeDatabase();
    void createTable(std::string table_name);

private:
    std::set<std::string> databases;
    std::string current_db;
    FileManager* fm;
    BufPageManager* bpm;
    int table_num;
    std::unordered_map<std::string, int> tables;

    bool on_use;
    int db_description_fd;

    std::unordered_map<int, int> table_meta_fd;
    std::unordered_map<int, int> table_data_fd;

    int nextTableID();

    DatabaseSystem()
    {
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