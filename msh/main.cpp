/****************************************************************************

    The Jsh mini shell 'msh'.
    Copyright (C) 2008  David W Orchard (davido@errol.org.uk)

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

   File   : main.cpp
   Applic : NONE
   Desc   : A description
   
AMENDMENTS

EDIT 001 26-Mar-08 davido
1.0A : Initial version based on sire.
*****************************************************************************/

#define MARK   1
#define ISSUE  0
#define LETTER "A"

/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "text.h"
#include "redundant.h"
#include "jake.h"
#include "state.h"
#include "machine.h"
#include "sys.h"
#include "env.h"
#include "help.h"

// List of arguments to be passed to config sub processes // 049
char **globalArgv = NULL;
static char *argList[2] = {"", NULL};


char const *findExtension(char const *file, char const **ext);

static void sirenv(char **arge);

/************* FUNCTIONS ****************************************************/


int main (int argc, char**argv, char **envp)
{
#if defined (INTEL_NT) || defined (INTEL_9X)
static char buff[128];
#endif

   int         preamble     = 1;
   Tjake      *input        = NULL;
   char const *name         = NULL;
   int         explicitName = FALSE;
   Tlist       target;                  // DEFAULT_TARGET
   char const *app          = argv[0];  // Application name and path

   char buffer[1024];
   
   doRunS(1);  // 051
   
   Tenv::setApp((char const * const *)envp);
   argList[0] = argv[0];  // 049
   globalArgv = argList;  // 049
   
   setDon(0);

   // Get the current working directory

   if (NULL != getcwd(buffer, sizeof(buffer)))
   {
       Tenv::appSet("ORI_PWD", buffer);
   }
      
   for (argv++; (NULL != *argv); argv++)
   {
      if ('-' == **argv)
      {
         char const *swt = (*argv)+1;
         
         if (!strcmp(swt, "quiet") || !strcmp(swt, "s"))
         {
            quietS(1);
         }
         else if (!strcmp(swt, "talk"))
         {
            talkS(1);
         }
         else if (!strcmp(swt, "debug") || !strcmp(swt, "d"))
         {
            debugS(1);
         }
         else if (!strcmp(swt, "nopp"))
         {
            TfileDat::preprocess(0);
         }
         else if (!strcmp(swt, "m4"))
         {
            TfileDat::preprocess(1);
         }
         else if (!strcmp(swt, "C"))
         {
            // Change the currect working directory

            if ((NULL == argv[1]) || chdir(argv[1]))
            {
                printf("Invalid directory for the '-C' option\n");
                exit(0);
            }
            argv++;
    
            /* Set the current directory */
            
            if (NULL != getcwd(buffer, sizeof(buffer)))
            {
                Tenv::appSet("PWD", buffer);
            }
         }
         else if (!strcmp(swt, "h"))
         {
            shortHelp(argc, argv);
            exit(0);
         }
         else if (!strcmp(swt, "help")  ||
                  !strcmp(swt, "-help") ||
                  !strcmp(swt, "?"))
         {
            argv++;
            help(argc, argv);
            exit(0);
         }
         else if (!strcmp(swt, "i")       ||
                  !strcmp(swt, "version") ||
                  !strcmp(swt, "-version") ||
                  !strcmp(swt, "-v"))
         {
            printf("msh    %d.%d%s - [%s] %s\n", MARK, ISSUE, LETTER);
            exit(0);
         }
         else if (!strcmp(swt, "auth") || !strcmp(swt, "v"))
         {
            printf("Sire version %d.%d%s, Copyright (C) 1999,2000,2001,2002,2003,2004 David Orchard\n", MARK, ISSUE, LETTER);
            printf("Sire comes with ABSOLUTELY NO WARRANTY. This is free\n");
            printf("software, and you are welcome to redistribute it\n");
            printf("under certain conditions. See the HTML documentation\n");
            printf("for further details.\n");
            printf("David Orchard - davido@errol.org.uk\n");
            exit(0);
         }
         else if (!strcmp(swt, "params"))   // 049
         {
            // Store the arguments for the preamble
            argv[0] = globalArgv[0];
            globalArgv = argv;
            
            // No more arguments - We have just used them all up
            break;
         }
         else if (!strcmp(swt, "f"))
         {
            // Specify the jakefile
            if (NULL == *(argv+1))
            {
               printT("Missing file for the '-f' switch\n");
               name = NULL;
            }
            else if ('-' == **(argv+1))
            {
               name = NULL;
               argv++;
            }
            else
            {
               name = *(++argv);
            }
            
            explicitName = TRUE;
         }
         else if (swt[0] == 'D')
         {
            /* Set the Environment variable -D<var>=<Value> */
            
            char *name  = (char*)swt+1;   /* Naughty - But I know its safe */
            char *value = strrchr(swt, '=');
            
            if (NULL == value)
            {
                /* No value given */
                value = "";
            }
            else
            {
                /* Replace the '=' */
                *(value++) = '\0';
            }
            
            Tenv::appSet(name, value);
         }
         else
         {
            printE("BAD switch %s (ignored)\n", swt);
         }
      }
      else
      {
         target.add(new Tstr(*argv));
      }
   }

   setDefaultEnv(Tenv::appGet("ORI_PWD"), app);
   
   // The first argument if the input file
   if (NULL == name)
   {
      printD("Using script [%s]\n", "<stdin>");
      input = new Tjake(stdin);
   }
   else if (explicitName)
   {
      // Explicitly named file is not found
      
      printE("Relations script '%s' missing\n", name);
   }
   else
   {
      input = new Tjake(name);
   }
      
   if (0 != messageCountE())        // 003
   {
      if (NULL == name)
         printE("Error parsing the standard input\n");
      else
         printE("Error parsing the %s\n",name);
         
      exit(1);
   }
   
   return(0);
}



