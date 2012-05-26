#pragma once

#include <functional>

namespace undoredo {
namespace transactions {

typedef std::function<void ()> Action;
typedef std::pair<Action/*Undo*/,Action/*Redo*/> Transaction;

} //transactions
} //undoredo
