// create by: ggx 2023-12-22
// describe field for database meta data

#pragma once

#include <string>
#include <vector>

class DbMeta
{
public:
    std::string name;
    std::vector<std::string> tables;
    int table_num=0;
    DbMeta(std::string name) : name(name) {}
    DbMeta(std::string name, std::vector<std::string> tables) : name(name), tables(tables) {
        table_num = tables.size();
    }
    DbMeta() {}
};
