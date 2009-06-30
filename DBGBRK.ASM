; '$Header:   P:/PVCS/MAX/STATUTIL/DBGBRK.ASV   1.0   25 Sep 1996 10:14:50   BOB  $
        TITLE   DBGBRK.ASM

CPUFLAGS record  $ID:1,$VIP:1,$VIF:1,$ACHI:1,$VMHI:1,$RFHI:1, \
                 $R0:1,$NT:1,$IOPL:2,$OF:1,$DF:1,$IF:1,$TF:1,$SF:1,$ZF:1,$R1:1,$AF:1,$R2:1,$PF:1,$R3:1,$CF:1

ifdef IS_32
        .386
        .model FLAT,C
        .code

        OPTION READONLY
        OPTION OLDSTRUCTS

        public  DbgBrk
DbgBrk  proc    near            ; Start DbgBrk procedure

COMMENT |

Signal a debug breakpoint

|

DB_STR  struc

        dd      ?               ; Caller's EBP
DB_FL   dd      ?               ; ...      EFL

DB_STR  ends

        pushfd                  ; Save the flags
        push    ebp             ; Prepare to address the stack
        mov     ebp,esp         ; Hello, Mr. Stack
        or      [ebp].DB_FL,mask $TF ; TF=1
        pop     ebp             ; Restore
        popfd                   ; Put TF into effect

        ret                     ; Return to caller

        assume  ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing

DbgBrk  endp                    ; End DbgBrk procedure

else
ifdef IS_64
        .code

        public  DbgBrk
DbgBrk  proc                    ; Start DbgBrk procedure

COMMENT |

Signal a debug breakpoint

|

DB_STR  struct

        dq      ?               ; Caller's RBP
DB_FL   dq      ?               ; ...      RFL

DB_STR  ends

        pushf                   ; Save the flags
        push    Rbp             ; Prepare to address the stack
        mov     Rbp,Rsp         ; Hello, Mr. Stack
        or      DB_STR.DB_FL[Rbp],mask $TF ; TF=1
        pop     Rbp             ; Restore
        popf                    ; Put TF into effect

        ret                     ; Return to caller

DbgBrk  endp                    ; End DbgBrk procedure

        public  iAsmAdd64
iAsmAdd64 proc                  ; Start iAsmAdd64 procedure

ADD64_STR struct

ADD64_TMP dq    ?               ; Temporary var
        dq      ?               ; Caller's RBP
        dq      ?               ; Caller's return Rip

ADD64_STR ends

        push    Rbp             ; Prepare to address the stack
        sub     Rsp,8           ; Make room for temp
        mov     Rbp,Rsp         ; Hello, Mr. Stack

        mov     ADD64_STR.ADD64_TMP[Rbp],Rdx
        fild    ADD64_STR.ADD64_TMP[Rbp]

        mov     ADD64_STR.ADD64_TMP[Rbp],R8
        fild    ADD64_STR.ADD64_TMP[Rbp]

        faddp   st(1),st(0);
        fistp   ADD64_STR.ADD64_TMP[Rbp]

        push    Rax
        mov     Rax,ADD64_STR.ADD64_TMP[Rbp]
        mov     [Rcx],Rax
        pop     Rax

        add     Rsp,8           ; Strip temp from the stack
        pop     Rbp             ; Restore

        ret                     ; Return to caller

iAsmAdd64 endp                  ; End iAsmAdd64 procedure

        public  iAsmSub64
iAsmSub64 proc                  ; Start iAsmSub64 procedure

SUB64_STR struct

SUB64_TMP dq    ?               ; Temporary var
        dq      ?               ; Caller's RBP
        dq      ?               ; Caller's return Rip

SUB64_STR ends

        push    Rbp             ; Prepare to address the stack
        sub     Rsp,8           ; Make room for temp
        mov     Rbp,Rsp         ; Hello, Mr. Stack

        mov     SUB64_STR.SUB64_TMP[Rbp],Rdx
        fild    SUB64_STR.SUB64_TMP[Rbp]

        mov     SUB64_STR.SUB64_TMP[Rbp],R8
        fild    SUB64_STR.SUB64_TMP[Rbp]

        fsubp   st(1),st(0);
        fistp   SUB64_STR.SUB64_TMP[Rbp]

        push    Rax
        mov     Rax,SUB64_STR.SUB64_TMP[Rbp]
        mov     [Rcx],Rax
        pop     Rax

        add     Rsp,8           ; Strip temp from the stack
        pop     Rbp             ; Restore

        ret                     ; Return to caller

iAsmSub64 endp                  ; End iAsmSub64 procedure
else
.err Need code for this architecture
endif
endif
        end
