/****************************************************************************

    The Sire build utility 'sire'.
    Copyright (C) 1999, 2000, 2002, 2003  David W Orchard (davido@errol.org.uk)

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

   File  : jake.cpp
   Class : Tjake
   Desc  : A description

EDIT 001 18-Apr-96 davido
Support for 'which' expansions.
       
EDIT 002 29-Apr-97 davido
Support TOUCH rules.
       
EDIT 003 16-Jan-97 davido
Support CREATE rules.

EDIT 004 17-Jun-98 davido
Support sourcing jake environment files

EDIT 004 17-Jun-98 davido
Support TOK_CALL and TOK_WHICH in IF conditions

EDIT 005 06-Aug-02 davido@errol.org.uk
Add in modifier USER to specify files not to be deleted.

EDIT 006 17-Oct-02 david@the-hut.net
Support TOK_CALL as a stand alone statement to provide a simple form
of preprocesing of the relations file.

EDIT 007 07-Nov-02 david@the-hut.net
Support sourcing scripts in a generic way.

EDIT 008 15-Nov-02 davido@errol.org.uk
Support script arguments

EDIT 009 23-Jun-03 davido@errol.org.uk
Fixed a'-remove' bug for raule with no block.

EDIT 010 18-Oct-03 davido@errol.org.uk
Permit assignments to things that look like match text.
Added support for sub-commands.

*****************************************************************************/



/************* INCLUDE ******************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "jake.h"
#include "text.h"
#include "rule.h"
#include "sys.h"
#include "nametarget.h"
#include "matchtarget.h"
#include "state.h"
#include "env.h"
#include "str.h"
#include "set.h"



/************* FUNCTIONS ****************************************************/



static char const *expandTokenText(Ttoken *token);








void Tjake::parser()
{
    if (FILE_END != parseStatements(TRUE, FALSE))
    {
        printE("<END> : End of IF Statement without IF Statement\n");
    }
}




int Tjake::parseFile(char const *file, int active, Ttoken **list, int len)   // 07
{
	Tjlex lex(file);
    
    // Set up the argument list
    
    if ((NULL != list) && (len > 0))
    {
        char const **argList = new char const*[len];
        
        for (int i = 0; (i < len); i++)
        {
            argList[i] = list[i]->text();
        }
    }
	
	// Push the lex.
	
	pushInput(&lex, list, len);
	
	// Read the lex file
	
    Tjake::parseStatements(active, FALSE);

    // Pop the lex file

	popInput();
    
    // Restore the old argument list
    
    return (0);
}











parseState_t Tjake::parseStatements(int active, int restricted)
{
   Ttoken *token;

   token = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }

   while (TOK_END != token->type())
   {
      Dinput->putBackToken(token);

      if (parsePreamble(active))
      {
         // State command
         if (restricted)
         {
                printE("%s : Not permitted in context\n", token->posn());
         }
      }
      else if (parsePostamble(active))
      {
         // State command
         if (restricted)
         {
                printE("%s : Not permitted in context\n", token->posn());
         }
      }
      else if (parsePartial(active))
      {
         // State command
         if (restricted)
         {
                printE("%s : Not permitted in context\n", token->posn());
         }
      }
      else if (parseState(active))
      {
         // State command
         if (restricted)
         {
                printE("%s : Not permitted in context\n", token->posn());
         }
      }
      else if (parseAssign(active))
      {
         // Assignment
      }
      else if (parseSource(active))
      {
         // Assignment
      }
      else if (parseIf(active, restricted))
      {
         // If
      }
      else if (parseElse(active))
      {
         // Else
         return(PARSE_ELSE);
      }
      else if (parseElseIf(active))
      {
         // ElseIf
         return(PARSE_ELSE_IF);
      }
      else if (parseEndIf(active))
      {
         // EndIf
         return(PARSE_END_IF);
      }
      else if (parseRule(active))
      {
         // Rule
         if (restricted)
         {
                printE("%s : Not permitted in context\n", token->posn());
         }
      }
      else
      {
         // Bad Token
         token = Dinput->getToken();
         while ((NULL != token) && TOK_COMMENT == token->type())
         {
            delete token;
            token = Dinput->getToken();
         }
         if (NULL != token)
         {
            printE("%s : Bad token <%s>\n", token->posn(), token->text());
            delete token;
         }
      }

      token = Dinput->getToken();
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
   }
   delete token;

   return (FILE_END);
}




