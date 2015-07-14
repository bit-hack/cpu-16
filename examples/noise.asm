-- 16 bit Linear Feedback Shift Register for CPU16

AT #0
function main
    - r3 = rand
    mov     #ff   r3
.loop
    - bit 11
    mov     r3    r4
    shr     #A    r4
    and     #1    r4
    mov     r4    r9
    - bit 13
    mov     r3    r4
    shr     #C    r4
    and     #1    r4
    xor     r4    r9
    - bit 14
    mov     r3    r4
    shr     #D    r4
    and     #1    r4
    xor     r4    r9
    - bit 16
    mov     r3    r4
    shr     #F    r4
    and     #1    r4
    xor     r4    r9
    - add to input
    shl     #1    r3
    or      r9    r3
    - store in framebuffer
    mov     r5    r7
    add     #4000 r7
    stb     r3    r7
    - increment and wrap i
    add     #1    r5
    and     #0fff r5
    jmp     %loop
end

at #fffc
jmp %main
