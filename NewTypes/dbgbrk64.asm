COMMENT|
/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2017 Sudley Place Software

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
|

        TITLE   DBGBRK64.ASM

CPUFLAGS record  $ID:1,$VIP:1,$VIF:1,$ACHI:1,$VMHI:1,$RFHI:1, \
                 $R0:1,$NT:1,$IOPL:2,$OF:1,$DF:1,$IF:1,$TF:1,$SF:1,$ZF:1,$R1:1,$AF:1,$R2:1,$PF:1,$R3:1,$CF:1

        .code

        public  NaN64
NaN64   dq      7FF0000000000001h   ; A Quiet NaN

        public  iAsmAdd64
iAsmAdd64 proc                  ; Start iAsmAdd64 procedure

COMMENT|

Add two signed 64-bit numbers,
handling overflow.

On entry:

Rdx     =       left arg
R8      =       right arg
Rcx     =       ptr to result

On exit:

Rax     =       1 if successful
        =       0 if not

|

        mov     Rax,Rdx         ; Copy left arg
        add     Rax,R8          ; Plus the right arg
                                ;   setting OF on error
        mov     qword ptr [Rcx],Rax ; Save in the result
        mov     Rax,0           ; Clear the result
        setno   al              ; Return 0 or 1 from the <add>

        ret                     ; Return to caller

iAsmAdd64 endp                  ; End iAsmAdd64 procedure

        public  iAsmSub64
iAsmSub64 proc                  ; Start iAsmSub64 procedure

COMMENT|

Subtract two signed 64-bit numbers,
handling overflow.

On entry:

Rdx     =       left arg
R8      =       right arg
Rcx     =       ptr to result

On exit:

Rax     =       1 if successful
        =       0 if not

|

        mov     Rax,Rdx         ; Copy left arg
        sub     Rax,R8          ; Less the right arg
                                ;   setting OF on error
        mov     qword ptr [Rcx],Rax ; Save in the result
        mov     Rax,0           ; Clear the result
        setno   al              ; Return 0 or 1 from the <sub>

        ret                     ; Return to caller

iAsmSub64 endp                  ; End iAsmSub64 procedure

        public  iAsmMul64
iAsmMul64 proc                  ; Start iAsmMul64 procedure

COMMENT|

Multiply two signed 64-bit numbers,
handling overflow.

On entry:

Rdx     =       left arg
R8      =       right arg
Rcx     =       ptr to result

On exit:

Rax     =       1 if successful
        =       0 if not

|

        push    Rdx             ; Save register clobbered by <imul>

        mov     Rax,Rdx         ; Copy left arg
        imul    R8              ; Times the right arg
                                ;   producing Rdx:Rax
                                ;   and setting OF and CF on error
        mov     qword ptr [Rcx],Rax ; Save in the result
        mov     Rax,0           ; Clear the result
        setno   al              ; Return 0 or 1 from the <imul>

        pop     Rdx             ; Restore

        ret                     ; Return to caller

iAsmMul64 endp                  ; End iAsmMul64 procedure

        public  iAsmMulPiInt
iAsmMulPiInt proc               ; Start iAsmMulPiInt procedure

COMMENT|

Multiply a signed 64-bit number by Pi.

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

(Nothing)

|

        finit                   ; Initialize the FPU

        fldpi                   ; Load Pi
        fild    qword ptr [Rdx] ; Load the integer right arg
        fmulp   st(1),st(0)     ; Multiply 'em
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmMulPiInt endp               ; End iAsmMulPiInt procedure

        public  iAsmMulPiFlt
iAsmMulPiFlt proc               ; Start iAsmMulPiFlt procedure

COMMENT|

Multiply a signed 64-bit number by Pi.

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

(Nothing)

|

        finit                   ; Initialize the FPU

        fldpi
        fld     qword ptr [Rdx] ; Load the float right arg
        fmulp   st(1),st(0)     ; Multiply 'em
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmMulPiFlt endp               ; End iAsmMulPiFlt procedure

        public  iAsmCircle4Int
iAsmCircle4Int proc             ; Start iAsmCircle4Int procedure

COMMENT|

Compute (1 + R * 2) * 0.5

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fild    qword ptr [Rdx] ; Load the integer right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fld1                    ;   1       R*R
        faddp   st(1),st(0)     ;   1+R*R
        fsqrt                   ;   (1+R*R)*0.5
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle4Int endp             ; End iAsmCircle4Int procedure

        public  iAsmCircle4Flt
