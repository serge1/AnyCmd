/*
Copyright (C) 2012-2013 by Serge Lamikhov-Center

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/ 

#ifndef TCDetectStringAST_H
#define TCDetectStringAST_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
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
    virtual unsigned int get_content_at( unsigned int i )
                                                    const = 0;
    virtual unsigned int get_content_size()         const = 0;
    virtual bool         find ( std::string str )   const = 0;
    virtual bool         findi( std::string str )   const = 0;
};


class TCDetectStringFileContent : public IFileContentProvider
{
  public:  
    TCDetectStringFileContent( std::string file_name_ = "" ) :
        file_name( file_name_ ), loaded( false ) {}

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
        char szExt[_MAX_EXT];
        _splitpath_s( file_name.c_str(), 0, 0, 0, 0, 0, 0, szExt, _MAX_EXT );
        std::string res( szExt );
        if ( ( !res.empty() ) && ( res[0] == '.' ) ) {
            res = res.substr( 1 );
        }
        return res;
    }

    virtual unsigned int get_file_size() const
    {
        struct stat filestatus;
        stat( file_name.c_str(), &filestatus );
        return filestatus.st_size;
    }

    virtual unsigned int get_content_at( unsigned int i ) const
    {
        if ( load_content() ) {
            return data[i];
        }
        return 0;
    }

    virtual unsigned int get_content_size() const
    {
        if ( load_content() ) {
            return data.size();
        }
        return 0;
    }

    virtual bool find( std::string str ) const
    {
        bool ret = load_content();

        if ( ret ) {
            unsigned int pos = data.find( str );
            ret = ( pos != std::string::npos );
        }

        return ret;
    }

    virtual bool findi( std::string str ) const
    {
        bool ret = load_content();

        if ( ret ) {
            std::string str1( data );
            std::transform( str.begin(), str.end(), str.begin(), toupper );
            std::transform( str1.begin(), str1.end(), str1.begin(), toupper );
            unsigned int pos = str1.find( str );
            ret = ( pos != std::string::npos );
        }

        return ret;
    }

  private:
    bool load_content() const
    {
        if ( loaded ) {
            return true;
        }

        std::ifstream ifs;
        ifs.open( file_name, std::ios_base::in | std::ios_base::binary );
        if ( !ifs.good() ) {
            return false;
        }

        char            buffer[8192];
        std::streamsize real_size;
        ifs.read( (char*)buffer, sizeof( buffer ) );
        real_size = ifs.gcount();
        ifs.close();

        loaded = true;
        data.assign( buffer, (size_t)real_size );

        return true;
    }

  private:
    std::string           file_name;
    mutable bool          loaded;
    mutable std::string   data;
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
        bool found;
        if ( str == "FINDI" ) {
            found = content_provider->findi( arg );
        }
        else {
            found = content_provider->find( arg );
        }

        ResultPtr res = std::move( ResultPtr( new BooleanResult( found ) ) );
        return res;
    };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::string ret;
        ret = str + "(\"" + arg + "\")";
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
                new BooleanResult( *(left->eval()) || *(right->eval()) ) ) );
            break;
        case Token::OP_AND:
            res = std::move( ResultPtr(
                new BooleanResult( *(left->eval()) && *(right->eval()) ) ) );
            break;
        case Token::OP_LG:
            res = std::move( ResultPtr(
                new BooleanResult( *(left->eval()) > *(right->eval()) ) ) );
            break;
        case Token::OP_SM:
            res = std::move( ResultPtr(
                new BooleanResult( *(left->eval()) < *(right->eval()) ) ) );
            break;
        case Token::OP_NEQ:
            res = std::move( ResultPtr(
                new BooleanResult( *(left->eval()) != *(right->eval()) ) ) );
            break;
        case Token::OP_EQ:
            res = std::move( ResultPtr(
                new BooleanResult( *(left->eval()) == *(right->eval()) ) ) );
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

#endif // TCDetectStringAST_H
