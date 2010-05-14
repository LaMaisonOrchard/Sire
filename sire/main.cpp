/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2001, 2002, 2004, 2005  David W Orchard (davido@errol.org.uk)

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

EDIT 031 18-Apr-96 davido
2.0A : Support 'which' expansions.
       
EDIT 032 04-Dec-96 davido
2.0B : PC port
       
EDIT 033 20-Dec-96 davido
2.0C : PC port
       
EDIT 034 14-Jan-97 davido
2.0D : Support compressed jake files
       
EDIT 035 16-Jan-97 davido
2.0E : Support TOUCH rules.
       A touch rule may get a target uptodate but may not create it.
       So if the target file does not exist then it will not match
       a touch rule.
       
EDIT 036 16-Jan-97 davido
2.0F : Rework the pipe handling to work with spawn.
       
EDIT 037 29-Apr-97 davido
2.0G : Support CREATE rules.
       
EDIT 037 29-Apr-97 davido
2.0H : Hopfully, final PC port.
       
EDIT 038 05-Feb-98 davido
2.1A : Clean up the suffixes for PC and the shell variables.
       Introduce the automatic removal of intermidiates
       
EDIT 039 21-Apr-98 davido
2.1B : Switch from colon to pipe
       
EDIT 040 17-Jun-98 davido
2.1C : Support for sourcing jake environment files.
       
EDIT 041 13-Jan-98 davido
2.1D : Use the filr extensions to determine if a file is exectable on NT.
       Check the '.object' file (if present) to ensure that we are building
       in the right object tree.
       
EDIT 042 15-Mar-99 davido
2.1E : Add basic Perl support.
       
EDIT 043 20-Mar-99 davido
2.1F : Add basic set statement.
       
EDIT 044 07-May-99 davido
2.1G : Support for 'rebuild all' option.
       
EDIT 045 23-Jul-99 davido
2.2A : Competly rewrite the way that the preable and environment stuff
       works so that we can set up an environment without re-running
       ourselves.
       
EDIT 046 23-Jul-99 davido
2.2B : Support the '#' wildcard for matching names without a suffix (ie '.').
       
