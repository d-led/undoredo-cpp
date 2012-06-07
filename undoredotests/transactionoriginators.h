#pragma once

#include <memory>
#include <string>

#include "transaction.h"
#include "compositetransaction.h"

namespace undoredo {
namespace transactions {
namespace test {

class SimpleTransactionStateExample : public std::enable_shared_from_this<SimpleTransactionStateExample>
{
private:
    int state;
public:
    void Set(int s)
    {
        state=s;
    }
	int Get() const
	{
		return state;
	}
    Transaction SetTransaction(int s)
    {
        Transaction Res=std::make_pair(
								std::bind(&SimpleTransactionStateExample::Set,shared_from_this(),state),
								std::bind(&SimpleTransactionStateExample::Set,shared_from_this(),s)
                             );
		Set(s);
		return Res;
    }
};

/// Transaction-undoable example class
class MyThirdOriginator : public std::enable_shared_from_this<MyThirdOriginator>
{
private:
    int state;
    std::string name;
 
public:
 
    void Set(int s)
    {
        state=s;
    }
 
    void SetName(std::string n)
    {
        name=n;
    }

	std::pair<int,std::string> Get()
	{
		return std::make_pair(state,name);
	}
 
//---- class-specific transaction
 
    Transaction UndoableSet(int s,std::string n)
    {
        std::shared_ptr<CompositeTransaction> res(new CompositeTransaction);
        if (n!=name)
        {
            res->AddTransaction(std::make_pair(
                                    std::bind(&MyThirdOriginator::SetName,shared_from_this(),name),
                                    std::bind(&MyThirdOriginator::SetName,shared_from_this(),n)
                                ));
            SetName(n);
        }
        if (s!=state)
        {
            res->AddTransaction(std::make_pair(
                                    std::bind(&MyThirdOriginator::Set,shared_from_this(),state),
                                    std::bind(&MyThirdOriginator::Set,shared_from_this(),s)
                                ));
            Set(s);
        }
        return res->Get();
    }
 
public:
 
    MyThirdOriginator(std::string n):state(0),name(n) {}
 
    virtual ~MyThirdOriginator()
    {
    }
};

class Target {
	int state;
public:
	Target():state(0){}
	void Increment(){++state;}
	void Decrement(){--state;}
	int Get() const { return state; }
};

} //test
} //transactions
} //undoredo