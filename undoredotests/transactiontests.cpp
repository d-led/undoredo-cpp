#include "transaction.h"

#include "transactionoriginators.h"

#include "catch.hpp"

using namespace undoredo::transactions;
using namespace undoredo::transactions::test;

namespace {

void _() {}

}

TEST_CASE("TransactionTests,Trivial")
{
	Transaction t;

	REQUIRE_FALSE(t.first);

	REQUIRE_FALSE(t.second);

	t.first=_;
	t.second=_;

	REQUIRE_FALSE(!t.first); //gnu trick

	REQUIRE_FALSE(!t.second);  //gnu trick
}

TEST_CASE("TransactionTests,Bind")
{
	Transaction t;
	Target target;

	t=std::make_pair(std::bind(&Target::Increment,&target),std::bind(&Target::Increment,&target));

	REQUIRE_FALSE(!t.first); //gnu trick
	REQUIRE_FALSE(!t.second);  //gnu trick

	t.first();
	REQUIRE( target.Get() == 1 );

	t.second();
	REQUIRE( target.Get() == 2 );
}