void setDefaultEnv(char const *cwd, char const *app)
{
    char const *var;

   if (NULL == Tenv::appGet("ROOTDIR"))
   {
#if defined (INTEL_NT) || defined (INTEL_9X)  /* 053 */
       Tenv::appSet("ROOTDIR", "C:");
#else
       Tenv::appSet("ROOTDIR", "");
#endif
   }

   if (NULL == Tenv::appGet(".EXE"))
   {
#if defined(INTEL_NT) || defined (INTEL_9X)
      Tenv::appSet(".EXE", ".exe");
#else
      Tenv::appSet(".EXE", "");
#endif
   }

   if (NULL == Tenv::appGet(".SH"))
   {
#if defined(INTEL_NT) || defined (INTEL_9X)
      Tenv::appSet(".SH", ".sh");
#else
      Tenv::appSet(".SH", "");
#endif
   }

   if (NULL == Tenv::appGet(".KSH"))
   {
#if defined(INTEL_NT) || defined (INTEL_9X)
      Tenv::appSet(".KSH", ".ksh");
#else
      Tenv::appSet(".KSH", "");
#endif
   }

   if (NULL == Tenv::appGet("FSEP"))
   {
#if defined(INTEL_NT) || defined (INTEL_9X)
      Tenv::appSet("FSEP", "\\");
#else
      Tenv::appSet("FSEP", "/");
#endif
   }

   if (NULL == Tenv::appGet("PSEP"))
   {
#if defined(INTEL_NT) || defined (INTEL_9X)
      Tenv::appSet("PSEP", ";");
#else
      Tenv::appSet("PSEP", ":");
#endif
   }

    if (NULL != (var = Tenv::appGet("TALK_STATE")))
    {
        talkS('t' == *var);
    }
    else
    {
        Tenv::appSet("TALK_STATE", "t");
    }

    if (NULL != (var = Tenv::appGet("DEBUG_STATE")))
    {
        debugS('t' == *var);
    }
    else
    {
        Tenv::appSet("DEBUG_STATE", "f");
    }

    if (NULL != (var = Tenv::appGet("REMOVE_STATE")))
    {
        removeS('t' == *var);
    }
    else
    {
        Tenv::appSet("REMOVE_STATE", "f");
    }

    if (NULL != (var = Tenv::appGet("ONE_STATE")))
    {
       oneS('t' == *var);
    }
    else
    {
        Tenv::appSet("ONE_STATE", "f");
    }

    if (NULL == Tenv::appGet("JAVAC"))
    {
        Tenv::appSet("JAVAC", getJava());
    }
    
    // Set the GROUP and USER  variables is there is such a concept

    if (NULL == Tenv::appGet("USER"))        // 064
    {
        char const *p;
        if (NULL != (p = getUser()))
        {
            Tenv::appSet("USER", p);
        }
    }

    if (NULL == Tenv::appGet("GROUP"))        // 064
    {
        char const *p;
        if (NULL != (p = getGroup()))
        {
            Tenv::appSet("GROUP", p);
        }
    }

    if (NULL == Tenv::appGet("CCP"))
    {
       Tenv::appSet("CCP", getCCP());
    }

    if (NULL == Tenv::appGet("CC"))
    {
       Tenv::appSet("CC", getCC());
    }

    Tenv::appSet("SH",  getSH());
    Tenv::appSet("CSH", getCSH());

    if (NULL == Tenv::appGet("SHELL"))
    {
        Tenv::appSet("SHELL", Tenv::appGet("SH"));
    }

    if ((NULL == Tenv::appGet("SIRE")) && (app != NULL))
    {
       Tenv::appSet("SIRE", findExec(app, cwd));
    }

    if ((NULL == Tenv::appGet("TEMP")) && (app != NULL))
    {
       Tenv::appSet("TEMP", getTmpDir());
    }
}


// Find the file with one of the posible extentions
char const *findExtension(char const *file, const char **ext)
{
   static char buff[1024];
   int len = 0;
    
   buff[0] = '\0';
    
   if (NULL != file)
   {
      strcpy(buff, file);
      len = strlen(file);
      
      while (NULL != *ext)
      {
         strcpy(buff + len, *ext);
         
         if (!access(buff, F_OK))
         {
            break;
         }
         
         ext++;
      }
   }
   
   if (NULL == *ext)
      return(NULL);
   else
      return(buff);
}


static void sirenv(char **arge)
{
    char *p;
    
    while (NULL != *arge)
    {
        /* Code the '\n' and '\r' charactors */
        for (p = *arge; ('\0' != *p); p++)
        {
            if ('\n' == *p) *p = '\1';
            if ('\r' == *p) *p = '\2';
        }

        /* Output it */
        printf("%s\n", *(arge++));
    }
}



/************* END OF FILE **************************************************/

