/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2002  David W Orchard (davido@errol.org.uk)

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

   File  : text.cpp
   Class : NONE
   Desc  : A description
   
AMENDMENTS

EDIT 001 12-Jul-99 davido
Permit '#' as a wild card matching everything except '/' and '.'.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "str.h"
#include "strbuf.h"
#include "text.h"
#include "nametarget.h"
#include "matchtarget.h"
#include "jake.h"
#include "rule.h"
#include "state.h"
#include "sys.h"
#include "targetstmplst.h"
#include "env.h"

/************* FUNCTIONS ****************************************************/



char const *strindent(int indent)
{
   static TstrBuf buff = "";
   
   int i;
   for (i = 0; (i < indent); i++)
      buff[i] = ' ';
      
   buff[i] = '\0';
   
   return((char const*)buff);
}


char const *sedMatch(char const *text,
                     char const *firstMatch,
                     char const *secondMatch)
{
   static TstrBuf value;
   static TstrBuf word;
   static Tstr    list[10];
   static Tstr    target;
   
   target = markMatch(secondMatch);
   
   unsigned long valuePos = 0;
   unsigned long wordPos  = 0;
   unsigned long textPos  = 0;
   
   while ('\0' != text[textPos])
   {
      // Remove leading spaces
      while (isspace(text[textPos])) textPos++;
      
      if ('\0' != text[textPos])
      {
         // Read the word
         wordPos = 0;
         
         while (('\0' != text[textPos])&&(!isspace(text[textPos])))
         {
            word[wordPos++] = text[textPos++];
         }
         word[wordPos++] = '\0';
         
         // Translate the word
         int maxIdx;
         if (0 < (maxIdx = wildMatchSections ((char const*)word, firstMatch, list)))
         {
             // Copy the second target substituting the matches
             
             char const *p = (char const*)target;
             while ('\0' != *p)
             {
                if ('\001' == *p)
                {
                   p++;
                   // First Dep has no meaning here - remove it
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
                      value.cat(list[ch - '1'], valuePos);
                      valuePos += list[ch - '1'].len();
                   }
                }
                else
                {
                   value[valuePos++] = *p++;
                }
             }
         }
         
         value[valuePos++] = ' ';
      }
   }
   if (0 < valuePos) valuePos--; // Take the last space off
   value[valuePos++] = '\0';
   
   return((char const*)value);
}






