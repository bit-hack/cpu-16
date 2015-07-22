-- CPU16, Linear Feedback Shift Register example

at #0
.seed
DATA #ffff

at #20
function rand
    - load seed
    ldw     %seed zr r10
    - bit 11
    mov     r10   r4
    shr     #A    r4
    and     #1    r4
    mov     r4    r9
    - bit 13
    mov     r10   r4
    shr     #C    r4
    and     #1    r4
    xor     r4    r9
    - bit 14
    mov     r10   r4
    shr     #D    r4
    and     #1    r4
    xor     r4    r9
    - bit 16
    mov     r10   r4
    shr     #F    r4
    and     #1    r4
    xor     r4    r9
    - add to input
    shl     #1    r10
    or      r9    r10
    - store seed
    stw     r10 %seed zr
    - return
    ret
end

at #80
function main
    - zero index
    mov     ZR    r5
    - setup stack
    mov     #a000 SP
    - enable interrupts
-    or      #0001 CR
.loop
    - get a random number in r10
    call %rand
    - store in framebuffer
    stb+    r10   #4000 r5
    and     #0fff r5
    jmp     %loop
end

at #ffe0
-- vblank
ret
nop
-- hblank
ret
nop

at #fffc
jmp %main
