#define BOOST_TEST_MODULE TCParser
#include <boost/test/unit_test.hpp>


#include "TCDetectStringParser.hpp"


//------------------------------------------------------------------------------
class TCParser
{
  public:
//------------------------------------------------------------------------------
    TCParser()
    {
    }

//------------------------------------------------------------------------------
    bool parse( std::string str )
    {
        TCDetectStringLexer lexer( str );
        Token token = lexer.get_next_token();
        while ( token.type != Token::EMPTY ) {
            std::cout << token.type << " ";
            token = lexer.get_next_token();
        }
        return true;
    }
};


BOOST_AUTO_TEST_CASE( test1 )
{
    Token               tk;
    TCDetectStringLexer lexer( "  [! ( = ) != &|=<     >]  " );

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


BOOST_AUTO_TEST_CASE( test2 )
{
    Token               tk;
    TCDetectStringLexer lexer( "  [! SIZE EXT EXT = FIND MULTIMEDIA!=FORCE" );

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


BOOST_AUTO_TEST_CASE( test3 )
{
    Token               tk;
    TCDetectStringLexer lexer( "  [! SIZE EXTI EXT =" );

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


BOOST_AUTO_TEST_CASE( test4 )
{
    Token               tk;
    TCDetectStringLexer lexer( "  [! SIZE 12 EXT 345 EXT 0 7890 11 = FIND 22 MULTIMEDIA 33!=44 FORCE 55" );

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


BOOST_AUTO_TEST_CASE( test5 )
{
    Token               tk;
    TCDetectStringLexer lexer( "  EXT=\"CPP\"|SIZE= \"aaa bbb \" &FIND( \"my12 34\\\" FIND\")" );

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
