#ifndef TCDetectStringParser_H
#define TCDetectStringParser_H

#include "TCDetectStringLexer.hpp"
#include "TCDetectStringAST.hpp"


//------------------------------------------------------------------------------
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

#endif // TCDetectStringParser_H
