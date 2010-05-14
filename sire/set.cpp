/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2001, 2002, 2003  David W Orchard (davido@errol.org.uk)

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

AMENDMENTS

EDIT 001 18-Jun-00  davido@errol.org.uk
Add support for GCC, YACC, BYSON, LEX and FLEX

EDIT 002 05-Apr-01  davido@errol.org.uk
Support cross compilation with GCC

EDIT 003 27-Aug-02  davido@errol.org.uk
QT Support

EDIT 004 07-Nov-02  davido@errol.org.uk
Support 'SET' scripts.

EDIT 005 15-Nov-02  davido@errol.org
Move QT support out to the configuration scripts

EDIT 006 07-Jan-03  davido@errol.org
Move YAC, BYSON, LEX and FLEX support out to the configuration script YACC.src

EDIT 007 23-Jun-03  davido@errol.org
Remove redundant variable.

*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "token.h"
#include "set.h"
#include "sys.h"
#include "env.h"
#include "jake.h"
#include "help.h"
#include "machine.h"



void helpOnSet(int &argc, const char**&argv)
{
    char const *text =
        "SET <arg> { <arg> } ;\n"
        "    The SET statement is used to configure various aspects of\n"
        "    Sire's enviromnent. These are many differnt SET statements.\n"
        "    These are documented individually as follows.\n"
        "    \n"
        "SET QUIET;\n"
        "    Set Sire into quiet mode. In this mode the build statements\n"
        "    are not echoed to the display by default.\n"
        "    \n"
        "SET TALK;\n"
        "    Set Sire into talk mode. This is the converse of quiet mode.\n"
        "    In this mode build statements are echoed to the display by default.\n"
        "    \n"
        "SET DEBUG;\n"
        "    Set Sire into debug mode. Debug mode provides tracing information\n"
        "    to enable the debugging of relations files which are not working\n"
        "    as expected.\n"
        "    \n"
        "SET ENV [ CLEAN ] <script> ;\n"
        "    The 'SET ENV' statement sources the given script to configure\n"
        "    the environment (set up the environment variables). If the 'clean'\n"
        "    option is given then the environment will be stripped down to a\n"
        "    minimum set of environment variables before running the script.\n"
        "    This can be used to ensure a more controlled environment.\n"
        "    \n"
        "    The preserved environment variables are :-\n"
        "        MACHINE       - The machine type\n"
        "        SHELL         - The shell used for running scripts\n"
        "        PWD           - The current working directory\n"
        "        USER          - The users name\n"
        "        TALK_STATE    - A state flag for Sire\n"
        "        DEBUG_STATE   - A state flag for Sire\n"
        "        REMOVE_STATE  - A state flag for Sire\n"
        "        ONE_STATE     - A state flag for Sire\n"
        "        SIRE_PARTIAL  - A state flag for Sire\n"
        "    \n"
    ;
    printf(text);
    
    writeHelp("set");
}



int doSet(Tjake *jake, int active, Ttoken **list, int len)   // 004
{
    int clean = FALSE;
    char const *SourceText;

    if ((NULL != list) && (NULL != list[0]) && !strcmp(list[0]->text(), "CLEAN"))
    {
        clean = TRUE;
        list++;
    }

    if ((NULL != list) && (NULL != list[0]))
    {
        // Process the set
          
        if (!strcmp(list[0]->text(), "ENV"))
        {
            // Setup the initial environment
            
            if (NULL != list[1])
            {
                cleanEnv(list[1]->text(), clean);
            }
            else
            {
                printE ("%s - Missing environment file\n", list[0]->posn());
                return (1);
            }
        }
        
        // look for a script
        
        else if (NULL != (SourceText = findConfig(list[0]->text(), "src")))  // 004
        {
            //// Template shell script definition //// 
            
            // Set up the arguments
            
		    // Source the file.
            
            printD("%s SET file <%s>\n", list[0]->posn(), SourceText);
		
		    jake->parseFile(SourceText, active, list, len);
            
            // Clear the srguments
        }
        
        else
        {
            printE ("%s - Unknown SET statement '%s'\n", list[0]->posn(), list[0]->text());
            return (1);
        }
    }
    
    return(0);
}