int wildMatchSections (char const *name,
                       char const *match,
                       Tstr        matchList[])
{
   char const *n = name;
   char const *m = match;
   
   static TstrBuf rtn;
   int    idx = 0;
   int    pos = 0;
   
   int  matched = TRUE;

   do
   {
      // Reset the buffer position
      pos = 0;
      
      if ('[' == *m)
      {
        // Range of matching charactors
        
        m++;
        matched = FALSE;
        while (('\0' != *m) && (']' != *m))
        {
           if (('~' == m[1]) && (']' != m[2]))
           {
              if ((m[0] <= *n) && (m[2] >= *n))
              {
                 rtn[pos++] = *(n++);
                 matched = TRUE;
                 while (('\0' != *m) && (']' != *m))
                 {
                    // read to the closed bracket
                    m++;
                 }
              }
              else
              {
                 m += 3;
              }
           }
           else
           {
              if (*n == *m)
              {
                 rtn[pos++] = *(n++);
                 matched = TRUE;
                 while (('\0' != *m) && (']' != *m))
                 {
                    // read to the closed bracket
                    m++;
                 }
              }
              else
              {
                 m++;
              }
           }
        }
        
        if (']' != *m)
        {
           matched = FALSE;
           printE("Bad match construct '%s'\n", match);
        }
        else
        {
           m++;
        }
        
        // The matched text
        rtn[pos++] = '\0';
        if (matched && (9 > idx))
            matchList[idx++] = rtn;
      }
      else if ('+' == *m)
      {
         // Wild card match (incl '/')
         matched = FALSE;
         
         m++;
         do
         {
            matched = wildMatch(n, m);
            rtn[pos++] = *(n++);
         }
         while (('\0' != *n) &&
                !matched);
         
         // If this is matched then we have taken off one to many char
         if (matched)
         {
            // Rewind one
            n--;
            pos--;
         }
         else
         {
            // Check the final position
            matched = wildMatch(n, m);
         }
        
         // The matched text
         rtn[pos++] = '\0';
         if (matched && (9 > idx))
             matchList[idx++] = rtn;
      }
      else if ('*' == *m)
      {
         // Wild card match (no '/')
         matched = FALSE;
         
         m++;
         do
         {
            matched = wildMatch(n, m);
            rtn[pos++] = *(n++);
         }
         while (('\0' != *n) &&
                ('/'  != *n) &&
                !matched);
         
         // If this is matched then we have taken off one to many char
         if (matched)
         {
            // Rewind one
            n--;
            pos--;
         }
         else
         {
            // Check the final position
            matched = wildMatch(n, m);
         }
        
         // The matched text
         rtn[pos++] = '\0';
         if (matched && (9 > idx))
             matchList[idx++] = rtn;
      }
      else if ('#' == *m) // 001
      {
         // Wild card match (no '.' or '/')
         matched = FALSE;
         
         m++;
         do
         {
            matched = wildMatch(n, m);
            rtn[pos++] = *(n++);
         }
         while (('\0' != *n) &&
                ('/'  != *n) &&
                ('.'  != *n) &&  // 005
                !matched);
         
         // If this is matched then we have taken off one to many char
         if (matched)
         {
            // Rewind one
            n--;
            pos--;
         }
         else
         {
            // Check the final position
            matched = wildMatch(n, m);
         }
        
         // The matched text
         rtn[pos++] = '\0';
         if (matched && (9 > idx))
             matchList[idx++] = rtn;
      }
      else if (('?' == *m) && ('\0' != *n)) // Dont match terminator // 001
      {
         m++;
         
         // The matched text
         rtn[pos++] = *(n++);
         rtn[pos++] = '\0';
         if (9 > idx) matchList[idx++] = rtn;
      }
      else if (*m == *n)
      {
         // Litteral part
         // Dont skip the terminator
         if ('\0' != *n)
         {
            m++;
            n++;
         }
      }
      else if ('\0' != *n)
      {
         // We have not got to the end of the input so it must be a clash
         matched = FALSE;
      }
   }
   while (matched && ('\0' != *m) && ('\0' != *n));
   
   // These can match nothing - assume they match nothing
   while (('*' == *m) || ('#' == *m))
   {
       m++;
       if (9 > idx) matchList[idx++] = "";
   }
   
   if (matched && ('\0' == *m) && ('\0' == *n))
      return(idx);
   else
      return(0);  // No match
}







