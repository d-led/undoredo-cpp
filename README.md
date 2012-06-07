Summary
=======


This is a research on the ways to implement generic Undo/Redo mechanisms in C++.
The Undo/Redo mechanisms presented here are not binary safe, but might assist in implementing binary safe undo/redo aware objects.


Used in the project
-------------------

 * [googletest](http://code.google.com/p/googletest/) for tests
 * [Premake](http://industriousone.com/premake) for generating makefiles

Compilers
-------------------

known to work with
 * Visual Studio 2010 (Express)
 * MinGW >4.6.1
 * gcc 4.6.3 (linux)
 * clang 3.0 (linux)

The Memento part can be used with older Microsoft compilers, however the tests are defined in terms of tr1 classes residing in the std namespace.
One could use BOOST.TR1 or provide compile time alias choice for the std::tr1 namespace


Usage
=====

### Memento
The classes provided in the project can be mixed arbitrarily. Let's start with the Memento itself. Considering a class MyOriginator:

<pre>
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
</pre>

we would like to be able to store its internal state, which is stored in a private inner class State. We could do it directly for each class we would
like to support saving its state. Considering that not each part of the internal state should known outside, the [Memento pattern](http://en.wikipedia.org/wiki/Memento_pattern) comes to mind. To document the pattern, it can be explicitly called Memento, implemented
as a C++ template class, taking the internal state as the template parameter. Only that class would be allowed to modify the state directly through the
dedicated interface added to the MyOriginator class:

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

### MementoStore

To be able to use the memento conveniently with Caretaker classes, the MementoType type is defined. Although the encapsulation in C++ is syntactical,
the explicit use of the Memento pattern should discourage using the "leaked" internal state. The <code>SaveState</code> and <code>RestoreState</code> methods
implement the pattern. Another convenience is to be able to use Caretaker classes matching the memento interface. Adding the following typedef we can use the
MementoStore:

    typedef MementoStore<MySecondOriginator> MementoStoreType;

Using the StlMementoStore class we can store and restore states of individual objects, or all objects at once. For the demonstration purposes the objects
are identified by their raw pointers, however other schemes can be envisioned. Using the Store looks like that:

    auto savedStates=StlMementoStore<MyOriginator,std::map<MyOriginator*,std::list<typename MyOriginator::MementoType> >>();
    MyOriginator O;
    O.Set("test",1);
    savedStates.Save(&O); // saving the current state
    O.Set("bla",2); // setting a new state
    ASSERT_EQ("bla",O.GetString());
    savedStates.Undo(&O); // restoring to the saved state
    ASSERT_EQ("test",O.GetString());

### Transaction and TransactionStore

A typical undo/redo scenario does not include explicit choice of the object to be restored to its previous state. The change of the state of many
different objects should be taken into account and assembled in an undo/redo stack. Each element on that stack is a transaction that can be undone or
redone. Here, the transaction is modeled by a pair of <code>std::function</code>s:

    typedef std::function<void ()> Action;
    typedef std::pair<Action/*Undo*/,Action/*Redo*/> Transaction;

An object can offer a method, which changes the state of the object and returns a transaction that can undo or redo the state change:

    class SimpleTransactionStateExample : public std::enable_shared_from_this<SimpleTransactionStateExample>
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
    	Transaction Res=std::make_pair(
    							std::bind(&SimpleTransactionStateExample::Set,shared_from_this(),state),
    							std::bind(&SimpleTransactionStateExample::Set,shared_from_this(),s)
    						 );
    	Set(s);
    	return Res;
    }
    };

Taking an undo/redo dedicated transaction store one can use the <code>SetTransaction</code> method of the example class as follows:

    TransactionStore<std::list<Transaction> > ts;

    std::shared_ptr<SimpleTransactionStateExample> E(new SimpleTransactionStateExample);
    E->Set(0);
    
    ts.AddTransaction(E->SetTransaction(1)); // undoable change of state
    ASSERT_EQ(1,E->Get());
    
    ts.UndoLastTransaction(); // undo
    ASSERT_EQ(0,E->Get());
    
    ts.RedoLastTransaction(); // redo
    ASSERT_EQ(1,E->Get());

#### Object lifetime management

The transactions may contain references to objects that no longer exist within the context, or the transaction
may be the creation of an object. This has to be taken care of by the implementer, i.e. with the help of some object
registration mechanism. In non-critical cases, such as here, this can be achieved by sharing the instance using a smart pointer.
Therefore, the example class uses <code>std::enable_shared_from_this</code>.
	
### Memento and transactions

One can combine the encapsulation of the internal state and the transactions by having a specialized object of type <code>DelayedTransaction</code> prepare the transaction for setting and resetting the internal state.

    TransactionStore<std::list<Transaction> > ts;
    std::shared_ptr<MyOriginator> MO(new MyOriginator); // Memento originator
    std::shared_ptr<DelayedTransaction<MyOriginator> > DT;
    
    DT.reset(new DelayedTransaction<MyOriginator>(MO.get()));
    DT->BeginTransaction(); // saves MO's state for undo
    MO->Set("test1",1);
    ts.AddTransaction(DT->EndTransaction()); // EndTransaction saves MO's state for redo and returns the transaction
    
    DT.reset(new DelayedTransaction<MyOriginator>(MO.get()));
    DT->BeginTransaction();
    MO->Set("test2",2);
    MO->Set("test3",3); //this state will be saved
    ts.AddTransaction(DT->EndTransaction());
    
    ts.UndoLastTransaction();
    ASSERT_EQ("test1",MO->GetString());
    ts.RedoLastTransaction();
    ASSERT_EQ("test3",MO->GetString());
    
    ASSERT_THROW(ts.RedoLastTransaction(),std::runtime_error);

	
+Follow the tests in the undoredotests folder


Hosting
=======

https://github.com/d-led/undoredo-cpp

License
=======

Author: Dmitry Ledentsov

MIT License (http://www.opensource.org/licenses/mit-license.php)

googletest, Premake and BOOST retain their respective licenses
