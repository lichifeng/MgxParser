# **MgxParser**
*This version(0.1.0) was compiled on 20221008*

## Introduction
MgxParser is a Age of Empires II game record parser written in C++.

It is firstly developed at 2020 to support running of aocrec.com as a
replacement of a prior php parser( which was a modified version of recanalyst).

## Basic usage
MgxParser takes a record file path as input and returns a JSON string contains
important information about this record.

No error was expected even if something unexpected happend in parsing process.
Users should check parsing result by the value of key `status` and `message` in
JSON response.

Mini maps can be generated two forms.

With `-m mapname.png`, a typical map was generated, and

with `-M HDmapname.png`, a prettier HD map was generated.

## Requirements
These libraries are required to build MgxParser:
- **CImg**: Used to generate mini maps. This library is bundled into `libs/CImg`
  and compiled with MgxParser.
- **libpng**: Required by CImg. 
  ```sh
  sudo apt-get install libpng-dev
  ```
- **libz**: Required by **libpng** and header data decompressing.
- **nlohmann/json**: Required to generate JSON output. A single-header version
  of `nlohmann/json` is bundled into `libs/nlohmann_json_3` and compiled with
  MgxParser.
- **doxygen**: Required to generate documents from source code. **Not
  necessary**.
- **Graphviz**: Required by **doxygen**.
  ```sh
  sudo apt-get install doxygen graphviz
  ```

All dependencies of MgxParser are bundled into the executive file after compilation.

## How to compile
To compile this project, latest version of CMake is recommended. See
https://apt.kitware.com/ for details on this. I use CMake `3.24.1` and never
tested other versions. On Ubuntu18 and newer versions, fresh version of cmake
can be installed by:
```sh
sudo snap install cmake --classic
```

A C++17 compiler is also needed. `G++-9`, `Clang++-10` or newer compilers are
recommended. Other compilers may work, but I never tested, too.
```sh
sudo apt-get install g++-8
#AND
sudo apt-get install clang++-10
#Then, build the source file
cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10 -DCMAKE_BUILD_TYPE=Release --fresh
#Last, compile it!
make
```

## Caution
This project is still under development. **No warrenty for anything!**
