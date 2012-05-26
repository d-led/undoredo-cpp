#pragma once

#include <memory>
#include <functional>

namespace undoredo {
namespace transactions {

/// the default implementation of the store
template<class T, class TMemento = typename T::MementoType>
class DelayedTransaction : public std::enable_shared_from_this<DelayedTransaction<T,typename T::MementoType> >
{
private:
    std::shared_ptr<TMemento> Undo_;
    std::shared_ptr<TMemento> Redo_;
    T* Object_;
 
public:
 
    DelayedTransaction(T* t)
    {
        Object_=t;
    }
 
    void BeginTransaction()
    {
        Undo_=Object_->SaveState();
    }
 
    Transaction EndTransaction()
    {
        Redo_=Object_->SaveState();
        return Get();
    }
 
    Transaction Get()
    {
        return std::make_pair(std::bind(&DelayedTransaction<T>::Undo,this->shared_from_this()),
                              std::bind(&DelayedTransaction<T>::Redo,this->shared_from_this()));
    }
 
private:
 
    virtual void Undo()
    {
        Object_->RestoreState(Undo_);
    }
    
    virtual void Redo()
    {
        Object_->RestoreState(Redo_);
    }
};


} //transactions
} //undoredo
