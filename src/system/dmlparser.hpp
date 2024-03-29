#pragma once

#include <variant>
#include "../parser/SQLBaseVisitor.h"
#include "../common/error.hpp"
#include "../field/field.hpp"
#include "../common/utils.hpp"

namespace parser
{
    class DmlParser : public SQLBaseVisitor
    {
    public:
        antlrcpp::Any visitProgram(SQLParser::ProgramContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitStatement(SQLParser::StatementContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitCreate_db(SQLParser::Create_dbContext *ctx) override
        {
            std::string db_name = ctx->Identifier()->getText();
            DbError().throw_info("create db " + db_name);
            DatabaseSystem::getInstance().createDatabase(db_name);
            return visitChildren(ctx);
        }

        antlrcpp::Any visitDrop_db(SQLParser::Drop_dbContext *ctx) override
        {
            std::string db_name = ctx->Identifier()->getText();
            DbError().throw_info("drop db" + db_name);
            DatabaseSystem::getInstance().dropDatabase(db_name);
            return visitChildren(ctx);
        }

        antlrcpp::Any visitShow_dbs(SQLParser::Show_dbsContext *ctx) override
        {
            DbError().throw_info("show dbs");
            DatabaseSystem::getInstance().showDatabases();
            return visitChildren(ctx);
        }

        antlrcpp::Any visitUse_db(SQLParser::Use_dbContext *ctx) override
        {
            std::string db_name = ctx->Identifier()->getText();
            DbError().throw_info("use db" + db_name);
            DatabaseSystem::getInstance().useDatabase(db_name);
            return visitChildren(ctx);
        }

        antlrcpp::Any visitShow_tables(SQLParser::Show_tablesContext *ctx) override
        {
            DbError().throw_info("show tables");
            DatabaseSystem::getInstance().showTables();
            return visitChildren(ctx);
        }

        antlrcpp::Any visitShow_indexes(SQLParser::Show_indexesContext *ctx) override
        {
            DbError().throw_info("show indexes");
            DatabaseSystem::getInstance().showIndexes();
            return visitChildren(ctx);
        }

        antlrcpp::Any visitCreate_table(SQLParser::Create_tableContext *ctx) override
        {
            std::string table_name = ctx->Identifier()->getText();
            DbError().throw_info("create table " + table_name);
            auto field_ctx_list{ctx->field_list()->field()};
            std::vector<TableField> fields;
            for (auto field_ctx : field_ctx_list)
            {
                auto normal_field_ctx{dynamic_cast<SQLParser::Normal_fieldContext *>(field_ctx)};
                std::string field_name = normal_field_ctx->Identifier()->getText();
                std::string field_type = normal_field_ctx->type_()->children[0]->getText();
                uint32_t field_type_int = type_string_to_int(field_type);
                if (field_type_int == -1)
                {
                    DbError().throw_error(ErrorTypeEnum::SyntaxError, "unknown field type");
                }
                if (field_type_int == 2)
                {
                    uint32_t field_length = std::stoi(normal_field_ctx->type_()->Integer()->getText());
                    DbError().throw_info("field name: " + field_name + " field type: " + field_type + " field length: " + std::to_string(field_length));
                    fields.push_back(TableField(field_name, field_length));
                }
                else
                {
                    DbError().throw_info("field name: " + field_name + " field type: " + field_type);
                    fields.push_back(TableField((TableField::Type)field_type_int, field_name));
                }
            }
            DatabaseSystem::getInstance().createTable(table_name, fields);
            return visitChildren(ctx);
        }

        antlrcpp::Any visitDrop_table(SQLParser::Drop_tableContext *ctx) override
        {
            std::string table_name = ctx->Identifier()->getText();
            DbError().throw_info("drop table " + table_name);
            DatabaseSystem::getInstance().dropTable(table_name);
            return visitChildren(ctx);
        }

        antlrcpp::Any visitDescribe_table(SQLParser::Describe_tableContext *ctx) override
        {
            std::string table_name = ctx->Identifier()->getText();
            DbError().throw_info("describe table " + table_name);
            DatabaseSystem::getInstance().descTable(table_name);
            return visitChildren(ctx);
        }

        antlrcpp::Any visitLoad_table(SQLParser::Load_tableContext *ctx) override
        {
            DbError().throw_unimplemented("load table");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitInsert_into_table(SQLParser::Insert_into_tableContext *ctx) override
        {
            std::string table_name = ctx->Identifier()->getText();
            DbError().throw_info("insert into table " + table_name);
            auto value_list_vec{ctx->value_lists()->value_list()};
            for (auto value_list : value_list_vec)
            {
                TableRow row;
                auto value_list_ctx{dynamic_cast<SQLParser::Value_listContext *>(value_list)};
                auto value_vec{value_list_ctx->value()};
                for (int i = 0; i < value_vec.size(); i++)
                {
                    auto value_ctx{value_vec[i]};
                    auto value{value_ctx->getText()};
                    DbError().throw_info("value: " + value);
                    if (value[0] == '\'')
                    {
                        DbError().throw_info("string value");
                        std::string true_value = value.substr(1, value.length() - 2);
                        row.values.push_back(true_value);
                    }
                    else if (value.find('.') != std::string::npos)
                    {
                        DbError().throw_info("float value");
                        float true_value = std::stof(value);
                        row.values.push_back(true_value);
                    }
                    else
                    {
                        DbError().throw_info("int value");
                        int true_value = std::stoi(value);
                        row.values.push_back(true_value);
                    }
                }
                DatabaseSystem::getInstance().insertIntoTable(table_name, row);
            }
            return visitChildren(ctx);
        }

        antlrcpp::Any visitDelete_from_table(SQLParser::Delete_from_tableContext *ctx) override
        {
            DbError().throw_unimplemented("delete from table");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitUpdate_table(SQLParser::Update_tableContext *ctx) override
        {
            DbError().throw_unimplemented("update table");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitSelect_table_(SQLParser::Select_table_Context *ctx) override
        {
            DbError().throw_unimplemented("select table_");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitSelect_table(SQLParser::Select_tableContext *ctx) override
        {
            DbError().throw_unimplemented("select table");
        }

        antlrcpp::Any visitAlter_add_index(SQLParser::Alter_add_indexContext *ctx) override
        {
            DbError().throw_unimplemented("alter add index");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx) override
        {
            DbError().throw_unimplemented("alter drop index");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx) override
        {
            DbError().throw_unimplemented("alter table drop pk");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx) override
        {
            DbError().throw_unimplemented("alter table drop foreign key");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx) override
        {
            DbError().throw_unimplemented("alter table add pk");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx) override
        {
            DbError().throw_unimplemented("alter table add foreign key");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext *ctx) override
        {
            DbError().throw_unimplemented("alter table add unique");
            return visitChildren(ctx);
        }

        antlrcpp::Any visitField_list(SQLParser::Field_listContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitNormal_field(SQLParser::Normal_fieldContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitPrimary_key_field(SQLParser::Primary_key_fieldContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitForeign_key_field(SQLParser::Foreign_key_fieldContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitType_(SQLParser::Type_Context *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitOrder(SQLParser::OrderContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitValue_lists(SQLParser::Value_listsContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitValue_list(SQLParser::Value_listContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitValue(SQLParser::ValueContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_and_clause(SQLParser::Where_and_clauseContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_operator_expression(SQLParser::Where_operator_expressionContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_operator_select(SQLParser::Where_operator_selectContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_null(SQLParser::Where_nullContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_in_list(SQLParser::Where_in_listContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_in_select(SQLParser::Where_in_selectContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitWhere_like_string(SQLParser::Where_like_stringContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitColumn(SQLParser::ColumnContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitExpression(SQLParser::ExpressionContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitSet_clause(SQLParser::Set_clauseContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitSelectors(SQLParser::SelectorsContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitSelector(SQLParser::SelectorContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitIdentifiers(SQLParser::IdentifiersContext *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitOperator_(SQLParser::Operator_Context *ctx) override
        {
            return visitChildren(ctx);
        }

        antlrcpp::Any visitAggregator(SQLParser::AggregatorContext *ctx) override
        {
            return visitChildren(ctx);
        }
    };
}