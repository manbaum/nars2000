//***************************************************************************
//  NARS2000 -- NARS Memory Allocation Functions Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2018 Sudley Place Software

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

// Uncomment one and only one of the following #defines
#define ANSI_MALLOC
//#define NARS_MALLOC

#if (defined ANSI_MALLOC)

void*   malloc  (size_t);
void    free    (void*);
void*   calloc  (size_t, size_t);
void*   realloc (void*, size_t);
#define malloc_usable_size(a)   HeapSize (GetProcessHeap (), 0, (a))

#elif (defined NARS_MALLOC)

void*  dlmalloc                 (size_t);
void   dlfree                   (void*);
void*  dlcalloc                 (size_t, size_t);
void*  dlrealloc                (void*, size_t);
size_t dlmalloc_usable_size     (void*);

#define malloc                  dlmalloc
#define free                    dlfree
#define calloc                  dlcalloc
#define realloc                 dlmalloc
#define malloc_usable_size      dlmalloc_usable_size

#endif


//***************************************************************************
//  End of File: NARSmalloc.h
//***************************************************************************
