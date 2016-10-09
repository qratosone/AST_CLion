#include <iostream>
#include <cstdio>
#include <memory>
#include <cstring>
using namespace std;

class Expression;
class NumberExpression;
class BinaryExpression;

enum class BinaryOperator
{
    Plus,
    Minus,
    Multiply,
    Divide,
};

class Expression
{
public:
    class IVisior
    {
        public:
            virtual void Visit(NumberExpression* node)=0;
            virtual void Visit(BinaryExpression* node)=0;
            virtual ~IVisior()= default;
    };
    virtual void Accept(IVisior* visitor)=0;
    virtual ~Expression()= default;
};

class NumberExpression:public Expression
{
public:
    int Value;
    void Accept(IVisior* visitor)
    {
        visitor->Visit(this);
    }
    NumberExpression(int number)
    {
        Value=number;
    }

};

class BinaryExpression:public Expression
{
public:
    shared_ptr<Expression> First;
    shared_ptr<Expression> Second;
    BinaryOperator Op;
    BinaryExpression(BinaryOperator theOp,shared_ptr<Expression> theLeft,shared_ptr<Expression> theRight):
            Op(theOp),First(theLeft),Second(theRight){

    }
    void Accept(IVisior* visitor)
    {
        visitor->Visit(this);
    }
};

class ExpressionPointer:public Expression::IVisior
{
public:
    void Visit(NumberExpression* node)
    {

    }
    void Visit(BinaryExpression* node)
    {

    }
};

struct Exception
{
    char* Start;
    char* Error;

    Exception(char* aStart , char* aError)
    {
        Start=aStart;
        Error=aError;
    }
};

bool Is(char*& Stream , const char* Text)
{
    size_t len=strlen(Text);
    char* Read=Stream;
    while(*Read==' ')Read++;
    if(strncmp(Read,Text,len)==0)
    {
        Stream=Read+len;
        return true;
    }
    else
    {
        return false;
    }
}

shared_ptr<Expression> GetNumber(char*& Stream)
{
    int Result=0;
    bool GotNumber=false;
    /*保存参数*/
    char* Read=Stream;
    /*过滤空格*/
    while(*Read==' ')Read++;
    while(true)
    {
        char c=*Read;
        /*检查字符是否为数字*/
        if('0'<=c && c<='9')
        {
            Result=Result*10+(c-'0');
            GotNumber=true;
            Read++;
        }
        else
        {
            break;
        }
    }
    if(GotNumber)
    {
        Stream=Read;

        return make_shared<NumberExpression>(&Result);
    }
    else
    {
        throw Exception(Stream,"此处需要表达式");
    }
}

shared_ptr<Expression> GetTerm(char*& Stream);
shared_ptr<Expression> GetFactor(char*& Stream);
shared_ptr<Expression> GetExp(char*& Stream);

shared_ptr<Expression> GetTerm(char*& Stream)
{
    try
    {
        return GetNumber(Stream);
    }
    catch (Exception& e)
    {
        char* Read=Stream;
        if(Is(Read,"("))
        {
            auto Result=GetExp(Read);
            if(Is(Read,")"))
            {
                Stream=Read;
                return Result;
            }
            else
            {
                throw Exception(Stream,"此处需要右括号");
            }
        }
        else
        {
            throw e;
        }
    }
}

shared_ptr<Expression> GetFactor(char*& Stream)
{
    char* Read=Stream;
    shared_ptr<Expression> Result=GetTerm(Stream);
    while(true)
    {
        BinaryOperator Operator;
        if(Is(Read,"*"))
        {
            Operator=BinaryOperator::Multiply ;
        }
        else if(Is(Read,"/"))
        {
            Operator=BinaryOperator::Divide ;
        }
        else
        {
            break;
        }

        Result=make_shared<BinaryExpression>(&Operator,&Result,GetTerm(Read));

    }
    Stream=Read;
    return Result;
}

shared_ptr<Expression> GetExp(char*& Stream)
{
    char* Read=Stream;
    shared_ptr<Expression> Result=GetFactor(Stream);
    while (true)
    {
        BinaryOperator Operator;
        if(Is(Read,"+"))
        {
            Operator=BinaryOperator::Plus;
        }
        else if(Is(Read,"-"))
        {
            Operator=BinaryOperator::Minus;
        }
        else
        {
            break;
        }
        Result=make_shared<BinaryExpression>(&Operator,&Result,GetFactor(Read));

    }
    Stream=Read;
    return Result;

}

int main(){
    char Buffer[1000];
    gets(Buffer);

    return 0;
}