/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2002, 2003, 2004, 2005  David W Orchard (davido@errol.org.uk)

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

   File  : sys.h
   Class : NONE
   Desc  : A description

*****************************************************************************/

#ifndef SYS_H
#define SYS_H

/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "build_v.h"

#if defined(INTEL_NT) || defined (INTEL_9X)

#include <time.h>
#include <io.h>
#include <process.h>
#include <direct.h>
#define access _access
#define chmod  _chmod
#define dup    _dup
#define dup2   _dup2
#define close  _close
#define strcasecmp stricmp

     inline int execve(char *s1, char **s2, char **s3)
     {
        return (_execve(s1, s2, s3));
     }

     inline void bcopy(register char *s1, register char *s2, size_t len)
     {
        memcpy(s2,s1,len);
     }
     
     inline int   pipe (int filedes[2])
     {
        return (_pipe(filedes, 256, _O_TEXT));
     }
      
#define popen _popen
     
#    define RW_OK	6	/* Test for read permission */
#    define R_OK	4	/* Test for read permission */
#    define W_OK	2	/* Test for write permission */
#    define X_OK	0	/* Test for existence of file */
#    define F_OK	0	/* Test for existence of file */

#define S_ISUID 0
#define S_ISGID 0
#define S_ISVTX 0
#define S_IRWXU (_S_IREAD | _S_IWRITE)
#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRIT
#define S_IXUSR _S_IREAD
#define S_IRWXG (_S_IREAD | _S_IWRITE)
#define S_IRGRP _S_IREAD
#define S_IWGRP _S_IWRIT
#define S_IXGRP _S_IREAD
#define S_IRWXO (_S_IREAD | _S_IWRITE)
#define S_IROTH _S_IREAD
#define S_IWOTH _S_IWRIT
#define S_IXOTH _S_IREAD

#define S_ISDIR(x) ((_S_IFDIR & (x)) == _S_IFDIR)

inline int waitpid(int process_id, int *status, int options)
{
   return cwait(status, process_id, _WAIT_GRANDCHILD);
}

#define PATH_SEP ';'
#define DIR_SEP  "\\"

#else

#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>

#if defined (INTEL_LINUX)
#include <time.h>
#else
#include <sys/time.h>
#endif

#include <sys/wait.h> 

#define PATH_SEP ':'
#define DIR_SEP  "/"

#endif

#include <limits.h>
#include <stdarg.h>

#if defined (NEC_RISC)

int strcasecmp(char const *a, char const *b);

#endif

#if defined(NEC_RISC) || defined(INTEL_NT) || defined(INTEL_LINUX)
#include <string.h>
#else
#include <strings.h>
#endif

#if ! defined(ALPHA_OSF)
enum Tboolean { FALSE = 0, TRUE };
#endif

#define DEFAULT_TARGET "TARGET"

#define IRELEVENT -2
#define BAD_FILE  -1
#define NO_FILE    0
#define OLD_FILE   1
#define NEW_FILE   INT_MAX
                    
char const *findExec (char const *name, char const *cwd = NULL);   // Find the executable
char const *findExecs(char const *name);                           // Find the executables
                    
int  isDir        (char const *name);     // 000
int  isExec       (char const *file);
int  isFile       (char const *file);
int  doesExist    (char const *name);
int  timeStamp    (char const *name);
int  touch        (char const *name);
int  execute      (char const *comm);
int  execute      (char const *comm, char **envp);
int  source       (char const *comm);
int  cleanEnv     (char const *script, int clean = 1);
int  sourceFile   (char const *script, char const *envProg, char const *envArg = "");
void setDefaultEnv(char const *cwd = NULL, char const *app = NULL);

int  setDir(char const *name);
char const *getDir();

const int MY_NOWAIT = 0;
const int MY_WAIT   = 1;

int  MYspawnve(int wait, char *script, char const **argv, char **envp);
void MYtmpFile(char *name, char const *base, char const *key = "");

FILE *readPipe(char const* command);
void  closePipe(FILE *fp);

void printT(char const *format ...);
void printD(char const *format ...);
void printE(char const *format ...);

void vprintT(char const *format, va_list ap);
void vprintD(char const *format, va_list ap);
void vprintE(char const *format, va_list ap);

void messageCountT(int i);
void messageCountD(int i);
void messageCountE(int i);

int  messageCountT();
int  messageCountD();
int  messageCountE();

int  setTon(int on);
int  setDon(int on);
int  setEon(int on);

char const * const* getExeList();

char const *getUser();
char const *getGroup();
char const *findTempl(char const *shell, char const *Type = "tmpl");
char const *findConfig(char const *shell, char const *Type = "tmpl");


#ifdef DECSTATION
#define RTN_TYPE       union wait
#define RTN_VALUE(rtn) rtn.w_status
#else
#define RTN_TYPE       int
#define RTN_VALUE(rtn) rtn
#endif


#ifdef SUN4
#define atexit(x) on_exit((void (*)(int ... ))x, NULL)
#else
#define malloc_t       char *
#endif

#define getenv !!! Dont use getenv() use Tenv::appGet() instead !!!


#endif


/************* END OF FILE **************************************************/















