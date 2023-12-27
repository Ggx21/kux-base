#pragma once
#include <string>


const int RESERVED_EVERY_PAGE = 4;

const int FIELD_NAME_LEN = 28;



const std::string base_dir = "./data";
const std::string db_description_file = "meta.bin";

struct table_entry
{
    u_int32_t table_id;
    const u_int32_t table_name_len=24;
    char table_name[24];
};

const int table_entry_size = sizeof(table_entry);//48

const int INT32 = 0;
const int FLOAT32 = 1;
const int VARCHAR = 2;