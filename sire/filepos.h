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

   File  : filepos.h
   Class : TfilePos
   Desc  : A description

*****************************************************************************/


#ifndef FILEPOS
#define FILEPOS


class TfileDat;


class TfilePos
{
public:

   TfilePos(TfilePos const &pos) : Dobject(pos.Dobject), Dposn(pos.Dposn) {}
   TfilePos()                    : Dobject(NULL), Dposn(0) {}
   ~TfilePos() {}
   
   TfilePos const & operator =  (TfilePos const &pos);
   int              operator == (TfilePos const &pos);
   
protected:

private:

   friend class TfileDat;
   
   TfileDat *Dobject;
   long      Dposn;

};
   
inline TfilePos const & TfilePos::operator = (TfilePos const &pos)
{
   Dobject = pos.Dobject;
   Dposn   = pos.Dposn;
   return(*this);
}
   
inline int TfilePos::operator == (TfilePos const &pos)
{
   return((Dobject == pos.Dobject) && (Dposn == pos.Dposn));
}



#endif
