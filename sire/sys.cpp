/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2002, 2003, 2004, 2005  David W Orchard (davido@errol.org.uk)

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

   File  : sys.cpp
   Class : NONE
   Desc  : A description
   
AMENDMENTS

EDIT 001 18-Apr-96 davido
Support for 'which' expansions.

EDIT 002 23-Jul-99 davido
Re-write source() to set the environment without rerunning jake.

EDIT 003 05-May-00 davido
Get the python preamble to work correctly. NOTE the preable section
must start in column 1 (0 for C programmers).

EDIT 004 11-May-00 davido
Name change from jake to sire.

EDIT 005 27-Jun-00 davido
Support script templates

EDIT 006 26-Jan-02 david@the-hut.net
Add support for building/running under CygWin32 (Cygnus Unix on Windows).
Set MACHINE=CYGWIN

EDIT 007 07-Nov-02 david@the-hut.net
Generalise the routine findTempl() for locating configuration files.

EDIT 008 14-Nov-02 david@the-hut.net
Locate the group if there is one

EDIT 009 20-Dec-02 david@the-hut.net
Fix the perl support (get it to work).
Moved the support for perl and python out to the resourse scripts.
These are no longer built-in.

EDIT 010 06-Jan-03 david@the-hut.net
Don't use getenv() directly.

EDIT 011 19-Dec-04 david@the-hut.net
Wrap the 'sirenv' application into 'sire' (via the '-env' switch). So 'sirenv'
becomes 'sire -env'.

EDIT 012 07-Jan-05 david@the-hut.net
Add a routine to 'touch' files.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "str.h"
#include "strbuf.h"
#include "sys.h"
#include "nametarget.h"
#include "matchtarget.h"
#include "jake.h"
#include "state.h"
#include "pipe.h"
#include "env.h"
#include "msh.h"


#define CSH_NAME       "csh"
#define PERL_NAME      "perl"
#define SH_NAME        "sh"
#define KSH_NAME       "ksh"
#define RKSH_NAME      "rksh"
#define BASH_NAME      "bash"
#define PYTHON_NAME    "python"
#define INTERNAL_SHELL "msh"


static int T = 1;
static int D = 1;
static int E = 1;


static int countT = 0;
static int countD = 0;
static int countE = 0;

static char const *SIRE_ENV_TAG = "####SIRE###TAG####";

/************* FUNCTIONS ****************************************************/




int setTon(int on) {int t = T; T = on; return(t);}
int setDon(int on) {int t = D; D = on; return(t);}
int setEon(int on) {int t = E; E = on; return(t);}




void messageCountT(int i) {countT = i;}
void messageCountD(int i) {countD = i;}
void messageCountE(int i) {countE = i;}

int  messageCountT()      {return(countT);}
int  messageCountD()      {return(countD);}
int  messageCountE()      {return(countE);}



/*********************** Output *******************************/

void vprintT(char const *format, va_list ap)
{
   if (T)
   {
      countT++;
      vfprintf(stdout, format, ap);
   }
}



void printT(char const *format ...)
{
   va_list ap;
   va_start(ap, format);
    
   vprintT(format, ap);
    
   va_end(ap);
}



void vprintD(char const *format, va_list ap)
{
   if (D)
   {
      countD++;
      vfprintf(stdout, format, ap);
   }
}



void printD(char const *format ...)
{
   va_list ap;
   va_start(ap, format);
    
   vprintD(format, ap);
      
   va_end(ap);
}



void vprintE(char const *format, va_list ap)
{
   if (E)
   {
      countE++;
      vfprintf(stderr, format, ap);
   }
}



void printE(char const *format ...)
{
   va_list ap;
   va_start(ap, format);
    
   vprintE(format, ap);
    
   va_end(ap);
}




static struct stat statBuff[10];

int isDir(char const *name)
{
    if (0 != stat((char*)name, statBuff)) return(0);
    return (S_ISDIR (statBuff[0].st_mode));
}

