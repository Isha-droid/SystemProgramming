MACRO myMacro
MOV A, B
ADD A, C
MEND

START:
    myMacro
    SUB B, A
    myMacro
END
