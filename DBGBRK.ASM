; '$Header:   P:/PVCS/MAX/STATUTIL/DBGBRK.ASV   1.0   25 Sep 1996 10:14:50   BOB  $
        TITLE   DBGBRK.ASM
        .386
        OPTION READONLY
        OPTION OLDSTRUCTS

CPUFLAGS record  $ID:1,$VIP:1,$VIF:1,$ACHI:1,$VMHI:1,$RFHI:1, \
                 $R0:1,$NT:1,$IOPL:2,$OF:1,$DF:1,$IF:1,$TF:1,$SF:1,$ZF:1,$R1:1,$AF:1,$R2:1,$PF:1,$R3:1,$CF:1

        .model FLAT,C
        .code

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

        end
