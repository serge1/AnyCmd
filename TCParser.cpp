#include <iostream>
#include <string>

class TCParser
{
  public:
    bool parse( std::string str )
    {
        current_parse_position = 0;
        Token token = get_next_token();
        while ( token.type != Token::EMPTY ) {
            std::cout << token.type << " ";
            switch ( token.type ) {
            case Token::BOOLEAN:
                std::cout << token.value_bool << std::endl;
                break;
            case Token::STRING:
            case Token::FUNC:
            case Token::OP:
                std::cout << token.value_str << std::endl;
                break;
            case Token::NUM:
                std::cout << token.value_num << std::endl;
                break;
            }
            token = get_next_token();
        }
        return true;
    }


    class Token {
      public:
        enum TokenType { EMPTY, NUM, STRING, BOOLEAN, FUNC, OP };
        Token() : type( Token::EMPTY ), value_bool( false ), value_str( "" ), value_num( 0 ) {};

      public:
        TokenType   type;
        bool        value_bool;
        std::string value_str;
        int         value_num;
    };

    Token get_next_token()
    {
        Token ret;
        return ret;
    }

    int current_parse_position;
};


int main()
{
    TCParser parser;

    parser.parse( "" );
    
    return 0;
}
