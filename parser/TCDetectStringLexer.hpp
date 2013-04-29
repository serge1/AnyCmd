#ifndef TCDetectStringLexer_H
#define TCDetectStringLexer_H

#include <iostream>
#include <string>
#include <locale>

//---------------------------------------------------------------------------
static void
searchAndReplace( std::string& value, std::string const& search,
                  std::string const& replace ) 
{ 
    std::string::size_type  next; 

    for ( next = value.find( search );      // Try and find the first match 
        next != std::string::npos;          // next is npos if nothing was found 
        next = value.find( search, next )   // search for the next match starting after 
        // the last match that was found. 
        ) { 
            // Inside the loop. So we found a match. 
            if ( next == 0 || value[next - 1] != '\r' ) {
                value.replace( next, search.length(), replace );  // Do the replacement. 
            }
            // Move to just after the replace. This is the point were we start 
            // the next search from. 
            next += replace.length();
    }
}


std::string GetFileExtension( const std::string& FileName )
{
    size_t pos      = FileName.find_last_of(".");
    size_t slashpos = FileName.find_last_of("\\");
    if ( slashpos != std::string::npos ) {
        if ( pos != std::string::npos &&
             pos < slashpos ) {
            return "";
        }
    }
    if( pos != std::string::npos ) {
        return FileName.substr( pos + 1 );
    }
    return "";
}

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
    void set_text( std::string source )
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
                std::transform( word.begin(), word.end(), word.begin(), ::toupper );
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
            ret.type                = Token::STRING;
            ret.value               = word;
            return ret;
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
        std::locale  loc;
        unsigned int size = src.length();
        unsigned int i    = current_parse_position;
        while ( ( i < size ) &&
                std::isalpha( src[i], loc ) ) {
            ++i;
        }

        if ( ( i < size ) && std::isdigit( src[i], loc ) ) {
            return false; 
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
        std::locale  loc;
        unsigned int size = src.length();
        unsigned int i    = current_parse_position;
        while ( ( i < size ) &&
                std::isdigit( src[i], loc ) ) {
            ++i;
        }

        if ( ( i < size ) && std::isalpha( src[i], loc ) ) {
            return false; 
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
        if ( src[current_parse_position] != '"' ) {
            return false;
        }

        unsigned int size = src.length();
        unsigned int i    = current_parse_position + 1;

        while ( i < size ) {
            if ( src[i] == '"' && src[i-1] != '\\') {
                break;
            }
            ++i;
        }

        word.clear();
        if ( i < size ) {
            word = src.substr( current_parse_position + 1,
                               i - current_parse_position - 1 );
            current_parse_position += word.length() + 2;
            searchAndReplace( word, "\\\"", "\"" );
            return true;
        }

        return false;
    }

  private:
    std::string  src;
    unsigned int current_parse_position;
};

#endif // TCDetectStringLexer_H
