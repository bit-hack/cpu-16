-- 16 bit Linear Feedback Shift Register for CPU16

AT #0
.seed
DATA #ffff

function rand
    - load seed
    load.w %seed r3
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
    - store seed
    store.w r3 %seed
    - return
    ret
end

function main
    - r3 = rand
    mov     #ff   r3
.loop
    - get a random number in r3
    call %rand
    - store in framebuffer
    mov     r5    r7
    add     #4000 r7
    store.b r3    r7
    - increment and wrap i
    add     #1    r5
    and     #0fff r5
    jmp     %loop
end

at #fffc
jmp %main
