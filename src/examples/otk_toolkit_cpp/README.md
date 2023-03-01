otk_install

====================

This example Visual Studio solution uses the ready-made source code provided by the Creo object toolkit. The solution file compiles the code as a .dll, so that it can be loaded thanks to the provided protk.dat file.

To compile the code, open `otk_install.sln`. Then click on **Build->Build otk_install**. The resulting .dll is stored in `.\otk_install\x64\Debug`. 

The file `protk.dat` is the one tasked with informing creo what .dll needs to be used. Open the file and make sure that the path in `exec_file` points to the library.
