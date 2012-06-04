#pragma once

#include "transaction.h"

#include <stdexcept>

namespace undoredo {
namespace transactions {

/// Storage of transactions
template <class TStore>
class TransactionStore
{
private:
    TStore Undo_;
    TStore Redo_;
 
public:
 
    void AddTransaction(Transaction t)
    {
        if (t.first && t.second)
        {
            Undo_.push_back(t);
            Redo_.clear();
        }
    }
 
    void UndoLastTransaction()
    {
        if (Undo_.size()<1) throw std::runtime_error("No more undo transactions");
        Undo_.back().first();
        Redo_.push_back(Undo_.back());
        Undo_.pop_back();
    }
 
    void RedoLastTransaction()
    {
        if (Redo_.size()<1) throw std::runtime_error("No more redo transactions");
        Redo_.back().second();
        Undo_.push_back(Redo_.back());
        Redo_.pop_back();
    }
 
    void Purge()
    {
        Undo_.clear();
        Redo_.clear();
    }
    
    bool HasUndo()
    {
        return Undo_.size()>0;
    }

    bool HasRedo()
    {
        return Redo_.size()>0;
    }
};

} //transactions
} //undoredo
