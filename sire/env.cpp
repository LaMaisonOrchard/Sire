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

   File  : sys.cpp
   Class : NONE
   Desc  : A description
   
AMENDMENTS

EDIT 001 23-Jul-99 davido
Add a version of setApp() to read the environment from a file.

EDIT 002 15-Nov-02 davido@errol.org.uk
Add support for application argument lists

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys.h"
#include "str.h"
#include "env.h"
#include "token.h"


#ifndef SUN4
#define malloc_t       char *
#endif

#if defined (INTEL_NT)

int envcmp(char const *a, char const *b)
{
   while (('\0' != *a) && ('\0' != *b) && (toupper(*a) == toupper(*b)))
   {
      a++;
      b++;
   }
   
   return (toupper(*a) - toupper(*b));
}

#else

#define envcmp strcmp

#endif

/************* FUNCTIONS ****************************************************/


Tenv    Tenv::DappEnvp;
Tstr   *Tenv::DappList = NULL;
int     Tenv::Dapplen  = 0;


Tenv::Tenv() : Dtarget(""), Dfirst(""), Ddeps(""), Dset(FALSE), Denvp(NULL)
{
   // If there is an application environment
   // N.B. This covers the constructor for DappEnvp itself.
   if (NULL != DappEnvp.Denvp)
   {
      char **p = NULL;
      char **q = NULL;
      int    i = 0;
   
      for (p = DappEnvp.Denvp; (NULL != *p); p++) i++;
   
      Denvp = q = (char**)malloc((i+1)*sizeof(char*));
   
      for (p = DappEnvp.Denvp; (NULL != *p); p++, q++)
      {
         *q = (char*)malloc(strlen(*p) +1);
         strcpy(*q, *p);
      }
   
      *q = NULL;
   }
   
   // If there is an standard environment
   // N.B. This covers the constructor for DappEnvp itself.
   if (DappEnvp.Dset)
   {
      Dtarget = DappEnvp.Dtarget;
      Dfirst  = DappEnvp.Dfirst;
      Ddeps   = DappEnvp.Ddeps;
      Dset    = DappEnvp.Dset;
   }
}


Tenv::Tenv(Tenv const &env) : Dtarget(""), Dfirst(""), Ddeps(""), Dset(FALSE), Denvp(NULL)
{
   if (this != &env)
   {
      if (NULL != env.Denvp)
      {
         char **p = NULL;
         char **q = NULL;
         int    i = 0;
   
         for (p = env.Denvp; (NULL != *p); p++) i++;
   
         Denvp = q = (char**)malloc((i+1)*sizeof(char*));
   
         for (p = env.Denvp; (NULL != *p); p++, q++)
         {
            *q = (char*)malloc(strlen(*p) +1);
            strcpy(*q, *p);
         }
   
         *q = NULL;
      }
      
      Dset = env.Dset;
      
      if (Dset)
      {
         Dtarget = env.Dtarget;
         Dfirst  = env.Dfirst;
         Ddeps   = env.Ddeps;
      }
   }
}


Tenv::Tenv(char const * const *envp) : Dtarget(""), Dfirst(""), Ddeps(""), Dset(FALSE), Denvp(NULL)
{
   if (NULL != envp)
   {
      char const * const  *p = NULL;
      char               **q = NULL;
      int                  i = 0;

      for (p = envp; (NULL != *p); p++) i++;
   
      Denvp = q = (char**)malloc((i+1)*sizeof(char*));
   
      for (p = envp; (NULL != *p); p++, q++)
      {
         *q = (char*)malloc(strlen(*p) +1);
         strcpy(*q, *p);
      }
   
      *q = NULL;
   }
}


Tenv::Tenv(FILE *fp) : Dtarget(""), Dfirst(""), Ddeps(""), Dset(FALSE), Denvp(NULL)
{
    if (NULL != fp)
    {
        char  *p;
        char   buff[1024];
 
        int size = 64;
        Denvp = (char**)malloc(size*sizeof(char*));
 
        int pos = 0;
        while (NULL != (p = fgets(buff, sizeof(buff), fp)))
        {
            // Remove any leading space
            while (isspace(*p)) p++;
 
            // Remove any trailing return
            int i = strlen(buff);
            if ('\n' == buff[i-1]) buff[1-1] = '\0';
            if ('\r' == buff[i-2]) buff[i-2] = '\0';
 
            if ('\0' != *p)
            {
                // Not a blank line so add this environment value
 
                // Check for enophe space in the table
                if (pos >= (size-1))
                {
                    size *= 2;
                    Denvp = (char**)realloc(Denvp, size*sizeof(char*));
                }
                
                // Decode special charactors
                for (char *q = p; ('\0' != *q); q++)
                {
                    if ('\1' == *q) *q = '\n';
                    if ('\2' == *q) *q = '\r';
                }
 
                // Add the environment variable to the table
                int len = strlen(p);
                if ('\n' == p[len]) p[len] = '\0';  // Remove the new-line
                Denvp[pos] = (char*)malloc(len +1);
                strcpy(Denvp[pos], p);
 
                // Increment the position
                pos++;
            }
        }
   
        Denvp[pos] = NULL;
    }
}


