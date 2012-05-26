#include "transactionstore.h"
#include "transaction.h"

#include "transactionoriginators.h"

#include <list>
#include <stdexcept>
#include <memory>

#include "gtest/gtest.h"

using namespace undoredo::transactions;
using namespace undoredo::transactions::test;

class TransactionStoreTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
	  o1.reset(new MyThirdOriginator("o1"));
	  o2.reset(new MyThirdOriginator("o2"));
  }
  TransactionStore<std::list<Transaction> > ts;
  Target target;
  std::shared_ptr<MyThirdOriginator> o1,o2;
};

TEST_F(TransactionStoreTests,Initial)
{
	ASSERT_NO_THROW(ts.Purge());

	ASSERT_THROW(ts.RedoLastTransaction(),std::runtime_error);

	ASSERT_THROW(ts.UndoLastTransaction(),std::runtime_error);
}

TEST_F(TransactionStoreTests,Purge)
{
	ASSERT_EQ(0,target.Get());
	target.Increment();
	ASSERT_EQ(1,target.Get());
	ts.AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	ASSERT_EQ(1,target.Get());

	ASSERT_NO_THROW(ts.UndoLastTransaction());
	ASSERT_NO_THROW(ts.RedoLastTransaction());
	ASSERT_EQ(1,target.Get());

	ts.Purge();
	ASSERT_EQ(1,target.Get());

	ASSERT_THROW(ts.UndoLastTransaction(),std::runtime_error);
}

TEST_F(TransactionStoreTests,Interleaving)
{
	ASSERT_EQ(0,target.Get());
	target.Increment();
	ASSERT_EQ(1,target.Get());
	ts.AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	ASSERT_EQ(1,target.Get());

	ASSERT_EQ(0,o1->Get().first);
	ASSERT_EQ("o1",o1->Get().second);
    ts.AddTransaction(o1->UndoableSet(1,"o1")); //2
	ASSERT_EQ(1,o1->Get().first);
	ASSERT_EQ("o1",o1->Get().second);

    ts.AddTransaction(o1->UndoableSet(2,"o1")); //3
	ASSERT_EQ(2,o1->Get().first);
	ASSERT_EQ("o1",o1->Get().second);

    ts.AddTransaction(o1->UndoableSet(3,"o1->1")); //4
	ASSERT_EQ(3,o1->Get().first);
	ASSERT_EQ("o1->1",o1->Get().second);

    ts.AddTransaction(o1->UndoableSet(4,"o1->2")); //5

	target.Increment();
	ts.AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //6
	ASSERT_EQ(2,target.Get());

    ts.AddTransaction(o2->UndoableSet(4,"o2")); //7
	ASSERT_EQ(4,o2->Get().first);
	ASSERT_EQ("o2",o2->Get().second);
	ASSERT_EQ(4,o1->Get().first);
	ASSERT_EQ("o1->2",o1->Get().second);

    ts.AddTransaction(o2->UndoableSet(5,"o2")); //8
 
    ts.UndoLastTransaction(); //7
	ASSERT_EQ(4,o2->Get().first);
	ASSERT_EQ("o2",o2->Get().second);
	ASSERT_EQ(4,o1->Get().first);
	ASSERT_EQ("o1->2",o1->Get().second);

    ts.UndoLastTransaction(); //6
	ASSERT_EQ(2,target.Get());

    ts.UndoLastTransaction(); //5
	ASSERT_EQ(1,target.Get());
    ts.RedoLastTransaction(); //6
	ASSERT_EQ(2,target.Get());

    ts.RedoLastTransaction(); //7
	ASSERT_EQ(4,o2->Get().first);
	ASSERT_EQ("o2",o2->Get().second);
	ASSERT_EQ(4,o1->Get().first);
	ASSERT_EQ("o1->2",o1->Get().second);

    ts.RedoLastTransaction(); //8

	for (int i=0; i<8; i++)
	{
		ASSERT_NO_THROW(ts.UndoLastTransaction());
	}
	ASSERT_THROW(ts.UndoLastTransaction(),std::runtime_error);

	ASSERT_EQ(0,target.Get());

	for (int i=0; i<8; i++)
	{
		ASSERT_NO_THROW(ts.RedoLastTransaction());
	}
	ASSERT_THROW(ts.RedoLastTransaction(),std::runtime_error);

	ASSERT_EQ(2,target.Get());
}

TEST_F(TransactionStoreTests,CompositeTransaction)
{
    std::shared_ptr<CompositeTransaction> composite(new CompositeTransaction);
   
	target.Increment();
	composite->AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	target.Increment();
	composite->AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	ASSERT_EQ(2,target.Get());

    ts.AddTransaction(composite->Get());

	ASSERT_NO_THROW(ts.UndoLastTransaction());

	ASSERT_EQ(0,target.Get());
	ASSERT_THROW(ts.UndoLastTransaction(),std::runtime_error);
	ASSERT_EQ(0,target.Get());

	ASSERT_NO_THROW(ts.RedoLastTransaction());
	ASSERT_EQ(2,target.Get());
	ASSERT_THROW(ts.RedoLastTransaction(),std::runtime_error);
}

TEST_F(TransactionStoreTests,UndoThenRedo)
{
	ts.AddTransaction(o1->UndoableSet(1,"o1"));
	ts.AddTransaction(o1->UndoableSet(2,"o1"));
	ts.AddTransaction(o1->UndoableSet(3,"o1"));
	ASSERT_EQ(3,o1->Get().first);

	ts.UndoLastTransaction();
	ts.UndoLastTransaction();

	ASSERT_EQ(1,o1->Get().first);
	ts.AddTransaction(o1->UndoableSet(4,"o1"));

	ASSERT_THROW(ts.RedoLastTransaction(),std::runtime_error);
	ASSERT_EQ(4,o1->Get().first);

	ASSERT_NO_THROW(ts.UndoLastTransaction());
	ASSERT_EQ(1,o1->Get().first);
}

class LifetimeTester : public MyThirdOriginator {
public:
	static int Count;
public:
	LifetimeTester(std::string n):MyThirdOriginator(n) { Count++; }
	~LifetimeTester() {
		--Count;
	}
};
int LifetimeTester::Count=0;

TEST_F(TransactionStoreTests,Lifetime)
{
	{
        std::shared_ptr<LifetimeTester> O3(new LifetimeTester("O3"));
        ts.AddTransaction(O3->UndoableSet(1,"O3.1"));
        ts.AddTransaction(O3->UndoableSet(2,"O3.2"));
    }

	ASSERT_NO_THROW(ts.UndoLastTransaction());
	ASSERT_NO_THROW(ts.RedoLastTransaction());
	ASSERT_EQ(1,LifetimeTester::Count);

	ASSERT_NO_THROW(ts.Purge());

	ASSERT_EQ(0,LifetimeTester::Count);
}
