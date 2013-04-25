#include <sstream>
#include "TCDetectStringLexer.hpp"


//------------------------------------------------------------------------------
class ASTNode
{
  public:
      virtual bool        eval() {};
      virtual std::string to_string() = 0;
};


//------------------------------------------------------------------------------
class ASTNumericNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    virtual bool eval() {};

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
    virtual bool eval() {};

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        return str;
    }

  private:
    std::string str;
};


//------------------------------------------------------------------------------
class ASTBoolNode : public ASTNode
{
  public:
//------------------------------------------------------------------------------
    virtual bool eval() {};

//------------------------------------------------------------------------------
    virtual std::string to_string()
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

  private:
    bool value;
};
