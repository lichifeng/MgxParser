# **MgxParser**
*This version(0.4.5) was compiled on 20240208*

## Introduction
MgxParser is a C++ lib used to parse Age of Empires II game records.

It is firstly developed at 2020 to support running of aocrec.com as a
replacement of a prior php parser( which was a modified version of recanalyst).

## Use a docker image
A docker image is available at https://hub.docker.com/r/lichifeng/mgxparser
Just pull and run it, send a record file to it and get a JSON response.
```sh
docker pull lichifeng/mgxparser
docker run -e MAX_CONNECTIONS=100 -p 4400:4400 lichifeng/mgxparser
```
Then, send a record file and a json command to it:
```sh
curl -X POST -F "file=@/path/to/record.mgx" -F "json={\"map\":\"HD\"}" http://localhost:4400/parse
```
To build the docker image manually, use the `Dockerfile` in root directory.
```sh
docker build -t mgxparser .
docker run -it -p 4400:4400 --rm mgxparser
```

## Basic usage
MgxParser `parse(Settings)` function:
One takes a record file path as input and returns a JSON string contains
important information about this record.  

If input is a zip archive, MgxParser will try to Extract2Files first file in the
archive and parse it as a record. This function is intended to tackle compressed
record files from old version of aocrec.com.

No error was expected even if something unexpected happend in parsing process.
Users should check parsing result by the value of key `status` and `message` in
JSON response.

**Read `src/include/MgxParse.h` for more details.**

A demo of MgxParser will be compiled using demo.cc. It can be used to parse a
record file and print the result to stdout.
```sh
mgxparser --help
```
Output:
```sh
Usage: ./mgxparser [options] [file]
Options:
  -m    Generate a normal map
  -M    Generate a HD map
  -u    Extract the original file in .zip archive
  -j    Indentation of the json string. i.e. -j2
  --help Display this help message
Example: ./mgxparser -m -j2 demo.mgx
```

For node addon usage, see `src/node/README.md`,    
See **Compile node addon with cmake-js** for details.

## Performance
I did't do elegent profiling for it, simply measured its performance    
by `\time -v MgxParser recordfile.mgx`   

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
or
```sh
sudo apt-get install cmake
```

A C++17 compiler is also needed. `G++-9`, `Clang++-10` or newer compilers are
recommended. Other compilers may work, but I never tested, too.
```sh
sudo apt-get install g++-8
#AND
sudo apt-get install clang
#Then, build the source file
cmake . -DCMAKE_CXX_COMPILER=/usr/bin/clang++-10 -DCMAKE_BUILD_TYPE=Release --fresh
#Last, compile it!
make
```

## Compile node addon with cmake-js
To compile it to a node addon, `cmake-js` and `node-addon-api` is required.   
Use `npm` to install `cmake-js` and `node-addon-api`:
```sh
npm install
```
Then, compile it with `cmake-js`:
```sh
npx cmake-js compile -p 6 # or other threads settings
```
The compiled node addon will be in `build/Release` directory.   
A demo of node addon is in `test/node_addon_test.js` directory.

## Version log
- **0.4.5**: Reorganized source code. Refactored `parse()` function. Add node addon support. Add docker workflow.   
- **0.4.2**: A version used on aocrec.com before Feb. 2024.
- **0.4.0**: Prepare to go online. Add english language pack. Fixed more bugs.
- **0.3.x**: Bug fix and some changes to meet requirements of mgxhub.
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
