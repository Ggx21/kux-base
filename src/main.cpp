#include "parser/SQLLexer.h"
#include "parser/SQLParser.h"
#include "parser/SQLBaseVisitor.h"

#include <system/dbsystem.hpp>
#include <system/dmlparser.hpp>
#include <iostream>
#include <string>

using namespace antlr4;
using namespace parser;

int main() {

    string inputSql;
    while (true)
    {
    std::cout << "Enter DML statement: ";
    std::getline(std::cin, inputSql);
    antlr4::ANTLRInputStream input(inputSql);
    SQLLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    SQLParser parser(&tokens);
    parser::DmlParser dmlParser;
    dmlParser.visitProgram(parser.program());
    }
}