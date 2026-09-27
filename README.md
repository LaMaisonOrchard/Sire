# Sire
A simple build utility written in D

Sire will try to define the following envirnment vaiables base on your environment. These can be overridden by explicit
environment variable declarations.

SIRE        // sire executable
CWD         // Current working directory
SHELL       // The shell to uise for running commands
SEP         // The file path separator

DC          // D compiler
CC          // C compiler
CPP         // C++ compiler
FORTRAN     // Fortran compiler
GO          // Go compiler
MOD2        // Modula-2 compiler

FRED = <fred> ;

The '<>' notation will expand to the full pathe to the given executable. The '<fred>' will match "fred.exe" and "fred.bat" on Windows and "fred" and "fred.sh" on everything else.
This allows for platform specific scripts to be selected on different platforms.
