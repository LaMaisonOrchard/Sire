/****************************************************************************

    The Sire include file extraction utility 'getinclude'.
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

   File  : main.cpp
   Class : None
   Desc  : A description

*****************************************************************************

AMENDMENTS :

EDIT 001 15-Feb-95 davido 
1.0A : Install a mark issue. Report unfound includes as local to the
source file.

EDIT 002 02-May-95 davido 
1.0B : Optimised for speed.

EDIT 003 14-Jan-97 davido 
1.0C : Read standard in as default

EDIT 004 16-Jan-97 davido 
1.0D : Bug fix

EDIT 005 30-Apr-97 davido 
1.0E : Don't mix errors with the output

EDIT 006 17-Dec-04 david@errol.org.uk
2.0A : First KDevelop project version

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if ! defined (INTEL_NT)
#include <unistd.h>
#endif
#include "include.h"

enum bool_t
{
   FALSE = 0,
   TRUE
};


static int reportIncludes (char  *fileName);


/************* FUNCTIONS ****************************************************/



#define BUFF_SIZE (1024)




int main (int argc, char **argv)
{
    char  *fileName = NULL;
    char **p;
 
    int    sire = 0;
 
    for (p = argv+1; (NULL != *p); p++)
    {
       if ('-' == (*p)[0])
       {
          if ('I' == (*p)[1])
          {
             addIncludePath((*p) +2);
          }
          else if (!strcmp("i", (*p)+1))
          {
             printf("getinclude 2.0 A\n");
             exit(0);
          }
          else if (!strcmp("h", (*p)+1))
          {
             printf("getinclude [-i] [-h] -sire { -I<include_path> } [<source_file>]\n");
             exit(0);
          }
          else if (!strcmp("sire", (*p)+1))
          {
             sire = 1;
          }
       }
       else
       {
           fileName = *p;

           if (sire)
           {
               fprintf(stdout, "%s | DUMMY ", fileName);
               reportIncludes (fileName);
               fprintf(stdout, " ;\n\n");
           }
       }
    }
   
    if (sire)
    {
        return(0);
    }
    else
    {
        return (reportIncludes (fileName));
    }
}


static int reportIncludes (char  *fileName)
{   
   char const *fullPath;
   
   char  *includeFile;
   FILE  *file;
   
   // Parse the file
   char  line[BUFF_SIZE];
   char *r;
   
   if (NULL != fileName)
   {
      file = fopen(fileName, "r");
   }
   else
   {
      file = stdin;
   }
   
   if (NULL != file)
   {
      // Get a buffer containing the base dir
      static char buff[BUFF_SIZE];
      
      if (NULL != fileName)
      {
         strcpy(buff, fileName);
         char *q = strrchr(buff, '/');
         if (NULL == q)
         {
            buff[0] = '.';
            q = buff +1;
         }
         q[0] = '/';
         q[1] = '\0';
      }
      else
      {
         strcpy(buff, "./");
      }
   
      while (NULL != fgets(line, BUFF_SIZE -1, file))
      {
         r = line;
         
         while (isspace(*r)) r++;
         
         if ('#' == *r)
         {
            r++;
            while (isspace(*r)) r++;
            if (!strncmp(r, "include", 7))
            {
               r += 7;
               while (isspace(*r)) r++;
               
               if ('\"' == *r)
               {
                  r++;
                  includeFile = r;
   	               
                  while (!isspace(*r) && ('\"' != *r) && ('\0' != *r))
                  {
                     r++;
                  }
                  
                  *r = '\0';
                  
                  fullPath = getInclude(includeFile, buff, 1);
                  if (NULL != fullPath)
                     fprintf(stdout, "%s ", fullPath);
               }
            }
         }
      }
      
      fclose(file);
   }
   else if (NULL != fileName)
   {
      /*fprintf(stderr, "Failed to open %s\n", fileName);*/  // 005
   }
   
   return(0);
}




/************* END OF FILE **************************************************/
