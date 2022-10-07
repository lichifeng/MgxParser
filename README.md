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
- **libz**: Required by **libpng** and header data decompressing.

All dependencies of MgxParser are bundled into the executive file.
