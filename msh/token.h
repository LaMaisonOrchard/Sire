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

   File  : token.h
   Class : Ttoken
   Desc  : A description

EDIT 001 18-Apr-96 davido
Support for 'which' expansions.
       
EDIT 002 16-Jan-97 davido
Support TOUCH rules.
       
EDIT 003 29-Apr-97 davido
Support CREATE rules.
       
EDIT 004 20-Mar-99 davido
Add the set statement.

EDIT 005 06-Aug-02 davido@errol.org.uk
Add in modifier USER to specify files not to be deleted.

*****************************************************************************/


#ifndef TOKEN
#define TOKEN


#include "str.h"

enum EtokenType {TOK_ILLEGAL = 1,
                 TOK_END,
                 TOK_IDENT,
                 TOK_TEXT,
                 TOK_CALL,
                 TOK_WHICH,       // 001 
                 TOK_ASIGN,
                 TOK_COLON,
                 TOK_SEMI_COLON,
                 TOK_COMMENT,
                 TOK_MATCH,

                 TOK_EQUAL,
                 TOK_NOT_EQUAL,
                 TOK_AND,
                 TOK_OR,
                 
                 TOK_SET,
                 TOK_SOURCE,
                 TOK_OPTIONAL,
                 TOK_SPACE,
                 TOK_OPEN,
                 TOK_CLOSE,
                 TOK_BEGIN,
                 TOK_NIGEB,
                 TOK_IF,
                 TOK_ELSE,
                 TOK_ELIF,
                 TOK_ENDIF,
                 TOK_SYBM,
                 
                 TOK_STATE,
                 TOK_BLOCK,
                 TOK_SUB_COMM,
                 TOK_KEY_PREAMBLE,
                 TOK_KEY_POSTAMBLE,
                 TOK_KEY_PARTIAL,
                 TOK_MODIFY_USER,
                 TOK_MODIFY_BREAK,  // 005
                 TOK_MODIFY_CONT,
                 TOK_MODIFY_RECURE,
                 TOK_MODIFY_ONCE,
                 TOK_MODIFY_TOUCH,    // 002
                 TOK_MODIFY_CREATE }; // 003

enum EtokenCat  {TOK_CAT_UNDEF = 0,
                 TOK_CAT_STATE };


class Ttoken
{
public:

   Ttoken (EtokenType type = TOK_ILLEGAL, char const *text = "");
   Ttoken (Ttoken &tok);
   ~Ttoken ();
   
   EtokenType  type();
   char const *text();
   char const *posn();
   
   void setType(EtokenType  type);
   void setText(char const *str);
   void setPosn(char const *str);
   
protected:

private:

   EtokenType Dtype;
   EtokenCat  Dcat;
   Tstr       Dtext;
   Tstr       Dposn;
};


   
inline EtokenType  Ttoken::type() {return(Dtype);}
inline char const *Ttoken::text() {return((char const*)Dtext);}
inline char const *Ttoken::posn() {return((char const*)Dposn);}
   
inline void Ttoken::setType(EtokenType type)  {Dtype = type;}
inline void Ttoken::setText(char const *str) {Dtext = str;}
inline void Ttoken::setPosn(char const *str) {Dposn = str;}

inline Ttoken::Ttoken (EtokenType type, char const *str) : Dtype(type), Dcat(TOK_CAT_UNDEF), Dtext(str), Dposn("") {}
inline Ttoken::Ttoken (Ttoken &tok)                      : Dtype(tok.Dtype), Dcat(tok.Dcat), Dtext(tok.Dtext), Dposn(tok.Dposn) {}

inline Ttoken::~Ttoken () {}



#endif
