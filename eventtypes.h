//***************************************************************************
//  NARS2000 -- Event Types
//***************************************************************************

// Use this macro to join major and minor events into a single value
#define MAKE_ET(Major,Minor)    ((((USHORT) Major) << 16) | (USHORT) Minor)

// Use these macros to split apart an EventType into its major and minor parts
#define ET_MAJOR(et)            (((UINT) et) >> 16)
#define ET_MINOR(et)            (((UINT) et) & 0xFFFF)

// The EventType type
#define EVENT_TYPES             UINT

// The event types, taken from the APL2 Language Reference Manual (Feb 1994)
#define EVENTTYPE_NOERROR       MAKE_ET (0,  0)
#define EVENTTYPE_UNK           MAKE_ET (0,  1)
#define EVENTTYPE_BREAK         MAKE_ET (1,  1)
#define EVENTTYPE_WS_FULL       MAKE_ET (1,  3)
#define EVENTTYPE_LIMIT_ST      MAKE_ET (1,  4)
#define EVENTTYPE_LIMIT_RANK    MAKE_ET (1,  8)
#define EVENTTYPE_LIMIT_IMPL    MAKE_ET (1, 13)
#define EVENTTYPE_SYNTAX_ERROR  MAKE_ET (2,   )
#define EVENTTYPE_              MAKE_ET ( ,   )
#define EVENTTYPE_              MAKE_ET ( ,   )
#define EVENTTYPE_              MAKE_ET ( ,   )
#define EVENTTYPE_              MAKE_ET ( ,   )
#define EVENTTYPE_              MAKE_ET ( ,   )





//***************************************************************************
//  End of File: eventtypes.h
//***************************************************************************
