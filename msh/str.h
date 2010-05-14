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

CLASS
   Tstr
   
DESC
   This is the standard string storage class. The class was originally
   based on the Tstr class in the generic class archive (gen.a).
   
FILE
   str.h
   
*****************************************************************************/


#ifndef STR
#define STR

#include "sys.h"


char const *strText(char const *str);
int strEqual(char const *s1, char const *s2);
int strNEqual(char const *s1, char const *s2);


class Tstr
{
public:

   //////////
   //
   // Copy constructor
   //
   Tstr (Tstr const &str);
   
   //////////
   //
   // Initialise a new object from litteral text
   // A copy of the text is held.
   //
   Tstr (char const *text = (char const*)NULL);
   
   //////////
   //
   // Destructor frees any allocated space
   //
   ~Tstr();
   
   //////////
   //
   // Return a litteral text representation of the
   // string. The return value is invalidated by any
   // modification of the object. The const should
   // not be cast away since modification of the
   // text bay corrupt the object.
   //
   operator char const * () const;
   
   //////////
   //
   // Assign a new value to this object.
   //
   Tstr &operator =  (char const *str);
   
   //////////
   //
   // Assign a new value to this object.
   //
   Tstr &operator =  (Tstr const &str);

   //////////
   //
   // Append to the string.
   //
   Tstr &operator +=  (char const *str);
   
   //////////
   //
   // Compare the value with a litteral text value.
   //
   int   operator == (char const *str) const;
   
   //////////
   //
   // Compare two object values.
   //
   int   operator == (Tstr &str) const;
   
   //////////
   //
   // Compare the value with a litteral text value.
   //
   int   operator != (char const *str) const;
   
   //////////
   //
   // Compare the value with a litteral text value.
   //
   int   operator != (Tstr &str) const;
   
   //////////
   //
   // Obtain the charactor at position idx in the string.
   // There is no range checking so idx values outside the
   // range of the string will return corrupt values.
   //
   char  operator [] (long idx) const;
   
   //////////
   //
   // The length of the string as for strlen()
   //
int   len() const;

private:

   
   char *Dtext;

   void  stringFree(char *s);
   char *stringAlloc(char const *s);
};

inline Tstr::Tstr (Tstr const &str) : Dtext(stringAlloc(str.Dtext)) {}

inline Tstr::Tstr (char const *text) : Dtext(stringAlloc(text)) {}

inline Tstr::~Tstr() {stringFree(Dtext);}
   
inline Tstr::operator char const * () const {return(Dtext);}
   
inline Tstr &Tstr::operator = (char const *str)
{
   stringFree(Dtext);
   Dtext = stringAlloc(str);
   return(*this);
}

inline Tstr &Tstr::operator = (Tstr const &str)
{
   stringFree(Dtext);
   Dtext = stringAlloc(str.Dtext);
   return(*this);
}

inline int Tstr::operator == (Tstr &str) const
{
   return(strEqual(Dtext, str.Dtext));
}
   
inline int Tstr::operator == (char const *str) const
{
   return(strEqual(Dtext, str));
}

inline int Tstr::operator != (Tstr &str) const
{
   return(strNEqual(Dtext, str.Dtext));
}
   
inline int Tstr::operator != (char const *str) const
{
   return(strNEqual(Dtext, str));
}
   
inline char Tstr::operator [] (long idx) const
{
   return((NULL != Dtext)?(Dtext[idx]):('\0'));
}
   
inline int Tstr::len() const
{
   if (NULL == Dtext)
      return(0);
   else
      return(strlen(Dtext));
}



#endif
