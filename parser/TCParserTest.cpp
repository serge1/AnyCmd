#define BOOST_TEST_MODULE TCParser
#include <boost/test/unit_test.hpp>


#include "TCDetectStringParser.hpp"


#define GET_AND_EXPECT( token, value2 ) \
    token = lexer.get_next_token();     \
    if ( token.type != value2 ) return false;


//------------------------------------------------------------------------------
//          expr0 -> expr1 [OP_EQ  expr1]*
//          expr1 -> expr2 [OP_NEQ expr2]*
//          expr2 -> expr3 [OP_SM  expr3]*
//          expr3 -> expr4 [OP_LG  expr4]*
//          expr4 -> expr5 [OP_AND expr5]*
//          expr5 -> term  [OP_OR  term ]*
//
//          term -> FUNC(EXT, SIZE, FORCE, MULTIMEDIA)
//               -> FUNC(FIND, FINDI) OPEN_BR STRING CLOSE_BR
//               -> NUM
//               -> STRING
//               -> OPEN_BR_SQ NUM CLOSE_BR_SQ
//               -> OPEN_BR expr CLOSE_BR
//               -> OP_NOT OPEN_BR expr CLOSE_BR
//------------------------------------------------------------------------------
class TCDetectStringParser
{
  public:
//------------------------------------------------------------------------------
    TCDetectStringParser()
    {
    }

//------------------------------------------------------------------------------
    bool parse( std::string str )
    {
        bool ret = false;

        lexer.set_text( str );

        tk = lexer.get_next_token();

        ret = parse_expr( 0 );

        return ret;
    }

  private:
//------------------------------------------------------------------------------
    const static int max_precidence = 5;
      
//------------------------------------------------------------------------------
    bool parse_expr( int precidence )
    {
        bool             ret     = false;
        Token::TokenType opers[] = { Token::OP_EQ,
                                     Token::OP_NEQ,
                                     Token::OP_SM,
                                     Token::OP_LG,
                                     Token::OP_AND,
                                     Token::OP_OR };

        do {
            if ( precidence != max_precidence ) {
                ret = parse_expr( precidence + 1 );
            }
            else {
                ret = parse_term();
            }

            if ( ret ) {
                if ( ( tk.type == Token::EMPTY ) ||
                     ( tk.type != opers[precidence] ) ) {
                    break;
                }
                tk = lexer.get_next_token();
            }
        } while ( ret );

        if ( ret && (precidence == 0) && ( tk.type != Token::EMPTY ) ) {
            ret = false;
        }

        return ret;
    }

//------------------------------------------------------------------------------
    bool parse_term()
    {
        bool ret;

        switch ( tk.type )
        {
        case Token::FUNC_EXT:
        case Token::FUNC_SIZE:
        case Token::FUNC_FORCE:
        case Token::FUNC_MULTIMEDIA:
            ret = true;
            break;
        case Token::FUNC_FIND:
        case Token::FUNC_FINDI:
            GET_AND_EXPECT( tk, Token::OPEN_BR );
            GET_AND_EXPECT( tk, Token::STRING );
            {
                ret = true;
            }
            GET_AND_EXPECT( tk, Token::CLOSE_BR );
            break;
        case Token::NUM:
            ret = true;
            break;
        case Token::STRING:
            ret = true;
            break;
        case Token::OPEN_BR_SQ:
            GET_AND_EXPECT( tk, Token::NUM );
            {
                ret = true;
            }
            GET_AND_EXPECT( tk, Token::CLOSE_BR_SQ );
            break;
        default:
            ret = false;
        }

        tk = lexer.get_next_token();
        
        return ret;
    }

    TCDetectStringLexer lexer;
    Token               tk;
};


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
