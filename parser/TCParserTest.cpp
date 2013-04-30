#define BOOST_TEST_MODULE TCParser
#include <boost/test/unit_test.hpp>

#include "TCDetectStringParser.hpp"


BOOST_AUTO_TEST_CASE( lexer_test1 )
{
    Token               tk;
    TCDetectStringLexer lexer;
    lexer.set_text( "  [! ( = ) != &|=<     >]  " );

    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OPEN_BR_SQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NOT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OPEN_BR );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_EQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::CLOSE_BR );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NEQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_AND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_OR );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_EQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_SM );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_LG );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::CLOSE_BR_SQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
}


BOOST_AUTO_TEST_CASE( lexer_test2 )
{
    Token               tk;
    TCDetectStringLexer lexer;
    lexer.set_text( "  [! SIZE EXT EXT = FIND MULTIMEDIA!=FORCE" );

    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OPEN_BR_SQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NOT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_SIZE );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_EXT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_EXT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_EQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_FIND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_MULTIMEDIA );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NEQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_FORCE );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
}


BOOST_AUTO_TEST_CASE( lexer_test3 )
{
    Token               tk;
    TCDetectStringLexer lexer;
    lexer.set_text( "  [! SIZE EXTI EXT =" );

    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OPEN_BR_SQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NOT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_SIZE );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
}


BOOST_AUTO_TEST_CASE( lexer_test4 )
{
    Token               tk;
    TCDetectStringLexer lexer;
    lexer.set_text( "  [! SIZE 12 EXT 345 EXT 0 7890 11 = FIND 22 MULTIMEDIA 33!=44 FORCE 55" );

    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OPEN_BR_SQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NOT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_SIZE );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "12" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_EXT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "345" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_EXT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "0" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "7890" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "11" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_EQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_FIND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "22" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_MULTIMEDIA );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "33" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NEQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "44" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_FORCE );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::NUM );
    BOOST_CHECK_EQUAL( tk.value, std::string( "55" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
}


BOOST_AUTO_TEST_CASE( lexer_test5 )
{
    Token               tk;
    TCDetectStringLexer lexer;
    lexer.set_text( "  !=  EXT=\"CPP\"|SIZE= \"aaa bbb \" &FIND( \"my12 34\\\" FIND\")" );

    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_NEQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_EXT );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_EQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::STRING );
    BOOST_CHECK_EQUAL( tk.value, std::string( "CPP" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_OR );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_SIZE );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_EQ );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::STRING );
    BOOST_CHECK_EQUAL( tk.value, std::string( "aaa bbb " ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_AND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_FIND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OPEN_BR );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::STRING );
    BOOST_CHECK_EQUAL( tk.value, std::string( "my12 34\" FIND" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::CLOSE_BR );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
}


