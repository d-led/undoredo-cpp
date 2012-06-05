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
	O1.Set("StateA",0);
    O1.Set("StateB",0);

    ASSERT_NO_THROW(savedStates->Save(&O1));

	ASSERT_THROW(savedStates->Undo(&O2),std::runtime_error);

	ASSERT_NO_THROW(savedStates->Undo(&O1));

	ASSERT_EQ("StateB",O1.GetString());

	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);
}

TEST_F(StlMementoStoreTests,MultiLevelUndo)
{
	O1.Set("StateA",0);
	savedStates->Save(&O1);
    O1.Set("StateB",0);
	savedStates->Save(&O1);
	O1.Set("StateC",0);
	savedStates->Save(&O1);

	ASSERT_NO_THROW(savedStates->Undo(&O1));
	ASSERT_EQ("StateC",O1.GetString());
	ASSERT_NO_THROW(savedStates->Undo(&O1));
	ASSERT_EQ("StateB",O1.GetString());
	ASSERT_NO_THROW(savedStates->Undo(&O1));
	ASSERT_EQ("StateA",O1.GetString());

	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);
}

TEST_F(StlMementoStoreTests,MultiLevelGlobalUndo)
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

	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_EQ("StateC",O1.GetString());
	ASSERT_EQ("StateC2",O2.GetString());
	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_EQ("StateB",O1.GetString());
	ASSERT_EQ("StateB2",O2.GetString());
	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_EQ("StateA",O1.GetString());
	ASSERT_EQ("StateA2",O2.GetString());

	ASSERT_NO_THROW(savedStates->Undo());
	ASSERT_THROW(savedStates->Undo(&O1),std::runtime_error);
	ASSERT_THROW(savedStates->Undo(&O2),std::runtime_error);
}