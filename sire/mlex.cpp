/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2002  David W Orchard (davido@errol.org.uk)

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

   File  : mlex.cpp
   Class : Tmlex
   Desc  : A description

EDIT 001 18-Apr-96 davido
Support for 'which' expansions.
       
EDIT 002 16-Jan-97 davido
Support TOUCH rules.
       
EDIT 003 29-Apr-97 davido
Support CREATE rules.
       
EDIT 004 17-Jun-98 davido
Support sourcing an enviroment setup.
       
EDIT 005 12-Jul-99 davido
Support index variables $(0-9) and the '#' wild card.

EDIT 006 07-Sep-99 davido
Implement late substitution for blocks.

EDIT 007 18-Jul-00 davido@errol.org.uk
Add in support for include files

EDIT 008 06-Aug-02 davido@errol.org.uk
Add in modifier USER to specify files not to be deleted.

EDIT 009 14-Nov-02 davido@errol.org.uk
Support nested variable expansion.

EDIT 010 15-Nov-02 davido@errol.org.uk
Support script arguments

EDIT 011 18-Oct-03 davido@errol.org.uk
Add new token type Sub Command.

EDIT 012 14-Sept-08 davido@errol.org.uk
Add additional conditional tokens.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mlex.h"
#include "strbuf.h"
#include "sys.h"
#include "text.h"
#include "env.h"
#include "token.h"


char Tmlex::Dbuff[1024];

int Tmlex::isIdent(int ch) {return(isalnum(ch) || (NULL != strchr(".:*?[]#+,\\/_-\001\004", ch)));}
int Tmlex::isSymb (int ch) {return(NULL != strchr("=&()|!", ch));}

/************* FUNCTIONS ****************************************************/


Ttoken *Tmlex::getToken()
{
   Ttoken        *token   = NULL;
   TstrBuf        text    = "";
   unsigned long  textIdx = 0;
   int            found = 0;
   
   if (NULL != (token = popToken()))
   {
      return(token);
   }
   else
   {
      token = new Ttoken;
   }
   
   token->setType(TOK_ILLEGAL);
   
   while (!found)
   {
      int ch = Dinput.get();
      if (EOF != ch) text[textIdx++] = ch;

      if (EOF == ch)
      {
         // End of file
         token->setType(TOK_END);
         token->setText("");
         token->setPosn(Dinput.posn());
         found = 1;
      }
      else if ('\n' == ch)
      {
         token->setType(TOK_EOL);
         token->setText("");
         found = 1;
      }
      else if (isspace(ch))
      {
         // Ignore white space
      }
      else if (SEP_ESC == ch)
      {
         // excape
         ch = Dinput.get();
         
         if ('\n' == ch)
         {
            // ignore the end-of-line
         }
         else if (SEP_ESC == ch)
         {
            // Litteral
            Dinput.putBack(ch);
            token->setPosn(Dinput.posn());
            found = readIdent(*token);
         }
         else
         {
            // Excaped charactor
            Dinput.putBack(ch);
         }
      }
      else if ('#' == ch)
      {
         // Check for comment
         token->setPosn(Dinput.posn());
         ch = Dinput.get();
         
         found = readComment(*token);
      }
      else if (SEP_TEXT == ch)
      {
         // Text
         token->setPosn(Dinput.posn());
         found = readText(*token);
      }
      else if (SEP_S_VAR == ch)
      {
         // Start an evaluated variable (define)
         readVar();
      }
      else if (SEP_CALL == ch)
      {
         // Call
         token->setPosn(Dinput.posn());
         found = readCall(*token);
      }
      else if (isSymb(ch))
      {
         // Symbol
         token->setPosn(Dinput.posn());
         Dinput.putBack(ch);
         found = readSymb(*token);
      }
      else if (isIdent(ch))
      {
         // Ident
         token->setPosn(Dinput.posn());
         Dinput.putBack(ch);
         found = readIdent(*token);
      }
      else
      {
         while ((EOF != ch) && !isspace(ch)) ch = Dinput.get();
         if (EOF != ch) Dinput.putBack(ch);
      }
   }
   
   return(token);
}


