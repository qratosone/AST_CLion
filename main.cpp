#include <iostream>
#include <cstdio>
#include <memory>
#include <cstring>
#include <string>

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

char GetOp(BinaryOperator B) {
    switch (B)
    {
        case BinaryOperator::Plus:
            return '+';
        case BinaryOperator::Minus:
            return '-';
        case BinaryOperator::Multiply:
            return '*';
        case BinaryOperator::Divide:
            return '/';
        default:
            break;
    }
}
class Expression
{
public:
    class IVisior
    {
    public:
        virtual void Visit(NumberExpression* node) = 0;
        virtual void Visit(BinaryExpression* node) = 0;
        virtual ~IVisior() = default;
    };
    virtual void Accept(IVisior* visitor) = 0;
    virtual ~Expression() = default;
};

class NumberExpression :public Expression
{
public:
    int Value;
    void Accept(IVisior* visitor)
    {
        visitor->Visit(this);
    }
    NumberExpression(int number)
    {
        Value = number;
    }

};

class BinaryExpression :public Expression
{
public:
    shared_ptr<Expression> First;
    shared_ptr<Expression> Second;
    BinaryOperator Op;
    BinaryExpression(BinaryOperator theOp, shared_ptr<Expression> theLeft, shared_ptr<Expression> theRight) :
            Op(theOp), First(theLeft), Second(theRight) {

    }
    void Accept(IVisior* visitor)
    {
        visitor->Visit(this);
    }
};

class ExpressionPointer :public Expression::IVisior
{
public:
    string Result;
    void Visit(NumberExpression* node)
    {
        Result += to_string(node->Value);
    }
    void Visit(BinaryExpression* node)
    {
        //Result += "(";
        //node->First->Accept(this);
        //Result += " ";
        //Result += GetOp(node->Op);
        //Result += " ";
        //node->Second->Accept(this);
        //  	Result += ")";

        Result += "(";
        Result += GetOp(node->Op);
        Result += " ";
        node->First->Accept(this);
        Result += " ";
        node->Second->Accept(this);
        Result += ")";

    }
};

struct Exception
{
    char* Start;
    char* Error;

    Exception(char* aStart, char* aError)
    {
        Start = aStart;
        Error = aError;
    }
};

bool Is(char*& Stream, const char* Text)
{
    size_t len = strlen(Text);
    char* Read = Stream;
    while (*Read == ' ')Read++;
    if (strncmp(Read, Text, len) == 0)
    {
        Stream = Read + len;
        return true;
    }
    else
    {
        return false;
    }
}

shared_ptr<Expression> GetNumber(char*& Stream)
{
    int Result = 0;
    bool GotNumber = false;
    /*保存参数*/
    char* Read = Stream;
    /*过滤空格*/
    while (*Read == ' ')Read++;
    while (true)
    {
        char c = *Read;
        /*检查字符是否为数字*/
        if ('0' <= c && c <= '9')
        {
            Result = Result * 10 + (c - '0');
            GotNumber = true;
            Read++;
        }
        else
        {
            break;
        }
    }
    if (GotNumber)
    {
        Stream = Read;

        return make_shared<NumberExpression>(Result);
    }
    else
    {
        throw Exception(Stream, "此处需要表达式");
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
        char* Read = Stream;
        if (Is(Read, "("))
        {
            auto Result = GetExp(Read);
            if (Is(Read, ")"))
            {
                Stream = Read;
                return Result;
            }
            else
            {
                throw Exception(Stream, "此处需要右括号");
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
    char* Read = Stream;
    shared_ptr<Expression> Result = GetTerm(Read);
    while (true)
    {
        BinaryOperator Operator;
        if (Is(Read, "*"))
        {
            Operator = BinaryOperator::Multiply;
        }
        else if (Is(Read, "/"))
        {
            Operator = BinaryOperator::Divide;
        }
        else
        {
            break;
        }

        Result = make_shared<BinaryExpression>(Operator, Result, GetTerm(Read));

    }
    Stream = Read;
    return Result;
}

shared_ptr<Expression> GetExp(char*& Stream)
{
    char* Read = Stream;
    shared_ptr<Expression> Result = GetFactor(Read);
    while (true)
    {
        BinaryOperator Operator;
        if (Is(Read, "+"))
        {
            Operator = BinaryOperator::Plus;
        }
        else if (Is(Read, "-"))
        {
            Operator = BinaryOperator::Minus;
        }
        else
        {
            break;
        }
        Result = make_shared<BinaryExpression>(Operator, Result, GetFactor(Read));

    }
    Stream = Read;
    return Result;

}

int main() {
    ExpressionPointer visitor;
    char* Exp = "1+2*3+4";
    try
    {
        auto p=GetExp(Exp);
        p->Accept(&visitor);
        cout << visitor.Result << endl;
    }
    catch (Exception& e)
    {
        cout << e.Error << " " << e.Start << endl;
    }

    return 0;
}