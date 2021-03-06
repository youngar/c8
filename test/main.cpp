#include <iostream>
#include <exception>

#include <gtest/gtest.h>

#include "AST.hpp"
#include "Debug.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

#include <lingo/file.hpp>
#include <lingo/io.hpp>
#include <lingo/error.hpp>
#include <lingo/character.hpp>

namespace c8 {

TEST(c8test, test) {
    // Get the test material directory
    auto test_material = getenv("C8_TEST_MATERIAL");
    ASSERT_NE(test_material, nullptr);

    // std::string test_file = test_material + "/test.js";
    std::string test_file = test_material;
    test_file += "/test.js";
    std::cout << test_file;

    // std::cout << (test_material += "/test.js");

    lingo::File input(test_file);
    lingo::Character_stream cs(input);
    lingo::Token_stream ts;

    c8::Lexer lex(cs,ts);

    lex();
    Parser parser(ts);

    ast::NodePtr<ast::FunctionDeclaration> funcptr = parser.ParseFunctionDef();
    ast::DebugPrinter dbg;
    dbg.Visit(funcptr.get());
}
} // namespace c8

