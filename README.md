# Dern [<img src="https://img.shields.io/github/license/Kirdow/Dern">](https://github.com/Kirdow/Dern/blob/master/LICENSE)
An interpreted custom programming language made by me initially in 2013-2014, now made anew from scratch. Name is the same used during that time, but the code is completely new.

## Why the name 'Dern'?
Back then it was a made up name, and still is in my opinion. I have learned it means ``Dark`` or ``Hidden`` which somewhat fits my style in a way, but this isn't my intention. The name actually comes from my own name, by taking the first two letters of my first name, and concatenate them with the first and last letter of my last name.

## How to set it up
This project is currently being made on Arch Linux using G++ with C++17 and Makefile in order for me to learn Linux programming.
Other platforms will probably work but are not officially supported.

<ins>**1. Clone the repo**</ins>

Open a command prompt and run ``git clone --recursive https://github.com/Kirdow/Dern``.
I don't currently use any submodules but it's always a good idea to include ``--recursive`` just in case.

<ins>**2. Run bash files**</ins>

Currently I'm using a set of 7 bash files for this project, mainly ``./execl.sh`` which runs ``./exec.sh`` on a loop, pausing for a key press after every run of the script. ``./exec.sh`` first runs ``./fclean.sh`` which wipes every built file completely, then ``./build.sh`` which builds the app, and finally ``./run.sh`` which runs the executable. Always running ``./fclean.sh`` before every build is not ideal for bigger projects, but it fixes a bug where it won't rebuild when I only edited a header file.

*Always check the bash files before running them as I can't make sure they always do what I've told you they do. Never ever run files that you don't know what they're doing.*

## Tools and Libraries
This project primarily depend on these:

### **Software/Tools**
- [<img src="https://code.visualstudio.com/assets/images/code-stable.png" alt="Code" width="16" height="16"> Visual Studio Code](https://code.visualstudio.com/ "Visual Studio Code Website") as primary editor/IDE
- [<img src="https://www.gnu.org/graphics/heckert_gnu.transp.small.png" alt="GNU" width="16" height="16" style="filter: invert(100%)"> G++](https://gcc.gnu.org/ "GNU GCC Website") as compiler/linker
- [<img src="https://www.gnu.org/graphics/heckert_gnu.transp.small.png" alt="GNU" width="16" height="16" style="filter: invert(100%)"> Make](https://www.gnu.org/software/make/ "GNU Make Website") as build script
- [<img src="https://en.cppreference.com/favicon.ico" alt="C++" width="16" height="16"> C++17](https://en.cppreference.com/w/cpp/17 "C++17 @ cppreference.com")