EDIT 047 23-Jul-99 davido
2.2C : Support the predef variables $(1-9).
       Support rules with no block (finshes with a semi colon
       
EDIT 048 14-Aug-99 davido
2.2D : Linux Fortran 77 support
       
EDIT 049 14-Aug-99 davido
2.2E : Added support for Windows suffixs in jakefiles
       
EDIT 050 24-Aug-99 davido
2.2F : Support preamble arguments
       
EDIT 051 07-Sep-99 davido
2.2G : Delay the evaluation of environment variables in blocks.
       Fixes to PREAMBLE for csh
       Added support for Python
       Enhance the SET statement
       Fix the wildcarding. '*' and '#' now matchs "".
       Added the GPL licensing
       
EDIT 051 05-May-00 davido
2.2H : Get the '-norun' to do something meaningful.
       
EDIT 052 11-May-00 davido
3.0A : Change the name to Sire to prevent confilct with the 'Jake Project'.
       
EDIT 053 18-May-00 davido
3.0B : Fix compile error
       Standardise the use of 'ROOTDIR'.
       
EDIT 054 18-May-00 davido@errol.org.uk
3.0C : Fix to the -remove option for CREATE rules.
       CREATE rules were being ignored when doing a -remove.
       Support script templates.

EDIT 054 18-Jul-00 davido@errol.org.uk
3.0D : Fix to the -remove option for CREATE rules.
       Add support for GCC, YACC, BYSON, LEX and FLEX
       Add in support for include files

EDIT 054 11-Dec-00 davido@errol.org.uk
3.0E : Support OPTIONAL SOURCE files.
       Support CLEANed and full environments for 'SET ENV'.
       Support additional help '-help <tag>'.
       The '=' charactor is not vaild in an identifier.
       Change the preprocess charactor from '!' to '%'. ('!' is now
       used in conditional statements.)

EDIT 055 18-Feb-01 david@the-hut.net
3.0F : Ignore the '-b' and '-m' switches. As for gmake.
       Add the '-C' (run in the given directory). As for gmake.
       Add the '-d' debug (-debug) mode. As for gmake.
       Add the '-n' don't execute the commands (-nobuild) mode. As for gmake.
       Add the '-s' silent (-quiet) mode. As for gmake.
       Add the '-w' report the current working directory on entry and exit. As for gmake.

EDIT 056 05-Apr-01 david@the-hut.net
3.0G : Support Environment variable settings form the command line

EDIT 057 13-Nov-01 david@the-hut.net
3.0H : Java support

EDIT 058 26-Jan-02 david@the-hut.net
3.0J : Add support for building/running under CygWin32 (Cygnus Unix on Windows).
       Set MACHINE=CYGWIN

EDIT 058 26-Jan-02 david@the-hut.net
3.0K : Add support the USER modifier. This modifer indicates that files matching
       this rule are user editable and should not be deleted. 
       
EDIT 059 06-Aug-02 davido@errol.org.uk
3.0K : Add in modifier USER to specify files not to be deleted.

EDIT 060 27-Aug-02 davido@errol.org.uk
3.0L : Add support for preprocessing

EDIT 061 10-Sep-02 davido@errol.org.uk
3.0M : Enhance the built in help.

EDIT 062 13-Sep-02 davido@errol.org.uk
3.0N : Fix to '-C' switch, inc argv

EDIT 063 17-Nov-02 david@the-hut.net
3.0P: Support TOK_CALL as a stand alone statement to provide a simple form
      of preprocesing of the relations file.
      Support nested variable expansion.

EDIT 064 14-Oct-02 david@the-hut.net
3.1A: Clean up the code for variable resolution.
      Complete rework of the 'SET' command.
      Look for files in the config directories

EDIT 065 05-Dec-02 david@the-hut.net
3.1B: Get the correct date file for rules with no build script.

EDIT 066 16-Dec-02 david@the-hut.net
3.1C: Minor fixes for RedHat Linux/Solaris.

EDIT 067 20-Dec-02 david@the-hut.net
3.1D: Fix the perl support (get it to work).
      Moved the support for perl and python out to the resourse scripts.
      These are no longer built-in.

EDIT 068 06-Jan-03 david@the-hut.net
3.1E: Fixed for SOLARIS crashes.

EDIT 069 07-Jan-03 david@the-hut.net
3.1F: Moved the support for yacc out to the resourse scripts.
      Ensure OSTYPE is always defined.
      Simplfy the defining of MACHINE and PLATFORM

EDIT 070 23-Jun-03 david@the-hut.net
3.1G: Fixed a '-remove' bug for rules with no block.

EDIT 071 18-Oct-03 david@the-hut.net
3.2A: Add support for sub-rules. A sub-rule is a rule whos script simply
      runs sire with different arguments.

EDIT 072 17-Dec-04 david@the-hut.net
4.0A: Convert to a KDevelop project

EDIT 073 07-Jan-05 david@the-hut.net
4.0B: Wrap the 'sirenv' application into 'sire' (via the '-env' switch).
      Revise the standard Environment settings. Define TEMP tp be the full
      path to the 'tempory files' directory for all platforms.
      Update the built in help.
      Add some make type build options.

EDIT 074 11-Jan-05 david@the-hut.net
4.0C: Ensure that SHELL is an absolute path and executable.
*****************************************************************************/

#define MARK   4
#define ISSUE  0
#define LETTER "C"

/************* INCLUDE ******************************************************/

#include "build_v.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if ! ( defined(NEC_RISC) || defined(INTEL_NT) || defined (INTEL_9X) )
#include <sys/utsname.h>
#endif
#include "text.h"
#include "redundant.h"
#include "jake.h"
#include "msh.h"
#include "state.h"
#include "machine.h"
#include "sys.h"
#include "env.h"
#include "help.h"

// List of arguments to be passed to config sub processes // 049
const char **globalArgv = NULL;
static const char *argList[2] = {"", NULL};


char const *findExtension(char const *file, char const **ext);

static void sirenv(char **arge);

/************* FUNCTIONS ****************************************************/


int main (int argc, const char**argv, char **envp)
{
#if defined (INTEL_NT) || defined (INTEL_9X)
static char buff[128];
#endif

   //int         doRun    = 1; // 051
   int         trackWorkingDirectory = 0;
   int         preamble     = 1;
   Tjake      *input        = NULL;
   char const *name         = "relations";
   int         explicitName = FALSE;
   Tlist       target;                  // DEFAULT_TARGET
   char const *app          = argv[0];  // Application name and path
   
   bool        run_msh = FALSE;

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
       Tenv::appSet("PWD", buffer);
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
         else if (!strcmp(swt, "w"))
         {
            trackWorkingDirectory = 1;
         }
         else if (!strcmp(swt, "msh"))
         {
            run_msh = TRUE;
         }
         else if (!strcmp(swt, "talk"))
         {
            talkS(1);
         }
         else if (!strcmp(swt, "debug") || !strcmp(swt, "d"))
         {
            debugS(1);
         }
         else if (!strcmp(swt, "all"))
         {
            allS(1);
         }
         //else if (!strcmp(swt, "one"))
         //{
         //   oneS(1);
         //}
         else if (!strcmp(swt, "remove"))
         {
            removeS(1);
         }
         else if (!strcmp(swt, "partial"))
         {
            Tenv::appSet("SIRE_PARTIAL", "t");
         }
         else if (!strcmp(swt, "nopreamble"))
         {
            preamble = 0;
         }
         else if (!strcmp(swt, "nopp"))
         {
            TfileDat::preprocess(0);
         }
         else if (!strcmp(swt, "m4"))
         {
            TfileDat::preprocess(1);
         }
         else if (!strcmp(swt, "force"))
         {
            Trule::ignoreErr(1);
         }
         else if (!strcmp(swt, "k"))
         {
            Trule::complete(1);
         }
         else if (!strcmp(swt, "S"))
         {
            Trule::complete(0);
         }
         else if (!strcmp(swt, "t"))
         {
            Trule::touchFiles(1);
         }
         else if (!strcmp(swt, "C"))
         {
            // Change the currect working directory

            if ((NULL == argv[1]) || setDir(argv[1]))
            {
                printf("Invalid directory for the '-C' option\n");
                exit(0);
            }
            argv++;
         }
         else if (!strcmp(swt, "env"))
         {
            sirenv(envp);
            exit(0);
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
            printf("sire    %d.%d%s - [%s] %s\n", MARK, ISSUE, LETTER, SIRE_HOST, SIRE_DATE);
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
         else if (!strcmp(swt, "fun"))
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
            ;
       
            printf(text);
            exit(0);
         }
         else if (!strcmp(swt, "nobuild") || !strcmp(swt, "norun") || !strcmp(swt, "n"))
         {
            // Do not attempt to build the target
            doRunS(0); // 051
         }
         else if (!strcmp(swt, "params"))   // 049
         {
            // Store the arguments for the preamble
            argv[0] = globalArgv[0];
            globalArgv = argv;
            
            // No more arguments - We have just used them all up
            break;
         }
         else if (!strcmp(swt, "pingu"))
         {
            printf("Alison is mad about penguins\n");
            exit(0);
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
         else if (!strcmp(swt, "b") || !strcmp(swt, "m"))
         {
            /* Ignore this switch */
         }
         else if (swt[0] == 'D')
         {
            /* Set the Environment variable -D<var>=<Value> */
            
            char *name  = (char*)swt+1;   /* Naughty - But I know its safe */
            char *value = strrchr(swt, '=');
            
            if (NULL == value)
            {
                /* No value given */
            	Tenv::appSet(name, "");
            }
            else
            {
                /* Replace the '=' */
                *(value++) = '\0';
            	Tenv::appSet(name, value);
            }
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
   
    static const char *extList[] =
    {
       "",
       ".Z",
       ".gz",
       ".gzip",
       NULL
    };
   
    // Are we running an msh script rather than a relations script
    if (run_msh)
    {
        char const *extName;
       
        // Msh script
        if (!explicitName)
        {
            printD("Using script [%s]\n", "<stdin>");
            Tmsh input(stdin);
        }
        else if (NULL != (extName = findExtension(name, extList)))
        {
            printD("Using script [%s]\n", extName);
            Tmsh input(extName);
        }
        else
        {
            // Explicitly named file is not found
          
            printE("Relations script '%s' missing\n", name);
        }
    }
    else
    {
       // Relations script
       
       // Check the platform
       FILE *platFp;
       if (NULL != (platFp = fopen(".object", "r")))
       {
            static char platform[64];
            fscanf(platFp, "%s", platform);
            if (strcmp(platform, Tenv::appGet("MACHINE")))
            {
                // Conflicting platforms
                printE("This build area is for [%s] not [%s]\n",
                       platform, Tenv::appGet("MACHINE"));
                exit(1);
            }
            fclose(platFp);
       }
       else
       {
            // No object type specified - assume it is ok
       }
       
       // Specify the valid extension for the jake file
       
       char *p;
       char const *extName;

       if (trackWorkingDirectory)
       {
            char path[PATH_MAX];
            printf("Entering [%s]\n", getcwd(path, PATH_MAX));
       }
       
       // The first argument if the input file
       if (NULL == name)
       {
          printD("Using script [%s]\n", "<stdin>");
          input = new Tjake(stdin);
       }
       else if (NULL != (extName = findExtension(name, extList)))
       {
          printD("Using script [%s]\n", extName);
          input = new Tjake(extName);
       }
       else if (explicitName) // 064
       {
          // Explicitly named file is not found
          
          printE("Relations script '%s' missing\n", name);
       }
       else if (NULL != (extName = findExtension("jakefile", extList))) // 052
       {
          // Try the old name (backwards compatable) // 052
          printD("Using script [%s]\n", extName);
          input = new Tjake(extName);
       }
       else if (NULL != (extName = findConfig("relations", NULL))) // 064
       {
          // Try the config directories // 064
          printD("Using script [%s]\n", extName);
          input = new Tjake(extName);
       }
       else if ('/' != name[0])       // 020 //
       {
          char const *home = findExec(globalArgv[0]);
          char *buff  = new char[strlen(home) + strlen(name) +1];
          char *buff1 = new char[strlen(home) + strlen(name) +1];
          
          // Look where the excutable is
          if (NULL == (p = strrchr(buff, '/')))
             strcpy(buff, ".");
          else
             *p = '\0';
          
          strcpy(buff, home);
          if (NULL == (p = strrchr(buff, '/')))
             strcpy(buff, ".");
          else
             *p = '\0';
             
          sprintf(buff1,"%s/%s",buff, name);
          
          if (NULL != (extName = findExtension(buff1, extList)))
          {
             printD("Using script [%s]\n", extName);
             input = new Tjake(extName);
          }
          else
          {
             // Try the old name (backwards compatable) // 052
             
             sprintf(buff1,"%s/jakefile",buff);
             
             if (NULL != (extName = findExtension(buff1, extList)))
             {
                printD("Using script [%s]\n", extName);
                input = new Tjake(extName);
             }
             else if (NULL != (p = strrchr(buff,'/')))
             {
                // Look one up from here
                *p = '\0';
                sprintf(buff1,"%s/%s",buff, name);
     
                if (NULL != (extName = findExtension(buff1, extList)))
                {
                   printD("Using script [%s]\n", extName);
                   input = new Tjake(extName);
                }
                else
                {
                   // Try the old name (backwards compatable) // 052
     
                   sprintf(buff1,"%s/jakefile",buff);
     
                   if (NULL != (extName = findExtension(buff1, extList)))
                   {
                      printD("Using script [%s]\n", extName);
                      input = new Tjake(extName);
                   }
                   else
                   {
                      printE("Default relations script '%s' missing\n", buff1);
                   }
                }
             }
             else
             {
                printE("Default relations script '%s' missing\n", buff1);
             }
          }
          
          delete buff;
          delete buff1;
       }

       // Check that the shell is exectable
       if (!isExec(Tenv::appGet("SHELL")))
       {
           printE("The shell [%s] is not executable\n", Tenv::appGet("SHELL"));
       }
          
       if (0 != messageCountE())        // 003
       {
          if (NULL == name)
             printE("Error parsing the standard input\n");
          else
             printE("Error parsing the %s\n",name);
             
          exit(1);
       }
       else
       {
          //if (doRun && preamble) // 051
          if (preamble)
          {
             input->preamble();
          }
       
          //if (doRun) // 051
          {
             int state = 0;
             if (0 == target.len())
             {
	            if (removeS())
                    state = (NO_FILE >= buildable(DEFAULT_TARGET));
                else
                    state = (NO_FILE >= date(DEFAULT_TARGET));
             }
             else if (removeS())
             {
                for (int i = 0; (i < target.len()); i++)
                   state = (state || (NO_FILE >= buildable((char const*)(*(Tstr*)target[i]))));
             }
             else
             {
                for (int i = 0; (i < target.len()); i++)
                   state = (state || (NO_FILE >= date((char const*)(*(Tstr*)target[i]))));
             }
             
             if (state)
             {
                printE("Failed\n");
                exit(1);
             }
          }

          input->postamble();
       }

       if (trackWorkingDirectory)
       {
            char path[PATH_MAX];
            printf("Leaving [%s]\n", getcwd(path, PATH_MAX));
       }
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

    if (NULL == (var = Tenv::appGet("SIRE_PARTIAL")))
    {
        Tenv::appSet("SIRE_PARTIAL", "f");
    }

    if (NULL == Tenv::appGet("JAVAC"))
    {
        Tenv::appSet("JAVAC", getJava());
    }

    if (NULL == Tenv::appGet("MACHINE"))
    {
       Tenv::appSet("MACHINE", SIRE_MACHINE);
    }

    if (NULL == Tenv::appGet("PLATFORM"))
    {
       Tenv::appSet("PLATFORM", SIRE_PLATFORM);
    }

    if (NULL == Tenv::appGet("OSTYPE"))
    {
       Tenv::appSet("OSTYPE", SIRE_OSTYPE);
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