BOOST_AUTO_TEST_CASE( parser_test1 )
{
    TCDetectStringParser parser;

    BOOST_CHECK_EQUAL( parser.parse( "EXT" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "SIZE" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "FORCE" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "MULTIMEDIA" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "FIND( \"Hello\")" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "FINDI(\"Hello\" )" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "0" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "\"My string\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "[0]" ), true );

    BOOST_CHECK_EQUAL( parser.parse( "ext" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "size" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "FOrce" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "Multimedia" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "finD( \"Hello\")" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "FindI(\"Hello\" )" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "0" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "\"My string\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "[0]" ), true );

    BOOST_CHECK_EQUAL( parser.parse( "EXT1" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "1SIZE" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "FORCE(" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "MULTIMEDIA)" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "FIND()" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "FINDI(" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "-2" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "\"My string" ), false );
    BOOST_CHECK_EQUAL( parser.parse( "[0" ), false );
}


BOOST_AUTO_TEST_CASE( parser_test2 )
{
    TCDetectStringParser parser;

    BOOST_CHECK_EQUAL( parser.parse( "EXT=\"CPP\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "EXT=\"CPP\" | EXT=\"EXE\" | EXT=\"BAT\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "SIZE > 100" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "SIZE > 100 | FORCE" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "[1]=\"a\" & [2] | [3]<5 & EXT!=\"b\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "ext=\"CPP\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "ext=\"CPP\" | ext=\"EXE\" | EXT=\"BAT\"" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "size > 100" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "size > 100 | FORCE" ), true );
    BOOST_CHECK_EQUAL( parser.parse( "[1]=\"a\" & [2] | [3]<5 & ext!=\"b\"" ), true );
}


BOOST_AUTO_TEST_CASE( parser_test3 )
{
    TCDetectStringParser parser;

    BOOST_CHECK_EQUAL( parser.parse( "EXT=\"CPP\"" ), true );
    std::unique_ptr<ASTNode> ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(EXT=\"CPP\")" );

    BOOST_CHECK_EQUAL( parser.parse( "EXT=\"CPP\" | EXT=\"EXE\" | EXT=\"BAT\"" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "((EXT=\"CPP\")|((EXT=\"EXE\")|(EXT=\"BAT\")))" );

    BOOST_CHECK_EQUAL( parser.parse( "SIZE > 100" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(SIZE>100)" );

    BOOST_CHECK_EQUAL( parser.parse( "SIZE > 100 | FORCE" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "((SIZE>100)|FORCE)" );

    BOOST_CHECK_EQUAL( parser.parse( "[1]=\"a\" & [2] | [3]<5 & EXT!=\"b\"" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "((([1]=\"a\")&[2])|(([3]<5)&(EXT!=\"b\")))" );

    BOOST_CHECK_EQUAL( parser.parse( "[1]=5&FORCE" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(([1]=5)&FORCE)" );

    BOOST_CHECK_EQUAL( parser.parse( "FORCE&[1]=5" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(FORCE&([1]=5))" );

    BOOST_CHECK_EQUAL( parser.parse( "[1]=(5&FORCE)" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "([1]=(5&FORCE))" );

    BOOST_CHECK_EQUAL( parser.parse( "(FORCE&[1])=5" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "((FORCE&[1])=5)" );

    BOOST_CHECK_EQUAL( parser.parse( "(EXT!=5)" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(EXT!=5)" );

    BOOST_CHECK_EQUAL( parser.parse( "(Size > 10 & Size < 1000) & (ext=\"bat\")" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(((SIZE>10)&(SIZE<1000))&(EXT=\"bat\"))" );
}


BOOST_AUTO_TEST_CASE( result_test1 )
{
    NumericResult nr( 24 );
    BOOST_CHECK_EQUAL( nr.to_bool(), true );
    BOOST_CHECK_EQUAL( nr.to_num(), 24 );
    BOOST_CHECK_EQUAL( nr.to_str(), "24" );

    NumericResult nr1( 0 );
    BOOST_CHECK_EQUAL( nr1.to_bool(), false );
    BOOST_CHECK_EQUAL( nr1.to_num(), 0 );
    BOOST_CHECK_EQUAL( nr1.to_str(), "0" );

    BooleanResult nr2( false );
    BOOST_CHECK_EQUAL( nr2.to_bool(), false );
    BOOST_CHECK_EQUAL( nr2.to_num(), 0 );
    BOOST_CHECK_EQUAL( nr2.to_str(), "0" );

    BooleanResult nr3( true );
    BOOST_CHECK_EQUAL( nr3.to_bool(), true );
    BOOST_CHECK_EQUAL( nr3.to_num(), 1 );
    BOOST_CHECK_EQUAL( nr3.to_str(), "1" );

    StringResult nr4( "Abc" );
    BOOST_CHECK_EQUAL( nr4.to_bool(), false );
    BOOST_CHECK_EQUAL( nr4.to_num(), 0 );
    BOOST_CHECK_EQUAL( nr4.to_str(), "Abc" );

    StringResult nr5( "10Abc" );
    BOOST_CHECK_EQUAL( nr5.to_bool(), true );
    BOOST_CHECK_EQUAL( nr5.to_num(), 10 );
    BOOST_CHECK_EQUAL( nr5.to_str(), "10Abc" );
}


BOOST_AUTO_TEST_CASE( result_test2 )
{
    std::unique_ptr<Result> res1( new StringResult( "10" ) );
    std::unique_ptr<Result> res2( new NumericResult( 12 ) );
    std::unique_ptr<Result> res3( new BooleanResult( true ) );

    BOOST_CHECK_EQUAL( res1->get_type(), Result::STRING );
    BOOST_CHECK_EQUAL( res2->get_type(), Result::NUMERIC );
    BOOST_CHECK_EQUAL( res3->get_type(), Result::BOOLEAN );

    BOOST_CHECK_EQUAL( Result::get_common( res1->get_type(), res2->get_type() ), Result::NUMERIC );
    BOOST_CHECK_EQUAL( Result::get_common( res1->get_type(), res1->get_type() ), Result::STRING );
    BOOST_CHECK_EQUAL( Result::get_common( res2->get_type(), res2->get_type() ), Result::NUMERIC );
    BOOST_CHECK_EQUAL( Result::get_common( res2->get_type(), res1->get_type() ), Result::NUMERIC );
    BOOST_CHECK_EQUAL( Result::get_common( res2->get_type(), res3->get_type() ), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( Result::get_common( res3->get_type(), res1->get_type() ), Result::BOOLEAN );

    res1 = std::move( Result::convert_to_type( Result::STRING,  *res1 ) );
    BOOST_CHECK_EQUAL( res1->get_type(), Result::STRING );
    BOOST_CHECK_EQUAL( res1->to_str(), "10" );
    res1 = std::move( Result::convert_to_type( Result::NUMERIC,  *res1 ) );
    BOOST_CHECK_EQUAL( res1->get_type(), Result::NUMERIC );
    BOOST_CHECK_EQUAL( res1->to_num(), 10 );
    res1 = std::move( Result::convert_to_type( Result::BOOLEAN,  *res1 ) );
    BOOST_CHECK_EQUAL( res1->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( res1->to_bool(), true );

    BOOST_CHECK_EQUAL( res2->to_bool(), true );
    BOOST_CHECK_EQUAL( res2->to_num(), 12 );
    BOOST_CHECK_EQUAL( res2->to_str(), "12" );
}


BOOST_AUTO_TEST_CASE( eval_test1 )
{
    TCDetectStringParser parser;

    BOOST_CHECK_EQUAL( parser.parse( "Size" ), true );
    std::unique_ptr<ASTNode> ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "SIZE" );
    TCDetectStringFileContent fc( "c:\\autoexec.bat" );
    ast->set_content_provider( &fc );
    ResultPtr result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::NUMERIC );
    BOOST_CHECK_EQUAL( result->to_num(), 24 );

    BOOST_CHECK_EQUAL( parser.parse( "Ext" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "EXT" );
    fc.set_file_name( "c:\\autoexec.bat" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::STRING );
    BOOST_CHECK_EQUAL( result->to_str(), "BAT" );

    BOOST_CHECK_EQUAL( parser.parse( "exT" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "EXT" );
    fc.set_file_name( "exec.batch" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::STRING );
    BOOST_CHECK_EQUAL( result->to_str(), "BATCH" );

    BOOST_CHECK_EQUAL( parser.parse( "multimeDia" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "MULTIMEDIA" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), true );

    BOOST_CHECK_EQUAL( parser.parse( "forcE" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "FORCE" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), true );
}


BOOST_AUTO_TEST_CASE( eval_test2 )
{
    TCDetectStringParser parser;

    BOOST_CHECK_EQUAL( parser.parse( "Size = 10" ), true );
    std::unique_ptr<ASTNode> ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(SIZE=10)" );
    TCDetectStringFileContent fc( "c:\\autoexec.bat" );
    ast->set_content_provider( &fc );
    ResultPtr result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), false );

    BOOST_CHECK_EQUAL( parser.parse( "Size = 10 | Size < 1000" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "((SIZE=10)|(SIZE<1000))" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), true );

    BOOST_CHECK_EQUAL( parser.parse( "Size > 10 & Size < 1000" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "((SIZE>10)&(SIZE<1000))" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), true );

    BOOST_CHECK_EQUAL( parser.parse( "(Size > 10 & Size < 1000) & (ext=\"bat\")" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(((SIZE>10)&(SIZE<1000))&(EXT=\"bat\"))" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), true );

    BOOST_CHECK_EQUAL( parser.parse( "(Size > 10 & Size < 1000) & ext=\"kkk\"" ), true );
    ast = parser.get_result_AST();
    BOOST_CHECK_EQUAL( ast->to_string(), "(((SIZE>10)&(SIZE<1000))&(EXT=\"kkk\"))" );
    ast->set_content_provider( &fc );
    result = ast->eval();
    BOOST_CHECK_EQUAL( result->get_type(), Result::BOOLEAN );
    BOOST_CHECK_EQUAL( result->to_bool(), false );
}
