#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <memory>
#include "TCDetectStringLexer.hpp"
#include "TCDetectStringResult.hpp"


class IFileContentProvider
{
  public:  
    virtual ~IFileContentProvider()             {};
    virtual std::string  get_file_name() const = 0;
    virtual const char*  get_content()         = 0;
    virtual unsigned int get_content_size()    = 0;
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

    virtual const char*  get_content()
    {
        return 0;
    }

    virtual unsigned int get_content_size()
    {
        return 0;
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
            const char* content = content_provider->get_content();
            res_value = content[value];
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

        struct stat filestatus;
        std::string ext;

        switch ( func ) {
        case Token::FUNC_EXT:
            ext = GetFileExtension( content_provider->get_file_name() );
            std::transform( ext.begin(), ext.end(), ext.begin(), toupper );
            res = std::move( ResultPtr( new StringResult( ext ) ) );
            break;
        case Token::FUNC_SIZE:
            stat( content_provider->get_file_name().c_str(), &filestatus );
            res = std::move( ResultPtr( new NumericResult( filestatus.st_size ) ) );
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