int wildMatch (char const *name, char const *match)
{
   char const *n = name;
   char const *m = match;
   
   int  matched = TRUE;

   do
   {
      if ('[' == *m)
      {
        // Range of matching charactors
        
        m++;
        matched = FALSE;
        while (('\0' != *m) && (']' != *m))
        {
           if (('~' == m[1]) && (']' != m[2]))
           {
              if ((m[0] <= *n) && (m[2] >= *n))
              {
                 n++;
                 matched = TRUE;
                 while (('\0' != *m) && (']' != *m))
                 {
                    // read to the closed bracket
                    m++;
                 }
              }
              else
              {
                 m += 3;
              }
           }
           else
           {
              if (*n == *m)
              {
                 n++;
                 matched = TRUE;
                 while (('\0' != *m) && (']' != *m))
                 {
                    // read to the closed bracket
                    m++;
                 }
              }
              else
              {
                 m++;
              }
           }
        }
        
        if (']' != *m)
        {
           matched = FALSE;
           printE("Bad match construct '%s'\n", match);
        }
        else
        {
           m++;
        }
      }
      else if ('+' == *m)
      {
         // Wild card match
         matched = FALSE;
         
         m++;
         do
         {
            matched = wildMatch(n, m);
            n++;
         }
         while (('\0' != *n) &&
                !matched);
                
         if (matched) n--;
      
         if (('\0' == *n) ||
             ('/'  == *n))
         {
            matched = wildMatch(n, m);
         }
      }
      else if ('*' == *m)
      {
         // Wild card match
         matched = FALSE;
         
         m++;
         do
         {
            matched = wildMatch(n, m);
            n++;
         }
         while (('\0' != *n) &&
                ('/'  != *n) &&
                !matched);
                
         if (matched) n--;
      
         if (('\0' == *n) ||
             ('/'  == *n))
         {
            matched = wildMatch(n, m);
         }
      }
      else if ('#' == *m) // 001
      {
         // Wild card match
         matched = FALSE;
         
         m++;
         do
         {
            matched = wildMatch(n, m);
            n++;
         }
         while (('\0' != *n) &&
                ('/'  != *n) &&
                ('.'  != *n) &&  // 005
                !matched);
                
         if (matched) n--;
      
         if (('\0' == *n) ||
             ('/'  == *n) ||
             ('.'  == *n))  // 005
         {
            matched = wildMatch(n, m);
         }
      }
      else if (('?' == *m) && ('\0' != *n)) // Dont match terminator // 001
      {
         // Don't skip terminator
         if ('\0' != *n)
         {
            m++;
            n++;
         }
      }
      else if (*m == *n)
      {
         // Don't skip terminator
         if ('\0' != *n)
         {
            m++;
            n++;
         }
      }
      else if ('\0' != *n)
      {
         // We have not got to the end of the input so it must be a clash
         matched = FALSE;
      }
   }
   while (matched && ('\0' != *m) && ('\0' != *n));
   
   // These can match nothing - assume they match nothing
   while (('*' == *m) || ('#' == *m)) m++;
   
   return(matched && ('\0' == *m) && ('\0' == *n));
}








char const *markMatch (char const *match)
{
    static TstrBuf buff;
    
    int pos = 0;
    int idx = 1;
    
    char const *m = match;

    do
    {
       if ('[' == *m)
       {
         // Range of matching charactors
 
         do
         {
             // read to the closed bracket
             m++;
         }
         while (('\0' != *m) && (']' != *m));
             
         // Mark the position
         if (10 >= idx)
         {
            buff[pos++] = '\004';
            buff[pos++] = '0'+idx++;
         }
 
         // Check the end of brackets
         if (']' != *m)
         {
            printE("Bad match construct '%s'\n", match);
         }
         else
         {
            m++;
         }
       }
       else if ('*' == *m)
       {
          // Wildcard position
          m++;
          if (10 >= idx)
          {
            buff[pos++] = '\004';
            buff[pos++] = '0'+idx++;
          }
       }
       else if ('#' == *m) // 001
       {
          // Wildcard position
          m++;
          if (10 >= idx)
          {
            buff[pos++] = '\004';
            buff[pos++] = '0'+idx++;
          }
       }
       else if ('?' == *m)
       {
          // Wildcard position
          m++;
          if (10 >= idx)
          {
            buff[pos++] = '\004';
            buff[pos++] = '0'+idx++;
          }
       }
       else if ('\0' != *m);
       {
          // Literal
          buff[pos++] = *(m++);
       }
    }
    while ('\0' != *m);
    
    buff[pos++] = '\0';
 
    return((char const*)buff);
}






int partialTarget(char const *name)
{
   // Posible Partial target
   
   int      cont   = TRUE;
   int      pos    = 0;
   Ttarget *target = NULL;

   while (cont &&                   // Not found ruleless target
          (Ttarget::len() > (pos = Ttarget::find(name, pos))))
   {
      target = Ttarget::get(pos);
      cont   = (NULL != target->rule());
      pos++;
   }
   
   return ((NULL != target) && (NULL == target->rule()));
}