int isFile(char const *name)
{
    if (0 != stat((char*)name, statBuff)) return(0);
    return (!S_ISDIR (statBuff[0].st_mode));
}

int doesExist(char const *name)
{
    return (0 == stat((char*)name, statBuff));
}

int setDir(char const *name)
{
    char buffer[1024];
   
    int rtn = chdir(name);
    
    if (NULL != getcwd(buffer, sizeof(buffer)))
    {
        Tenv::appSet("PWD", buffer);
    }
    
    return rtn;
}

char const *getDir()
{
    static char buffer[1024];
    
    if (NULL != getcwd(buffer, sizeof(buffer)))
    {
        return buffer;
    }
    else
    {
        return "<unknown>";
    }
}




int touch(char const *name)
{
    // Touch the file

    // Does the file exist
    if (0 != stat((char*)name, statBuff)) return(0);

    // Open the file
    FILE *fp;
    if (NULL != (fp = fopen(name, "w"))) fclose(fp);
    return (0);
}




int timeStamp(char const *name)
{
   if (NULL == name)
   {
      // Current time
      time_t tloc;
      time(&tloc);
      return((int)tloc);
   }
   else if (!strcmp(DEFAULT_TARGET, name))
   {
      // Default target
      return(OLD_FILE);
   }
   else if (access((char*)name, F_OK))
   {
      return(NO_FILE);
   }
   else
   {
      if (0 == stat(name, statBuff))
      {
         return((int)statBuff[0].st_mtime);
      }
      else
      {
         return(NO_FILE);
      }
   }
}



FILE *readPipe(char const* command)
{ 
   Tpipe *pipe;
   
   pipe = new Tpipe(command);
   
   return(pipe->file());
}




void closePipe(FILE *fp)
{
   Tpipe *pipe;
   
   if (NULL != (pipe = Tpipe::find(fp)))
   {
      delete pipe;
   }
}



int execute  (char const *comm)
{
   return(execute(comm, Tenv::appGetEnvp()));
}


#if defined (NEC_RISC)

int strcasecmp(char const *a, char const *b)
{
    if ((NULL == a) || (NULL == b))
    {
        return (a-b);
    }
    
    int state;
    while ((0 == (state = (toupper(*a) - toupper(*b)))) &&
           ('\0' != *(a++)) && ('\0' != *(b++)))
    {
    }
    
    return (state);
}

#endif



int execute  (char const *comm, char **envp)
{
   int         status = 0;
   char const *(argv[1024]);
   char const *script;
   char const *shell = Tenv::appGet("SHELL");
   
   if (NULL == comm) return(0);
   
   if ('\0' == *comm)
   {
      status = 0;
   }
   else if ((shell == NULL) || !strcmp(shell, INTERNAL_SHELL))
   {
   	  // Use the internal shell
      int rtn;
      int i;
      
      // Pass on the target and dependents as arguments
      Tstr target = Tenv::appGet("TARGET");
      Tstr deps   = Tenv::appGet("DEPS");
         
      char *d = (char*)malloc(strlen((char const*)deps)+1);
      strcpy(d, (char const*)deps);
            
      char *p = d;
      char *q = p;
      
      argv[0] = INTERNAL_SHELL;
            
      argv[1] = (char const*)target;
         
      while (isspace(*p)) p++;
      for (i = 2; (i < 1023) && ('\0' != *p); i++)
      {
         q = p;
         while ((!isspace(*p)) && ('\0' != *p)) p++;
         *(p++) = '\0';
         while (isspace(*p)) p++;
            
         argv[i] = q;
      }
         
      argv[i] = NULL;
            
      rtn = msh(comm, argv, envp);
      
      // Free the space
      free(d);
      
      if (0 != rtn) printT("Build failed [%d]\n", rtn);
      status = rtn;
   }
   else if (NULL != (script = makeScript(comm)))
   {
   	  // Use the internal shell
      int rtn;
      int i;
      
      // Pass on the target and dependents as arguments
      Tstr target = Tenv::appGet("TARGET");
      Tstr deps   = Tenv::appGet("DEPS");
         
      char *d = (char*)malloc(strlen((char const*)deps)+1);
      strcpy(d, (char const*)deps);
            
      char *p = d;
      char *q = p;
      
      argv[0] = INTERNAL_SHELL;
            
      argv[1] = (char const*)target;
         
      while (isspace(*p)) p++;
      for (i = 2; (i < 1023) && ('\0' != *p); i++)
      {
         q = p;
         while ((!isspace(*p)) && ('\0' != *p)) p++;
         *(p++) = '\0';
         while (isspace(*p)) p++;
            
         argv[i] = q;
      }
         
      argv[i] = NULL;
            
      rtn = MYspawnve(MY_WAIT, (char*)script, argv, envp);
      
      // Free the space
      free(d);
      
      unlink((char*)script);
      
      if (0 != rtn) printT("Build failed [%d]\n", rtn);
      status = rtn;
   }
   
   return(status);
}

