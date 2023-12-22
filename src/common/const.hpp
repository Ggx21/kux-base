#pragma once
#include <string>

enum class ErrorTypeEnum : int
{
    UnimplementedError,
    SyntaxError,
    TypeError,
    SemanticError,
    InternalError,
};

const std::string base_dir = "./data";
const std::string db_description_file = "meta.bin";

struct table_entry
{
    u_int32_t table_id;
    const u_int32_t table_name_len=24;
    char table_name[24];
};

const int table_entry_size = sizeof(table_entry);//48
