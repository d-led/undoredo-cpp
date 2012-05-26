#include "stlmementostore.h"
#include "mementooriginators.h"

#include <memory>
#include <map>
#include <list>

#include "gtest/gtest.h"

namespace {

}

using namespace undoredo::memento;
using namespace undoredo::memento::test;

class StlMementoStoreTests : public ::testing::Test {
 protected:
  virtual void SetUp() {
	  teststring="test";
	  savedStates.reset(new StlMementoStore<MyOriginator,std::map<MyOriginator*,std::list<typename MyOriginator::MementoType> >>);
  }

  MyOriginator O1,O2;
  std::string teststring;
  std::shared_ptr<MyOriginator::MementoStoreType> savedStates;
};

TEST_F(StlMementoStoreTests,Construction)
{
	ASSERT_NE(nullptr,savedStates);
}

TEST_F(StlMementoStoreTests,Initial)
{
	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);

	ASSERT_NO_THROW(savedStates->Undo());
}

TEST_F(StlMementoStoreTests,SaveRestorePerObject)
{
	O1.Set("StateA");
    O1.Set("StateB");

    ASSERT_NO_THROW(savedStates->Save(&O1));

	ASSERT_THROW(savedStates->Undo(&O2),std::runtime_error);

	ASSERT_NO_THROW(savedStates->Undo(&O1));

	ASSERT_EQ("StateB",O1.Get());

	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);
}

TEST_F(StlMementoStoreTests,MultiLevelUndo)
{
	O1.Set("StateA");
	savedStates->Save(&O1);
    O1.Set("StateB");
	savedStates->Save(&O1);
	O1.Set("StateC");
	savedStates->Save(&O1);

	ASSERT_NO_THROW(savedStates->Undo(&O1));
	ASSERT_EQ("StateC",O1.Get());
	ASSERT_NO_THROW(savedStates->Undo(&O1));
	ASSERT_EQ("StateB",O1.Get());
	ASSERT_NO_THROW(savedStates->Undo(&O1));
	ASSERT_EQ("StateA",O1.Get());

	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);
}

TEST_F(StlMementoStoreTests,MultiLevelGlobalUndo)
{
	O1.Set("StateA");
	savedStates->Save(&O1);
    O1.Set("StateB");
	savedStates->Save(&O1);
	O1.Set("StateC");
	savedStates->Save(&O1);

	O2.Set("StateA2");
	savedStates->Save(&O2);
    O2.Set("StateB2");
	savedStates->Save(&O2);
	O2.Set("StateC2");
	savedStates->Save(&O2);

	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_EQ("StateC",O1.Get());
	ASSERT_EQ("StateC2",O2.Get());
	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_EQ("StateB",O1.Get());
	ASSERT_EQ("StateB2",O2.Get());
	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_EQ("StateA",O1.Get());
	ASSERT_EQ("StateA2",O2.Get());

	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);
	ASSERT_THROW(savedStates->Undo(&O2),std::runtime_error);
}