// Implement my own platform indepent version of spawnve()
// On UNIX this is a fork/execve combination
// On NT this is a generalised spawnve implementation
// ALL sub-processes are created using MYspawnve().

int MYspawnve(int wait, char *script, char const **argv, char **envp)
{
    RTN_TYPE rtn = 0;
    int      pid = 0;
    
#if defined(INTEL_NT) || defined(INTEL_9X)
    static char *shell = NULL;
    
    if (NULL == shell)
    {
       //char const *local = findExec("sh.exe");
       char const *local = Tenv::appGet("SHELL");
       shell = new char [strlen(local) +1];
       strcpy(shell, local);
    }
    
    char  *(MYargv[1024]);
    char **p;
    char **q;
    
    MYargv[0] = shell;
    MYargv[1] = script;
    
    for (p = argv +1, q = MYargv +2; (NULL != *p); p++, q++)
       *q = *p;
       
    *q = NULL;

    if (MY_WAIT == wait)
       rtn = spawnve(_P_WAIT, shell, (const char* const*)MYargv, (const char* const*)envp);
    else 
       pid = spawnve(_P_NOWAIT, shell, (const char* const*)MYargv, (const char* const*)envp);
    
#else

#if defined(CYGWIN)  /* 006 */
    static char *shell = NULL;

    if (NULL == shell)
    {
       char const *local = Tenv::appGet("SHELL");
       shell = new char [strlen(local) +1];
       strcpy(shell, local);
    }

    char  *(MYargv[1024]);
    char **p;
    char **q;

    MYargv[0] = shell;
    MYargv[1] = script;

    for (p = argv +1, q = MYargv +2; (NULL != *p); p++, q++)
       *q = *p;

    *q = NULL;
    argv = MYargv;
    script = shell;
#endif
      
   if (0 == (pid = fork()))
   {
      // Child
      
      execve(script, (char**)argv, envp);
      printE("Executeable [%s] not found or not executable\n", script);
      exit(-1);
   }
   else if (-1 < pid)
   {
      // Parent
      
      if (MY_WAIT == wait)
      {
         waitpid(pid, &rtn, 0);
      }
   }
   else
   {
      // Fork fail
      printT("Fork failed [-1]\n");
      return (-1);
   }
   
#endif
      
    if (MY_WAIT == wait)
    {
        return(RTN_VALUE(rtn));
    }
    else
    {
        return(pid);
    }
}


   
void MYtmpFile(char *name, char const *base, char const *key)
{
    const char *path = Tenv::appGet("TEMP");

    sprintf(name, "%s/%s%s%d", path, base, key, getpid());
    
    // Check the path
    for (char *p = name; ('\0' != *p); p++)
    {
        if ('\\' == *p) *p = '/';
    }
}