Tenv::~Tenv()
{
   clear();
}


void Tenv::setApp(char const * const *envp)
{
   char const * const  *p = NULL;
   char               **q = NULL;
   int                  i = 0;

   // Free the existing space
   
   DappEnvp.clear();
   
   // Get the required size of the new table
   
   for (p = envp; (NULL != *p); p++) i++;
   
   // Make space for the new table
   
   DappEnvp.Denvp = q = (char**)malloc((i+1)*sizeof(char*));
   
   // Fill in the blanks
   
   for (p = envp; (NULL != *p); p++, q++)
   {
      *q = (char*)malloc(strlen(*p) +1);
      strcpy(*q, *p);
   }
   
   *q = NULL;
}


void Tenv::setApp(FILE *fp)  /* 001 */
{
   char  *p;
   char   buff[1024];
   char **q = NULL;

   // Free the existing space
   
   DappEnvp.clear();

   // Construct the new table
   
   int size = 64;
   q = DappEnvp.Denvp = (char**)malloc(size*sizeof(char*));
   
   int pos = 0;
   while (NULL != (p = fgets(buff, sizeof(buff), fp)))
   {
       while (isspace(*p)) p++;
 
       // Remove any trailing return
       int i = strlen(p);
       if ('\n' == p[i-1]) p[i-1] = '\0';
       if ('\r' == p[i-2]) p[i-2] = '\0';
       
       if ('\0' != *p)
       {
           // Not a blank line so add this environment value
           
           // Check for enophe space in the table
           if (pos >= (size-1))
           {
               size *= 2;
               q = DappEnvp.Denvp = (char**)realloc(q, size*sizeof(char*));
           }
                
           // Decode special charactors
           char *r;
           for (r = p; ('\0' != *r); r++)
           {
               if ('\1' == *r) *r = '\n';
               if ('\2' == *r) *r = '\r';
           }
           
           // Add the environment variable to the table
           int len = strlen(p);
           if ('\n' == p[len]) p[len] = '\0';  // Remove the new-line
           q[pos] = (char*)malloc(len +1);
           strcpy(q[pos], p);
           
           // Increment the position
           pos++;
       }
   }
   
   q[pos] = NULL;
}






char  **Tenv::getEnvp()
{
   static char  **envp = NULL;
   char **p;
   char **q;
   int    len;

   if (!Dset)
   {
      return (Denvp);
   }
   else
   { 
      if (NULL != envp)
      {
         // Free the allocated space
         free(envp[0]);
         free(envp[1]);
         free(envp[2]);
      
         free((char*)envp);
         envp = NULL;
      }
     
      // Get the size of the environment
      for (p = Denvp, len = 0; (NULL != *p); p++, len++) {}
   
      // Allocate enophe space for the existing environment plus 3 new variables
      q = envp = (char**)malloc((len +4) * sizeof(char*));
   
      // Add the 3 new variables
   
      *q = (char*)malloc(strlen("TARGET=") + strlen((char const*)Dtarget) +1);
      sprintf(*q, "TARGET=%s", (char const*)Dtarget);
      q++;
   
      *q = (char*)malloc(strlen("FIRST_DEP=") + strlen((char const*)Dfirst) +1);
      sprintf(*q, "FIRST_DEP=%s", (char const*)Dfirst);
      q++;
   
      *q = (char*)malloc(strlen("DEPS=") + strlen((char const*)Ddeps) +1);
      sprintf(*q, "DEPS=%s", (char const*)Ddeps);
      q++;
   
      // Add the rest of the environment
      for (p = Denvp; (NULL != *p); p++, q++)
      {
         *q = *p;
      }
      *q = NULL;
   
      return (envp);
   }
}





void Tenv::set(char const *name, char const *value)
{
   char **p = Denvp;
   
   char *val = (char*)malloc(strlen(name) + strlen(value) +2);
   sprintf(val, "%s=%s", name, value);
   
   if (NULL == p)
   {
      Denvp    = (char**)malloc(2 * sizeof(char*));
      Denvp[0] = val;
      Denvp[1] = NULL;
   }
   else
   {
      int i = 0;
      
      for (; (NULL != *p); p++)
      {
          i++;
          char *c = strchr(*p, '=');
          if (NULL != c)
          {
             // Split the string at the equals
             *c = '\0';
          }
          
          if (0 == envcmp(*p, name))
          {
             // Set this variable
             free(*p);
             *p = val;
             break;
          }
          else
          {
             // Rejoin the string with the equals
             if (NULL != c) *c = '=';
          }
      }
      
      if (NULL == *p)
      {
         Denvp      = (char**)realloc((malloc_t)Denvp, (i+2)*sizeof(char*));
         Denvp[i+0] = val;
         Denvp[i+1] = NULL;
      }
   }
}


