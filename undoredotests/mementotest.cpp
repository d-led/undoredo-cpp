#include "mementooriginators.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace undoredo::memento;
using namespace undoredo::memento::test;

class MyOriginatorMementoFixture {
 public:
  MyOriginatorMementoFixture() {
		teststring="test";
		teststring2=" test ";
  }

  MyOriginator O;
  std::string teststring;
  std::string teststring2;
};

TEST_CASE_METHOD(MyOriginatorMementoFixture,"SaveState") {
 MyOriginator::MementoType Memento=O.SaveState();
	
 REQUIRE( Memento != nullptr );
}

TEST_CASE_METHOD(MyOriginatorMementoFixture," GetSavedState") {
	MyOriginator::MementoType Memento=O.SaveState();

	REQUIRE( Memento->GetSavedState().GetString() == "");

	O.Set(teststring,0);

	Memento=O.SaveState();

	REQUIRE( Memento->GetSavedState().GetString() == teststring );
}

TEST_CASE_METHOD(MyOriginatorMementoFixture," RestoreState") {
	O.Set(teststring,0);

	MyOriginator::MementoType Memento=O.SaveState();

	O.Set(teststring2,0);
	MyOriginator::MementoType Memento2=O.SaveState();

	REQUIRE( Memento2->GetSavedState().GetString() == teststring2 );

	O.RestoreState(Memento2);

	REQUIRE( O.GetString() == teststring2 );
}