int cleanEnv  (char const *script, int clean)  /* 002 */
{
    if (clean)
    {
        // This is much the same as source() below except we need
        // pass a cut down environment to MYspawnve()
        // The reduced environment only preserves
        // MACHINE
        // SHELL
        // PWD
        // USER
        // TALK_STATE    -- Sire control variables
        // DEBUG_STATE   -- Sire control variables
        // REMOVE_STATE  -- Sire control variables
        // ONE_STATE     -- Sire control variables

        // Lets cut down our environment

        // Preserve the values

        Tstr machine = Tenv::appGet("MACHINE");
        Tstr shell   = Tenv::appGet("SHELL");
        Tstr pwd     = Tenv::appGet("PWD");
        Tstr user    = Tenv::appGet("USER");
        Tstr talk    = Tenv::appGet("TALK_STATE");
        Tstr debug   = Tenv::appGet("DEBUG_STATE");
        Tstr remove  = Tenv::appGet("REMOVE_STATE");
        Tstr one     = Tenv::appGet("ONE_STATE");
        Tstr partial = Tenv::appGet("SIRE_PARTIAL");
        Tstr sire    = Tenv::appGet("SIRE");
        Tstr temp    = Tenv::appGet("TEMP");

        // Clear the environment

        Tenv::appClear();

        // Restore the values

        Tenv::appSet("MACHINE",      machine);
        Tenv::appSet("SHELL",        shell);
        Tenv::appSet("PWD",          pwd);
        Tenv::appSet("USER",         user);
        Tenv::appSet("TALK_STATE",   talk);
        Tenv::appSet("DEBUG_STATE",  debug);
        Tenv::appSet("REMOVE_STATE", remove);
        Tenv::appSet("ONE_STATE",    one);
        Tenv::appSet("SIRE_PARTIAL", partial);
        Tenv::appSet("SIRE",         sire);
        Tenv::appSet("TEMP",         temp);
    }
    
    // Lets source the file
    
    int  rtn = sourceFile  (script, (char const*)Tenv::appGet("SIRE"), "-env");
    
    // We need to set up the standard environment variable again
    // when we are finished.

    // Strip the code for this out of the start of main() into a new
    // function we can call here and in main().

    setDefaultEnv();
       
    return(rtn);
}




int source  (char const *comm)  /* 002 */
{
    Tstr filename = makeScript(comm, TRUE); // 003
    int rtn = sourceFile  ((char const*)filename, (char const*)Tenv::appGet("SIRE"), "-env");
    unlink((char const*)filename);
    return(rtn);
}



// filename - name of the source file
// envProg  - full path to the 'env' program