int Tjake::parsePreamble(int active)
{
   ruleStates  mode;
   Ttoken     *token = NULL;
   int         state = 0;
   TtokList    list;
      
   token   = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }
      
   if (TOK_KEY_PREAMBLE != token->type())
   {
      // This is not a preamble rule
      Dinput->putBackToken(token);
   }
   else
   {
      Tstr posn = token->posn();
      
      delete token;
      token = Dinput->getToken();
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
   
      if (TOK_COLON == token->type())
      {
         // Parse the modifiers
         delete token;
   
         if (!parseRuleModifiers(mode))
         {
            // Failed to parse the modifiers
            printE("%s : Illegal modifiers\n", (char const*)posn);
         }
         
         token = Dinput->getToken();
         while ((NULL != token) && TOK_COMMENT == token->type())
         {
            delete token;
            token = Dinput->getToken();
         }
      }
      
      if (TOK_BLOCK != token->type())
      {
         // Error
         printE("%s : Missing PREAMBLE definition\n", (char const*)posn);
         Dinput->putBackToken(token);
      }
      else if (NULL != Dpreamble)
      {
         // Error
         printE("%s : Redefinition of PREAMBLE\n", (char const*)posn);
         delete token;
      }
      else if (active)
      {
         Dpreamble = new Trule(list, token->text(), mode);
         Dpreamble->setPosn((char const *)posn);
         delete token;
         state = 1;
      }
      else
      {
         delete token;
         state = 1;
      }
   }
   
   return (state);
}



int Tjake::parsePostamble(int active)
{
   ruleStates  mode;
   Ttoken     *token = NULL;
   int         state = 0;
   TtokList    list;
      
   token   = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }
      
   if (TOK_KEY_POSTAMBLE != token->type())
   {
      // This is not a postamble rule
      Dinput->putBackToken(token);
   }
   else
   {
      Tstr posn = token->posn();
      
      delete token;
      token = Dinput->getToken();
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
      
      if (TOK_COLON == token->type())
      {
         // Parse the modifiers
         delete token;
   
         if (!parseRuleModifiers(mode))
         {
            // Failed to parse the modifiers
            printE("%s : Illegal modifiers\n", (char const*)posn);
         }
         
         token = Dinput->getToken();
         while ((NULL != token) && TOK_COMMENT == token->type())
         {
            delete token;
            token = Dinput->getToken();
         }
      }
   
      if (TOK_BLOCK != token->type())
      {
         // Error
         printE("%s : Missing POSTAMBLE definition\n", (char const*)posn);
         Dinput->putBackToken(token);
      }
      else if (NULL != Dpostamble)
      {
         // Error
         printE("%s : Redefinition of POSTAMBLE\n", (char const*)posn);
         delete token;
      }
      else if (active)
      {
         Dpostamble = new Trule(list, token->text(), mode);
         Dpostamble->setPosn((char const *)posn);
         delete token;
         state = 1;
      }
      else
      {
         delete token;
         state = 1;
      }
   }
   
   return (state);
}



int Tjake::parsePartial(int active)
{
   Ttoken *token = NULL;
   int     state = 0;
      
   token   = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }
      
   if (TOK_KEY_PARTIAL != token->type())
   {
      // This is not a partial command
      Dinput->putBackToken(token);
   }
   else
   {
      Tstr posn = token->posn();
      
      delete token;
      token = Dinput->getToken();
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
      
      if (TOK_COLON != token->type())
      {
         // Error
         printE("%s : Missing PARTIAL definition\n", (char const*)posn);
         Dinput->putBackToken(token);
      }
      else
      {
         delete token;
         token = Dinput->getToken();
         
         state = 1;
         while (TOK_SEMI_COLON != token->type())
         {
            switch (token->type())
            {
               case TOK_IDENT :
                    if (active) new TnameTarget(cleanName(token->text()), NULL);
                    break;
               case TOK_CALL  :
                  // Call operation
                  {
                     FILE          *fp  = readPipe(token->text());
                     unsigned long  pos = 0;
                     static         TstrBuf buff;
                   
                     if (NULL != fp)
                     {
                        int ch;
                      
                        while (EOF != (ch = fgetc(fp)))
                        {
                           // Minimise the use of spaces
                           if (isspace(ch))
                           {
                              if (0 < pos)
                              {
                                 buff[pos++] = '\0';
                                 
                                 // add this token
                                 if (active) new TnameTarget(cleanName((char const*)buff), NULL);
                              }
                              pos = 0;
                         
                              while (isspace(ch))
                              {
                                 ch = fgetc(fp);
                              }
                           }
                         
                           if (EOF != ch)
                              buff[pos++] = ch;
                        }
                        
                        if (0 < pos)
                        {
                           buff[pos++] = '\0';
                                 
                           // add this token
                           if (active) new TnameTarget(cleanName((char const*)buff), NULL);
                        }
                      
                        closePipe(fp);
                     }
                     else
                     {
                        printE("%s : Pipe [%s] failed\n", token->posn(), token->text());
                     }
                  }
                  break;
               case TOK_WHICH  :        // 001
                  // Which operation
                  {
                     char const *value = findExecs(token->text());
                     if (active) new TnameTarget(cleanName(value), NULL);
                  }
                  break;
               case TOK_MATCH :
                  if (active) new TmatchTarget(cleanName(token->text()), NULL);
                  break;
               default :
                  printE("%s : Illegal token in PARTIAL definition\n", token->posn());
                  break;
            }
            delete token;
            token = Dinput->getToken();
         }
      }
   }
   
   return (state);
}



