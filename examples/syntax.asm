- this is a comment
AT #20
FUNCTION TEST_FUNC
    SWAP R1 R2
    NOP
    NOT R1
    JMP %TEST_FUNC
    MOV #AABB R3
.label1
    ADD R3 R6
    PUSH R2
    PUSH R0
    JMP %label1
    CALL %TEST_FUNC
END

DATA $HELLO #20 $WORLD

- process entry point
FUNCTION MAIN
    NOP
END

FUNCTION MAIN2
    NOP
END

AT #FFFE
- reset vector
DATA %MAIN