iAsmCircle4Flt proc             ; Start iAsmCircle4Flt procedure

COMMENT|

Compute (1 + R * 2) * 0.5

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fld     qword ptr [Rdx] ; Load the float right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fld1                    ;   1       R*R
        faddp   st(1),st(0)     ;   1+R*R
        fsqrt                   ;   (1+R*R)*0.5
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle4Flt endp             ; End iAsmCircle4Flt procedure

        public  iAsmCircle3Int
iAsmCircle3Int proc             ; Start iAsmCircle3Int procedure

COMMENT|

Compute tan(R)

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fild    qword ptr [Rdx] ; Load the integer right arg
        fsincos                 ;   cos(R)  sin(R)
        fstsw   ax              ; AX = Status Word
                                ; AH = B:1,C3:1,TOP:3 ,C2:1,C1:1,C0:1
        fdivp   st(1),st(0)     ;   tan(R)
        sahf                    ; AH = SF ,ZF  ,0,AF,0,PF  ,1   ,CF
        jnp     iAsm3Int        ; Jump if C2 (Out of Range) flag is not set

        ffree   st(0)           ;  <empty>
        fld     NaN64           ;   NaN
iAsm3Int:
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle3Int endp             ; End iAsmCircle3Int procedure

        public  iAsmCircle3Flt
iAsmCircle3Flt proc             ; Start iAsmCircle3Flt procedure

COMMENT|

Compute tan(R)

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fld     qword ptr [Rdx] ; Load the float right arg
        fsincos                 ;   cos(R)  sin(R)
        fstsw   ax              ; AX = Status Word
                                ; AH = B:1,C3:1,TOP:3 ,C2:1,C1:1,C0:1
        fdivp   st(1),st(0)     ;   tan(R)
        sahf                    ; AH = SF ,ZF  ,0,AF,0,PF  ,1   ,CF
        jnp     iAsm3Flt        ; Jump if C2 (Out of Range) flag is not set

        ffree   st(0)           ;  <empty>
        fld     NaN64           ;   NaN
iAsm3Flt:
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle3Flt endp             ; End iAsmCircle3Flt procedure

        public  iAsmCircle2Int
iAsmCircle2Int proc             ; Start iAsmCircle2Int procedure

COMMENT|

Compute cos(R)

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fild    qword ptr [Rdx] ; Load the integer right arg
        fcos                    ;   cos(R)
        fstsw   ax              ; AX = Status Word
                                ; AH = B:1,C3:1,TOP:3 ,C2:1,C1:1,C0:1
        sahf                    ; AH = SF ,ZF  ,0,AF,0,PF  ,1   ,CF
        jnp     iAsm2Int        ; Jump if C2 (Out of Range) flag is not set

        ffree   st(0)           ;  <empty>
        fld     NaN64           ;   NaN
iAsm2Int:
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle2Int endp             ; End iAsmCircle2Int procedure

        public  iAsmCircle2Flt
iAsmCircle2Flt proc             ; Start iAsmCircle2Flt procedure

COMMENT|

Compute cos(R)

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fld     qword ptr [Rdx] ; Load the float right arg
        fcos                    ;   cos(R)
        fstsw   ax              ; AX = Status Word
                                ; AH = B:1,C3:1,TOP:3 ,C2:1,C1:1,C0:1
        sahf                    ; AH = SF ,ZF  ,0,AF,0,PF  ,1   ,CF
        jnp     iAsm2Flt        ; Jump if C2 (Out of Range) flag is not set

        ffree   st(0)           ;  <empty>
        fld     NaN64           ;   NaN
iAsm2Flt:
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle2Flt endp             ; End iAsmCircle2Flt procedure

        public  iAsmCircle1Int
iAsmCircle1Int proc             ; Start iAsmCircle1Int procedure

COMMENT|

Compute sin(R)

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fild    qword ptr [Rdx] ; Load the integer right arg
        fsin                    ;   sin(R)
        fstsw   ax              ; AX = Status Word
                                ; AH = B:1,C3:1,TOP:3 ,C2:1,C1:1,C0:1
        sahf                    ; AH = SF ,ZF  ,0,AF,0,PF  ,1   ,CF
        jnp     iAsm1Int        ; Jump if C2 (Out of Range) flag is not set

        ffree   st(0)           ;  <empty>
        fld     NaN64           ;   NaN