int Tjake::parseState(int active)
{
   Ttoken *token = NULL;
   int     state = 0;
      
   token   = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }
      
   if (TOK_SET == token->type())
   {
      state = 1;
      
      delete token;
      token = Dinput->getToken();
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
      
      if (TOK_STATE == token->type())
      {
         // This may be a state command
         if (!strcmp(token->text(), "QUIET"))
         {
            if (active) quietS(1);
         }
         else if (!strcmp(token->text(), "TALK"))
         {
            if (active) talkS(1);
         }
         else if (!strcmp(token->text(), "DEBUG_ON"))
         {
            if (active) debugS(1);
         }
         else
         {
            // This is not a known state command
            printE("%s : Unknown state %s\n", (char const*)token->posn(),token->text());
            state = 0;
         }
         
         // Get the next token
         delete token;
         token = Dinput->getToken();
      }
      else if (TOK_IDENT == token->type())
      {
          // Get the arguments to SET and pass on to doSet() to process it
          
          Ttoken *list[64];
          int     idx = 0;
          int     rtn = 0;
          
          // Build up the argument list
          do
          {
              list[idx++] = token;
              token = Dinput->getToken();
              
              while((NULL != token) && TOK_COMMENT == token->type())
              {
                  delete token;
                  token = Dinput->getToken();
              }
          }
          while ((TOK_IDENT == token->type()) || (TOK_TEXT == token->type()));
          list[idx] = NULL;
          
          // Check for a good list
          
          if (TOK_SEMI_COLON == token->type())
          {
              // This is ok so do the call
              if (active)
                rtn = doSet(this, active, list, idx);
              else
                rtn = 1;
          }
          else
          {
              printE ("%s - Error illegal SET argument %s\n", token->posn(), token->text());
          }
          
          // Now clean up
          while (0 < idx)
          {
              delete list[--idx];
          }
          
          if (0 != rtn) return (rtn); // Check for errors from doSet();
      }
      else if (TOK_SEMI_COLON == token->type())
      {
          // Right - Empty statement
          printE ("%s - Warning Empty SET\n", token->posn());
         
          delete token;
          return (1);
      }
      else
      {
          printE ("%s - Warning Illegal SET command\n", token->posn());
      }
      
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
            
      if (TOK_SEMI_COLON != token->type())
      {
         printE ("%s - Warning Extra data\n", token->posn());
         
         while ((TOK_SEMI_COLON != token->type()) &&
                (TOK_END        != token->type()))
         {
            delete token;
            token =  Dinput->getToken();
         }
      }
      
      delete token;
   }
   else
   {
      // This is not a set command
      Dinput->putBackToken(token);
   }
   
   return (state);
}



