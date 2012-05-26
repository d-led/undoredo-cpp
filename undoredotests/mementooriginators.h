#pragma once

#include <memory>

#include <string>

#include "memento.h"
#include "mementostore.h"

namespace undoredo {
namespace memento {
namespace test {

/// example state-undoable class
class MyOriginator
{
private:
	class State
	{
	public:
		void Set(const std::string& state)
		{
			s = state;
		}

		std::string Get() const {
			return s;
		}
	private:
		std::string s;
	};
    State state_;
 
public:
 
    void Set(const std::string& state)
    {
        state_.Set(state);
    }

	std::string Get() const {
		return state_.Get();
	}
 
//--- class-specific memento
 
public:
    typedef std::shared_ptr<Memento<State> > MementoType;
    typedef MementoStore<MyOriginator> MementoStoreType;
 
    MementoType SaveState()
    {
        return MementoType(new Memento<State>(state_));
    }
 
    void RestoreState(MementoType memento)
    {
        state_ = memento->GetSavedState();
    }
};
 
/// the other example class
class MySecondOriginator
{
private:
    int s;
 
public:
 
    void Set(int state)
    {
        s = state;
    }
 
    MySecondOriginator():s(0){}
 
//--- class-specific memento
 
public:
    typedef std::shared_ptr<Memento<int> > MementoType;
    typedef MementoStore<MySecondOriginator> MementoStoreType;
 
    MementoType SaveState()
    {
        return MementoType(new Memento<int>(s));
    }
 
    void RestoreState(MementoType memento)
    {
        s = memento->GetSavedState();
    }
};

} //test
} //memento
} //undoredo
