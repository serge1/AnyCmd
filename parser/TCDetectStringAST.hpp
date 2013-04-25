#include <sstream>
#include <memory>
#include "TCDetectStringLexer.hpp"


//------------------------------------------------------------------------------
class ASTNode
{
  public:
      virtual bool eval() { return true; };
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
    virtual bool eval() { return true; };

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
    virtual bool eval() { return true; };

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
    virtual bool eval() { return true; };

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
    ASTFuncNode( std::string val )
    {
        str = val;
    }

//------------------------------------------------------------------------------
    virtual bool eval() { return true; };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        return str;
    }

  private:
    std::string str;
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
    virtual bool eval() { return true; };

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
    ASTOpNode( std::string operation,
               std::unique_ptr<ASTNode> val1,
               std::unique_ptr<ASTNode> val2 )
    {
        op    = operation;
        left  = std::move( val1 );
        right = std::move( val2 );
    }

//------------------------------------------------------------------------------
    virtual bool eval() { return true; };

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::string ret;
        ret = "(" + left->to_string() + op + right->to_string() + ")";
        return ret;
    }

  private:
    std::string              op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};