int Tmlex::readSymb(Ttoken &token)
{
   int   ch   = Dinput.get();
   char  tmp[2];

   tmp[0] = ch;
   tmp[1] = '\0';
   token.setText(tmp);

   switch (ch)
   {
      case '=' :
        {
            ch = Dinput.get();
            if ('=' == ch)
            {
                token.setType(TOK_EQUAL);
            }
            else
            {
                Dinput.putBack(ch);
                token.setType(TOK_ILLEGAL);
            }
        }
        break;

      case '!' :
        {
            ch = Dinput.get();
            if ('=' == ch)
            {
                token.setType(TOK_NOT_EQUAL);
            }
            else
            {
                Dinput.putBack(ch);
                token.setType(TOK_ILLEGAL);
            }
        }
        break;

      case '&' :
        {
            ch = Dinput.get();
            if ('&' == ch)
            {
                token.setType(TOK_AND);
            }
            else
            {
                Dinput.putBack(ch);
                token.setType(TOK_ILLEGAL);
            }
        }
        break;

      case '|' :
        {
            ch = Dinput.get();
            if ('|' == ch)
            {
                token.setType(TOK_OR);
            }
            else
            {
                Dinput.putBack(ch);
                token.setType(TOK_COLON);
            }
        }
        break;

      case '(' : token.setType(TOK_OPEN);       break;
      case ')' : token.setType(TOK_CLOSE);      break;
      default  : token.setType(TOK_ILLEGAL);    break;
   }
   
   return(TOK_ILLEGAL != token.type());
}


int Tmlex::readIdent(Ttoken &token)
{
   static TstrBuf buff;
   
   int            ch    = Dinput.get();
   Tstr           posn  = Dinput.posn();
   unsigned long  pos   = 0;
   int            mType = 0;

   while (isIdent(ch) || (SEP_S_VAR == ch))
   {
      if (SEP_S_VAR == ch)
      {
         // Posible var
         if (!readVar())
            buff[pos++] = ch;
      }
      else if ('\\' == ch)
      {
         // Excape any special (or non-special charactors)
         
         ch = Dinput.get();
         if (isIdent(ch))
         {
            buff[pos++] = ch;
         }
         else
         {
            Dinput.putBack(ch); 
         }
      }
      else
      {
         mType = (mType       ||
                  ('+' == ch) ||
                  ('#' == ch) ||
                  ('*' == ch) ||
                  ('?' == ch) ||
                  ('[' == ch) ||
                  (']' == ch) ||
                  ('\004' == ch));
         buff[pos++] = ch;
      }
      ch = Dinput.get();
   }
   buff[pos++] = '\0'; 
   
   Dinput.putBack(ch);
   
   if (mType)
   {
      token.setType(TOK_MATCH);
   }
   else
   {
      // This may be a state command
      if (!strcmp(buff, "echo"))
      {
         token.setType(TOK_ECHO);
      }
      else if (!strcmp(buff, "cd"))
      {
         token.setType(TOK_CHANGE_DIR);
      }
      else if (!strcmp(buff, "pwd"))
      {
         token.setType(TOK_PRINT_DIR);
      }
      else if (!strcmp(buff, "exit"))
      {
         token.setType(TOK_EXIT);
      }
      else if (!strcmp(buff, "IF"))          // 012
      {
         token.setType(TOK_IF);
      }
      else if (!strcmp(buff, "ELSE"))        // 012
      {
         token.setType(TOK_ELSE);
      }
      else if (!strcmp(buff, "ELIF"))        // 012
      {
         token.setType(TOK_ELIF);
      }
      else if (!strcmp(buff, "FI"))          // 012
      {
         token.setType(TOK_ENDIF);
      }
      else if (!strcmp(buff, "NOT"))         // 012
      {
         token.setType(TOK_NOT);
      }
      else if (!strcmp(buff, "IsFILE"))         // 012
      {
         token.setType(TOK_FILE);
      }
      else if (!strcmp(buff, "IsDIR"))         // 012
      {
         token.setType(TOK_DIR);
      }
      else if (!strcmp(buff, "IsEXE"))         // 012
      {
         token.setType(TOK_EXECUTABLE);
      }
      else if (!strcmp(buff, "EXISTS"))         // 012
      {
         token.setType(TOK_EXISTS);
      }
      else
      {
         token.setType(TOK_IDENT);
      }
   }
      
   token.setText((char const*)buff); 
   
   return(1);
}


