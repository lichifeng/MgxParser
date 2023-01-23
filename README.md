# **MgxParser**
*This version(0.3.4) was compiled on 20230123*

## Introduction
MgxParser is a C++ lib used to parse Age of Empires II game records.

It is firstly developed at 2020 to support running of aocrec.com as a
replacement of a prior php parser( which was a modified version of recanalyst).

## Basic usage
MgxParser exports two forms of `parse()` function:
One takes a record file path as input and returns a JSON string contains
important information about this record.  
The other accepts a byte(uint8_t) array and parses the data.

If input is a zip archive, MgxParser will try to Extract2Files first file in the
archive and parse it as a record. This function is intended to tackle compressed
record files from old version of aocrec.com.

No error was expected even if something unexpected happend in parsing process.
Users should check parsing result by the value of key `status` and `message` in
JSON response.

Mini maps can be generated two forms, too.

With `-m`, a typical map was generated, and

with `-M`, a prettier HD map was generated.

## Performance
I did't do elegent profiling for it, simply measured its performance by `\time -v
MgxParser recordfile.mgx`

Pure json output without map generation is very fast. Normally cost
<50ms and <10m peak memory( in my synology DS1621+ NAS docker container).

> Command being timed: "/Workspace/mgxParserCPP-master/build/MgxParser
> test/sampleRecords/AOC10_4v4_5_5e3b2a7e.mgx"  
> User time (seconds): 0.01  
> System time (seconds): 0.00  
> Percent of CPU this job got: 100%  
> Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.01  
> ... ...   
> **Maximum resident set size (kbytes): 9124**  
> Average resident set size (kbytes): 0  
> ... ...   
> Exit status: 0  

## Exceptions
Parsing an AoE record is fretful, exceptions are expected everywhere.   
But most of the time, exceptions don't mean worthless, some part of the  
record can still be read, so MgxParser is wrapped with an outermost   
try-catch block to avoid easy crash.   
When exceptions happen, they are normally caught and previously extracted   
information are returned. Some status hints are attached, so users will  
get notified.

## Requirements
These libraries are required to build MgxParser:
- **CImg(bundled)**: Used to generate mini maps. This library is bundled into `libs/CImg`
  and compiled with MgxParser.
- **movfuscator**: md5 function in this project was used to generate retroGuid
  of record. [Project page](https://github.com/xoreaxeaxeax/movfuscator/blob/ea37dae93fbcd93f642c71a53878da588bd7ddb4/validation/crypto-algorithms/md5_test.c)
- **libpng**: Required by CImg. 
  ```sh
  sudo apt-get install libpng-dev
  ```
- **libz**: Required by **libpng** and header data decompressing.
- **iconv**: Used to convert encoding of old records.
- **nlohmann/json**: Required to generate JSON output. A single-header version
  of `nlohmann/json` is bundled into `libs/nlohmann_json_3` and compiled with
  MgxParser.
- **doxygen**: Required to generate documents from source code. **Not
  necessary**.
- **Graphviz**: Required by **doxygen**.
  ```sh
  sudo apt-get install doxygen graphviz
  ```
  **googletest**: Required for testing.

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

## Compile with cmake-js
- Use `add_subdirectories()` funtion of CMake to integrate MgxParser into a
Node.js addon. 
- `set(NODE_ADDON 1)` in root `CMakeLists.txt`, MgxParser should adjust the proper
  CMake settings automatically.
- Put `$<TARGET_OBJECTS:MgxParser>` as an `<item>` of `add_library()` directive
  of root `CMakeLists.txt`.
- Include `src/include/MgxParse.h`.

Then `MgxParser::parse()` should be available.

## Version log
- **0.2.0**: Reoragnized source files, tested with 130000+ records in
  aocrec.com.
- **0.1.0**: Now MgxParser can work with Node.js(Fastify) and parse a uploaded
  record. All operations are done in memory without file storage.

## Resources
Find records for test: https://www.ageofempires.com/stats/ageiide
https://github.com/topics/age-of-empires
DE Replays Manager: https://github.com/gregstein/DE-Replays-Manager
Awesome Age of Empires II resources: https://github.com/Arkanosis/awesome-aoe2
https://aok.heavengames.com/blacksmith/lister.php?category=utilities
https://aok.heavengames.com/blacksmith/lister.php?category=records   
AoE II Development Collective https://siegeengineers.org/
https://www.twaoe2wiki.com/

## Caution
This project is still under development. **No warrenty for anything!**
