# Sire
A simple dependency rule based build utility written in D. The Untility is aimed and platform independence.

## Command Line

## Rules

The general rule structure is:

{\<targets\>} ':' {\<flags\>} ':' {\<dependents\>} '{' \<script\> '}'

i.e.
```
*.o : QUIET : *.c
{
    gcc -o $(TARGET) $(DEPS)
}
```

The flags are optional:

{\<targets\>} ':' {\<dependents\>} '{' \<script\> '}'

i.e.
```
*.o : *.c
{
    gcc -o $(TARGET) $(DEPS)
}
```


The dependents are also optional are optional:

{\<targets\>} '{' <script> '}'

i.e.
```
test
{
    RunTests.sh
}
```

The scripts are optional:

{\<targets\>} ':' {\<flags\>} ':' {\<dependents\>} ';'

i.e.
```
*.c : TOUCH : *.h ;
```

The most minimal rule:

{\<targets\>} ':' ';'

i.e.
```
*.c : ;
```

For a given target each rule in turn is checked starting with the first and ending with the last. Is the rule matches then the dependents are checked in turn.
If any of the dependents are younger than the target then the script is run. If there are no dependents then the script (if it exists) will be run.

If the rule has the TOUCH flag then he rule only matches is the target exists.

If the rule has the CREATE flag the rule only maches if he target does not exist.

If the rule has the CONTINUE flag then even if the rule matches further rules will be checked.

### Special Rules
There are four special rules:

The script of the PRE rule is run before any other rules (flags and dependents are ignored).

The TARGET rules is run if no other targets are given on the command line.

The script of the POST rule is run if the build process succeeds (flags and dependents are ignored).

The script of the FAIL rule is run if the build process fails (flags and dependents are ignored).

```
PRE : QUIET :
{
    echo Start
}

TARGET : file$(EXE)
{
    $(DC) app.d
}

POST : QUIET :
{
    echo Stop
}

FAIL : QUIET :
{
    echo "ERROR !!!!"
}
```

## Variables

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

### Executables

```
FRED = <fred> ;
```

The '<>' notation will expand to the full pathe to the given executable. The '<fred>' will match "fred.exe" and "fred.bat" on Windows and "fred" and "fred.sh" on everything else.
This allows for platform specific scripts to be selected on different platforms.

### Files

```
FRED = 'source/*.c' ;
```

The "''" notation expands the a list of files that match the string. It is important to note that the value of a variable is not a string but a list of zero
or more strings.

### Quotes

The values in a variable can be quotes and the quotes string wil be a single entry in the list

```
FRED = "the main.c" ;
```
### Transforms

The list entries can be transformes using matching

```
FRED = 'Src/*.c' : Src/*.c : Obj/$(0).o ;
```
This creates a list of objects one for each source file. Each element matched with an '*' becomes a number environment variable expanded as '$(n)'.

## Scripts

The rule scripts are run using a scripting language define by the variable SHELL. By default SHELL is set to 'sire' which means sire's built in scripting languag.
This is a very simple scripting language to do basic operations. You can use alternative scripting languages by setting the SHELL vaiable.

```
SHELL = <python> ;
```

In this case the rule scripts will be run in the python interpreter.

### Variable Expansion

Variables withe the notation '$(<name>)' will be expanded in the script before the script is run. If the value is a list it will be expaned as a space separated list.
Any this placed directly around the variable will be placed around each expaned item so '"$(<name>)"' will expand to a space separated list of quoted items. The name of the 
variable can itself contain variables i.e. '$(HELLO$(WORLD))'.

```
NUM  = 1 2;
Let  = A B;
ARG1 = Harry
ARG2 = Fred

TARGET
{
    $(LET)        ==> A B
    "$(NUM)",     ==> "1", "2",
    $(LET)_$(NUM) ==> A_1 A_2 B_1 B_2
    $(ARG$(NUM))  ==> Harry Fred
}
```
### Variable Indexing

A variables list can be indexed and spliced.

```
FRED = A B C D;


PRE
{
echo $(FRED:1)    ==> B C D
echo $(FRED:1:3)  ==> B C
echo $(FRED:1:2)  ==> B
echo $(FRED:0:-1) ==> A B C
}
```

### Special Rule Variables

The variable TARTGET is set to the current target for the rule and the variable DEPS is set to the dependent for the rule. Any piece of text
matched tp a '*' in the rule target becomes a numbered variable.

## Sire Script

Sire's build is script langague is a a basic script to support simple operations.

## Examples

### sire

```
SRC = 'source/*.d' ;
OBJ = $(SRC) : */*.d : $(0)/$(1).o ;

PRE : QUIET :
{
echo "Start"
}

TARGET
{
echo "$(OBJ)"
$(DC) -of=$(TARGET) $(SRC)
}

POST : QUIET :
{
echo "Stop"
}

FAIL : QUIET :
{
echo "FAIL"
}
```

### Python

```
SHELL = <python>;

SRC = 'source/*.d' ;
OBJ = $(SRC) : */*.d : $(0)/$(1).o ;

PRE : QUIET :
{
print("Start")
}

TARGET
{
import sys
import subprocess
print( "$(OBJ)", )
subprocess.run( ["$(DC)", "-of=$(TARGET)" ,"$(SRC)" ]);

sys.exit(0)
}

POST : QUIET :
{
print("Stop")
}

FAIL : QUIET :
{
print("FAIL")
}
```