int Tjake::parseAssign(int active)
{
   static  TstrBuf textValue   = "";
   
   int      textPos     = 0;
   int      firstItem   = TRUE;
   Ttoken  *nmToken     = NULL;
   Ttoken  *token       = NULL;
   int      endList     = 0;
   Tstr     firstMatch  = "";
   Tstr     secondMatch = "";
      
   nmToken = Dinput->getToken();
   while ((NULL != nmToken) && TOK_COMMENT == nmToken->type())
   {
      delete nmToken;
      nmToken = Dinput->getToken();
   }
   
   token   = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }
   
   if ((TOK_IDENT != nmToken->type()) || (TOK_ASIGN != token->type()))
   {
      // This cannot be an assign statement
      Dinput->putBackToken(token);
      Dinput->putBackToken(nmToken);
      return(0);
   }
   else
   {
      // This must be an assignment statement
      
      // We don't need the '=' sign any more
      delete token;
      
      // Read the name list
      
      token = Dinput->getToken();
      while ((NULL != token) && TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
   
      while ((TOK_END != token->type()) && !endList)
      {
         switch (token->type())
         {
             case TOK_IDENT      :
             case TOK_TEXT       :
             case TOK_MATCH      :  // 010
                // Identifier token
                if (firstItem)
                   firstItem = FALSE;
                else
                   textValue[textPos++] = ' ';
                   
                textValue.cat(token->text(), textPos);
                textPos += strlen(token->text());
                break;
                   
             case TOK_CALL       :
                // Call operation
                {
                   // FILE *fp = popen(token->text(), "r");
                   FILE *fp = NULL;
                   
                   if (NULL != (fp = readPipe(token->text())))
                   {
                      int start = textPos;
                      int ch    = EOF;
                   
                      // Identifier token
                      if (firstItem)
                         firstItem = FALSE;
                      else
                         textValue[textPos++] = ' ';
                      
                      // Remove leading spaces
                      if (EOF != (ch = fgetc(fp)))
                      {
                         while (isspace(ch)) ch = fgetc(fp);
                         
                         if (EOF != ch) textValue[textPos++] = ch;
                      }
                      
                      while (EOF != (ch = fgetc(fp)))
                      {
                         // Minimise the use of spaces
                         if (isspace(ch))
                         {
                            textValue[textPos++] = ' ';
                         
                            while (isspace(ch))
                            {
                               ch = fgetc(fp);
                            }
                         }
                         
                         if (EOF != ch)
                            textValue[textPos++] = ch;
                      }
                      
                      // pclose(fp);
                      closePipe(fp);
                      
                      // Remove trailing space
                      while ((start < textPos) && isspace(textValue[textPos -1])) textPos--;
                   }
                   else
                   {
                      printE("%s : Pipe [%s] failed\n", token->posn(), token->text());
                   }
                }
                break;
                   
             case TOK_WHICH       :   // 001
                // Which operation
                {
                   // Build up the text
                   char const *value = findExecs(token->text());
                   textValue.cat(value, textPos);
                   textPos += strlen(value);
                }
                break;
                
             case TOK_COLON      :
             case TOK_SEMI_COLON :
                // End the name list
                endList = 1;
                break;
                
             case TOK_COMMENT    :
                // Ignore comment
                break;
                
             default             :
                // Error : illegal token
                printE("%s : Bad value <%s> in assignment\n",
                       token->posn(), token->text());
                while ((TOK_END        != token->type()) &&
                       (TOK_SEMI_COLON != token->type()))
                {
                   delete token;
                   token = Dinput->getToken();
                }
                
                delete token;
                delete nmToken;
                return(0);
         }
      
         if (!endList)
         {
            delete token;
            token = Dinput->getToken();
            while ((NULL != token) && TOK_COMMENT == token->type())
            {
               delete token;
               token = Dinput->getToken();
            }
         }
      }
      
      // Terminate the list text
      textValue[textPos] = '\0';
      
      // Check for match fields
      if (TOK_COLON == token->type())
      {
         // We have match lists
         
         // First match field
         do
         {
            delete token;
            token = Dinput->getToken();
         }
         while ((NULL != token) && TOK_COMMENT == token->type());
         
         if (TOK_MATCH != token->type())
         {
            // Error Illegal match field
            printE("%s : Bad match field <%s> in assignment\n",
                   token->posn(), token->text());
            while ((TOK_END        != token->type()) &&
                   (TOK_COLON      != token->type()) &&
                   (TOK_SEMI_COLON != token->type()))
            {
               delete token;
               token = Dinput->getToken();
            }
            
            delete token;
            delete nmToken;
            return(0);
         }
         else
         {
            // Match field
            firstMatch = token->text();
            do
            {
               delete token;
               token = Dinput->getToken();
            }
            while ((NULL != token) && TOK_COMMENT == token->type());
         }
         
         if (TOK_COLON != token->type())
         {
            // Error Illegal match field
            printE("%s : Missing second match field in assignment\n", token->posn());
            while ((TOK_END        != token->type()) &&
                   (TOK_SEMI_COLON != token->type()))
            {
               delete token;
               token = Dinput->getToken();
            }
            
            delete token;
            delete nmToken;
            return(0);
         }
         
         // Second match field
         do
         {
            delete token;
            token = Dinput->getToken();
         }
         while ((NULL != token) && TOK_COMMENT == token->type());
         
         if (TOK_MATCH != token->type())
         {
            // Error Illegal match field
            printE("%s : Bad match field <%s> in assignment\n",
                   token->posn(), token->text());
            while ((TOK_END        != token->type()) &&
                   (TOK_COLON      != token->type()) &&
                   (TOK_SEMI_COLON != token->type()))
            {
               delete token;
               token = Dinput->getToken();
            }
            
            delete token;
            delete nmToken;
            return(0);
         }
         else
         {
            // Match field
            secondMatch = token->text();
            
            // Find the final semi colon
            
            do
            {
               delete token;
               token = Dinput->getToken();
            }
            while ((NULL != token) && TOK_COMMENT == token->type());
            
            if (TOK_SEMI_COLON != token->type())
            {
               printE ("%s - Warning Extra data\n", token->posn());
               
               while ((TOK_SEMI_COLON != token->type()) &&
                      (TOK_END        != token->type()))
               {
                  delete token;
                  token =  Dinput->getToken();
               }
            }
            
            // Transform the text
            
            textValue = sedMatch((char const*)textValue,
                                 (char const*)firstMatch,
                                 (char const*)secondMatch);
         }
      }

      if (active)
      {
        printD("set [%s] = [%s]\n",nmToken->text(), (char const*)textValue);
        Tenv::appSet(nmToken->text(), (char const*)textValue);
      }
   }
      
   delete token;
   delete nmToken;
   return(1);
}



