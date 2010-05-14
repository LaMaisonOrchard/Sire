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

   File  : jlex.cpp
   Class : Tjlex
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
#include "jlex.h"
#include "strbuf.h"
#include "sys.h"
#include "text.h"
#include "env.h"


char Tjlex::Dbuff[1024];

int Tjlex::isIdent(int ch) {return(isalnum(ch) || (NULL != strchr(".:*?[]#+,\\/_-\001\004", ch)));}
int Tjlex::isSymb (int ch) {return(NULL != strchr("=;&()|!", ch));}

/************* FUNCTIONS ****************************************************/


Ttoken *Tjlex::getToken()
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

      if ('%' == ch)
      {
          // Check for preprocessor
          readProc();
      }
      else if (EOF == ch)
      {
         // End of file
         token->setType(TOK_END);
         token->setText("");
         token->setPosn(Dinput.posn());
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
      else if ('/' == ch)
      {
         // Check for comment
         token->setPosn(Dinput.posn());
         ch = Dinput.get();
         
         if ('/' == ch)
         {
            // comment '//'
            found = readComment(*token);
         }
         else
         {
            // Litteral
            Dinput.putBack(ch);
            Dinput.putBack('/');
            found = readIdent(*token);
         }
      }
      else if (SEP_TEXT == ch)
      {
         // Text
         token->setPosn(Dinput.posn());
         found = readText(*token);
      }
      else if (SEP_O_BRK == ch)
      {
         // Block
         token->setPosn(Dinput.posn());
         found = readBlock(*token);
      }
      else if (SEP_O_SUB == ch)
      {
         // Block
         token->setPosn(Dinput.posn());
         found = readSubComm(*token);
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
      else if (SEP_WHICH_OPEN == ch)    // 001
      {
         // Call
         token->setPosn(Dinput.posn());
         found = readWhich(*token);
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
      else if ('\002' == ch)
      {
         printE("%s : Predefined variable $(DEPS) used out of context\n", Dinput.posn(), ch);
         while ((EOF != ch) && !isspace(ch)) ch = Dinput.get();
         if (EOF != ch) Dinput.putBack(ch);
      }
      else if ('\003' == ch)
      {
         printE("%s : Predefined variable $(FIRST_DEP) used out of context\n", Dinput.posn(), ch);
         while ((EOF != ch) && !isspace(ch)) ch = Dinput.get();
         if (EOF != ch) Dinput.putBack(ch);
      }
      else if (SEP_C_BRK == ch)
      {
         printE("%s : End of block missing start of block\n", Dinput.posn(), ch);
         while ((EOF != ch) && !isspace(ch)) ch = Dinput.get();
         if (EOF != ch) Dinput.putBack(ch);
      }
      else
      {
         while ((EOF != ch) && !isspace(ch)) ch = Dinput.get();
         if (EOF != ch) Dinput.putBack(ch);
      }
   }
   
   return(token);
}


void Tjlex::readProc()
{
   Ttoken *tok;
   Tstr    posn;
   
   posn = Dinput.posn();
            
   tok = getToken();
   if (TOK_IDENT != tok->type())
   {
      // Illegal preprocessor
      printE ("%s : Illegal preprocessor directive [%s]\n",
              (char const*)posn, tok->text());
      exit(1);
   }
   
   if (!strcmp("include", tok->text()))
   {
      // Include preprocessor directive
      posn = Dinput.posn();
      delete tok;
      tok = getToken();
               
      if ((TOK_TEXT != tok->type()) || ('\0' == tok->text()[0]))
      {
         // Illegal preprocessor
         printE ("%s : Illegal preprocessor argument [%s]\n",
                  (char const*)posn, tok->text());
         exit(1);
      }

      Tstr fileName = tok->text();

      // Check for terminating semi-colon

      delete tok;
      tok = getToken();
      if (TOK_SEMI_COLON != tok->type())
      {
          printE ("%s - Warning Extra data\n", tok->posn());
          while ((TOK_SEMI_COLON != tok->type()) &&
                 (TOK_END        != tok->type()))
          {
              delete tok;
              tok = getToken();
          }
      }
      delete tok;

      // Start reading from the new source file

      Dinput.push((char const*)fileName);
   }
   else
   {
      // Illegal preprocessor
      printE ("%s : Illegal preprocessor directive [%s]\n",
              (char const*)posn, tok->text());
      exit(1);
   }
}


int Tjlex::readIdent(Ttoken &token)
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
      if (!strcmp(buff, "SET"))
      {
         token.setType(TOK_SET);
      }
      else if (!strcmp(buff, "QUIET"))
      {
         token.setType(TOK_STATE);
      }
      else if (!strcmp(buff, "TALK"))
      {
         token.setType(TOK_STATE);
      }
      else if (!strcmp(buff, "DEBUG"))
      {
         token.setType(TOK_STATE);
      }
      else if (!strcmp(buff, "PREAMBLE"))
      {
         token.setType(TOK_KEY_PREAMBLE);
      }
      else if (!strcmp(buff, "POSTAMBLE"))
      {
         token.setType(TOK_KEY_POSTAMBLE);
      }
      else if (!strcmp(buff, "PARTIAL"))
      {
         token.setType(TOK_KEY_PARTIAL);
      }
      else if (!strcmp(buff, "USER"))    // 008
      {
         token.setType(TOK_MODIFY_USER);
      }
      else if (!strcmp(buff, "BREAK"))
      {
         token.setType(TOK_MODIFY_BREAK);
      }
      else if (!strcmp(buff, "CONT"))
      {
         token.setType(TOK_MODIFY_CONT);
      }
      else if (!strcmp(buff, "ONCE"))
      {
         token.setType(TOK_MODIFY_ONCE);
      }
      else if (!strcmp(buff, "RECURE"))
      {
         token.setType(TOK_MODIFY_RECURE);
      }
      else if (!strcmp(buff, "TOUCH"))       // 002
      {
         token.setType(TOK_MODIFY_TOUCH);
      }
      else if (!strcmp(buff, "CREATE"))      // 003
      {
         token.setType(TOK_MODIFY_CREATE);
      }
      else if (!strcmp(buff, "SOURCE"))      // 004
      {
         token.setType(TOK_SOURCE);
      }
      else if (!strcmp(buff, "OPTIONAL"))    // 010
      {
         token.setType(TOK_OPTIONAL);
      }
      else if (!strcmp(buff, "IF"))          // 000
      {
         token.setType(TOK_IF);
      }
      else if (!strcmp(buff, "ELSE"))        // 000
      {
         token.setType(TOK_ELSE);
      }
      else if (!strcmp(buff, "ELIF"))        // 000
      {
         token.setType(TOK_ELIF);
      }
      else if (!strcmp(buff, "FI"))          // 000
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


int Tjlex::readText(Ttoken &token)
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
         readWhich(token);
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


int Tjlex::readSymb(Ttoken &token)
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
                token.setType(TOK_ASIGN);
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
      case ';' : token.setType(TOK_SEMI_COLON); break;
      default  : token.setType(TOK_ILLEGAL);    break;
   }
   
   return(TOK_ILLEGAL != token.type());
}


