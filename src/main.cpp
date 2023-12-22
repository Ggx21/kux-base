#include "parser/SQLLexer.h"
#include "parser/SQLParser.h"
#include "parser/SQLBaseVisitor.h"

#include <system/dbsystem.hpp>
#include <system/dmlparser.hpp>
#include <iostream>
#include <string>

using namespace antlr4;
using namespace parser;

int main()
{

    string inputSql;
    std::ostringstream fullInput; // 用于存储多行输入
    while (true)
    {
        int consecutiveNewlines = 0;
        std::cout << "SQL> ";
        while (consecutiveNewlines < 2)
        {
            std::string line;
            std::getline(std::cin, line);

            if (line.empty())
            {
                consecutiveNewlines++;
            }
            else
            {
                consecutiveNewlines = 0;
            }

            fullInput << line << '\n'; // 将每行添加到完整输入字符串中
        }

        //covert to string
        std::string fullInputStr = fullInput.str();
        antlr4::ANTLRInputStream input(fullInputStr);
        SQLLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        SQLParser parser(&tokens);
        parser::DmlParser dmlParser;
        dmlParser.visitProgram(parser.program());
    }
}