#pragma once

namespace undoredo {
namespace memento {

/// A convenience class for storage of mementos
template <class T, class TMemento = typename T::MementoType>
struct MementoStore
{
    virtual void Save(T* t)=0;
    virtual void Undo(T* t)=0;
    virtual void Undo()=0;
    virtual ~MementoStore() {}
};

} //memento
} //undoredo