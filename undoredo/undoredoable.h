#pragma once

namespace undoredo {
namespace undoable {

struct Undoable
{
	virtual void Undo()=0;
 	virtual bool HasUndo()=0;
	virtual void ClearUndo()=0;
	virtual ~Undoable() {}
};

struct Redoable
{
	virtual void Redo()=0;
	virtual bool HasRedo()=0;
	virtual void ClearRedo()=0;
	virtual ~Redoable() {}
};

} //undoable
} //undoredo
