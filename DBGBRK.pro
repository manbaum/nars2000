#define Dbgbrk  DbgBrk
#define Dbgstop DbgStop
void DbgBrk (void);

UBOOL iAsmAdd64 (LPAPLINT, APLINT, APLINT);
UBOOL iAsmSub64 (LPAPLINT, APLINT, APLINT);
UBOOL iAsmMul64 (LPAPLINT, APLINT, APLINT);

void iAsmMulPiInt    (LPAPLFLOAT, LPAPLINT  );
void iAsmMulPiFlt    (LPAPLFLOAT, LPAPLFLOAT);
void iAsmCircle4Int  (LPAPLFLOAT, LPAPLINT  );
void iAsmCircle4Flt  (LPAPLFLOAT, LPAPLFLOAT);
void iAsmCircle3Int  (LPAPLFLOAT, LPAPLINT  );
void iAsmCircle3Flt  (LPAPLFLOAT, LPAPLFLOAT);
void iAsmCircle2Int  (LPAPLFLOAT, LPAPLINT  );
void iAsmCircle2Flt  (LPAPLFLOAT, LPAPLFLOAT);
void iAsmCircle1Int  (LPAPLFLOAT, LPAPLINT  );
void iAsmCircle1Flt  (LPAPLFLOAT, LPAPLFLOAT);
void iAsmCircle0Int  (LPAPLFLOAT, LPAPLINT  );
void iAsmCircle0Flt  (LPAPLFLOAT, LPAPLFLOAT);
void iAsmCircleN4Int (LPAPLFLOAT, LPAPLINT  );
void iAsmCircleN4Flt (LPAPLFLOAT, LPAPLFLOAT);
