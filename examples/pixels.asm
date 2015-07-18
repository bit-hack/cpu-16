AT #0
.sin_table
data #80 #83 #86 #89 #8d #90 #93 #96 #99 #9c #9f #a2 #a5 #a8 #ab #ae
data #b1 #b4 #b7 #ba #bc #bf #c2 #c4 #c7 #ca #cc #cf #d1 #d4 #d6 #d8
data #db #dd #df #e1 #e3 #e5 #e7 #e9 #ea #ec #ee #ef #f1 #f2 #f4 #f5
data #f6 #f7 #f9 #fa #fa #fb #fc #fd #fe #fe #ff #ff #ff #ff #ff #ff
data #ff #ff #ff #ff #ff #ff #ff #fe #fe #fd #fc #fb #fa #fa #f9 #f7
data #f6 #f5 #f4 #f2 #f1 #ef #ee #ec #ea #e9 #e7 #e5 #e3 #e1 #df #dd
data #db #d8 #d6 #d4 #d1 #cf #cc #ca #c7 #c4 #c2 #bf #bc #ba #b7 #b4
data #b1 #ae #ab #a8 #a5 #a2 #9f #9c #99 #96 #93 #90 #8d #89 #86 #83
data #80 #7d #7a #77 #73 #70 #6d #6a #67 #64 #61 #5e #5b #58 #55 #52
data #4f #4c #49 #46 #44 #41 #3e #3c #39 #36 #34 #31 #2f #2c #2a #28
data #25 #23 #21 #1f #1d #1b #19 #17 #16 #14 #12 #11 #0f #0e #0c #0b
data #0a #09 #07 #06 #06 #05 #04 #03 #02 #02 #01 #01 #01 #00 #00 #00
data #00 #00 #00 #00 #01 #01 #01 #02 #02 #03 #04 #05 #06 #06 #07 #09
data #0a #0b #0c #0e #0f #11 #12 #14 #16 #17 #19 #1b #1d #1f #21 #23
data #25 #28 #2a #2c #2f #31 #34 #36 #39 #3c #3e #41 #44 #46 #49 #4c
data #4f #52 #55 #58 #5b #5e #61 #64 #67 #6a #6d #70 #73 #77 #7a #7d

AT #1000
function main
    - r3 = x
    - r4 = y
    - r5 = i
    mov     ZR    r5
.loop
    - calculate x coordinate
    mov     r5    r3
    and     #3f   r3
    shl     #2    r3
    - calculate y coordinate
    mov     r5    r4
    shr     #6    r4
    shl     #2    r4 
    - multiply to make gradient
    mov     r3    r6
    mul     r4    r6
    shr     #8    r6
-   ldb     r6    r6
    - store in framebuffer
    mov     r5    r7
    add     #4000 r7
    stb     r6    r7
    - increment and wrap i
    add     #1    r5
    and     #0fff r5
    jmp     %loop
end

at #fffc
jmp %main
