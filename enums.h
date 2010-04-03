//***************************************************************************
//  NARS2000 -- Enums
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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


typedef enum tagMENUPOS_SM      // Positions of items in SM menu
{
    IDMPOS_SM_FILE = 0,         // 00:  File menu item
    IDMPOS_SM_EDIT    ,         // 01:  Edit ...
    IDMPOS_SM_VIEW    ,         // 02:  View ...
    IDMPOS_SM_WINDOW  ,         // 03:  Window ...
    IDMPOS_SM_HELP    ,         // 04:  Help ...
};

typedef enum tagMENUPOS_FE      // Positions of items in FE menu
{
    IDMPOS_FE_FILE = 0,         // 00:  File menu item
    IDMPOS_FE_EDIT    ,         // 01:  Edit ...
    IDMPOS_FE_VIEW    ,         // 02:  View ...
    IDMPOS_FE_WINDOW  ,         // 03:  Window ...
    IDMPOS_FE_OBJECTS ,         // 04:  Objects ...
    IDMPOS_FE_HELP    ,         // 05:  Help ...
};

// If you change either of the above tagMENUPOSxx enums, be sure
//   to change the one below so that it always has an entry for
//   each of the menu names across all such enums, and be sure to
//   change all <GetIDMPOS_xx> functions.

typedef enum tagALLMENUPOS      // All Menu Positions (the union of the above menu positions)
{
    IDMPOSNAME_FILE = 0,        // 00:  File menu name
    IDMPOSNAME_EDIT    ,        // 01:  Edit ...
    IDMPOSNAME_VIEW    ,        // 02:  View ...
    IDMPOSNAME_WINDOW  ,        // 03:  Window ...
    IDMPOSNAME_OBJECTS ,        // 04:  Objects ...
    IDMPOSNAME_HELP    ,        // 05:  Help ...
} ALLMENUPOS, *LPALLMENUPOS;

typedef UINT (*GETIDMPOS_XX) (ALLMENUPOS);


typedef enum tagWINDOWIDS       // Window IDs
{
    IDWC_DB_LB = 1,             // 01:  Window ID for ListBox       in DB window
    IDWC_FE_EC    ,             // 02:  ...           EditCtrl      in FE ...
    IDWC_SM_EC    ,             // 03:  ...           EditCtrl      in SM ...
    IDWC_SM_DB    ,             // 04:  ...           DebugCtrl     in SM ...
    IDWC_TC_MC    ,             // 05:  ...           MDI Client    in TC ...
    IDWC_CC_LB    ,             // 06:  ...           ListBox       in CC ...
    IDWC_PM_LB    ,             // 07:  ...           ListBox       in PM ...
    IDWC_MF_ST    ,             // 08:  ...           Static Wnd    in MF ...
    IDWC_TC       ,             // 09:  ...                            TC ...
    IDWC_RB       ,             // 0A:  ...                            RB ...
    IDWC_WS_RB    ,             // 0B:  ...           WS Window     in RB control
    IDWC_ED_RB    ,             // 0C:  ...           ED ...        in RB control
    IDWC_FN_RB    ,             // 0D:  ...           FN ...        in RB control
    IDWC_FW_RB    ,             // 0E:  ...           FW ...        in RB control
    IDWC_CBFN_FW  ,             // 0F:  ...           CB Font Name  in FW window
    IDWC_CBFS_FW  ,             // 10:  ...           CB Font Style in ...
    IDWC_EC_FW    ,             // 11:  ...           EC Font Size  in ...
    IDWC_UD_FW    ,             // 12:  ...           UpDown Ctrl   in ...
    IDWC_LW_RB    ,             // 13:  ...           LW Window     in RB control
    IDWC_TB_LW    ,             // 14:  ...           Toolbar       in LW window
};


typedef enum tagWINDOWCLASS     // Window Classes
{
    WINDOWCLASS_MF = 0  ,       // 00:  Master Frame
    WINDOWCLASS_RB      ,       // 01:  Rebar
    WINDOWCLASS_SM      ,       // 02:  Session Manager
    WINDOWCLASS_FE      ,       // 03:  Function Editor
    WINDOWCLASS_VE      ,       // 04:  Vector Editor
    WINDOWCLASS_ME      ,       // 05:  Matrix Editor
} WINDOWCLASS, *LPWINDOWCLASS;


//***************************************************************************
//  End of File: enums.h
//***************************************************************************