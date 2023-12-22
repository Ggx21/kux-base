#!/bin/sh

cd src &&
  java -jar ../3rd_party/antlr-4.8-complete.jar \
    -Dlanguage=Cpp -visitor -no-listener \
    parser/SQL.g4 -o . -package parser
