#include "transaction.h"

#include "transactionoriginators.h"

#include "gtest/gtest.h"

using namespace undoredo::transactions;
using namespace undoredo::transactions::test;

namespace {

void _() {}

}

TEST(TransactionTests,Trivial)
{
	Transaction t;

	ASSERT_FALSE(t.first);

	ASSERT_FALSE(t.second);

	//t=std::make_pair(std::bind(_),std::bind(_));
	t.first=_;
	t.second=_;

	ASSERT_FALSE(!t.first); //gnu trick

	ASSERT_FALSE(!t.second);  //gnu trick
}

TEST(TransactionTests,Bind)
{
	Transaction t;
	Target target;

	t=std::make_pair(std::bind(&Target::Increment,&target),std::bind(&Target::Increment,&target));

	ASSERT_FALSE(!t.first); //gnu trick
	ASSERT_FALSE(!t.second);  //gnu trick

	t.first();
	ASSERT_EQ(1,target.Get());

	t.second();
	ASSERT_EQ(2,target.Get());
}