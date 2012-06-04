+Summary
+=======
+
+
+This is a research on the ways to implement generic Undo/Redo mechanisms in C++
+
+
+Used in the project
+-------------------
+
+ * [googletest](http://code.google.com/p/googletest/) for tests
+ * [Premake](http://industriousone.com/premake) for generating makefiles
+
+Compilers
+-------------------
+
+known to work with
+ * Visual Studio 2010 (Express)
+ * MinGW >4.6.1
+
+The Memento part can be used with older Microsoft compilers, however the tests are defined in terms of tr1 classes residing in the std namespace.
+One could use BOOST.TR1 or provide compile time alias choice for the std::tr1 namespace
+
+
+Usage
+=====
+
+
+Follow the tests in the undoredotests folder
+
+
+Hosting
+=======
+
+https://github.com/d-led/undoredo-cpp
+
+License
+=======
+
+MIT License (http://www.opensource.org/licenses/mit-license.php)
+
+googletest, Premake and BOOST retain their respective licenses
