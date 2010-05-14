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



#ifndef LIST
#define LIST

typedef int (*cmpFunc_t)(void *, void *);

class Tlist
{
public:

   Tlist(cmpFunc_t sortFunc = NULL, cmpFunc_t idxFunc = NULL);
   Tlist(Tlist &list);
   ~Tlist();

   void    add(void *data);
   void    del(void *data);
   void    set(void *data, int idx);

   void    insert(int idx, void *data);
   void   *remove(int idx);

   void    start();
   void   *next();
   void   *find(void *data, cmpFunc_t idxFunc = NULL);
   void   *operator [] (int);

   int     len();

   void    sort(cmpFunc_t sortFunc = NULL, cmpFunc_t idxFunc = NULL);
   
private:
   void      **Dtable;
   int         Dlength;
   int         Dsize;
   int         Dpos;
   cmpFunc_t   DcmpFunc;
   cmpFunc_t   DidxFunc;

   int getPos(void *data, cmpFunc_t func, int *found = NULL);
};


inline   void    Tlist::start() {Dpos = 0;}
inline   int     Tlist::len() {return(Dlength);}


#define for_each(list)     for(int z = list->len() -1; (0 <= z); z--)
#define get_mbr(list,type) type list[z]





#endif
