#pragma once

namespace undoredo {
namespace memento {

/// Memento for the encapsulation of the state and its handling
template <class T>
class Memento
{
 
private:
 
    T state_;
 
public:
 
    Memento(const T& stateToSave) : state_(stateToSave) { }
    const T& GetSavedState() const
    {
        return state_;
    }
};

} //memento
} //undoredo