int sourceFile  (char const *filename, char const *envProg, char const *envArg)  /* 002 */
{
   extern char **globalArgv;
   
   if ('\0' == *filename)
   {
      return(0);
   }
   else
   {
      char script[512];
      
      MYtmpFile(script,  "sirea");
      
      FILE *file  = NULL;
      
      if (NULL == (file = fopen(script, "w")))
      {
         return(-1);
      }
      else
      {
         static char buff[2048];  // Working space !! Fixed size space !!
         char       *shell;       // Name of the shell
         char       *q;           // Transient pointer
         
         char const *scriptText;
         
         // Right - First off lets try and figure out what kind of shell
         // we are using !
         
         // Get the name of the shell executable
         
         buff[0] = '\0';   // Working space !! Fixed size space !!
         shell   = buff;
         q       = (char*)Tenv::appGet("SHELL");
         if (NULL != q)
         {
            // Strip out the first space separated token from ${SHELL}
            
            while (isspace(*q)) q++;
            while (!isspace(*q) && ('\0' != *q)) *(shell++) = *(q++);
            *(shell++) = '\0';
            
            // Strip out the program name (no suffix)
            
            shell = buff;
            if (NULL != (q = (char*)strrchr(shell, '\\'))) shell = q+1;
            if (NULL != (q = (char*)strrchr(shell, '/')))  shell = q+1;
            
            // Strip off the suffix
               
            if (NULL != (q = (char*)strchr(shell, '.'))) *q = '\0';
         }
         
         // Get the script to run the correct shell
         
         fprintf(file, "#!%s\n\n", Tenv::appGet("SHELL"));
         
         if (NULL != (scriptText = findTempl(shell)))  // 005
         {
            //// Template shell script definition //// 
            
            // Run the command
            fprintf(file, scriptText, filename, SIRE_ENV_TAG, envProg, envArg);
         }
         
         else if (!strncmp(shell, CSH_NAME, sizeof(CSH_NAME)-1))
         {
            //// C shell script //// 
            
            // Run the command
            fprintf(file, "source %s\n\n", filename);
            
            fprintf(file, "echo '%s'\n\n", SIRE_ENV_TAG);
            fprintf(file, "%s %s\n\n", envProg, envArg);
         }
         
         else if ((!strncmp(shell, SH_NAME,   sizeof(SH_NAME)-1)) ||
                  (!strncmp(shell, KSH_NAME,  sizeof(KSH_NAME)-1)) ||
                  (!strncmp(shell, RKSH_NAME, sizeof(RKSH_NAME)-1)) ||
                  (!strncmp(shell, BASH_NAME, sizeof(BASH_NAME)-1)))
         {
            //// Bourn/Korn shell script //// 
            
            fprintf(file, ". %s\n", filename);
            
            fprintf(file, "echo '%s'\n\n", SIRE_ENV_TAG);
            fprintf(file, "%s %s\n\n", envProg, envArg);
         }
         
         else
         {
            // Unknown script type
            
            printE("Unsupported shell [%s]\n", shell);
            fclose(file);
            unlink (script);
            return(1);  //// BAILING OUT ////
         }
         
         fclose(file);
            
         chmod (script, S_IRWXU | S_IRWXG);
         Tenv::appUnsetStd();
   
         // Right I admit it this is cut and paste with a bit re-hacking
         // Run the script and capture the environment variable settings
         // that 'sirenv' passes back.
         {
             char buff[1024];
             int  pipes[2];
 
             if (0 == pipe(pipes))
             {
                char  *p;
                FILE  *fd;
                int    tmpStd = -1;

                // Configure the pipes for the sub-process
                tmpStd = dup(1);   // Take a copy of standard out
                dup2(pipes[1], 1); // Make the pipe standard out
                close(pipes[1]);
 
                // Spawn the sub process
                MYspawnve(MY_NOWAIT, script, (const char**)globalArgv, Tenv::appGetEnvp());
 
                // Reset the pipes
                dup2(tmpStd, 1);             // Put back standard out
                close(tmpStd);               // Close the spare copy
                fd = fdopen(pipes[0], "r");  // Read the other end
                
                while (NULL != (p = fgets(buff, sizeof(buff), fd)))
                {
                    while (isspace(*p)) p++;
                    
                    if ('\0' == *p)
                    {
                        // Pass through the blank lines
                        printf("%s", buff);
                    }
                    else if (!strncmp(p, SIRE_ENV_TAG, sizeof(SIRE_ENV_TAG)-1))
                    {
                        // Read the environment
                        // The tag marks the start of the environment
                        
                        // Clear any output
                        fflush(stdout);
                        
                        // Read the environment
                        Tenv::setApp(fd);
                    }
                    else
                    {
                        // Pass through any other output
                        printf("%s", buff);
                    }
                }
                
                fclose(fd);
             }
         }
      
         unlink (script);
      }
   }
   
   return (0);
}




char const *makeScript(char const *text, int source) // 003
{
   static int  unique = 0;
   static char file[512];
   static char suff[5];
   
   FILE       *fd     = NULL;
   char       *script = NULL;
   
   for (int i = 4, j = unique; (i); i--, j /= ('z' - 'a'))
   {
      suff[i-1] = 'a' + (j % ('z' - 'a'));
   }
   suff[4] = '\0';
   unique++;
   
   MYtmpFile(file, "dwo", suff);
   
   if (NULL != (fd = fopen(file, "w")))
   {
      char const *p = strrchr(Tenv::appGet("SHELL"), '/');
      if (NULL == p) p = Tenv::appGet("SHELL");
      
      if (!strcasecmp(p, "command.com"))
      {
          fprintf(fd, "%s\n", text);
          fprintf(fd, "exit\n");
      }
      else
      {
          if (!source) fprintf(fd, "#!%s\n\n",    Tenv::appGet("SHELL"));
          fprintf(fd, "%s\n", text);
      }
      
      fclose(fd);
      script = file;
      chmod (script, S_IRWXU | S_IRWXG);
   }
   
   return(script);
}