void Tenv::unset(char const *name)
{
   char **p = Denvp;
   char **q = NULL;
   
   if (NULL != p)
   {
      for (; (NULL != *p); p++)
      {
          char *c = strchr(*p, '=');
          if (NULL != c)
          {
             *c = '\0';
          
             if (0 == envcmp(*p, name))
             {
                q = p;
                p--;
                
                free(*q);
                
                do
                {
                   *q = *(q+1);
                   q++; 
                   
                } while (NULL != *q);
             }
             else
             {
                if (NULL != c) *c = '=';
             }
         }
      }
   }
}


// Expand simple environment variable expressions

char const *Tenv::get(char const *name)
{
    char       **p = Denvp;
    char const  *v = NULL;
 
    if (Dset)
    {
        if (!envcmp(name, "TARGET"))
        {
            return((char const*)Dtarget);
        }
        else if (!envcmp(name, "DEPS"))
        {
            return((char const*)Ddeps);
        }
        else if (!envcmp(name, "FIRST_DEP"))
        {
            return((char const*)Dfirst);
        }
    }
 
    if (NULL != p)
    {
        for (; (NULL != *p); p++)
        {
            char *c = strchr(*p, '=');
            if (NULL != c)
            {
                *c = '\0';
 
                if (0 == envcmp(*p, name))
                {
                   v = c+1;
                   if (NULL != c) *c = '=';
                   break;
                }
 
                if (NULL != c) *c = '=';
            }
        }
    }
    
    // Is the variable defined in the environment
    
    if (NULL == v)
    {
        // Check for arguments

        if (!strncmp(name, "arg", 3))
        {
            if ('\0' == name[3])
            {
                // Return the number of arguments

                static char lenText[16];
                sprintf(lenText, "%d", Dapplen);
                
                v = lenText;
            }
            else
            {
                char const *p = name+3;
                while (isdigit(*p)) p++;

                if ('\0' == *p)
                {
                    // Return the numbered argument

                    int idx = atoi(name+3);

                    if (idx < Dapplen)
                    {
                        v = (char const*)DappList[idx];
                    } 
                    else
                    {
                        v = "";
                    } 
                }
            }
        }
    }

    return((char const *)v);
}


char const *Tenv::expand(char const *text)
{
   static char buff1[1024];
   static char buff2[1024];

   char const *expanded = text;
   
   if (NULL != strchr(text, '$'))
   {
      expanded = buff2;
      
      char *p = buff1;
      char *q = buff2;
      
      strcpy(buff1, text);
      
      while ('\0' != *p)
      {
         char  ch;
         char *var;
         char *val;
         
         if ('$' == *p)
         {
            p++;
            if ('{' == *p)
            {
               p++;
               var = p;
            
               while (('\0' != *p) && ('}' != *p))
               {
                  p++;
               }
               
               if ('}' == *p) *(p++) = '\0';
            }
            else if ('(' == *p)
            {
               p++;
               var = p;
            
               while (('\0' != *p) && (')' != *p))
               {
                  p++;
               }
               
               if (')' == *p) *(p++) = '\0';
            }
            else
            {
               var = p;
            
               while (('\0' != *p) && (NULL == strchr(" \n\t/:;", *p)))
               {
                  p++;
               }
            }
            
            ch  = *p;
            *p  = '\0';
            val = (char*)get(var);
            *p  = ch;
            
            if (NULL != val)
            {
               strcat(q, val);
               q += strlen(val);
            }
         }
         else if ('\\' == *p)
         {
            p++;
            
            if ('\0' != *p)
               *(q++) = *(p++);
         }
         else
         {
            *(q++) = *(p++);
         }
      }
      
      *(q++) = '\0';
   }

   return(expanded);
}





void Tenv::clear()
{
    // Clear the special settings
   
    Dset = FALSE;
   
    // Clear the rest
   
    if (NULL != Denvp)
    {
        char **q = NULL;
   
        for (q = Denvp; (NULL != *q); q++)
        {
            free(*q);
        }
      
        free ((char*)Denvp);
      
        Denvp = NULL;
    }
}





void Tenv::setStd(char const *target, char const *deps)
{
   // Get the name of the first dependent
   unsigned long  pos = 0;
   char const    *p;
   for (p = deps; isspace(*p); p++) {}
   while (('\0' != *p) && !isspace(*p)) Dfirst[pos++] = *(p++);
   Dfirst[pos++] = '\0';
   
   Dtarget = target;
   Ddeps   = deps;
   Dset    = TRUE;
}

void Tenv::unsetStd()
{
   Dset = FALSE;
}



// Set up the applications argument list

void Tenv::appSetArg (Ttoken **List, int len)  // 002
{
    // Remove the existing list
    
    delete [] DappList;
    DappList = NULL;
    Dapplen  = 0;
    
    // Setup the new list
    
    if ((NULL != List) && (len >0))
    {
        int i;
        
        DappList = new Tstr[len];
        Dapplen  = len;
        
        // Copy the values
        
        for (i = 0; (i < Dapplen); i++)
        {
            DappList[i] = List[i]->text();
        }
    }
}



/************* END OF FILE **************************************************/
















