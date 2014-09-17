#include "stlmementostore.h"
#include "mementooriginators.h"

#include <memory>
#include <map>
#include <list>

#include "catch.hpp"

namespace {

}

using namespace undoredo::memento;
using namespace undoredo::memento::test;

class StlMementoStoreTests {
 public:
  StlMementoStoreTests() {
	  teststring="test";
	  savedStates.reset(new StlMementoStore<MyOriginator,std::map<MyOriginator*,std::list<typename MyOriginator::MementoType> >>);
  }

  MyOriginator O1,O2;
  std::string teststring;
  std::shared_ptr<MyOriginator::MementoStoreType> savedStates;
};

TEST_CASE("StlMementoDocuTest,Main")
{
	auto savedStates=StlMementoStore<MyOriginator,std::map<MyOriginator*,std::list<typename MyOriginator::MementoType> >>();
	MyOriginator O;
	O.Set("test",1);
	savedStates.Save(&O);
	O.Set("bla",2);
	REQUIRE( O.GetString() == "bla" );
	savedStates.Undo(&O);
	REQUIRE( O.GetString() == "test" );
}

TEST_CASE_METHOD(StlMementoStoreTests,"Construction")
{
	REQUIRE( savedStates != nullptr );
}

TEST_CASE_METHOD(StlMementoStoreTests,"StlMementoStoreTests,Initial")
{
	REQUIRE_THROWS_AS(savedStates->Undo(&O1),std::runtime_error);

	REQUIRE_NOTHROW(savedStates->Undo());
}

TEST_CASE_METHOD(StlMementoStoreTests,"SaveRestorePerObject")
{
	O1.Set("StateA",0);
    O1.Set("StateB",0);

    REQUIRE_NOTHROW(savedStates->Save(&O1));

	REQUIRE_THROWS_AS(savedStates->Undo(&O2),std::runtime_error);

	REQUIRE_NOTHROW(savedStates->Undo(&O1));

	REQUIRE( O1.GetString() == "StateB" );

	REQUIRE_THROWS_AS(savedStates->Undo(&O1),std::runtime_error);
}

TEST_CASE_METHOD(StlMementoStoreTests,"MultiLevelUndo")
{
	O1.Set("StateA",0);
	savedStates->Save(&O1);
    O1.Set("StateB",0);
	savedStates->Save(&O1);
	O1.Set("StateC",0);
	savedStates->Save(&O1);

	REQUIRE_NOTHROW(savedStates->Undo(&O1));
	REQUIRE( O1.GetString() == "StateC" );
	REQUIRE_NOTHROW(savedStates->Undo(&O1));
	REQUIRE( O1.GetString() == "StateB" );
	REQUIRE_NOTHROW(savedStates->Undo(&O1));
	REQUIRE( O1.GetString() == "StateA" );

	REQUIRE_THROWS_AS(savedStates->Undo(&O1),std::runtime_error);
}

TEST_CASE_METHOD(StlMementoStoreTests,"MultiLevelGlobalUndo")
{
	O1.Set("StateA",0);
	savedStates->Save(&O1);
    O1.Set("StateB",0);
	savedStates->Save(&O1);
	O1.Set("StateC",0);
	savedStates->Save(&O1);

	O2.Set("StateA2",0);
	savedStates->Save(&O2);
    O2.Set("StateB2",0);
	savedStates->Save(&O2);
	O2.Set("StateC2",0);
	savedStates->Save(&O2);

	REQUIRE_NOTHROW(savedStates->Undo());
	REQUIRE( O1.GetString() == "StateC" );
	REQUIRE( O2.GetString() == "StateC2" );
	REQUIRE_NOTHROW(savedStates->Undo());
	REQUIRE( O1.GetString() == "StateB" );
	REQUIRE( O2.GetString() == "StateB2" );
	REQUIRE_NOTHROW(savedStates->Undo());
	REQUIRE( O1.GetString() == "StateA" );
	REQUIRE( O2.GetString() == "StateA2" );

	REQUIRE_NOTHROW(savedStates->Undo());
	REQUIRE_THROWS_AS(savedStates->Undo(&O1),std::runtime_error);
	REQUIRE_THROWS_AS(savedStates->Undo(&O2),std::runtime_error);
}