//////////////////////////// Tpipe ///////////////////////////////




Tpipe *Tpipe::Dhead  = NULL;
int    Tpipe::Dstate = 0;
int    Tpipe::DsetClear = TRUE;




Tpipe::Tpipe(char const *comm, int read, Tenv *env) : Dnext(Dhead), Dscript("")
{
   Dhead = this;
   Dpid  = -1;
   Dfd   = NULL;
   
   if (DsetClear) {DsetClear = FALSE; atexit(Tpipe::clear);}
   
   while (isspace(*comm)) comm++;
   
   int pipes[2];
   
   Dscript = makeScript(comm);
      
   if ((0 != strlen(comm)) && (0 == pipe(pipes)))
   {
      char **localEnv;
      char  *(argv[2]);
      int    tmpStd = -1;
      
      if (read)
      {
         tmpStd = dup(1);   // Take a copy of standard out
         dup2(pipes[1], 1); // Make the pipe standard out
         close(pipes[1]);
      }
      else
      {
         tmpStd = dup(1);   // Take a copy of standard in
         dup2(pipes[0], 0); // Make the pipe standard in
         close(pipes[0]);
      }
            
      // Get the environment
      if (NULL == env)
          localEnv = Tenv::appGetEnvp();
      else
          localEnv = env->getEnvp();

      // Set up the arguments to the call
      argv[0] = (char*)malloc(strlen((char const*)Dscript)+1);
      strcpy(argv[0], (char const*)Dscript);
      argv[1] = NULL;
      
      // Spawn the sub process
      Dpid = MYspawnve(MY_NOWAIT, (char*)(char const*)Dscript, (const char**)argv, localEnv);
      
      // Free the allocated space
      free(argv[0]);
      
      if (read)
      {
         dup2(tmpStd, 1);             // Put back standard out
         close(tmpStd);               // Close the spare copy
         Dfd = fdopen(pipes[0], "r"); // Read the other end
      }
      else
      {
         dup2(tmpStd, 0);             // Put back standard in
         close(tmpStd);               // Close the spare copy
         Dfd = fdopen(pipes[1], "w"); // Write the other end
      }
   }
}




Tpipe::Tpipe(FILE *fd, int pid, char const *script) : Dnext(Dhead), Dfd(fd), Dpid(pid), Dscript(script)
{
   Dhead = this;
   
   if (DsetClear) {DsetClear = FALSE; atexit(Tpipe::clear);}
}



Tpipe::~Tpipe()
{
   RTN_TYPE rtn;
   
   // Remove from the list
   for (Tpipe *p = Dhead; (NULL != p); p = p->Dnext)
   {
      if (this == p->Dnext) p->Dnext = this->Dnext;
   }
   
   if (Dhead == this) Dhead = this->Dnext;
   
   // Close the link
   if (-1   != Dpid)          waitpid(Dpid, &rtn, 0);
   if (NULL != Dfd)           fclose (Dfd);
   if (0    != Dscript.len()) unlink ((char*)(char const*)Dscript);
   
   Dstate = RTN_VALUE(rtn);
}



Tpipe *Tpipe::find(FILE *fd)
{
    Tpipe *p;
    
    for (p = Dhead; (NULL != p); p = p->Dnext)
    {
        if (fd == p->Dfd) break;
    }
   
    return(p);
}



Tpipe *Tpipe::find(int pid)
{
    Tpipe *p;
    
    for (p = Dhead; (NULL != p); p = p->Dnext)
    {
        if (pid == p->Dpid) break;
    }
   
    return(p);
}



void Tpipe::clear()
{
   while (NULL != Dhead) delete Dhead;
}
   
static const char *exeList[] =
{
#if defined (INTEL_NT) || defined (INTEL_9X)
    ".exe",
    ".ksh",
    ".sh",
    ".com",
    ".bat",
#endif

    "",
    NULL
};

