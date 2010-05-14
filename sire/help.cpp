/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 2002, 2004, 2005  David W Orchard (davido@errol.org.uk)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


*****************************************************************************

   File   : help.cpp
   Applic : NONE
   Desc   : Provide the built in application help
   
AMENDMENTS

EDIT 001 10-Sep-02 david@errol.org.uk
Extracted this source from main.cpp.

EDIT 002 15-Sep-02 david@errol.org.uk
Add support for help files.
*****************************************************************************/

/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "help.h"
#include "set.h"

/************* FUNCTIONS ****************************************************/


void shortHelp(int &, const char**&)
{
    printf("sire [-i] [-help [<topic>]] [-auth] [-quiet] [-talk] [-debug] [-partial] [-remove] [-norun | -nobuild] [-f <file>] {<target>} [ -params {<args>} ]\n");
}

void help(int &argc, const char**&argv)
{
    if ((NULL == *argv) || !strncasecmp("switches", *argv, strlen(*argv)))
    {
        char const *text =
            "sire [switches] {<target>}\n"
            "    \n"
            "-i                       - Output mark and issue\n"
            "-h                       - Output the short help\n"
            "-help [<topic]           - Output the long help (sire -help help)\n"
            "-auth                    - Authorisation data\n"
            "-env                     - Print out the environment\n"
            "-all                     - Rebuild all files\n"
            "-quiet | -s              - Don't output tracing info\n"
            "-talk                    - Output build tracing info\n"
            "-debug | -d              - Output rule tracing info\n"
            "-partial                 - Do a partial build\n"
            "-remove                  - Remove the intermidiate objects\n"
            "-w                       - Report the current working directory on entry and exit\n"
            "-C <dir>                 - Run in the given directory.\n"
            "-force                   - Ignore error when building targets\n"
            "-k                       - Rebuild as many depenents a posible after a fail\n"
            "-S                       - When the build fails fail immediately\n"
            "-t                       - Only 'touch' the files, don't build them\n"
            "-b | -m                  - Both these switches are ignored.\n"
            "-D<variable>=<value>     - Set the environment variable.\n"
            "-norun | -nobuild | -n   - Do not execute the build\n"
            "-f <file>                - Read the sire data from the file\n"
            "<target>                 - Build the given target(s)\n"
            "-params {<args>}         - Arguments for the PREAMBLE\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("source", *argv, strlen(*argv)))
    {
        char const *text =
            "SOURCE [OPTIONAL] <file> ;\n"
            "    \n"
            "    Include the file as part of this relations file\n"
            "    If the 'source' file is optional then an error\n"
            "    is not raised if the file is not present.\n"
            "\n"
            "SOURCE `<shell command>` ;\n"
            "    \n"
            "    Include the results of the shell command as part\n"
            "    of this relations file.\n"
            "\n"
            "See also ENV\n"
            "\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("help", *argv, strlen(*argv)))
    {
        char const *text =
            "sire -help [<topic>]\n"
            "    \n"
            "    help          - This help\n"
            "    switches      - Standard switches for sire\n"
            "    env           - Set the build environment\n"
            "    preamble      - The use of preamble rules\n"
            "    postamble     - The use of postamble rules\n"
            "    rule          - How to define build rules\n"
            "    set           - The set command provides automatic configuration\n"
            "    variables     - How to set/use environment variables\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("fun", *argv, strlen(*argv)))
    {
        char const *text =
            "Speak roughly to your little boy\n"
            "and spank him when he sneezes.\n"
            "He only does it to annoy\n"
            "because he knows it teases.\n"
            "\n"
            "Speak roughly to your little boy\n"
            "and spank him when he sneezes.\n"
            "For he can fully enjoy\n"
            "the pepper when he pleases.\n"
            "\n"
            "See also LIFE\n"
            "\n"
        ;
    
        printf(text);
    }
    else if (!strncasecmp("env", *argv, strlen(*argv)))
    {
        char const *text =
            "SET ENV [CLEAN] <file> ;\n"
            "    \n"
            "    The file is sourced to modify the sire\n"
            "    environement in situe. If the CLEAN modifier\n"
            "    is specified then the current environment\n"
            "    is reduced to the following variables prior\n"
            "    to sourcing the script.\n"
            "    \n"
            "    - MACHINE\n"
            "    - SHELL\n"
            "    - PWD\n"
            "    - USER\n"
            "    - TALK_STATE\n"
            "    - DEBUG_STATE\n"
            "    - REMOVE_STATE\n"
            "    - ONE_STATE\n"
            "    - SIRE_PARTIAL\n"
            "    \n"
            "    The purpose of 'SET ENV' is to provide greater\n"
            "    flexability in configuring sire's environment.\n"
            "\n"
            "See also SET and SOURCE\n"
            "\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("preamble", *argv, strlen(*argv)))
    {
        char const *text =
            "PREAMBLE { ... } ;\n"
            "    \n"
            "    The PREAMBLE rule is used to configure the\n"
            "    build environment. Since PREAMBLE rule is a\n"
            "    rule it is not run until after the relations\n"
            "    file has been read. As a result the PREAMBLE\n"
            "    rule will not affect the sire environment only\n"
            "    the build environment for subsequent rules.\n"
            "    \n"
            "    The PREAMBLE rule is not as powerful as 'SET ENV'\n"
            "    but provides greater visablity of the evironment.\n"
            "\n"
            "See also ENV, POSTAMBLE ans RULES\n"
            "\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("postamble", *argv, strlen(*argv)))
    {
        char const *text =
            "POSTAMBLE { ... } ;\n"
            "    \n"
            "    The compliment of the PREAMBLE rule. It serves\n"
            "    no real purpose (since everything has finished\n"
            "    by the time it is run).\n"
            "\n"
            "See also ENV, PREAMBLE ans RULES\n"
            "\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("rules", *argv, strlen(*argv)))
    {
        char const *text =
            "{<target>} [ [ '|' {<modifier>} ] '|' {<dependent>} ] [ '{' <build script> '}' ] ;\n"
            "    \n"
            "    Rules specify the relationships between files\n"
            "    and how to create/update a target file from a set\n"
            "    of dependencies. The nature of the relationship\n"
            "    can be changed by a number of rule modifiers.\n"
            "    \n"
            "Explicite target :-\n"
            "    \n"
            "rm remove | {sire -remove}\n"
            "    \n"
            "    Define a target 'rm' and 'remove' then uses the '-remove'\n"
            "    \n"
            "    Any file that matches an explicte rule target must use the rule or\n"
            "    fail the build.\n"
            "    \n"
            "prog | $(OBJECTS}\n"
            "{\n"
            "   $(CC) -o $(TARGET} $(LFLAGS) $(DEPS)\n"
            "}\n"
            "    \n"
            "    Define a target to link the program. Note the use of the\n"
            "    automatically defined vaiable 'TARGET' and 'DEPS'. There\n"
            "    is an additional variable 'FIRST_DEP' which is set to the \n"
            "    first rule dependent.\n"
            "    \n"
            "    Since the depenedent and target are passed to the command\n"
            "    script are arguments you can define the rule as follows :-\n"
            "    \n"
            "prog | $(OBJECTS}\n"
            "{\n"
            "   fred=${1}    \n"
            "   shift        \n"
            "   $(CC) -o ${fred} $(LFLAGS) ${*}\n"
            "}\n"
            "    \n"
            "    Explicite rules do not need a build command.\n"
            "    \n"
            "    build | rm TARGET install ;\n"
            "    \n"
            "    This rule ('build') will run the 'rm' rule to remove intermediate\n"
            "    files. Then it build the default ('TARGET') rule and finallt it\n"
            "    runs the 'install' rule to install the files.\n"
            "    \n"
            "    \n"
            "Implicite target :-\n"
            "    \n"
            "*.o | *.cpp\n"
            "{\n"
            "   $(CC) -c -o $(TARGET} $(CFLAGS) $(FIRST_DEP)\n"
            "}\n"
            "    \n"
            "    A file that matches an implict rules target but cannot use the\n"
            "    rule (because of missing dependent) will go on to look for other\n"
            "    implice rules of a first defined first matched basis. (Explicite\n"
            "    rules always match before implicite rules without regard to the\n"
            "    order of definition.\n"
            "    \n"
            "    The text matched by the wild cards are available as automaticly\n"
            "    defined variables.\n"
            "    \n"
            "*.gen.[ch]xx | *.defn\n"
            "{\n"
            "   class_gen -o $(TARGET) $(FIRST_DEP)  -class $(1) -type $(2)xx\n"
            "}\n"
            "    \n"
            "    For the file 'fred.gen.h' this will produce the command :-\n"
            "    \n"
            "    class_gen -o fred.gen.h fred.defn -class fred -type h\n"
            "    \n"
            "Modifiers\n"
            "    \n"
            "    A rules behaviour can be modified\n"
            "    \n"
            "    QUIET  - Don't echo the command before running it.\n"
            "    TALK   - Echo the command before running it.\n"
            "    DEBUG  - Enable debug for instances of 'sire' run by the rule.\n"
            "    \n"
            "    TOUCH  - This rule can only update not create the target.\n"
            "    CREATE - This rule can only create not update the target.\n"
            "    \n"
            "    CONT   - Continue to look for other rules for this target.\n"
            "    BREAK  - Don't look for rules for the depenent.\n"
            "    \n"
            "    USER   - The rule can create/update the file but the file also\n"
            "             contains user content so it should not be deleted.\n"
            "             This is often used with the 'CREATE' modifer.\n"
            "    \n"
            "*.[ch] *.[ch]xx | TOUCH CONT | DUMMY `getinclude $(CFLAGS) $(TARGET)`\n"
            "{\n"
            "   touch $(TARGET)\n"
            "}\n"
            "    \n"
            "    This rule checks the include file dependencies. There may be\n"
            "    other rules that apply to the source files so we use 'CONT'\n"
            "    to look for there. Also by using 'touch' you can't create a\n"
            "    meaningful file you can only 'update' it (change the time stamp)\n"
            "    So we use 'TOUCH'.\n"
            "    \n"
            "    We also use the special dependent 'DUMMY'. 'DUMMY' is an imaginary\n"
            "    file that is always older than the target. If a file has no dependents\n"
            "    then it is rebuilt. The use of the 'DUMMY' dependent prevents this\n"
            "    by supplying a dummy depenedent.\n"
            "    \n"
            "    There is a simular dependent 'FORCE'. 'FORCE' is an imaginary dependent\n"
            "    that is always newer than the target. So the use of this depenedent\n"
            "    forces the file to always be rebuilt.\n"
            "    \n"
            "build_data.h | gen_build_data FORCE\n"
            "{\n"
            "   gen_build_data > build_data.h\n"
            "}\n"
            "\n"
            "See also PREAMBLE, POSTAMBLE and PARTIAL\n"
            "\n"
        ;
        printf(text);
    }
    else if (!strncasecmp("variables", *argv, strlen(*argv)))
    {
        char const *text =
            "<variable> = {<arg>} [ '|' <pattern> '|' <pattern> ] ;\n"
            "    \n"
            "    The variable definition syntax defines the values of environment\n"
            "    varables within sire.  Although an number of while spaces may be\n"
            "    used between  arguments only one single space will appear in the\n"
            "    value of the variable.  If you wish to force spaces the you must\n"
            "    use quotes, i.e.\n"
            "    \n"
            "    TITTLE = \"William the    Tell\" ;\n"
            "    \n"
            "    The value of the variable can be obtained by using `$(TITTLE)`.\n"
            "    \n"
            "    Sire ensures that the following variables are always defined\n"
            "    \n"
            "        SHELL          - The shell interpreter\n"
            "        ROOTDIR        - The root directory\n"
            "        FSEP           - The file separator charactor\n"
            "        PSEP           - The path separator charactor\n"
            "        .EXE           - The file extension for executables\n"
            "        .SH            - The file extension for bourn scripts\n"
            "        .KSH           - The file extension for K-Shell scripts\n"
            "        TEMP           - The system default directory for tempory files\n"
            "        MACHINE        - The machine type\n"
            "        PLATFORM       - The platform type\n"
            "        OSTYPE         - The operating system type (as used by bash)\n"
            "        USER           - The users username\n"
            "        GROUP          - The users primary group name\n"
            "        SIRE           - The full path to the sire application\n"
            "        CCP            - The C++ compiler\n"
            "        CC             - The C compiler\n"
            "        JAVAC          - The Java compiler\n"
            "        SH             - The full path to a bourn shell interpreter\n"
            "        CSH            - The full path to a C shell interpreter\n"
            "    \n"
            "    \n"
        ;
        printf(text);
    }
    else if (!strncasecmp("set", *argv, strlen(*argv)))
    {
        helpOnSet(argc, argv);
    }
    else
    {
        writeHelp(*argv);
    }
}


int  writeHelp(const char *help)
{
    char const *HelpFile;
    
    if (NULL != (HelpFile = findConfig(help, "help")))
    {
        FILE *file;
        if (NULL != (file = fopen(HelpFile, "r")))
        {
            char buff[1024];
            
            while (NULL != fgets(buff, sizeof(buff), file))
            {
                printf("%s", buff);
            }
            
            return(0);
        }
        else
        {
            return(2);
        }
    }
    else
    {
        return(1);
    }
}

/************* END OF FILE **************************************************/

