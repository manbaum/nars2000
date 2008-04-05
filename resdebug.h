//****************************************************************************
//  NARS2000 -- Resource Debugging Header File
//****************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2008 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#if RESDEBUG
  #define MyCreateCompatibleBitmap(a,b,c) _MyCreateCompatibleBitmap(a,b,c,__LINE__)
  #define MyCreateCompatibleDC(a)         _MyCreateCompatibleDC(a,__LINE__)
  #define MyCreateFontIndirect(a)         _MyCreateFontIndirect(a,__LINE__)
  #define MyCreatePen(a,b,c)              _MyCreatePen(a,b,c,__LINE__)
  #define MyCreatePolygonRgn(a,b,c)       _MyCreatePolygonRgn(a,b,c,__LINE__)
  #define MyCreateRectRgnIndirect(a)      _MyCreateRectRgnIndirect(a,__LINE__)
  #define MyCreateSolidBrush(a)           _MyCreateSolidBrush(a,__LINE__)
  #define MyDeleteDC(a)                   _MyDeleteDC(a,__LINE__)
  #define MyDeleteObject(a)               _MyDeleteObject(a,__LINE__)
  #define MyGetDC(a)                      _MyGetDC(a,__LINE__)
  #define MyGetWindowDC(a)                _MyGetWindowDC(a,__LINE__)
  #define MyGlobalAlloc(a,b)              _MyGlobalAlloc(a,b,__LINE__)
  #define MyGlobalFree(a)                 _MyGlobalFree(a,__LINE__)
  #define MyGlobalLock(a)                 _MyGlobalLock(a,FNLN)
  #define MyGlobalSize(a)                 _MyGlobalSize(a,__LINE__)
  #define MyGlobalFlags(a)                _MyGlobalFlags(a,__LINE__)
  #define MyGlobalReAlloc(a,b,c)          _MyGlobalReAlloc(a,b,c,__LINE__)
  #define MyGlobalUnlock(a)               _MyGlobalUnlock(a,__LINE__)
  #define MyLoadBitmap(a,b)               _MyLoadBitmap(a,b,__LINE__)
  #define MyLoadImage(a,b,c,d,e,f)        _MyLoadImage(a,b,c,d,e,f,__LINE__)
  #define MyQueryObject(a,b)              _MyQueryObject(a,b,__LINE__)
  #define MyReleaseDC(a,b)                _MyReleaseDC(a,b,__LINE__)

  #define OBJ_GLOBAL      15
#else
  #define MyCreateCompatibleBitmap(a,b,c) CreateCompatibleBitmap(a,b,c)
  #define MyCreateCompatibleDC(a)         CreateCompatibleDC(a)
  #define MyCreateFontIndirect(a)         CreateFontIndirect(a)
  #define MyCreatePen(a,b,c)              CreatePen(a,b,c)
  #define MyCreatePolygonRgn(a,b,c)       CreatePolygonRgn(a,b,c)
  #define MyCreateRectRgnIndirect(a)      CreateRectRgnIndirect(a)
  #define MyCreateSolidBrush(a)           CreateSolidBrush(a)
  #define MyDeleteDC(a)                   DeleteDC(a)
  #define MyDeleteObject(a)               DeleteObject(a)
  #define MyGetDC(a)                      GetDC(a)
  #define MyGetWindowDC(a)                GetWindowDC(a)
  #define MyGlobalAlloc(a,b)              GlobalAlloc(a,b)
  #define MyGlobalFree(a)                 GlobalFree(a)
  #define MyGlobalLock(a)                 GlobalLock(a)
  #define MyGlobalSize(a)                 GlobalSize(a)
  #define MyGlobalFlags(a)                GlobalFlags(a)
  #define MyGlobalReAlloc(a,b,c)          GlobalReAlloc(a,b,c)
  #define MyGlobalUnlock(a)               GlobalUnlock(a)
  #define MyLoadBitmap(a,b)               LoadBitmap(a,b)
  #define MyLoadImage(a,b,c,d,e,f)        LoadImage(a,b,c,d,e,f)
  #define MyQueryObject(a,b)
  #define MyReleaseDC(a,b)                ReleaseDC(a,b)
#endif

//***************************************************************************
//  End of File: resdebug.h
//***************************************************************************
