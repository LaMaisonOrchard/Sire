/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999  David W Orchard (davido@errol.org.uk)

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

   File  : dep.cpp
   Class : Tdep
   Desc  : A description
       
EDIT 001 12-Jul-99 davido
Support for indexed vaiables.
       
EDIT 002 07-Sep-99 davido
Implement late substitution for blocks.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sys.h"
#include "strbuf.h"
#include "identdep.h"
#include "matchdep.h"
#include "calldep.h"
#include "whichdep.h"
#include "text.h"


/************* FUNCTIONS ****************************************************/




Tdep::~Tdep()
{
}



char const *Tdep::realise(char const *target,
                          char const *targetMatch,
                          Tstr       *list,    // 002
                          int         maxIdx)  // 002
{
   static TstrBuf rtn = (char const*)"";
   //static Tstr    list[10];        // 002
   
   char const    *p      = text();
   unsigned long  rtnPos = 0;
   
   //int maxIdx = wildMatchSections (target, targetMatch, list);  // 002
   
   while ('\0' != *p)
   {
      if ('\001' == *p)
      {
         p++;
         rtn.cat(target, rtnPos);
         rtnPos += strlen(target);
      }
      else if ('\002' == *p)
      {
         p++;
         // First Dep has no meaning here - remove it
      }
      else if ('\003' == *p)
      {
         p++;
         // Deps has no meaning here - remove it
      }
      else if ('\004' == *p)  // 001
      {
         // Indexed variable
         p++;
         int ch = *(p++);  // Get the number
         if (('0' < ch) && (ch < ('1' +maxIdx)))
         {
            rtn.cat(list[ch - '1'], rtnPos);
            rtnPos += list[ch - '1'].len();
         }
      }
      else
      {
         rtn[rtnPos++] = *p++;
      }
   }
   
   rtn[rtnPos++] = '\0';
   
   return ((char const*)rtn);
}



char const *Tdep::match(char const *dep,
                        char const *targetMatch)
{
   return ((char const*)NULL);
}



char const *Tdep::match(char const *dep)
{
   return ((char const*)NULL);
}



char const *TcallDep::realise(char const *target,
                              char const *targetMatch,
                              Tstr       *list,
                              int         maxIdx)
{
   static TstrBuf rtn = "";
   
   char const    *comm   = Tdep::realise(target, targetMatch, list, maxIdx);
   FILE          *input  = NULL;
   unsigned long  rtnPos = 0;
   
   // if (NULL != (input = popen(comm, "r")))
   if (NULL != (input = readPipe(comm)))
   {
      int ch;
      
      while (EOF != (ch = fgetc(input)))
      {
         if (isspace(ch))
         {
            rtn[rtnPos++] = ' ';
            while (isspace(ch)) ch = fgetc(input);
         }
         
         if (EOF != ch) rtn[rtnPos++] = ch; 
      }
      
      // pclose(input);
      closePipe(input);
   }
   
   rtn[rtnPos++] = '\0';
   
   return ((char const*)rtn);
}



char const *TwhichDep::realise(char const *target,
                               char const *targetMatch,
                               Tstr       *list,
                               int         maxIdx)
{
   char const *comm = Tdep::realise(target, targetMatch, list, maxIdx);
   
   return (findExecs(comm));
}



/************* END OF FILE **************************************************/
