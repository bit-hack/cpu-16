-- 16 bit Linear Feedback Shift Register for CPU16

AT #0
function main
    - r7 = rand
    mov     #ff   r7
.loop
    - bit 11
    mov     r7    r4
    shr     #A    r4
    and     #1    r4
    mov     r4    r9
    - bit 13
    mov     r7    r4
    shr     #C    r4
    and     #1    r4
    xor     r4    r9
    - bit 14
    mov     r7    r4
    shr     #D    r4
    and     #1    r4
    xor     r4    r9
    - bit 16
    mov     r7    r4
    shr     #F    r4
    and     #1    r4
    xor     r4    r9
    - add to input
    shl     #1    r7
    or      r9    r7
    - store in framebuffer
    stb+    r7    #4000 r5
    - wrap index
    and     #0fff r5
    jmp     %loop
end

at #fffc
jmp %main
