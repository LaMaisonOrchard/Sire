/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2002, 2003  David W Orchard (davido@errol.org.uk)

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

   File  : machine.cpp
   Class : None
   Desc  : A description
   
AMENDMENTS

EDIT 001 21-Nov-97 davido
Check the root directory as well.

EDIT 002 13-Jan-99 davido
Support SuSe Linux file positions.

EDIT 003 14-Aug-99 davido
Support Linux Fortran 77

EDIT 004 19-May-00 davido
Take out the platform specific dependency.

EDIT 005 15-Nov-02 davido
Remove support for infrequently used options

EDIT 006 10-Jan-03 davido
Remove getPython, getMachine() and getPlatform(). Now redundant.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "env.h"
#include "machine.h"
#include "build_v.h"


/************* FUNCTIONS ****************************************************/


static char const *findName(char **list)
{ 
   static char buff1[256];
   static char buff2[256];
   
   int len1;
   int len2;
   
   for (char **p = list; (NULL != *p); p++)
   {
      strcpy(buff1, (char*)Tenv::appExpand(*p));
      strcpy(buff2, (char*)Tenv::appGet("ROOTDIR"));  // 004
      strcat(buff2, buff1);

      len1 = strlen(buff1);
      len2 = strlen(buff2);
      for (char const * const *q = getExeList(); (NULL != *q); q++)
      {
         strcpy(buff1 +len1, *q);
         strcpy(buff2 +len2, *q);

         if (!access(buff1, X_OK))  // Check the given path
            return (buff1);

         if (!access(buff2, X_OK))  // Check against the file system root
            return (buff2);
      }
   }
         
   return (*list);
}


char const *getCCP()
{
   // C++ Compiler
   static char *list[] =
   {
   "/bin/CC",
   "/bin/ccp",
   "/usr/bin/CC",
   "/usr/bin/ccp",
   "/usr/ucb/CC",
   "/usr/ucb/ccp",
   "/usr/lang/CC",
   "/usr/lang/ccp",
   "/opt/SUNWspro/bin/CC",
   "/opt/SUNWspro/bin/ccp",
   "${RELEASE}/SC1.0/CC",
   "${RELEASE}/SC1.0/ccp",
   "/usr/local/bin/gcc",
   "${Init}/bin/cl",
   "/usr/bin/g++",       // 002
   NULL
   };
   
   return(findName(list));
}


char const *getCPP()
{
   // C Preprocessor
   static char *list[] =
   {
   "/bin/cpp",
   "/usr/bin/cpp",
   "/usr/ucb/cpp",
   "/usr/lang/cpp",
   "/opt/SUNWspro/bin/cpp",
   "${RELEASE}/SC1.0/cpp",
   NULL
   };

   return(findName(list));
}


char const *getCC()
{
   // C Compiler
   static char *list[] =
   {
   "/bin/cc",
   "/usr/ucb/cc",
   "/usr/bin/cc",
   "/usr/lang/cc",
   "/opt/SUNWspro/bin/cc",
   "${RELEASE}/SC1.0/cc",
   "${Init}/bin/cl",
   "/usr/bin/gcc",       // 002
   NULL
   };

   return(findName(list));
}


char const *getJava()
{
   // C Compiler
   static char *list[] =
   {
   "/usr/bin/javac",
   "javac",
   NULL
   };

   return(findName(list));
}


char const *getSH()
{
   // C Compiler
   static char *list[] =
   {
   "/bin/sh",
   NULL
   };

   return(findName(list));
}


char const *getCSH()
{
   // C Compiler
   static char *list[] =
   {
   "/bin/csh",
   NULL
   };

   return(findName(list));
}


char const *getTmpDir()
{
    const char *path = "/tmp";   // UNIX - default

#if defined (INTEL_NT) || defined (INTEL_9X)

    if (NULL != (path = Tenv::appGet("TMP")))
    {
        if (access((char*)path, F_OK) || !isDir(path))
        {
            // Does not exist
            path = NULL;
        }
    }

    if ((NULL == path) && (NULL != (path = Tenv::appGet("TEMP"))))
    {
        if (access((char*)path, F_OK) || !isDir(path))
        {
            // Does not exist
            path = NULL;
        }
    }

    if (NULL == path)
    {
        // PC - default
        char buff[512];
        sprintf(buff, "%s/tmp", Tenv::appGet("ROOTDIR"));
        path = buff;
    }

#endif

   return(path);
}




/************* END OF FILE **************************************************/