int Tjake::parseSource(int active)   // 004
{
    Ttoken *token    = NULL;
	int     optional = FALSE;
      
    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && TOK_SOURCE == token->type())
    {
	    delete token;
	    token = Dinput->getToken();

	    // Check for Optional files

	    if ((NULL != token) &&
	        (TOK_OPTIONAL  == token->type()))
	    {
	        delete token;
	        token = Dinput->getToken();
	
	        optional = TRUE;
	    }

	    // Get the file name
        
   

	    if ((NULL != token) &&
	        ((TOK_TEXT  == token->type()) ||
	         (TOK_IDENT == token->type()) ||
	         (TOK_CALL  == token->type())))
	    {
	        Tstr       file = token->text();
            EtokenType Type = token->type();  // 006
            
	        delete token;
	        token = Dinput->getToken();

	        // Strip comments
            while ((NULL != token) && TOK_COMMENT == token->type())
            {
                delete token;
                token = Dinput->getToken();
            }
            
	        // Check for terminator
            if (TOK_SEMI_COLON != token->type())
            {
                printE ("%s - Warning Extra data\n", token->posn());
               
                while ((TOK_SEMI_COLON != token->type()) &&
                       (TOK_END        != token->type()))
                {
                    delete token;
                    token =  Dinput->getToken();
                }
            }

	        if (Type == TOK_CALL)  // 006
            {
                // Call operation
                Dinput->pipe(file);  // Push the output into the input

		        return (1); // Got it OK
            }
            else if (access((char const*)file, R_OK))
	        {
	            if (optional)
	            {
	                /* Not a problem */
		            return (1); // Got it OK
	            }
	            else
	            {
		            printE ("%s - ERROR Cannot read file [%s]\n", token->posn(), (char const*)file);
		        }
	        }
	        else
	        {
		        // Source the file.
                
                parseFile((char const*)file, active);

		        return (1); // Got it OK
	        }
	    }
	    else
	    {
	        printE ("%s - ERROR Bad source file\n", token->posn());
               
	        while ((TOK_SEMI_COLON != token->type()) &&
                      (TOK_END        != token->type()))
	        {
		        delete token;
		        token =  Dinput->getToken();
	        }
	    }
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return(0);
}



int Tjake::parseRule(int active)
{
   ruleStates  mode;
   TtokList    nameLst;
   TtokList    depsLst;
   Ttoken     *token;
   Tstr        posn = "";
      
   // Parse the name list
   
   if (!parseRuleNmLst(nameLst))
   {
      return(0);
   }
   
   // Get the colon and its position
   token = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }
            
   posn  = token->posn();
   if ((TOK_COLON      != token->type()) &&
       (TOK_SEMI_COLON != token->type()) &&
       (TOK_BLOCK      != token->type()) &&
       (TOK_SUB_COMM   != token->type()))  // 010
   {
      // Failed to get a colon
      Dinput->putBackToken(token);
      return(0);
   }
   else if (TOK_COLON == token->type())
   {
       // Parse the modifiers
       delete token;
 
       if (!parseRuleModifiers(mode))
       {
           // Failed to parse the modifiers
           printE("%s : Illegal modifiers\n", (char const*)posn);
       }
 
       // Parse the dependents list
 
       if (!parseRuleDepsLst(depsLst))
       {
           // Failed to parse the dependent
           return(0);
       }
 
       token = Dinput->getToken();
       while ((NULL != token) && TOK_COMMENT == token->type())
       {
          delete token;
          token = Dinput->getToken();
       }
   }
   
   // Parse block
            
   if ((TOK_SEMI_COLON != token->type()) &&
       (TOK_BLOCK      != token->type()) &&
       (TOK_SUB_COMM   != token->type()))  // 010
   {
      // Failed to get a block
      Dinput->putBackToken(token);
      return(0);
   }

   if (active)
   {
       // Construct and add the rules

       Trule *rule;
       if (TOK_BLOCK == token->type())
       {
           rule = new Trule(depsLst, token->text(), mode);
       }
       else if (TOK_SUB_COMM == token->type())                        // 010
       {
           rule = new TsubComm(depsLst, token->text(), mode);
       }
       else
       {
           // If there is no block then we can't rebuild the target   // 009
           mode.user = TRUE;
           rule = new Trule(depsLst, "", mode);
       }

       rule->setPosn((char const*)posn);
       delete token;

       for (int i = 0; (i < nameLst.len()); i++)
       {
           Ttoken *token = nameLst[i];

           switch (token->type())
           {
              case TOK_IDENT :
                   new TnameTarget(cleanName(token->text()), rule);
                   break;

              case TOK_MATCH :
                   new TmatchTarget(cleanName(token->text()), rule);
                   break;

              default :
                   // We don't put anything else into this list
                   printE("%s : Illegal parameter to rule\n", (char const*)posn);
                   break;
           }
       }
   }
   
   // Return parse success
   return(1);
}



