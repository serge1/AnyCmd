#include <sstream>
#include <memory>
#include "TCDetectStringLexer.hpp"


class Result;
typedef std::unique_ptr<Result> ResultPtr;
class Result
{
  public:
    enum ResultType { STRING, NUMERIC, BOOLEAN };

  public:
      ~Result() {};

    virtual bool         to_bool() const = 0;
    virtual unsigned int to_num()  const = 0;
    virtual std::string  to_str()  const = 0;

    ResultType get_type() { return type; };

    static ResultType get_common( Result::ResultType rt1,
                                  Result::ResultType rt2 );
    static ResultPtr  convert_to_type( Result::ResultType type,
                                       ResultPtr&         result );

  protected:
    ResultType type;
};

class NumericResult : public Result
{
  public:
    NumericResult( unsigned int value_ ) : value( value_ ) {
        type = ResultType::NUMERIC;
    };

    virtual bool to_bool() const
    {
        return ( value != 0 );
    }

    virtual unsigned int to_num() const
    {
        return value;
    }

    virtual std::string to_str() const
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

  private:
    unsigned int value;
};

class StringResult : public Result
{
  public:
    StringResult( std::string value_ ) : value( value_ ) {
        type = ResultType::STRING;
    };

    virtual bool to_bool() const
    {
        return ( to_num() != 0 );
    }

    virtual unsigned int to_num() const
    {
        return std::strtoul( value.c_str(), 0 , 0 );
    }

    virtual std::string to_str() const
    {
        return value;
    }

  private:
    std::string value;
};

struct BooleanResult : public Result
{
  public:
    BooleanResult( bool value_ ) : value( value_ ) {
        type = ResultType::BOOLEAN;
    };

    virtual bool to_bool() const
    {
        return value;
    }

    virtual unsigned int to_num() const
    {
        return ( value ? 1 : 0 );
    }

    virtual std::string to_str() const
    {
        return ( value ? "1" : "0" );
    }

  private:
    bool value;
};


//------------------------------------------------------------------------------
Result::ResultType
Result::get_common( Result::ResultType rt1, Result::ResultType rt2 )
{
    return std::max( rt1, rt2 ); 
}


//------------------------------------------------------------------------------
ResultPtr
Result::convert_to_type( Result::ResultType type, ResultPtr& result )
{
    if ( type == result->get_type() ) {
        return std::move( result );
    }

    ResultPtr res;
    switch ( type ) {
    case Result::STRING:
        res = std::move( ResultPtr( new StringResult( result->to_str() ) ) );
        break;
    case Result::NUMERIC:
        res = std::move( ResultPtr( new NumericResult( result->to_num() ) ) );
        break;
    case Result::BOOLEAN:
        res = std::move( ResultPtr( new BooleanResult( result->to_bool() ) ) );
        break;
    default:
        break;
    }

    return res;
}


//------------------------------------------------------------------------------
class ASTNode
{
  public:
      virtual                         ~ASTNode()  {}
      virtual ResultPtr eval()      = 0;
      virtual std::string             to_string() = 0;
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
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new NumericResult( value ) ) );
        return res;
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
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new StringResult( str ) ) );
        return res;
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
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new NumericResult( value ) ) );
        return res;
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
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new NumericResult( 0 ) ) );
        return res;
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
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new BooleanResult( false ) ) );
        return res;
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
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new NumericResult( 0 ) ) );
        return res;
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
