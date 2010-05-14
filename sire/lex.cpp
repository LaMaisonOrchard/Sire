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

   File  : lex.cpp
   Class : Tlex
   Desc  : A description

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"
#include "strbuf.h"
#include "sys.h"
#include "text.h"
#include "env.h"



/************* FUNCTIONS ****************************************************/


Tlex::~Tlex() {}


Ttoken *Tlex::popToken()
{
   Ttoken  *token = NULL;
   
   if (!DtokenStack.empty())
   {
      token = DtokenStack.pop();
   }
   
   return(token);
}


void Tlex::putBackToken(Ttoken *token)
{
   DtokenStack.push(token);
}


void Tlex::substitute(char const *var)
{
   char const *val = getEnv()->get(var);
      
   if (NULL != val) Dinput.putBack(val);
}



#if defined (INTEL_NT)

// Hacky stub


void Tlex::expand(char const *item, DIR *dir)
{
   static char command[1024];
   static char cwd    [512];
   char *p  = NULL;
   FILE *fp = NULL;
   
   sprintf(command, "echo %s", item);
   
   if (NULL != (fp = popen(command, "r")))
   {
      while (NULL != fgets(cwd, 512, fp))
      {
         p = cwd + strlen(cwd) -1;
         if ('\n' == *p) *p = '\0';
         for (p = cwd; (isspace(*p)); p++)
         if ('\0' != *p)
         {
            putBackToken(new Ttoken(TOK_IDENT, p));
         }
      }
    
      fclose(fp);
   }
}

#else

// Expand the item, to match with the directory structure
// See directory(3C)

void Tlex::expand(char const *item, DIR *dir)
{
   static char     cwd[512];
   int             idx = 0;
   TstrBuf         elem;
   DIR            *df  = NULL;
   DIR            *ndf = NULL;
   char const     *p;
   struct dirent  *q;
   
   if (NULL == (p = strchr(item, '/')))
   {
      // End of the list
      
      if (NULL == dir)
         df = opendir(".");
      else
         df = dir;
      
      if (NULL != df)
      {
         elem = item; 
         idx  = strlen(cwd);
         while (NULL != (q = readdir(df)))
         {
            if (wildMatch(q->d_name, (char const*)elem))
            {
               strcat(cwd, q->d_name);
               putBackToken(new Ttoken(TOK_IDENT, cwd));
        
               //Reset the text
               cwd[idx] = '\0';
            }
         }
         if (NULL == dir) closedir(df);
      }
   }
   else if (p == item)
   {
      // Root dir
      strcpy(cwd, "/");
      if (NULL != (df = opendir("/")))
      {
         printf("%s\n",cwd);
         expand(p+1, df);
         closedir(df);
      }
   }
   else
   {
      // Relative dir
      
      if (NULL == dir)
         df = opendir(".");
      else
         df = dir;
      
      if (NULL != df)
      {
         idx = strlen(cwd);
         int pos = 0;
         while (item != p) elem[pos++] = *(item++);
         elem[pos++] = '\0';
      
         while (NULL != (q = readdir(df)))
         {
            if (wildMatch(q->d_name, (char const*)elem))
            {
                strcat(cwd, q->d_name);
                if (NULL != (ndf = opendir(cwd)))
                {
                   strcat(cwd, "/");
                   expand(p+1, ndf);
                   closedir(ndf);
                }
        
                //Reset the text
                cwd[idx] = '\0';
            }
         }
      
         if (NULL == dir) closedir(df);
      }
   }
}

#endif



/************* END OF FILE **************************************************/

