/****************************************************************************

    The Sire include file extarction utility 'getinclude'.
    Copyright (C) 1999, 2002, 2004  David W Orchard (davido@errol.org.uk)

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

   File  : include.cpp
   Class : None
   Desc  : 

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(HP700)
#include <strings.h>
#endif

#if defined(SOLARIS) || defined(DECSTATION) || defined(NEC_RISC)
extern "C" {void bcopy(register char *s1, register char *s2, size_t len);}
#endif

#if ! defined (INTEL_NT)
#    include <unistd.h>
#else

#    include <io.h>
#    define access _access
     inline void bcopy(register char *s1, register char *s2, size_t len)
     {
        memcpy(s2,s1,len);
     }
     
#    define RW_OK	6	/* Test for read permission */
#    define R_OK	4	/* Test for read permission */
#    define W_OK	2	/* Test for write permission */
#    define F_OK	0	/* Test for existence of file */
#endif 

#include "strbuf.h"
#include "include.h"

/************* FUNCTIONS ****************************************************/

typedef struct includePath_t
{
   char path[1024];
   int  len;
};

static includePath_t includeList[64];
static int           includePos = 0;

static char          stdInclude[1024] = "/usr/include/";
static int           stdLen           = strlen("/usr/include");


void addIncludePath(char const *name)
{
   if ('\0' == *name) name = ".";
   
   int   len = includeList[includePos].len = strlen(name);
   char *p   = includeList[includePos].path;
   bcopy((char*)name, p, len +1);
   if ('/' != p[len -1])
   {
      includeList[includePos].len++;
      p[len]    = '/';
      p[len +1] = '\0';
   }
   includePos++;
}


char const *getInclude(char const *name, char *source, int quote)
{
   static char src[1024];
   static int  srcLen = -1;
   
   register char *buff;
   register int   len;
   register int   nameLen = strlen(name);
   
   if (quote)
   {
      // Local file
    
      if (0 > srcLen)
      {
         srcLen = strlen(source);
         bcopy(source, src, srcLen +1);
      }
      bcopy((char*)name, src+srcLen, nameLen +1);
      if (!access(src, F_OK))
      {
         return(src);
      }
   }
   else
   {
      // Standard places

      bcopy((char*)name, stdInclude + stdLen, nameLen +1);
      if (!access(stdInclude, F_OK))
      {
         return(stdInclude); 
      }
   }
   
   for (int i = 0; (i < includePos); i++)
   {
      len  = includeList[i].len;
      buff = includeList[i].path;
      bcopy((char*)name, buff +len, nameLen +1);
      if (!access(buff, F_OK))
      {
         return(buff);  
      }
   }
   
   
   if (quote)
   {
      // Error - Local file
    
      src[srcLen] = '\0';
      bcopy((char*)name, src+srcLen, nameLen +1);
      return(src);
   }
   else
   {
      return(NULL);
   }
}



/************* END OF FILE **************************************************/

