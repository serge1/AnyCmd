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
    lexer.set_text( "  EXT=\"CPP\"|SIZE= \"aaa bbb \" &FIND( \"my12 34\\\" FIND\")" );

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
    BOOST_CHECK_EQUAL( parser.to_string(), "(EXT=\"CPP\")" );
}
