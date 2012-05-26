#pragma once

namespace undoredo {
namespace undoable {

/// A container of undoables that undoes all
class UndoableAggregate : public Undoable
{
    typedef std::list<std::shared_ptr<Undoable> > Container;
private:
    Container list_;
 
public:
    virtual void Undo()
    {
        for (Container::iterator it=list_.begin(); it!=list_.end(); ++it)
        {
            (*it)->Undo();
        }
    }
 
public:
    void AddUndoable(std::shared_ptr<Undoable> instance)
    {
        list_.push_back(instance);
    }
};

} //undoable
} //undoredo
