#define BOOST_TEST_MODULE TCParser
#include <boost/test/unit_test.hpp>
//#define private public


#include <iostream>
#include <string>
#include <locale>


//------------------------------------------------------------------------------
class Token {
  public:
    enum TokenType { EMPTY,
                        NUM, STRING, BOOLEAN,
                        FUNC_EXT, FUNC_SIZE, FUNC_FORCE, FUNC_MULTIMEDIA,
                        FUNC_FIND, FUNC_FINDI,
                        OPEN_BR, CLOSE_BR, OPEN_BR_SQ, CLOSE_BR_SQ,
                        OP_NOT, OP_EQ, OP_NEQ, OP_AND, OP_OR, OP_LG, OP_SM };

    Token() : type( Token::EMPTY ), value( "" ) {};

  public:
    TokenType   type;
    std::string value;
};


//------------------------------------------------------------------------------
class TCDetectStringLexer
{
  public:
//------------------------------------------------------------------------------
    TCDetectStringLexer( std::string source )
    {
        src                    = source;
        current_parse_position = 0;
    }

//------------------------------------------------------------------------------
    Token get_next_token()
    {
        Token ret;

        if ( current_parse_position >= src.length() ) {
            ret.type = Token::EMPTY;
            return ret;
        }

        skip_blanks();

        struct word_to_token
        {
            std::string      word;
            Token::TokenType type;
        };
        word_to_token words[] = {
            { "EXT" ,       Token::FUNC_EXT        },
            { "SIZE",       Token::FUNC_SIZE       },
            { "FORCE",      Token::FUNC_FORCE      },
            { "MULTIMEDIA", Token::FUNC_MULTIMEDIA },
            { "FIND",       Token::FUNC_FIND       },
            { "FINDI",      Token::FUNC_FINDI      },
        };

        std::string word;
        if ( get_next_word( word ) ) {
            for ( int i = 0; i < sizeof( words ) / sizeof( words[0] ); ++i ) {
                if ( words[i].word == word ) {
                    ret.type                = words[i].type;
                    current_parse_position += word.length();
                    return ret;
                }
            }
        }

        if ( get_next_num( word ) ) {
            ret.type                = Token::NUM;
            ret.value               = word;
            current_parse_position += word.length();
            return ret;
        }

        if ( get_next_str( word ) ) {
        }

        if ( current_parse_position + 1 < src.length() ) {
            if ( src.substr( current_parse_position, 2 ) == "!=" ) {
                ret.type                = Token::OP_NEQ;
                current_parse_position += 2;
                return ret;
            }
        }

        struct sym_to_token
        {
            char             sym;
            Token::TokenType type;
        };
        sym_to_token symbols[] = {
            { '!', Token::OP_NOT      },
            { '&', Token::OP_AND      },
            { '|', Token::OP_OR       },
            { '=', Token::OP_EQ       },
            { '<', Token::OP_SM       },
            { '>', Token::OP_LG       },
            { '(', Token::OPEN_BR     },
            { ')', Token::CLOSE_BR    },
            { '[', Token::OPEN_BR_SQ  },
            { ']', Token::CLOSE_BR_SQ },
        };

        for ( int i = 0; i < sizeof( symbols ) / sizeof( symbols[0] ); ++i ) {
            if ( src[current_parse_position] == symbols[i].sym ) {
                ret.type = symbols[i].type;
                ++current_parse_position;
                return ret;
            }
        }

        return ret;
    }

  private:
//------------------------------------------------------------------------------
    void skip_blanks()
    {
        std::locale  loc;
        unsigned int size = src.length();
        while ( ( current_parse_position < size ) &&
                std::isspace( src[current_parse_position], loc ) ) {
            ++current_parse_position;
        }
    }

//------------------------------------------------------------------------------
    bool get_next_word( std::string& word )
    {
        bool ret = false;

        std::locale  loc;
        unsigned int size = src.length();
        unsigned int i    = current_parse_position;
        while ( ( i < size ) &&
                std::isalpha( src[i], loc ) ) {
            ++i;
        }

        word.clear();
        if ( i != current_parse_position ) {
            word = src.substr( current_parse_position, i - current_parse_position );
        }

        return !word.empty();
    }

//------------------------------------------------------------------------------
    bool get_next_num( std::string& word )
    {
        bool ret = false;

        std::locale  loc;
        unsigned int size = src.length();
        unsigned int i    = current_parse_position;
        while ( ( i < size ) &&
                std::isdigit( src[i], loc ) ) {
            ++i;
        }

        word.clear();
        if ( i != current_parse_position ) {
            word = src.substr( current_parse_position, i - current_parse_position );
        }

        return !word.empty();
    }

//------------------------------------------------------------------------------
    bool get_next_str( std::string& word )
    {
        bool ret = false;

        return ret;
    }

  private:
    std::string  src;
    unsigned int current_parse_position;
};


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
    TCDetectStringLexer lexer( "  [! SIZE 12 EXT 345 EXT 0 7890 11 = FIND 22MULTIMEDIA33!=44FORCE55" );

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
    TCDetectStringLexer lexer( "  EXT=\"CPP\"|SIZE= \"aaa bbb \" &FIND( \"my12 34 FIND\")" );

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
    BOOST_CHECK_EQUAL( tk.value, std::string( "aaa bbb" ) );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::OP_AND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::FUNC_FIND );
    tk = lexer.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, Token::EMPTY );
}
