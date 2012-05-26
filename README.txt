= Summary =


This is a research on the ways to implement generic Undo/Redo mechanisms in C++ by Dmitry Ledentsov


== Used in the project ==


 * [http://code.google.com/p/googletest/ googletest] for tests
 * [http://industriousone.com/premake Premake] for generating makefiles

== Compilers ==

known to work with
 * Visual Studio 2010 (Express)
 * MinGW 4.6.1

The Memento part can be used with older Microsoft compilers, however the tests are defined in terms of tr1 classes residing in the std namespace


= Usage =


Follow the tests in the undoredotests folder


= Info =

See http://code.google.com/p/undoredo-cpp/