int Tjake::parseIf(int active, int restricted)
{
    Ttoken       *token       = NULL;
    int           localActive = FALSE;
    parseState_t  rtn;
    Tstr          posn = "";

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && (TOK_IF == token->type()))
    {
        posn = token->posn();
	    delete token;
	
        do
        {
            if (localActive) active = FALSE; // We have done one of the conditions
            if (!parseCondition(localActive))
            {
                // Illegal condition
                printE("%s : Bad condition\n", (char const*)posn);
            }
        }
        while(PARSE_ELSE_IF == (rtn = parseStatements((active && localActive), restricted)));

        if (PARSE_ELSE == rtn)
        {
            rtn = parseStatements((active && !localActive), restricted);
        }

        if (PARSE_END_IF != rtn)
        {
            printE("%s : IF Statement not terminated\n", (char const*)posn);
        }

        return(1);
    }
    else
    {
        // Not an IF statement
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tjake::parseElse(int active)
{
    Ttoken *token    = NULL;

    token = Dinput->getToken();
    while ((NULL != token) && (TOK_COMMENT == token->type()))
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && TOK_ELSE == token->type())
    {
	    delete token;
        return (1);
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tjake::parseElseIf(int active)
{
    Ttoken *token    = NULL;

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && (TOK_ELIF == token->type()))
    {
	    delete token;
        return (1);
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tjake::parseEndIf(int active)
{
    Ttoken *token    = NULL;

    token = Dinput->getToken();
    while ((NULL != token) && TOK_COMMENT == token->type())
    {
	    delete token;
	    token = Dinput->getToken();
    }

    // Check for a source statement
    if ((NULL != token) && (TOK_ENDIF == token->type()))
    {
	    delete token;
        return (1);
    }
    else
    {
        Dinput->putBackToken(token);
    }

    return (0);
}



int Tjake::parseCondition(int &active)
{
    int     ok        = FALSE;
    int     state     = 0;
    int     thisState = 0;

    Ttoken *arg1;

    ok = parseComparisonExpression(state);

    arg1 = Dinput->getToken();

    while (ok && (NULL != arg1) && ((TOK_AND == arg1->type()) || (TOK_OR == arg1->type())))
    {
        ok = parseComparisonExpression(thisState);

        switch (arg1->type())
        {
            case TOK_AND :
                state = state && thisState;
                break;

            case TOK_OR :
                state = state || thisState;
                break;

            default:
                printE("%s : Bad token in expression [%s]\n", arg1->posn(), arg1->text());
                ok = FALSE;
                break;
        }

        delete arg1;
        arg1 = Dinput->getToken();
    }

    if (ok) active = state;

    Dinput->putBackToken(arg1);

    return (ok);
}



void Tjake::pushInput(Tjlex *input, Ttoken **list, int len)
{
    if (NULL == input)
    {
        printE("Internal error - Bad input source\n");
    }
    else if (DinputStackPos == sizeof(DinputStack)/sizeof(DinputStack[0]))
    {
        printE("Internal error - Input Stack overflow\n");
    }
    else
    {
        if (list == NULL)
        {
            // If the arguments are not defined use the existing arguments
            
            list = NULL;
            len  = 0;
        }
        else
        {
            // Set the new arg list
        
            Tenv::appSetArg(list, len);
        }
        
        DinputStack[DinputStackPos].argList = list;
        DinputStack[DinputStackPos].argLen  = len;
        
        DinputStack[DinputStackPos++].input   = Dinput;
        Dinput = input;
    }
}



void Tjake::popInput()
{
    if (0 == DinputStackPos)
    {
        printE("Internal error - Input Stack underflow\n");
    }
    else
    {
        Dinput = DinputStack[--DinputStackPos].input;
        
        // Restore the argument list
        
        if (DinputStackPos > 0)
        {
            Tenv::appSetArg(DinputStack[DinputStackPos].argList,
                            DinputStack[DinputStackPos].argLen);
        }
        else
        {
            Tenv::appSetArg(NULL, 0);
        }
    }
}



int Tjake::parseRuleNmLst(TtokList &list)
{
   int    fail  = 0;
   
   Ttoken *token = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }    
   
   while ((TOK_COLON    != token->type()) &&
          (TOK_BLOCK    != token->type()) &&
          (TOK_SUB_COMM != token->type()) &&   // 010
          (TOK_END      != token->type()) && !fail)
   {
      if ((TOK_IDENT == token->type()) || (TOK_MATCH == token->type()))
      {
          // Legal token
          list.add(token);
          token = Dinput->getToken();
          while ((NULL != token) && TOK_COMMENT == token->type())
          {
             delete token;
             token = Dinput->getToken();
          }    
      }
      else
      {
          // Illegal token
          fail = 1;
      }
   }
   
   fail = fail || ((TOK_COLON    != token->type()) &&
                   (TOK_BLOCK    != token->type()) &&
                   (TOK_SUB_COMM != token->type()));  // 010
   
   if (fail)
   {
      // Put every thing back
      Dinput->putBackToken(token);
      
      while (0 < list.len())
      {
         Dinput->putBackToken((Ttoken*)list[list.len() -1]);
         list.remove(list.len() -1);
      }
   }
   else
   {
      Dinput->putBackToken(token);
   }
   
   // Return
   return(!fail);
}



int Tjake::parseRuleDepsLst(TtokList &list)
{
   int    fail  = 0;
   
   Ttoken *token = Dinput->getToken();
   while ((NULL != token) && TOK_COMMENT == token->type())
   {
      delete token;
      token = Dinput->getToken();
   }    
   
   while ((TOK_SEMI_COLON != token->type()) &&
          (TOK_BLOCK      != token->type()) &&
          (TOK_SUB_COMM   != token->type()) &&  // 010
          (TOK_END        != token->type()) && !fail)
   {
      if ((TOK_TEXT  == token->type()) ||   // 004
          (TOK_IDENT == token->type()) ||
          (TOK_MATCH == token->type()) ||
          (TOK_WHICH == token->type()) ||   // 001
          (TOK_CALL  == token->type()))
      {
          // Legal token
          list.add(token);
          token = Dinput->getToken();
          while ((NULL != token) && TOK_COMMENT == token->type())
          {
             delete token;
             token = Dinput->getToken();
          }    
      }
      else
      {
          // Illegal token
          fail = 1;
      }
   }
   
   fail = fail || ((TOK_BLOCK      != token->type()) &&
                   (TOK_SUB_COMM   != token->type()) &&  // 010
                   (TOK_SEMI_COLON != token->type()));
   
   if (fail)
   {
      // Clean up
      
      while (0 < list.len())
      {
         delete (Ttoken*)list.remove(list.len() -1);
      }
   
      while ((TOK_SEMI_COLON != token->type()) &&
             (TOK_BLOCK      != token->type()) &&
             (TOK_SUB_COMM   != token->type()) &&  // 010
             (TOK_END        != token->type()))
      {
         delete token;
         token = Dinput->getToken();
      }
      
      if (TOK_END == token->type())
         Dinput->putBackToken(token);
      else
         delete token;
   }
   else
   {
      // Put  back the block
      Dinput->putBackToken(token);
   }
   
   // Return
   return(!fail);
}



int Tjake::parseRuleModifiers(ruleStates &mode)
{
   int     fail  = FALSE;
   int     mods  = FALSE;
   Ttoken *token;
   
   // Get the modifiers
   
   do
   {
      token = Dinput->getToken();
      
      while (TOK_COMMENT == token->type())
      {
         delete token;
         token = Dinput->getToken();
      }
      
      switch (token->type())
      {
          case TOK_MODIFY_USER :   // 005
               delete token;
               mode.user = TRUE;
               mods      = TRUE;
               break;
              
          case TOK_MODIFY_BREAK :
               delete token;
               mode.breakS = TRUE;
               mods        = TRUE;
               break;
              
          case TOK_MODIFY_CONT :
               delete token;
               mode.cont   = TRUE;
               mods        = TRUE;
               break;
              
          case TOK_MODIFY_ONCE :
               delete token;
               mode.recure = FALSE;
               mode.once   = TRUE;
               mods        = TRUE;
               break;
              
          case TOK_MODIFY_RECURE :
               delete token;
               mode.recure = TRUE;
               mode.once   = FALSE;
               mods        = TRUE;
               break;
              
          case TOK_MODIFY_TOUCH :   // 002
               delete token;
               mode.create = FALSE;
               mode.touch  = TRUE;
               mods        = TRUE;
               break;
              
          case TOK_MODIFY_CREATE :   // 003
               delete token;
               mode.create = TRUE;
               mode.touch  = FALSE;
               mods        = TRUE;
               break;
              
          case TOK_STATE :
               if (!strcmp(token->text(), "QUIET"))
               {
                  mode.quiet = TRUE;
                  mode.talk  = FALSE;
                  mode.debug = FALSE;
               }
               else if (!strcmp(token->text(), "TALK"))
               {
                  mode.talk  = TRUE;
               }
               else if (!strcmp(token->text(), "DEBUG"))
               {
                  mode.debug = TRUE;
               }
               else
               {
                  // This is not a known state command
                  printE("%s : Unknown state %s\n", token->posn(),token->text());
               }
               delete token;
               mods = TRUE;
               break;
           
         case TOK_BLOCK :
         case TOK_SUB_COMM :  // 010
              Dinput->putBackToken(token);
              mods = FALSE;
              break;
           
         case TOK_COLON :
              delete token;
              mods = FALSE;
              break;
      
         default :
              if (mods)
              {
                 printE("%s : Illegal modifier <%s>\n", token->posn(), token->text());
                 fail = TRUE;
                 mods = FALSE;
              }
              Dinput->putBackToken(token);
              break;
      }
   }
   while (mods);
   
   // Return
   return(!fail);
}



int Tjake::parseComparisonExpression(int &active)
{
    int     ok        = FALSE;
    int     openExpr  = FALSE;
    int     state     = 0;
    int     thisState = 0;

    Ttoken *arg1 = Dinput->getToken();

    if  ((NULL != arg1) && (TOK_OPEN  == arg1->type()))
    {
        delete arg1;
        openExpr = TRUE;
        ok = parseComparisonExpression(state);
    }
    else
    {
        Dinput->putBackToken(arg1);
        ok = parseComparisonCondition(state);
    }

    arg1 = Dinput->getToken();

    while (ok && (NULL != arg1) && ((TOK_AND == arg1->type()) || (TOK_OR == arg1->type())))
    {
        ok = parseComparisonExpression(thisState);

        switch (arg1->type())
        {
            case TOK_AND :
                state = state && thisState;
                break;

            case TOK_OR :
                state = state || thisState;
                break;

            default:
                printE("%s : Bad token in expression [%s]\n", arg1->posn(), arg1->text());
                ok = FALSE;
                break;
        }

        delete arg1;
        arg1 = Dinput->getToken();
    }

    if (!ok)
    {
        // We had an error somewhere
        Dinput->putBackToken(arg1);
    }
    else if (openExpr && ((NULL == arg1) || (TOK_CLOSE  != arg1->type())))
    {
        Dinput->putBackToken(arg1);
        printE("%s : Missing close bracket\n", arg1->posn());
        ok = 0;
    }
    else if (openExpr)
    {
        // Delete the close bracket
        delete arg1;
        active = state;
    }
    else
    {
        // Read beyond the end
        Dinput->putBackToken(arg1);
        active = state;
    }

    return (ok);
}



int Tjake::parseComparisonCondition(int &active)
{
    int     ok       = 0;
    int     state    = 0;
    int     notState = 0;

    Ttoken *arg1 = Dinput->getToken();
    Ttoken *op   = Dinput->getToken();
    Ttoken *arg2 = Dinput->getToken();

    if ((NULL != arg2) &&
	    ((TOK_TEXT  == arg1->type()) ||
         (TOK_IDENT == arg1->type()) ||
         (TOK_CALL  == arg1->type()) ||
         (TOK_WHICH == arg1->type()) ||
         (TOK_MATCH == arg1->type()))
                                         &&
	    ((TOK_TEXT  == arg2->type()) ||
         (TOK_IDENT == arg2->type()) ||
         (TOK_CALL  == arg2->type()) ||
         (TOK_WHICH == arg2->type()) ||
         (TOK_MATCH == arg2->type()))
	    )
    {
        char const *text1;
        char const *text2;
        
        switch (op->type())
        {
            case TOK_NOT_EQUAL:
                notState = 1;

            case TOK_EQUAL:
                ok = 1;

                if ((TOK_MATCH == arg2->type()))
                {
                    text1 = expandTokenText(arg1);
                    state = wildMatch(text1, arg2->text());
                }
                else if ((TOK_MATCH == arg1->type()))
                {
                    text2 = expandTokenText(arg2);
                    state = wildMatch(text2, arg1->text());
                }
                else
                {
                    text1 = expandTokenText(arg1);
                    text2 = expandTokenText(arg2);
                    state = !strcmp(text1, text2);
                }

                break;

            default:
                break;
        }
    }

    if (!ok)
    {
        Dinput->putBackToken(arg2);
        Dinput->putBackToken(op);
        Dinput->putBackToken(arg1);
    }

    active = (notState)?(!state):(state);

    return(ok);
}
      
      
static char const *expandTokenText(Ttoken *token)
{
    switch (token->type())
    {
        case TOK_CALL  :
            {
               FILE          *fp  = readPipe(token->text());
               unsigned long  pos = 0;
               static TstrBuf buff;
             
               if (NULL != fp)
               {
                  int ch;
                
                  while (EOF != (ch = fgetc(fp)))
                  {
                     // Minimise the use of spaces
                     if (isspace(ch))
                     {
                        do
                        {
                            ch = fgetc(fp);
                        }
                        while (isspace(ch));
                        
                        if (EOF != ch)
                            buff[pos++] = ' ';
                     }
                   
                     if (EOF != ch)
                        buff[pos++] = ch;
                  }
                  
                  buff[pos++] = '\0';
                
                  closePipe(fp);
                  
                  return ((char const*)buff);
               }
               else
               {
                  /* Failed to execute the command */
                  return ("");
               }
            }
               
        case TOK_WHICH : return (findExecs(token->text()));
        
        default        : return (token->text());
    }
}





/************* END OF FILE **************************************************/