int Tjlex::readCall(Ttoken &token)
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


int Tjlex::readWhich(Ttoken &token)  // 001 
{
   int   ch   = Dinput.get();
   Tstr  posn = Dinput.posn();
   char *pos  = Dbuff;
   
   while ((EOF != ch) && ('\n' != ch) && (SEP_WHICH_CLOSE != ch))
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
   
   if (SEP_WHICH_CLOSE != ch)
   {
      printE("%s : Unterminated text\n", (char const*)posn);
      if (EOF != ch) Dinput.putBack(ch); 
   }
   
   token.setType(TOK_WHICH);
   token.setText(Dbuff);
   
   return(1);
}


int Tjlex::readComment(Ttoken &token)
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
   
   token.setType(TOK_COMMENT);
   token.setText(Dbuff);
   
   return(1);
}


int Tjlex::readBlock(Ttoken &token)
{
   int           ch   = Dinput.get();
   Tstr          posn = Dinput.posn();
   TstrBuf       buff;
   unsigned long buffPtr = 0;
   int           count   = 1;

   // Variable resolution is handled late at rule execution
   
   while ((EOF != ch) && (0 != count))
   {
      if (SEP_O_BRK == ch)
      {
         // Open bracket - nested
         count++;
         buff[buffPtr++] = ch;
      }
      else if (SEP_C_BRK == ch)
      {
         // Close bracket - nested
         count--;
         if (0 != count) buff[buffPtr++] = ch;
      }
      else
      {
         buff[buffPtr++] = ch;
      }
      ch = Dinput.get();
   }
   buff[buffPtr] = '\0';
   
   if (0 != count)
   {
      printE("Unbalanced brace at EOF\n");
      exit(-1);
   }
   
   token.setType(TOK_BLOCK);
   token.setText((char const*)buff);
   
   return(1);
}


int Tjlex::readSubComm(Ttoken &token)  // 000
{
   int           ch   = Dinput.get();
   Tstr          posn = Dinput.posn();
   TstrBuf       buff;
   unsigned long buffPtr = 0;
   int           count   = 1;

   // Variable resolution is handled late at rule execution
   
   while ((EOF != ch) && (0 != count))
   {
      if (SEP_O_SUB == ch)
      {
         // Open bracket - nested
         count++;
         buff[buffPtr++] = ch;
      }
      else if (SEP_C_SUB == ch)
      {
         // Close bracket - nested
         count--;
         if (0 != count) buff[buffPtr++] = ch;
      }
      else if ('\n' == ch)
      {
         // Replace end of line with white space
         if (0 != count) buff[buffPtr++] = ' ';
      }
      else
      {
         buff[buffPtr++] = ch;
      }
      ch = Dinput.get();
   }
   buff[buffPtr] = '\0';
   
   if (0 != count)
   {
      printE("Unbalanced brace at EOF\n");
      exit(-1);
   }
   
   token.setType(TOK_SUB_COMM);
   token.setText((char const*)buff);
   
   return(1);
}


int Tjlex::readVar()
{
   TstrBuf buff;
   
   int           ch   = Dinput.get();
   Tstr          posn = Dinput.posn();
   unsigned long pos  = 0;
   
   if ('(' == ch)
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
      
      if (')' != ch)
      {
         // Bad variable name
         Dinput.putBack((char const*)buff);
         Dinput.putBack('(');
         return(0);
      }
      else
      {
         // Look up the variable and do a put back on its value
         if (!strcmp("TARGET", (char const*)buff))
         {
            Dinput.putBack('\001');   // Push on the tag
         }
         else if (!strcmp("DEPS", (char const*)buff))
         {
            // Handled by late variable resolution in blocks
            printE("Variable DEPS out of context\n");
         }
         else  if (!strcmp("FIRST_DEP", (char const*)buff))
         {
            // Handled by late variable resolution in blocks
            printE("Variable FIRST_DEP out of context\n");
         }
         else if (('0' <= buff[0]) && (buff[0] <= '9') && ('\0' == buff[1]))  // 005
         {
            Dinput.putBack(buff[0]);  // Push on the number
            Dinput.putBack('\004');   // Push on the tag
         }
         else
         {
            substitute((char const*)buff);
         }
         
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

