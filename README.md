# BigInteger
A C++ library for infinite-precision integer arithmetic 

## Download instructions

To clone the repository use the following git command:
`git clone https://github.com/adellafrattina/BigInteger BigInteger`

## Build instructions
To generate project files, you can run the script files inside the `vendor` folder or execute manually the commands down below. The generated project files will be stored in the `build` folder. This project uses [premake5](https://premake.github.io/) to generate configuration files

### Windows
  Inside `vendor\windows`, run the command:
  - `premake5.exe --file=build.lua vs2022` or run the script `build-vs2022.bat` to generate Visual Studio 2022 project files
  - `premake5.exe --file=build.lua gmake2` or run the script `build-gmake.bat` to generate Makefiles project files

### MacOSX
  Inside `vendor/macosx`, run the command:
  - `./premake5 --file=build.lua xcode4` or run the script `build-xcode.sh` to generate Xcode project files
  - `./premake5 --file=build.lua gmake2` or run the script `build-gmake.sh` to generate Makefiles project files

## Algorithms
The main algorithms used in this library are the following:
- [Double dabble algorithm](https://en.wikipedia.org/wiki/Double_dabble) to convert integers from an array of binary numbers to a string-represented decimal number and its reverse version to convert from a string-represented decimal number to an array of binary numbers
- [Karatsuba algorithm](https://en.wikipedia.org/wiki/Karatsuba_algorithm) to multiply two small-medium integers in O($`n^{\log_2{3}}`$) time
- [Toom-Cook 3-way algorithm](https://en.wikipedia.org/wiki/Toom%E2%80%93Cook_multiplication) to multiply two medium-large integers in O($`n^{\log{5}/\log{3}}`$) time
- [Knuth's algorithm](https://skanthak.hier-im-netz.de/division.html) to divide two integers in O($`nm`$) where $`n`$ is the dividend's size and $`m`$ is the divisor's size

## Endianness
Currently, only little-endianness systems are supported
