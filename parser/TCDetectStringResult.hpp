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

    ResultType get_type() const { return type; };

    static ResultType get_common( Result::ResultType rt1,
                                  Result::ResultType rt2 );
    static ResultPtr  convert_to_type( Result::ResultType type,
                                       const Result&      result );

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
Result::get_common( const Result::ResultType rt1, const Result::ResultType rt2 )
{
    return std::max( rt1, rt2 ); 
}


//------------------------------------------------------------------------------
ResultPtr
Result::convert_to_type( Result::ResultType type, const Result& result )
{
    ResultPtr res;
    switch ( type ) {
    case Result::STRING:
        res = std::move( ResultPtr( new StringResult( result.to_str() ) ) );
        break;
    case Result::NUMERIC:
        res = std::move( ResultPtr( new NumericResult( result.to_num() ) ) );
        break;
    case Result::BOOLEAN:
        res = std::move( ResultPtr( new BooleanResult( result.to_bool() ) ) );
        break;
    default:
        break;
    }

    return res;
}


//------------------------------------------------------------------------------
bool
operator==( const Result& value1, const Result& value2 )
{
    Result::ResultType type = Result::get_common( value1.get_type(),
                                                  value2.get_type() );

    ResultPtr new1 = Result::convert_to_type( type, value1 );
    ResultPtr new2 = Result::convert_to_type( type, value2 );

    switch ( type ) {
    case Result::BOOLEAN:
        return new1->to_bool() == new2->to_bool();
    case Result::NUMERIC:
        return new1->to_num() == new2->to_num();
    case Result::STRING:
        return new1->to_str() == new2->to_str();
    }

    return false;
}


//------------------------------------------------------------------------------
bool
operator<( const Result& value1, const Result& value2 )
{
    Result::ResultType type = Result::get_common( value1.get_type(),
                                                  value2.get_type() );

    ResultPtr new1 = Result::convert_to_type( type, value1 );
    ResultPtr new2 = Result::convert_to_type( type, value2 );

    switch ( type ) {
    case Result::BOOLEAN:
        return new1->to_bool() < new2->to_bool();
    case Result::NUMERIC:
        return new1->to_num() < new2->to_num();
    case Result::STRING:
        return new1->to_str() < new2->to_str();
    }

    return false;
}


//------------------------------------------------------------------------------
bool
operator||( const Result& value1, const Result& value2 )
{
    ResultPtr new1 = Result::convert_to_type( Result::BOOLEAN, value1 );
    ResultPtr new2 = Result::convert_to_type( Result::BOOLEAN, value2 );

    return new1->to_bool() || new2->to_bool();
}


//------------------------------------------------------------------------------
bool
operator&&( const Result& value1, const Result& value2 )
{
    ResultPtr new1 = Result::convert_to_type( Result::BOOLEAN, value1 );
    ResultPtr new2 = Result::convert_to_type( Result::BOOLEAN, value2 );

    return new1->to_bool() && new2->to_bool();
}


//------------------------------------------------------------------------------
bool
operator!=( const ResultPtr& ptr1, const ResultPtr& ptr2 )
{
    return !(ptr1 == ptr2);
}


//------------------------------------------------------------------------------
bool
operator>( const ResultPtr& ptr1, const ResultPtr& ptr2 )
{
    return !(ptr1 < ptr2) && !(ptr1 == ptr2);
}

#endif // TCDetectStringResult_H
