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

*****************************************************************************/


#include <stdlib.h>
#include <stdio.h>

#include "str.h"

#include "list.h"





Tlist::Tlist(cmpFunc_t sortFunc, cmpFunc_t idxFunc)
{
   Dtable   = NULL;
   Dlength  = 0;
   Dsize    = 0;
   Dpos     = 0;
   DcmpFunc = sortFunc;
   DidxFunc = idxFunc;
}



Tlist::Tlist(Tlist &list)
{
   Dlength  = list.Dlength;
   Dpos     = list.Dpos;
   DcmpFunc = list.DcmpFunc;
   DidxFunc = list.DidxFunc;

   Dsize    = Dlength;
   Dtable   = new void*[Dsize];
   for (int i = 0; (i < Dlength); i++) Dtable[i] = list.Dtable[i];
}






Tlist::~Tlist()
{
   if (NULL != Dtable) delete Dtable;
}






void Tlist::add(void *data)
{
   if (Dlength >= Dsize)
   {
      Dsize = Dlength + 100;
      void **tmp = new void*[Dsize];
      if (NULL != Dtable)
      {
         for (int i = 0; (i < Dlength); i++) tmp[i] = Dtable[i];
         delete Dtable;
      }

      Dtable = tmp;
   }

   if (NULL != DcmpFunc)
   {
      int i = getPos(data, DcmpFunc);
      for (int j = Dlength; (j > i); j--) Dtable[j] = Dtable[j-1];
      Dtable[i] = data;
   }
   else
   {
      Dtable[Dlength] = data;
   }
   Dlength++;
}




void Tlist::del(void *data)
{
    int i;
    for (i = 0; (i < Dlength); i++)
       if (Dtable[i] == data)
          break;

    if (i >= Dlength) printf("List Del : Not found\n");

    int j;
    for (j = i +1; (j < Dlength); j++)
       Dtable[j - 1] = Dtable[j];

    if (i < Dlength)
    {
       if (Dpos >= i) Dpos--;
       Dlength--;
    }

    int k;
    for (k = 0; (k < Dlength); k++)
       if (Dtable[k] == data)
          printf("Not deleteed !!!!!!!!!\n");
}







void Tlist::insert(int idx, void *data)
{
   DcmpFunc = NULL;
   DidxFunc = NULL;

   if (Dlength >= Dsize)
   {
      Dsize = Dlength + 100;
      void **tmp = new void*[Dsize];
      if (NULL != Dtable)
      {
         for (int i = 0; (i < Dlength); i++) tmp[i] = Dtable[i];
         delete Dtable;
      }

      Dtable = tmp;
   }

   if (0 > idx)       idx = 0;
   if (Dlength < idx) idx = Dlength;

   for (int j = Dlength; (j > idx); j--) Dtable[j] = Dtable[j-1];
   Dtable[idx] = data;
   Dlength++;
}




void *Tlist::remove(int idx)
{
   void *data = NULL;

   if ((0 <= idx) && (Dlength > idx))
   {
      data = Dtable[idx];

      for (int j = idx +1; (j < Dlength); j++)
         Dtable[j - 1] = Dtable[j];

      if (Dpos >= idx) Dpos--;
      Dlength--;
   }
   return(data);
}




void *Tlist::next()
{
   void *data;

   if (Dlength > Dpos)
   {
      data = Dtable[Dpos];
      Dpos++;
   }
   else
   {
      data = NULL;
   }

   return(data);
}




int Tlist::getPos(void *data, cmpFunc_t func, int *found)
{
   int bot = 0;
   int top = Dlength -1;
   int mid = 0;
   int cmp = 0;

   if (NULL != found) *found = FALSE;

   while (bot <= top)
   {
      mid = (bot + top)/2;
      cmp = (*func)(Dtable[mid], data);

      if (0 == cmp)
      {
         bot = mid;
         top = mid -1;
      }
      else if (0 > cmp)
         bot = mid +1;
      else
         top = mid -1;
   }

   if (bot < Dlength)
      if (0 > (*func)(Dtable[bot], data))
         bot++;

   if ((bot < Dlength) && (NULL != found))
      *found = (0 == (*func)(Dtable[bot], data));

   return(bot);
}





void *Tlist::find(void *data, cmpFunc_t idxFunc)
{
   int i;
   int found = TRUE;

   if (NULL == idxFunc)
   {
      if (NULL == DidxFunc)
      {
         printf("List : Bad find() [No Index function]\n");
         i = Dlength;
      }
      else
      {
         i = getPos(data, DidxFunc, &found);
         if (!found) i = Dlength;
      }
   }
   else
   {
      for (i = 0; (i < Dlength) && (0 != (*idxFunc)(Dtable[i], data)); i++);
   }

   if (i < Dlength)
      return(Dtable[i]);
   else
      return(NULL);
}




void *Tlist::operator [] (int idx)
{
   void *data;

   if ((Dlength > idx) && (0 <= idx))
      data = Dtable[idx];
   else
      data = NULL;

   return(data);
}



static cmpFunc_t cmpFnc = NULL;

static int localCmpFnc(const void *A, const void *B)
{
   return((*cmpFnc)(*(void**)A, *(void**)B));
}



void Tlist::set(void *data, int idx)
{
   if ((0 <= idx)&&(Dlength > idx))
   {
      Dtable[idx] = data;
      
      if ((0 < Dlength) && (NULL != DcmpFunc))
         qsort(Dtable,Dlength,sizeof(void*),localCmpFnc);
   }
}


void Tlist::sort(cmpFunc_t sortFunc, cmpFunc_t idxFunc)
{
   DcmpFunc = sortFunc;
   DidxFunc = idxFunc;
   cmpFnc   = sortFunc;
   if ((0 < Dlength) && (NULL != DcmpFunc))
      qsort(Dtable,Dlength,sizeof(void*),localCmpFnc);
}
