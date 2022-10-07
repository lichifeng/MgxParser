# **MgxParser**
*This version(0.1.0) was compiled on 20221007*

## INTRODUCTION
MgxParser is a Age of Empires II game record parser written in C++.

It is firstly developed at 2020 to support running of aocrec.com as a
replacement of recanalyst.


## Requirements
- **CImg**: Used to generate mini maps. This library is bundled into `libs/CImg`
  and compiled with MgxParser.
- **libpng**: Required by CImg. 
  ```shell
  sudo apt-get install libpng-dev
  ```
- **libz**: Required by **libpng** and header data decompressing.
- **nlohmann/json**: Required to generate JSON output. A single-header version
  of `nlohmann/json` is bundled into `libs/nlohmann_json_3` and compiled with
  MgxParser.
- **doxygen**: Required to generate documents from source code. **Not
  necessary**.
- **Graphviz**: Required by **doxygen**.
  ```shell
  sudo apt-get install doxygen graphviz
  ```

All dependencies of MgxParser are bundled into the executive file after compilation.

To build this project, latest version of CMake is recommended. See
https://apt.kitware.com/ for details on this. I use CMake `3.24.1` and never
tested other versions.

A C++17 compiler is also needed. `G++-9`, `Clang++-10` or newer compilers are
recommended. Other compilers may work, but I never tested, too.
```shell
sudo apt-get install g++-8
#OR
sudo apt-get install clang++-10
```
