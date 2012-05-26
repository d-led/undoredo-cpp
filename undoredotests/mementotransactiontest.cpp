#include "transactionstore.h"
#include "transaction.h"
#include "delayedtransaction.h"

#include "mementooriginators.h"

#include <list>
#include <stdexcept>
#include <memory>

#include "gtest/gtest.h"

using namespace undoredo::transactions;
using namespace undoredo::memento::test;

TEST(MementoTransactionTest,Main)
{
	TransactionStore<std::list<Transaction> > ts;
    std::shared_ptr<MySecondOriginator> MSO(new MySecondOriginator);
    std::shared_ptr<DelayedTransaction<MySecondOriginator> > DT;
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(1);
    ts.AddTransaction(DT->EndTransaction());
	ASSERT_EQ(1,MSO->Get());
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(2);
    ts.AddTransaction(DT->EndTransaction());
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(3);
    MSO->Set(4);
    ts.AddTransaction(DT->EndTransaction());
	ASSERT_EQ(4,MSO->Get());
 
    DT.reset(new DelayedTransaction<MySecondOriginator>(MSO.get()));
    DT->BeginTransaction();
    MSO->Set(5);
    ts.AddTransaction(DT->EndTransaction());
	ASSERT_EQ(5,MSO->Get());
 
    ts.UndoLastTransaction();
	ASSERT_EQ(4,MSO->Get());
    ts.UndoLastTransaction();
	ASSERT_EQ(2,MSO->Get());
    ts.UndoLastTransaction();
	ASSERT_EQ(1,MSO->Get());
    ts.RedoLastTransaction();
	ASSERT_EQ(2,MSO->Get());
    ts.RedoLastTransaction();
	ASSERT_EQ(4,MSO->Get());
    ts.RedoLastTransaction();
	ASSERT_EQ(5,MSO->Get());

	ASSERT_THROW(ts.RedoLastTransaction(),std::runtime_error);
}