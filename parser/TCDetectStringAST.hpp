#include <sstream>
#include <memory>
#include "TCDetectStringLexer.hpp"


//------------------------------------------------------------------------------
class ASTNode
{
  public:
    enum ResultType { BOOLEAN, NUMERIC, STRING };
    struct Result
    {
        ResultType type;

        unsigned int num_result;  // Valid if type == NUMERIC
        std::string  str_result;  //       if type == STRING
        bool         bool_result; //       if type == BOOLEAN
    };

  public:
      virtual             ~ASTNode()  {}
      virtual Result      eval()      = 0;
      virtual std::string to_string() = 0;
};


//------------------------------------------------------------------------------
class ASTNumericNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTNumericNode( std::string val )
    {
        value = std::strtoul( val.c_str(), 0 , 0 );
    }

//------------------------------------------------------------------------------
    virtual Result eval()
    {
        Result ret;
        ret.type       = NUMERIC;
        ret.num_result = value;
        return ret;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

  private:
    unsigned int value;
};


//------------------------------------------------------------------------------
class ASTStringNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTStringNode( std::string val )
    {
        str = val;
    }

//------------------------------------------------------------------------------
    virtual Result eval()
    {
        Result ret;
        ret.type       = STRING;
        ret.str_result = str;
        return ret;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        return "\"" + str + "\"";
    }

  private:
    std::string str;
};


//------------------------------------------------------------------------------
class ASTIndexNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTIndexNode( std::string val )
    {
        value = std::strtoul( val.c_str(), 0 , 0 );
    }

//------------------------------------------------------------------------------
    virtual Result eval()
    {
        Result ret;
        ret.type       = NUMERIC;
        ret.num_result = 0;
        return ret;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::ostringstream oss;
        oss << "[" << value << "]";
        return oss.str();
    }

  private:
    unsigned int value;
};


//------------------------------------------------------------------------------
class ASTFuncNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTFuncNode( Token::TokenType val )
    {
        func = val;
    }

//------------------------------------------------------------------------------
    virtual Result eval()
    {
        Result ret;
        ret.type       = NUMERIC;
        ret.num_result = 0;
        return ret;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::string funcstr;
        switch ( func ) {
        case Token::FUNC_EXT:
            funcstr = "EXT";
            break;
        case Token::FUNC_SIZE:
            funcstr = "SIZE";
            break;
        case Token::FUNC_FORCE:
            funcstr = "FORCE";
            break;
        case Token::FUNC_MULTIMEDIA:
            funcstr = "MULTIMEDIA";
            break;
        default:
            funcstr = "UNKNOWN";
            break;
        }

        return funcstr;
    }

  private:
      Token::TokenType func;
};


//------------------------------------------------------------------------------
class ASTFunc1Node : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTFunc1Node( std::string val1, std::string val2 )
    {
        str = val1;
        arg = val2;
    }

//------------------------------------------------------------------------------
    virtual Result eval()
    {
        Result ret;
        ret.type        = BOOLEAN;
        ret.bool_result = true;
        return ret;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::string ret;
        ret = str + "(" + arg + ")";
        return ret;
    }

  private:
    std::string str;
    std::string arg;
};


//------------------------------------------------------------------------------
class ASTOpNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTOpNode( Token::TokenType         operation,
               std::unique_ptr<ASTNode> val1,
               std::unique_ptr<ASTNode> val2 )
    {
        op    = operation;
        left  = std::move( val1 );
        right = std::move( val2 );
    }

//------------------------------------------------------------------------------
    virtual Result eval()
    {
        Result ret;
        ret.type       = NUMERIC;
        ret.num_result = 0;
        return ret;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::string opstr;
        switch ( op ) {
        case Token::OP_OR:
            opstr = "|";
            break;
        case Token::OP_AND:
            opstr = "&";
            break;
        case Token::OP_LG:
            opstr = ">";
            break;
        case Token::OP_SM:
            opstr = "<";
            break;
        case Token::OP_NEQ:
            opstr = "!=";
            break;
        case Token::OP_EQ:
            opstr = "=";
            break;
        default:
            opstr = "";
            break;
        }

        return "(" + left->to_string() + opstr + right->to_string() + ")";
    }

  public:
    Token::TokenType         op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};