int Tmlex::readText(Ttoken &token)
{
   static TstrBuf buff;
   
   int           ch   = Dinput.get();
   Tstr          posn = Dinput.posn();
   unsigned long pos = 0;
   
   while ((EOF != ch) && ('\n' != ch) && (SEP_TEXT != ch))
   {
      if (SEP_ESC == ch)                              // 005
      {
         // Escaped Charactor
         ch = Dinput.get();
         if ((EOF != ch) && ('\n' != ch))
         {
            switch (ch)
            {
               case 'n' : buff[pos++] = '\n'; break;
               case 'r' : buff[pos++] = '\r'; break;
               case 't' : buff[pos++] = '\t'; break;
               case 'v' : buff[pos++] = '\v'; break;
               case 'b' : buff[pos++] = '\b'; break;
               default  : buff[pos++] =  ch;  break;
            }
         }
      }
      else if (SEP_S_VAR == ch)
      {
         // Posible var
         if (!readVar()) buff[pos++] = ch;
      }
      else if (SEP_CALL == ch)                       // 005
      {
         // Call argument
         Ttoken token;
         token.setPosn(Dinput.posn());
         readCall(token);
         Dinput.push(new Tpipe(token.text()));
      }
      else if (SEP_WHICH_OPEN == ch)                // 005
      {
         // Posible which argument
         Ttoken token;
         token.setPosn(Dinput.posn());
         Dinput.putBack(findExecs(token.text()));
      }
      else
      {
         buff[pos++] = ch;
      }
      ch = Dinput.get();
   }
   buff[pos++] = '\0';
   
   if (SEP_TEXT != ch)
   {
      printE("%s : Unterminated text\n", (char const*)posn);
   }
   
   token.setType(TOK_TEXT);
   token.setText((char const*)buff);
   
   return(1);
}


int Tmlex::readCall(Ttoken &token)
{
   int   ch   = Dinput.get();
   Tstr  posn = Dinput.posn();
   char *pos  = Dbuff;
   
   while ((EOF != ch) && ('\n' != ch) && (SEP_CALL != ch))
   {
      if (SEP_S_VAR == ch)
      {
         // Posible var
         if (!readVar()) *(pos++) = ch;
      }
      else
      {
         *(pos++) = ch;
      }
      ch = Dinput.get();
   }
   *pos = '\0';
   
   if (SEP_CALL != ch)
   {
      printE("%s : Unterminated text\n", (char const*)posn);
      if (EOF != ch) Dinput.putBack(ch); 
   }
   
   token.setType(TOK_CALL);
   token.setText(Dbuff);
   
   return(1);
}


int Tmlex::readComment(Ttoken &token)
{
   int   ch   = Dinput.get();
   Tstr  posn = Dinput.posn();
   char *pos  = Dbuff;
   
   while ((EOF != ch) && ('\n' != ch))
   {
      *(pos++) = ch;
      ch       = Dinput.get();
   }
   *pos = '\0';
   
   token.setType(TOK_EOL);
   token.setText(Dbuff);
   
   return(1);
}


int Tmlex::readVar()
{
   TstrBuf buff;
   
   int           ch   = Dinput.get();
   Tstr          posn = Dinput.posn();
   unsigned long pos  = 0;
   
   if ('{' == ch)
   {
      ch = Dinput.get();
      while (isIdent(ch) || (SEP_S_VAR == ch)) // 009
      {
         if (SEP_S_VAR == ch)                  // 009
         {
             readVar();  // Recursivly expand the variable
         }
         else
         {
             buff[pos++] = ch;
         }
         ch = Dinput.get();
      }
      buff[pos++] = '\0';
      
      if ('}' != ch)
      {
         // Bad variable name
         Dinput.putBack((char const*)buff);
         Dinput.putBack('{');
         return(0);
      }
      else
      {
         // Look up the variable and do a put back on its value
         substitute((char const*)buff);
         
         return(1);
      }
   }
   else
   {
      // Bad variable name
      Dinput.putBack(ch);
      return(0);
   }
}



/************* END OF FILE **************************************************/