inline int isExecutable(char const *file)
{
    if (!strcmp(file, INTERNAL_SHELL))
    {
        return true;
    }
#if defined(INTEL_NT) || defined (INTEL_9X)

    // Check the file extensions

    char const  *p = strrchr(file, '.');
    char       **l;
    if (NULL != p)
        for (l = exeList; (NULL != *l); l++)
            if (!strcmp(p, *l))
                return (0 == access((char*)file, R_OK | F_OK));
    
    return (0);
#else
    return (0 == access((char*)file, R_OK | X_OK | F_OK));
#endif
}

int isExec(char const *file)
{
    return (isExecutable(file));
}


char const * const* getExeList()
{
    return ((char const * const *)exeList);
}

                    
char const *findExec(char const *name, char const *cwd)       // 001
{
   static TstrBuf file;   // Working space - self reallocating

   unsigned long len = 0;
   
   if (NULL == cwd) cwd = Tenv::appGet("PWD");

   char const *tmp = Tenv::appGet("PATH");
   if (NULL == tmp) tmp = "";
   
   // Take a local copy of the path
   char *path  = new char [strlen((char const*)tmp) + 10];
   strcpy(path, (char const*)tmp);
   
   int   start = 0;
   int   end   = 0;
   char  c     = '\0';
   
   while ('\0' != path[start])
   {
      while (isspace(path[start])) start++;
      
      end = start;
      while (('\0' != path[end]) && (PATH_SEP != path[end])) end++;
      c = path[end];
      path[end] = '\0';
      
      // Process
      if ('\0' == path[start])
      {
         // Look in the working directory
         file = cwd;
      }
      else if ('.' == path[start])
      {
         // Relative path (make absolute)
         file = cwd;
         file.cat(DIR_SEP);
         file.cat(path +start);
      }
      else
      {
         // Absolute path
         file = path +start;
      }
      
      file.cat(DIR_SEP);
      file.cat(name);
      
      // Check if found
      
      /* Try different extensions - Principly for PC's */
      len = strlen((char const*)file);
         
      for (const char **p = exeList; (NULL != *p); p++)
      {
         file.cat(*p);
 
         // Check if found
         //if (0 == access((char*)(char const*)file, R_OK | X_OK | F_OK))
         if (isExecutable((char const*)file))
         {
            if (!isDir((char const*)file))
            {
               delete path;
 
               for (long i = file.len()-1; (i >= 0); i--)
        	   if (DIR_SEP[0] == file[(unsigned long)i])
        	       file[(unsigned long)i] = '/';
 
               return((char const*)file);
            }
         }
 
         file[len] = '\0';
      }
      
      path[end] = c;
      start = end;
      if ('\0' != path[start]) start++;
   }
   
   delete path;
   return ((char const*)name);
}





char const *findExecs(char const *name)    // 001
{
   static TstrBuf tmp;
   static TstrBuf rtn;
   
   int first = 1;
   
   tmp = name;
   rtn = "";
   
   char *start = (char*)(char const*)tmp;
   char *end   = (char*)(char const*)tmp;
   char  c;
   
   while (isspace(*start)) start++;
   while ('\0' != *start)
   {
      end = start;
      while (('\0' != *end) && !isspace(*end)) end++;
      c = *end;
      *end = '\0';
      
      // Process
      if (first) first = 0; else rtn.cat(" ");
      char const *fred = findExec(start);
      rtn.cat(fred);
      
      *end = c;
      start = end;
      while (isspace(*start)) start++;
   }
   
   return ((char const*)rtn);
}





char const *getUser()    // 008
{
    char const *rtn = NULL;
    
#if defined(INTEL_NT) || defined (INTEL_9X) || defined (CYGWIN)

    rtn = "nobody";
        
#else

    static char buff[256];

    struct passwd *user = getpwuid(getuid());  // User passwd entry
    
    if (NULL != user)
    {
        rtn = strcpy(buff, user->pw_name);
    }
    else
    {
        rtn = "nobody";
    }
    
#endif
   
    return (rtn);
}





