#pragma once

#include "../parser/SQLBaseVisitor.h"
#include "../common/error.hpp"

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
            DbError().throw_info("create db " + db_name );
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
            DbError().throw_unimplemented("show dbs");
        }

        antlrcpp::Any visitUse_db(SQLParser::Use_dbContext *ctx) override
        {
            DbError().throw_unimplemented("use db");
        }

        antlrcpp::Any visitShow_tables(SQLParser::Show_tablesContext *ctx) override
        {
            DbError().throw_unimplemented("show tables");
        }

        antlrcpp::Any visitShow_indexes(SQLParser::Show_indexesContext *ctx) override
        {
            DbError().throw_unimplemented("show indexes");
        }

        antlrcpp::Any visitCreate_table(SQLParser::Create_tableContext *ctx) override
        {
            DbError().throw_unimplemented("create table");
        }

        antlrcpp::Any visitDrop_table(SQLParser::Drop_tableContext *ctx) override
        {
            DbError().throw_unimplemented("drop table");
        }

        antlrcpp::Any visitDescribe_table(SQLParser::Describe_tableContext *ctx) override
        {
            DbError().throw_unimplemented("describe table");
        }

        antlrcpp::Any visitLoad_table(SQLParser::Load_tableContext *ctx) override
        {
            DbError().throw_unimplemented("load table");
        }

        antlrcpp::Any visitInsert_into_table(SQLParser::Insert_into_tableContext *ctx) override
        {
            DbError().throw_unimplemented("insert into table");
        }

        antlrcpp::Any visitDelete_from_table(SQLParser::Delete_from_tableContext *ctx) override
        {
            DbError().throw_unimplemented("delete from table");
        }

        antlrcpp::Any visitUpdate_table(SQLParser::Update_tableContext *ctx) override
        {
            DbError().throw_unimplemented("update table");
        }

        antlrcpp::Any visitSelect_table_(SQLParser::Select_table_Context *ctx) override
        {
            DbError().throw_unimplemented("select table_");
        }

        antlrcpp::Any visitSelect_table(SQLParser::Select_tableContext *ctx) override
        {
            DbError().throw_unimplemented("select table");
        }

        antlrcpp::Any visitAlter_add_index(SQLParser::Alter_add_indexContext *ctx) override
        {
            DbError().throw_unimplemented("alter add index");
        }

        antlrcpp::Any visitAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx) override
        {
            DbError().throw_unimplemented("alter drop index");
        }

        antlrcpp::Any visitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx) override
        {
            DbError().throw_unimplemented("alter table drop pk");
        }

        antlrcpp::Any visitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx) override
        {
            DbError().throw_unimplemented("alter table drop foreign key");
        }

        antlrcpp::Any visitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx) override
        {
            DbError().throw_unimplemented("alter table add pk");
        }

        antlrcpp::Any visitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx) override
        {
            DbError().throw_unimplemented("alter table add foreign key");
        }

        antlrcpp::Any visitAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext *ctx) override
        {
            DbError().throw_unimplemented("alter table add unique");
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