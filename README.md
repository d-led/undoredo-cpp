Summary
=======

[![Build Status](https://travis-ci.org/d-led/undoredo-cpp.png?branch=catchmoci)](https://travis-ci.org/d-led/undoredo-cpp) [![Coverity Status](https://scan.coverity.com/projects/3012/badge.svg)](https://scan.coverity.com/projects/3012)

This is a collection of header-only classes enabling some undo-redo/transaction functionality, started as a research on the ways to implement generic Undo/Redo mechanisms in C++.
The classes are combinable to form sensible undo/redo features and should be easy to use after a couple of exercises.

- [Used in the project](#used-in-the-project)
- [Compilers](#compilers)
- [Usage](#usage)
	- [Memento](#memento)
	- [MementoStore](#mementostore)
	- [Transaction and TransactionStore](#transaction-and-transactionstore)
		- [Object lifetime management](#object-lifetime-management)
	- [Memento and transactions](#memento-and-transactions)
- [Hosting](#hosting)
- [License](#license)
=======

This is a research on the ways to implement generic Undo/Redo mechanisms in C++.

Used in the project
-------------------

 * [catch](http://catch-lib.net) for tests
 * [Premake](http://industriousone.com/premake) for generating makefiles

Compilers
-------------------

known to work with
 * Visual Studio 2008, 2010, 2012 (Express)
 * MinGW >4.6.1
 * gcc 4.6.3, 4.7.2 (linux)
 * clang 3.0 (linux)

The Memento part can be used with older Microsoft compilers, however the tests use the c++11 namespaces and not tr1.


Usage
=====

### Memento
The classes provided in the project can be mixed arbitrarily. Let's start with the Memento itself. Considering a class MyOriginator:

```cpp
class MyOriginator
{
private:
    class State
    {
    public:
        void Set(const std::string& state,int i)
        {
            s = state;
            n = i;
        }

        std::string GetString() const
        {
            return s;
        }
    private:
        std::string s;
        int n;
    };
    
    State state_;
public:

    void Set(const std::string& state, int i)
    {
        state_.Set(state,i);
    }

    std::string GetString() const
    {
        return state_.GetString();
    }
};
```

we would like to be able to store its internal state, which is stored in a private variable of an inner class State. We could do it directly for each class we would
like to support saving its state. Considering that not each part of the internal state should known outside, the [Memento pattern](http://en.wikipedia.org/wiki/Memento_pattern) comes to mind. To document the pattern, it can be explicitly called Memento, implemented
as a C++ template class, taking the internal state as the template parameter. Only that class would be allowed to modify the state directly through the
dedicated interface added to the MyOriginator class:

```cpp
public:
	typedef std::shared_ptr<Memento<State> > MementoType;

MementoType SaveState()
{
	return MementoType(new Memento<State>(state_));
}

void RestoreState(MementoType memento)
{
	state_ = memento->GetSavedState();
}
```

### MementoStore

To be able to use the memento conveniently with Caretaker classes, the MementoType type is defined. Although the encapsulation in C++ is syntactical,
the explicit use of the Memento pattern should discourage using the "leaked" internal state. The <code>SaveState</code> and <code>RestoreState</code> methods
implement the pattern. Another convenience is to be able to use Caretaker classes matching the memento interface. Adding the following typedef we can use the
MementoStore:

```cpp
typedef MementoStore<MySecondOriginator> MementoStoreType;
```

Using the StlMementoStore class we can store and restore states of individual objects, or all objects at once. For the demonstration purposes the objects
are identified by their raw pointers, however other schemes can be envisioned. Using the Store looks like that:

```cpp
auto savedStates=StlMementoStore<
	MyOriginator,
	std::map<MyOriginator*,
			std::list<typename MyOriginator::MementoType>>>();
MyOriginator O;
O.Set("test",1);
savedStates.Save(&O); // saving the current state
O.Set("bla",2); // setting a new state
REQUIRE("bla",O.GetString());
savedStates.Undo(&O); // restoring to the saved state
REQUIRE("test",O.GetString());
```

### Transaction and TransactionStore

A typical undo/redo scenario does not include explicit choice of the object to be restored to its previous state. The change of the state of many
different objects should be taken into account and assembled in an undo/redo stack. Each element on that stack is a transaction that can be undone or
redone. Here, the transaction is modeled by a pair of <code>std::function</code>s:

```cpp
typedef std::function<void ()> Action;
typedef std::pair<Action/*Undo*/,Action/*Redo*/> Transaction;
```

An object can offer a method, which changes the state of the object and returns a transaction that can undo or redo the state change:

```cpp
class SimpleTransactionStateExample :
	  public std::enable_shared_from_this<SimpleTransactionStateExample>
{
private:
    int state;
public:
	void Set(int s)
	{
		state=s;
	}
	
	int Get() const
	{
		return state;
	}
	
	Transaction SetTransaction(int s)
	{
		Transaction Res=std::make_pair	(
			std::bind(&SimpleTransactionStateExample::Set,shared_from_this(),state),
			std::bind(&SimpleTransactionStateExample::Set,shared_from_this(),s)
		);
		Set(s);
		return Res;
	}
};
```

Taking an undo/redo dedicated transaction store one can use the <code>SetTransaction</code> method of the example class as follows:

```cpp
TransactionStore<std::list<Transaction> > ts;

std::shared_ptr<SimpleTransactionStateExample> E(new SimpleTransactionStateExample);
E->Set(0);

ts.AddTransaction(E->SetTransaction(1)); // undoable change of state
REQUIRE(1,E->Get());

ts.UndoLastTransaction(); // undo
REQUIRE(0,E->Get());

ts.RedoLastTransaction(); // redo
REQUIRE(1,E->Get());
```

#### Object lifetime management

The transactions may contain references to objects that no longer exist within the context, or the transaction
may be the creation of an object. This has to be taken care of by the implementer, i.e. with the help of some object
registration mechanism. In non-critical cases, such as here, this can be achieved by sharing the instance using a smart pointer.
Therefore, the example class uses <code>std::enable_shared_from_this</code>.
	
### Memento and transactions

One can combine the encapsulation of the internal state and the transactions by having a specialized object of type <code>DelayedTransaction</code> prepare the transaction for setting and resetting the internal state.

```cpp
TransactionStore<std::list<Transaction> > ts;
std::shared_ptr<MyOriginator> MO(new MyOriginator); // Memento originator
std::shared_ptr<DelayedTransaction<MyOriginator> > DT;

DT.reset(new DelayedTransaction<MyOriginator>(MO.get()));
DT->BeginTransaction(); // saves MO's state for undo
MO->Set("test1",1);
ts.AddTransaction(DT->EndTransaction());
// EndTransaction saves MO's state for redo and returns the transaction

DT.reset(new DelayedTransaction<MyOriginator>(MO.get()));
DT->BeginTransaction();
MO->Set("test2",2);
MO->Set("test3",3); //this state will be saved
ts.AddTransaction(DT->EndTransaction());

ts.UndoLastTransaction();
REQUIRE("test1",MO->GetString());
ts.RedoLastTransaction();
REQUIRE("test3",MO->GetString());

REQUIRE_THROWS(ts.RedoLastTransaction(),std::runtime_error);
```
	
+Follow the tests in the undoredotests folder


Hosting
=======

https://github.com/d-led/undoredo-cpp

License
=======

Author: Dmitry Ledentsov

MIT License (http://www.opensource.org/licenses/mit-license.php)

googletest, Premake and BOOST retain their respective licenses
