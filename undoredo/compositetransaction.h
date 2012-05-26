#pragma once

#include <functional>
#include <memory>
#include <list>

#include "transaction.h"

namespace undoredo {
namespace transactions {

class CompositeTransaction : public std::enable_shared_from_this<CompositeTransaction>
{
private:
    std::list<Transaction> Undo_;
    std::list<Transaction> Redo_;
 
public:
 
    void AddTransaction(Transaction t)
    {
        if (t.first && t.second)
        {
            Undo_.push_back(t);
            Redo_.clear();
        }
    }
 
    void UndoAll()
    {
        while (Undo_.size())
        {
            Undo_.back().first();
            Redo_.push_back(Undo_.back());
            Undo_.pop_back();
        }
    }
 
    void RedoAll()
    {
        while (Redo_.size())
        {
            Redo_.back().second();
            Undo_.push_back(Redo_.back());
            Redo_.pop_back();
        }
    }
 
/// a composite transaction, instance must be in shared_ptr
    Transaction Get()
    {
        return std::make_pair(std::bind(&CompositeTransaction::UndoAll,this->shared_from_this()),
                              std::bind(&CompositeTransaction::RedoAll,this->shared_from_this()));
    }
};


} //transactions
} //undoredo