char const *getGroup()    // 008
{
    char const *rtn = NULL;
    
#if defined(INTEL_NT) || defined (INTEL_9X) || defined (CYGWIN)

    rtn = "nobody";
        
#else

    static char buff[256];

    gid_t gid = getgid();  // Group Id
    struct group *gident;  // Group file entry
    
    // Open the group file
    setgrent();
    
    // scan the group file
    while ((NULL != (gident = getgrent())) && (gid != gident->gr_gid));
    
    // Return the group
    if (gid == gident->gr_gid)
    {
        rtn = strcpy(buff, gident->gr_name);
    }
    else
    {
        rtn = "nobody";
    }
    
    // close the group file
    endgrent();
    
#endif
   
    return (rtn);
}


// Locate the template for the given shell script
// The template give the text that will allow a script to be sourced
// and the environment returned. The script pluggins are in order
// script - the name of the script file to be sourced
// tag    - the tag to be output to stdout just prior to the environment
// env    - the environment program to output the environment to stdout

char const *findConfig(char const *shell, char const *Type)  // 005, 007
{
    static char    name[1024];
    static char    exe[256];
    
    char *p;
    
    // Resolve the text of the file extension (if any)
    
    if (NULL != Type)
    {
        sprintf(exe, ".%s", Type);
        Type = exe;
    }
    else
    {
        Type = "";
    }
    
    // Look for the file in the home account
    
    if (NULL != Tenv::appGet("HOME"))
    {
        sprintf(name, "%s/.sirerc/%s%s", Tenv::appGet("HOME"), shell, Type);  // 007
        
        if (isFile(name))
        {
            return (name);
        }
    }
    
    // Look for the file in the group account
    
    if (NULL != Tenv::appGet("GROUP"))
    {
#if defined(INTEL_NT) || defined (INTEL_9X) || defined (CYGWIN)
        sprintf(name, "%s/group/%s/sire/%s%s", Tenv::appGet("ROOTDIR"), Tenv::appGet("GROUP"), shell, Type);     // 007
#else
        sprintf(name, "%s/usr/group/%s/sirerc/%s%s", Tenv::appGet("ROOTDIR"), Tenv::appGet("GROUP"), shell, Type);     // 007
#endif
        
        if (isFile(name))
        {
            return (name);
        }
    }
    
    // Look for the file in the global account
    
#if defined(INTEL_NT) || defined (INTEL_9X) || defined (CYGWIN)
    sprintf(name, "%s/sire/%s%s", Tenv::appGet("ROOTDIR"), shell, Type);     // 007
#else
    sprintf(name, "%s/usr/sirerc/%s%s", Tenv::appGet("ROOTDIR"), shell, Type);     // 007
#endif
    
    if (isFile(name))
    {
        return (name);
    }

    // Look for the file in the same directory as the executable

    strcpy(name, Tenv::appGet("SIRE"));
    if (NULL == (p = strrchr(name, *DIR_SEP)))
    {
        // Assume it is in the current directory
        sprintf(name, "%s/sirerc/%s%s", Tenv::appGet("PWD"), shell, Type);
    }
    else
    {
        // Append to the path
        sprintf(p+1, "sirerc/%s%s", shell, Type);
    }

    if (isFile(name))
    {
        return (name);
    }
    
    return (NULL);
}

char const *findTempl(char const *shell, char const *Type)  // 005, 007
{
    static char const *name;
    static char        block[1024+1];
    static TstrBuf     templ;
    
    FILE *fp = NULL;
    
    if ((NULL != (name = findConfig(shell))) && (fp = fopen(name, "r")))
    {
        int buffLen = 0;
        int len;
        while (0 < (len = fread(block, 1, sizeof(block)-1, fp)))
        {
             block[len] = '\0';
             templ.cat(block, buffLen);
             buffLen += len;
        }
        
        fclose(fp);
        
        return ((char const *)templ);
    }
    
    return (NULL);
}







/************* END OF FILE **************************************************/
















