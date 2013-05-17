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

#ifndef TCDetectStringResult_H
#define TCDetectStringResult_H

#include <memory>
#include <sstream>
#include <algorithm>

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

    ResultType        get_type() const { return type; };
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
        return value[0];
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
    return ( rt1 > rt2 ) ? rt1 : rt2;
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

    ResultPtr   new1 = Result::convert_to_type( type, value1 );
    ResultPtr   new2 = Result::convert_to_type( type, value2 );
    std::string new_str1;
    std::string new_str2;

    switch ( type ) {
    case Result::BOOLEAN:
        return new1->to_bool() == new2->to_bool();
    case Result::NUMERIC:
        return new1->to_num() == new2->to_num();
    case Result::STRING:
        new_str1 = new1->to_str();
        new_str2 = new2->to_str();
        std::transform( new_str1.begin(), new_str1.end(), new_str1.begin(), toupper );
        std::transform( new_str2.begin(), new_str2.end(), new_str2.begin(), toupper );
        return new_str1 == new_str2;
    }

    return false;
}


//------------------------------------------------------------------------------
bool
operator<( const Result& value1, const Result& value2 )
{
    Result::ResultType type = Result::get_common( value1.get_type(),
                                                  value2.get_type() );

    ResultPtr   new1 = Result::convert_to_type( type, value1 );
    ResultPtr   new2 = Result::convert_to_type( type, value2 );
    std::string new_str1;
    std::string new_str2;

    switch ( type ) {
    case Result::BOOLEAN:
        return new1->to_bool() < new2->to_bool();
    case Result::NUMERIC:
        return new1->to_num() < new2->to_num();
    case Result::STRING:
        new_str1 = new1->to_str();
        new_str2 = new2->to_str();
        std::transform( new_str1.begin(), new_str1.end(), new_str1.begin(), toupper );
        std::transform( new_str2.begin(), new_str2.end(), new_str2.begin(), toupper );
        return new_str1 < new_str2;
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
operator!=( const Result& ptr1, const Result& ptr2 )
{
    return !(ptr1 == ptr2);
}


//------------------------------------------------------------------------------
bool
operator>( const Result& ptr1, const Result& ptr2 )
{
    return !(ptr1 < ptr2) && !(ptr1 == ptr2);
}

#endif // TCDetectStringResult_H
