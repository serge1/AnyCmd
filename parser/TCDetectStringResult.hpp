#ifndef TCDetectStringResult_H
#define TCDetectStringResult_H

//------------------------------------------------------------------------------
class Result;
typedef std::unique_ptr<Result> ResultPtr;
class Result
{
  public:
    enum ResultType { STRING, NUMERIC, BOOLEAN };

  public:
    virtual ~Result() {};

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


//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
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

#endif // TCDetectStringResult_H
