#include "transactionstore.h"
#include "transaction.h"

#include "transactionoriginators.h"

#include <list>
#include <stdexcept>
#include <memory>

#include "catch.hpp"

using namespace undoredo::transactions;
using namespace undoredo::transactions::test;

TEST_CASE("DocuTransactionStoreTest,Main")
{
	TransactionStore<std::list<Transaction> > ts;

	std::shared_ptr<SimpleTransactionStateExample> E(new SimpleTransactionStateExample);
	E->Set(0);

	ts.AddTransaction(E->SetTransaction(1));
	REQUIRE( E->Get() == 1 );

	ts.UndoLastTransaction();
	REQUIRE( E->Get() == 0 );

	ts.RedoLastTransaction();
	REQUIRE( E->Get() == 1 );
}

class TransactionStoreTests {
 public:
  TransactionStoreTests() {
	  o1.reset(new MyThirdOriginator("o1"));
	  o2.reset(new MyThirdOriginator("o2"));
  }
  TransactionStore<std::list<Transaction> > ts;
  Target target;
  std::shared_ptr<MyThirdOriginator> o1,o2;
};

TEST_CASE_METHOD(TransactionStoreTests,"TransactionStoreTests,Initial")
{
	REQUIRE_NOTHROW(ts.Purge());

	REQUIRE_THROWS_AS(ts.RedoLastTransaction(),std::runtime_error);

	REQUIRE_THROWS_AS(ts.UndoLastTransaction(),std::runtime_error);
}

TEST_CASE_METHOD(TransactionStoreTests,"Purge")
{
	REQUIRE( target.Get() == 0 );
	target.Increment();
	REQUIRE( target.Get() == 1 );
	ts.AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	REQUIRE( target.Get() == 1 );

	REQUIRE_NOTHROW(ts.UndoLastTransaction());
	REQUIRE_NOTHROW(ts.RedoLastTransaction());
	REQUIRE( target.Get() == 1 );

	ts.Purge();
	REQUIRE( target.Get() == 1 );

	REQUIRE_THROWS_AS(ts.UndoLastTransaction(),std::runtime_error);
}

TEST_CASE_METHOD(TransactionStoreTests,"Interleaving")
{
	REQUIRE( target.Get() == 0 );
	target.Increment();
	REQUIRE( target.Get() == 1 );
	ts.AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	REQUIRE( target.Get() == 1 );

	REQUIRE( o1->Get().first == 0 );
	REQUIRE( o1->Get().second == "o1" );
    ts.AddTransaction(o1->UndoableSet(1,"o1")); //2
	REQUIRE( o1->Get().first == 1 );
	REQUIRE( o1->Get().second == "o1" );

    ts.AddTransaction(o1->UndoableSet(2,"o1")); //3
	REQUIRE( o1->Get().first == 2 );
	REQUIRE( o1->Get().second == "o1" );

    ts.AddTransaction(o1->UndoableSet(3,"o1->1")); //4
	REQUIRE( o1->Get().first == 3 );
	REQUIRE( o1->Get().second == "o1->1" );

    ts.AddTransaction(o1->UndoableSet(4,"o1->2")); //5

	target.Increment();
	ts.AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //6
	REQUIRE( target.Get() == 2 );

    ts.AddTransaction(o2->UndoableSet(4,"o2")); //7
	REQUIRE( o2->Get().first == 4 );
	REQUIRE( o2->Get().second == "o2" );
	REQUIRE( o1->Get().first == 4 );
	REQUIRE( o1->Get().second == "o1->2" );

    ts.AddTransaction(o2->UndoableSet(5,"o2")); //8
 
    ts.UndoLastTransaction(); //7
	REQUIRE( o2->Get().first == 4 );
	REQUIRE( o2->Get().second == "o2" );
	REQUIRE( o1->Get().first == 4 );
	REQUIRE( o1->Get().second == "o1->2" );

    ts.UndoLastTransaction(); //6
	REQUIRE( target.Get() == 2 );

    ts.UndoLastTransaction(); //5
	REQUIRE( target.Get() == 1 );
    ts.RedoLastTransaction(); //6
	REQUIRE( target.Get() == 2 );

    ts.RedoLastTransaction(); //7
	REQUIRE( o2->Get().first == 4 );
	REQUIRE( o2->Get().second == "o2" );
	REQUIRE( o1->Get().first == 4 );
	REQUIRE( o1->Get().second == "o1->2" );

    ts.RedoLastTransaction(); //8

	for (int i=0; i<8; i++)
	{
		REQUIRE_NOTHROW(ts.UndoLastTransaction());
	}
	REQUIRE_THROWS_AS(ts.UndoLastTransaction(),std::runtime_error);

	REQUIRE( target.Get() == 0 );

	for (int i=0; i<8; i++)
	{
		REQUIRE_NOTHROW(ts.RedoLastTransaction());
	}
	REQUIRE_THROWS_AS(ts.RedoLastTransaction(),std::runtime_error);

	REQUIRE( target.Get() == 2 );
}

TEST_CASE_METHOD(TransactionStoreTests,"CompositeTransaction")
{
    std::shared_ptr<CompositeTransaction> composite(new CompositeTransaction);
   
	target.Increment();
	composite->AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	target.Increment();
	composite->AddTransaction(std::make_pair(std::bind(&Target::Decrement,&target),std::bind(&Target::Increment,&target))); //1
	REQUIRE( target.Get() == 2 );

    ts.AddTransaction(composite->Get());

	REQUIRE_NOTHROW(ts.UndoLastTransaction());

	REQUIRE( target.Get() == 0 );
	REQUIRE_THROWS_AS(ts.UndoLastTransaction(),std::runtime_error);
	REQUIRE( target.Get() == 0 );

	REQUIRE_NOTHROW(ts.RedoLastTransaction());
	REQUIRE( target.Get() == 2 );
	REQUIRE_THROWS_AS(ts.RedoLastTransaction(),std::runtime_error);
}

TEST_CASE_METHOD(TransactionStoreTests,"UndoThenRedo")
{
	ts.AddTransaction(o1->UndoableSet(1,"o1"));
	ts.AddTransaction(o1->UndoableSet(2,"o1"));
	ts.AddTransaction(o1->UndoableSet(3,"o1"));
	REQUIRE( o1->Get().first == 3 );

	ts.UndoLastTransaction();
	ts.UndoLastTransaction();

	REQUIRE( o1->Get().first == 1 );
	ts.AddTransaction(o1->UndoableSet(4,"o1"));

	REQUIRE_THROWS_AS(ts.RedoLastTransaction(),std::runtime_error);
	REQUIRE( o1->Get().first == 4 );

	REQUIRE_NOTHROW(ts.UndoLastTransaction());
	REQUIRE( o1->Get().first == 1 );
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

TEST_CASE_METHOD(TransactionStoreTests,"Lifetime")
{
	{
        std::shared_ptr<LifetimeTester> O3(new LifetimeTester("O3"));
        ts.AddTransaction(O3->UndoableSet(1,"O3.1"));
        ts.AddTransaction(O3->UndoableSet(2,"O3.2"));
    }

	REQUIRE_NOTHROW(ts.UndoLastTransaction());
	REQUIRE_NOTHROW(ts.RedoLastTransaction());
	REQUIRE( LifetimeTester::Count == 1 );

	REQUIRE_NOTHROW(ts.Purge());

	REQUIRE( LifetimeTester::Count == 0 );
}