iAsm1Int:
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle1Int endp             ; End iAsmCircle1Int procedure

        public  iAsmCircle1Flt
iAsmCircle1Flt proc             ; Start iAsmCircle1Flt procedure

COMMENT|

Compute sin(R)

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fld     qword ptr [Rdx] ; Load the float right arg
        fsin                    ;   sin(R)
        fstsw   ax              ; AX = Status Word
                                ; AH = B:1,C3:1,TOP:3 ,C2:1,C1:1,C0:1
        sahf                    ; AH = SF ,ZF  ,0,AF,0,PF  ,1   ,CF
        jnp     iAsm1Flt        ; Jump if C2 (Out of Range) flag is not set

        ffree   st(0)           ;  <empty>
        fld     NaN64           ;   NaN
iAsm1Flt:
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle1Flt endp             ; End iAsmCircle1Flt procedure

        public  iAsmCircle0Int
iAsmCircle0Int proc             ; Start iAsmCircle0Int procedure

COMMENT|

Compute (1 - R * 2) * 0.5

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fild    qword ptr [Rdx] ; Load the integer right arg
        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fchs                    ;   -R*R
        fld1                    ;   1       -R*R
        faddp   st(1),st(0)     ;   1-R*R
        fsqrt                   ;   (1-R*R)*0.5
        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle0Int endp             ; End iAsmCircle0Int procedure

        public  iAsmCircle0Flt
iAsmCircle0Flt proc             ; Start iAsmCircle0Flt procedure

COMMENT|

Compute (1 - R * 2) * 0.5

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fld     qword ptr [Rdx] ; Load the float right arg

        fld     st(0)           ;   R       R
        fmulp   st(1),st(0)     ;   R*R
        fchs                    ;   -R*R
        fld1                    ;   1       -R*R
        faddp   st(1),st(0)     ;   1-R*R
        fsqrt                   ;   (1-R*R)*0.5

        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircle0Flt endp             ; End iAsmCircle0Flt procedure

        public  iAsmCircleN4Int
iAsmCircleN4Int proc            ; Start iAsmCircleN4Int procedure

COMMENT|

Compute (R + 1) × sqrt ((R - 1) / (R + 1))
a.k.a.  ((-1) + R * 2) * 0.5

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fild    qword ptr [Rdx] ; Load the integer right arg

        fld     st(0)           ;   R           R
        fld1                    ;   1           R      R
        faddp   st(1),st(0)     ; R+1           R
        fld1                    ;   1           R+1    R
        fsubp   st(2),st(0)     ; R+1           R-1
        fxch                    ; R-1           R+1
        fld     st(1)           ; R+1           R-1    R+1
        fdivp   st(1),st(0)     ; (R-1)/(R+1)   R+1
        fsqrt                   ; sqrt(...)     R+1
        fmulp   st(1),st(0)     ; sqrt(...) * (R+1)

        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircleN4Int endp            ; End iAsmCircleN4Int procedure

        public  iAsmCircleN4Flt
iAsmCircleN4Flt proc            ; Start iAsmCircleN4Flt procedure

COMMENT|

Compute (R + 1) × sqrt ((R - 1) / (R + 1))
a.k.a.  ((-1) + R * 2) * 0.5

On entry:

Rdx     =       ptr to right arg
Rcx     =       ptr to result

On exit:

EAX     =       clobbered

|

        finit                   ; Initialize the FPU

        fld     qword ptr [Rdx] ; Load the float right arg

        fld     st(0)           ;   R           R
        fld1                    ;   1           R      R
        faddp   st(1),st(0)     ; R+1           R
        fld1                    ;   1           R+1    R
        fsubp   st(2),st(0)     ; R+1           R-1
        fxch                    ; R-1           R+1
        fld     st(1)           ; R+1           R-1    R+1
        fdivp   st(1),st(0)     ; (R-1)/(R+1)   R+1
        fsqrt                   ; sqrt(...)     R+1
        fmulp   st(1),st(0)     ; sqrt(...) * (R+1)

        fstp    qword ptr [Rcx] ; Save in result

        ret                     ; Return to caller

iAsmCircleN4Flt endp            ; End iAsmCircleN4Flt procedure

        end
