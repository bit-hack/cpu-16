- hello world from CPU16

function main
    - load start of string
    mov %s1 r4
.loop
    - load next character
    ldb+ r4 r5
    jeq zr r5 %end
    - write to serial out
    stb r5 #8000
    - if not a end of string
    jmp %loop
.end
    jmp %end
end

.s1
data $HELLO #20 $WORLD #00

at #fffc
jmp %main
