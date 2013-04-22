#define BOOST_TEST_MODULE TCParser
#include <boost/test/unit_test.hpp>
#define private public


#include <iostream>
#include <string>
#include <locale>

class TCParser
{
  public:
    TCParser()
    {
    }

    bool parse( std::string str )
    {
        src                    = str;
        current_parse_position = 0;
        Token token            = get_next_token();
        while ( token.type != Token::EMPTY ) {
            std::cout << token.type << " ";
            token = get_next_token();
        }
        return true;
    }

    class Token {
      public:
        enum TokenType { EMPTY, NUM, STRING, BOOLEAN, FUNC, OPEN_BR, CLOSE_BR, OP_NOT, OP_EQ, OP_NEQ, OP_AND, OP_OR, OP_LG, OP_SM };
        Token() : type( Token::EMPTY ), value_bool( false ), value_str( "" ), value_num( 0 ) {};

      public:
        TokenType   type;
        bool        value_bool;
        std::string value_str;
        int         value_num;
    };

    void skip_blanks()
    {
        std::locale loc;
        while ( std::isspace( src[current_parse_position], loc ) ) {
            ++current_parse_position;
        }
    }

    Token get_next_token()
    {
        Token ret;

        if ( current_parse_position >= src.length() ) {
            ret.type = Token::EMPTY;
            return ret;
        }

        skip_blanks();

        if ( current_parse_position + 1 < src.length() ) {
            if ( src.substr( current_parse_position, 2 ) == "!=" ) {
                current_parse_position += 2;
                ret.type = Token::OP_NEQ;
                return ret;
            }
        }

        switch ( src[current_parse_position] ) {
        case '!':
            ret.type = Token::OP_NOT;
            ++current_parse_position;
            return ret;
        case '(':
            ret.type = Token::OPEN_BR;
            ++current_parse_position;
            return ret;
        case ')':
            ret.type = Token::CLOSE_BR;
            ++current_parse_position;
            return ret;
        case '&':
            ret.type = Token::OP_AND;
            ++current_parse_position;
            return ret;
        case '|':
            ret.type = Token::OP_OR;
            ++current_parse_position;
            return ret;
        case '=':
            ret.type = Token::OP_EQ;
            ++current_parse_position;
            return ret;
        case '<':
            ret.type = Token::OP_SM;
            ++current_parse_position;
            return ret;
        case '>':
            ret.type = Token::OP_LG;
            ++current_parse_position;
            return ret;
        }

        return ret;
    }

  private:
    std::string  src;
    unsigned int current_parse_position;
};


BOOST_AUTO_TEST_CASE( test1 )
{
    TCParser parser;

    parser.src                    = "  ! ( = ) != &|=<     >";
    parser.current_parse_position = 0;

    TCParser::Token tk;
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_NOT );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OPEN_BR );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_EQ );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::CLOSE_BR );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_NEQ );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_AND );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_OR );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_EQ );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_SM );
    tk = parser.get_next_token();
    BOOST_CHECK_EQUAL( tk.type, TCParser::Token::OP_LG );
}