char const *cleanName(char const *name)
{
    static TstrBuf buff;
    
    unsigned long  len = 0;
    char const    *p   = name;
    
    if ('/' == p[0]) buff[len++] = *p++;
    
    while ('\0' != *p)
    {
       if (('.' == p[0]) && ('/' == p[1]))
       {
          p += 2;
       }
       else if ('/' == p[0])
       {
          p += 1;
       }
       else
       {
          while (('\0' != *p) && ('/' != *p))
          {
             buff[len++] = *p++;
          }
          
          if ('/' == *p) buff[len++] = *p++;
       }
    }
    
    buff[len++] = '\0';
    
    return ((char const*)buff);
}





static TadrStack targetStack;


int recursive (char const *dep)
{
   for (int idx = ((int)targetStack.depth())-1; (0 <= idx); idx--)
   {
      if (*((Tstr*)targetStack.peek(idx)) == dep)
         return(TRUE);
   }
   
   return(FALSE);
}



int date (char const *fullName)
{
   static TtargetStmpLst targetList;
   
   TtargetStamp *targetRef;

   int      stamp  = IRELEVENT;
   int      pos    = 0;
   Ttarget *target = NULL;
   
   char const *p;
   
   // Check for the DUMMY dependent
   if (!strcmp(fullName, NONE_DEP))  return(NO_FILE);
   if (!strcmp(fullName, DUMMY_DEP)) return(OLD_FILE);
   if (!strcmp(fullName, FORCE_DEP)) return(NEW_FILE);
   
   // Clean up the target name
   Tstr        buffer = cleanName(fullName);
   char const *name   = (char const*)buffer;
   
   // If the file exists - Check for an old target
   if ((OLD_FILE < timeStamp(name)) &&
       (NULL != (targetRef = targetList.find(name))))
   {
      printD("Already done '%s'\n", name);
      return(targetRef->stamp());
   }
   
   // Log this target
   targetStack.push((void*)new Tstr(name));
   
   int actualStamp = timeStamp(name);
   
   // Check for partial targets
   if ((NO_FILE < actualStamp)                      &&
       (NULL != (p = Tenv::appGet("SIRE_PARTIAL"))) &&
       (*p == 'y') &&
       partialTarget(name))
   {
      printD("Partial target '%s'\n", name);
      return (actualStamp);
   }

   if (TnameTarget::len() > (pos = TnameTarget::find(name, pos)))
   {
      // Explicite target

      target = TnameTarget::get(pos);
      stamp  = target->date(name);
      
      if (IRELEVENT == stamp) return(BAD_FILE);        // Rule fails
      if (NO_FILE   == stamp) stamp = NEW_FILE; //timeStamp(NULL); // No target file produced
   }
   else
   {
      // Posible Implicite target
      
      pos   = 0;
      stamp = IRELEVENT;
      while (TmatchTarget::len() > (pos = TmatchTarget::find(name, pos)))
      {
         target  = TmatchTarget::get(pos);
         int stp = target->date(name);
         if (((BAD_FILE != stp)) && (target->rule()->continueTarget() || (IRELEVENT == stp)))
         {
            if (stp > stamp) stamp = stp;
         }
         else
         {
            stamp = stp;
            break;
         }
         if (target->rule()->breakHere()) break;
         pos++;
      }
   }
      
   if (IRELEVENT == stamp)
   {
       printD("No target '%s'\n", name);
       stamp = timeStamp(name); // All rules fail
   }
   
   // Add the target name with time stamp to the list
   if (NULL != (targetRef = targetList.find(name)))
      targetList.del(targetRef);
   
   targetList.add(new TtargetStamp(name, stamp));
   
   // Clear this target
   delete (Tstr*) targetStack.pop();
 
   return(stamp);
}


/************* END OF FILE **************************************************/















