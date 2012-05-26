#pragma once

#include <stdexcept>

#include "memento.h"
#include "mementostore.h"

namespace undoredo {
namespace memento {

/// the default implementation of the store
template <class T, class TMap, class TMemento = typename T::MementoType>
class StlMementoStore : public MementoStore < T, TMemento>
{
private:
	TMap Store;

public:

	virtual void Save(T* t)
	{
		PushState(t,t->SaveState());
	}

	virtual void Undo(T* t)
	{
		t->RestoreState(PopState(t));
	}

	/// tries to undo 1 state change per object for all objects
	virtual void Undo()
	{
		TryUndoAll();
	}

private:

	void PushState(T* t,TMemento m)
	{
		if (t)
		{
			Store[t].push_back(m);
		}
	}

	TMemento PopState(T* t)
	{
		if (!t || Store[t].size()<1) throw std::runtime_error("No more undo states");
		TMemento res=Store[t].back();
		Store[t].pop_back();
		return res;
	}

	void TryUndoAll()
	{
		for (typename TMap::iterator it=Store.begin(); it!=Store.end(); ++it)
		{
			try
			{
				it->first->RestoreState(PopState(it->first));
			}
			catch(std::exception& e)
			{
				/*trying, anyway*/ e;
			}
		}
	}
};

} //memento
} //undoredo