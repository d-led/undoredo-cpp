#include "transactionstore.h"
#include "transaction.h"
#include "delayedtransaction.h"

#include "mementooriginators.h"

#include <list>
#include <stdexcept>
#include <memory>

#include "catch.hpp"

using namespace undoredo::transactions;
using namespace undoredo::memento::test;

TEST_CASE("MementoTransactionDocuTest,Main")
{

	TransactionStore<std::list<Transaction> > ts;
    std::shared_ptr<MyOriginator> MO(new MyOriginator); // Memento originator
    std::shared_ptr<DelayedTransaction<MyOriginator> > DT;
 
    DT.reset(new DelayedTransaction<MyOriginator>(MO.get()));
    DT->BeginTransaction();
    MO->Set("test1",1);
    ts.AddTransaction(DT->EndTransaction());
 
    DT.reset(new DelayedTransaction<MyOriginator>(MO.get()));
    DT->BeginTransaction();
    MO->Set("test2",2);
	MO->Set("test3",3); //this state will be saved
    ts.AddTransaction(DT->EndTransaction());
 
 
    ts.UndoLastTransaction();
	REQUIRE( MO->GetString() == "test1" );
    ts.RedoLastTransaction();
	REQUIRE( MO->GetString() == "test3" );

	REQUIRE_THROWS_AS(ts.RedoLastTransaction(),std::runtime_error);
}

TEST_CASE("MementoTransactionTest,Main")
{
	TransactionStore<std::list<Transaction> > ts;
    std::shared_ptr<MySecondOriginator> MSO(new MySecondOriginator);
    std::shared_ptr<DelayedTransaction<MySecondOriginator> > DT;
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(1);
    ts.AddTransaction(DT->EndTransaction());
	REQUIRE( MSO->Get() == 1 );
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(2);
    ts.AddTransaction(DT->EndTransaction());
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(3);
    MSO->Set(4);
    ts.AddTransaction(DT->EndTransaction());
	REQUIRE( MSO->Get() == 4 );
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(5);
    ts.AddTransaction(DT->EndTransaction());
	REQUIRE( MSO->Get() == 5 );
 
    ts.UndoLastTransaction();
	REQUIRE( MSO->Get() == 4 );
    ts.UndoLastTransaction();
	REQUIRE( MSO->Get() == 2 );
    ts.UndoLastTransaction();
	REQUIRE( MSO->Get() == 1 );
    ts.RedoLastTransaction();
	REQUIRE( MSO->Get() == 2 );
    ts.RedoLastTransaction();
	REQUIRE( MSO->Get() == 4 );
    ts.RedoLastTransaction();
	REQUIRE( MSO->Get() == 5 );

	REQUIRE_THROWS_AS(ts.RedoLastTransaction(),std::runtime_error);
}