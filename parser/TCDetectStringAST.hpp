#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <fstream>
#include <memory>
#include "TCDetectStringLexer.hpp"
#include "TCDetectStringResult.hpp"


class IFileContentProvider
{
  public:  
    virtual ~IFileContentProvider()                        {};
    virtual std::string  get_file_name()            const = 0;
    virtual std::string  get_file_ext()             const = 0;
    virtual unsigned int get_file_size()            const = 0;
    virtual unsigned int get_content_at( unsigned int i ) = 0;
    virtual unsigned int get_content_size()               = 0;
    virtual bool         find( std::string str, bool case_insensitive ) = 0;
};


class TCDetectStringFileContent : public IFileContentProvider
{
  public:  
    TCDetectStringFileContent( std::string file_name_ = "" ) : file_name( file_name_ ) {};

    void set_file_name( std::string file_name_ )
    {
        file_name = file_name_;
    }

    virtual std::string  get_file_name() const
    {
        return file_name;
    }

    virtual std::string get_file_ext() const
    {
        size_t pos      = file_name.find_last_of(".");
        size_t slashpos = file_name.find_last_of("\\");
        if ( slashpos != std::string::npos ) {
            if ( pos != std::string::npos &&
                 pos < slashpos ) {
                return "";
            }
        }
        if( pos != std::string::npos ) {
            return file_name.substr( pos + 1 );
        }
        return "";
    }

    virtual unsigned int get_file_size() const
    {
        struct stat filestatus;
        stat( file_name.c_str(), &filestatus );
        return filestatus.st_size;
    }

    virtual unsigned int get_content_at( unsigned int i )
    {
        return 0;
    }

    virtual unsigned int get_content_size()
    {
        return 0;
    }

    virtual bool find( std::string str, bool case_insensitive )
    {
        return true;
    }

  private:
    std::string file_name;
};


//------------------------------------------------------------------------------
class ASTNode
{
  public:
    virtual             ~ASTNode()  {}
    virtual void        set_content_provider( IFileContentProvider* provider ) {};
    virtual ResultPtr   eval()      = 0;
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
    ASTIndexNode( std::string val, IFileContentProvider* content_provider_ = 0 )
    {
        content_provider = content_provider_;
        value = std::strtoul( val.c_str(), 0 , 0 );
    }

//------------------------------------------------------------------------------
    virtual void set_content_provider( IFileContentProvider* provider )
    {
        content_provider = provider;
    }

//------------------------------------------------------------------------------
    virtual ResultPtr eval()
    {
        unsigned int res_value = 0;
        if ( value < content_provider->get_content_size() ) {
            res_value = content_provider->get_content_at( value );
        }
        ResultPtr res = std::move( ResultPtr( new NumericResult( res_value ) ) );
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
    IFileContentProvider* content_provider;
    unsigned int          value;
};


//------------------------------------------------------------------------------
class ASTFuncNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTFuncNode( Token::TokenType val, IFileContentProvider* content_provider_ = 0 )
    {
        content_provider = content_provider_;
        func             = val;
    }

//------------------------------------------------------------------------------
    virtual void set_content_provider( IFileContentProvider* provider )
    {
        content_provider = provider;
    }

//------------------------------------------------------------------------------
    virtual ResultPtr eval()
    {
        ResultPtr res = std::move( ResultPtr( new NumericResult( 0 ) ) );

        std::string ext;

        switch ( func ) {
        case Token::FUNC_EXT:
            ext = content_provider->get_file_ext();
            std::transform( ext.begin(), ext.end(), ext.begin(), toupper );
            res = std::move( ResultPtr( new StringResult( ext ) ) );
            break;
        case Token::FUNC_SIZE:
            res = std::move( ResultPtr(
                new NumericResult( content_provider->get_file_size() ) ) );
            break;
        case Token::FUNC_FORCE:
            res = std::move( ResultPtr( new BooleanResult( true ) ) );
            break;
        case Token::FUNC_MULTIMEDIA:
            res = std::move( ResultPtr( new BooleanResult( true ) ) );
            break;
        default:
            res = std::move( ResultPtr( new BooleanResult( false ) ) );
            break;
        }

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
    IFileContentProvider* content_provider;
    Token::TokenType      func;
};


//------------------------------------------------------------------------------
class ASTFunc1Node : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    ASTFunc1Node( std::string val1, std::string val2,
                  IFileContentProvider* content_provider_ = 0 )
    {
        content_provider = content_provider_;
        str              = val1;
        arg              = val2;
    }

//------------------------------------------------------------------------------
    virtual void set_content_provider( IFileContentProvider* provider )
    {
        content_provider = provider;
    }

//------------------------------------------------------------------------------
    virtual ResultPtr eval()
    {
        bool found = content_provider->find( arg, str == "FINDI" );
        ResultPtr res = std::move( ResultPtr( new BooleanResult( found ) ) );
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
    std::string           str;
    std::string           arg;
    IFileContentProvider* content_provider;
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
    virtual void set_content_provider( IFileContentProvider* provider )
    {
        left ->set_content_provider( provider );
        right->set_content_provider( provider );
    }

//------------------------------------------------------------------------------
    virtual ResultPtr eval()
    {
        ResultPtr res;
        switch ( op ) {
        case Token::OP_OR:
            res = std::move( ResultPtr(
                new BooleanResult( left->eval() || right->eval() ) ) );
            break;
        case Token::OP_AND:
            res = std::move( ResultPtr(
                new BooleanResult( left->eval() && right->eval() ) ) );
            break;
        case Token::OP_LG:
            res = std::move( ResultPtr(
                new BooleanResult( left->eval() > right->eval() ) ) );
            break;
        case Token::OP_SM:
            res = std::move( ResultPtr(
                new BooleanResult( left->eval() < right->eval() ) ) );
            break;
        case Token::OP_NEQ:
            res = std::move( ResultPtr(
                new BooleanResult( left->eval() != right->eval() ) ) );
            break;
        case Token::OP_EQ:
            res = std::move( ResultPtr(
                new BooleanResult( left->eval() == right->eval() ) ) );
            break;
        default:
            break;
        }
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
