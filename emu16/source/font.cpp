#include <stdint.h>
#include <assert.h>

extern uint8_t font_3_5[2048];

uint8_t * font_3x5_get( char ch ) {
    assert(ch >= 0 && ch <= 127);
    return font_3_5 + (ch * 16);
}

uint8_t font_3x5[2048] = {

    // 0
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 1
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 2
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 3
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 4
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 5
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 6
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 7
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 8
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 9
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 10
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 11
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 12
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 13
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 14
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 15
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 16
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 17
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 18
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 19
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 20
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 21
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 22
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 23
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 24
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 25
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 26
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 27
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 28
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 29
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 30
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 31
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1, 0,

    // 32 ' '
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,

    // 33 '!'
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 0, 0,
    0, 1, 0, 0,

    // 34 '#'
    1, 0, 1,
    1, 0, 1,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,

    // 35 '$'
    1, 0, 1,
    1, 1, 1,
    1, 0, 1,
    1, 1, 1,
    1, 0, 1, 0,

    // 36 '%'
    0, 1, 1,
    1, 1, 0,
    0, 1, 1,
    1, 1, 0,
    0, 1, 0, 0,

    // 37 '%'
    1, 0, 0,
    0, 0, 1,
    0, 1, 0,
    1, 0, 0,
    0, 0, 1, 0,

    // 38 '&'
    0, 1, 0,
    1, 0, 1,
    0, 1, 0,
    1, 0, 1,
    0, 1, 1, 0,

    // 39 '''
    0, 1, 0,
    0, 1, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,

    // 40 '('
    0, 0, 1,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 0, 1, 0,

    // 41 ')'
    1, 0, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    1, 0, 0, 0,

    // 42 '*'
    1, 0, 1,
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    1, 0, 1, 0,

    // 43 '+'
    0, 0, 0,
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    0, 0, 0, 0,

    // 44 ','
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 1, 0,
    1, 0, 0, 0,

    // 45 '-'
    0, 0, 0,
    0, 0, 0,
    1, 1, 1,
    0, 0, 0,
    0, 0, 0, 0,

    // 46 ','
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 47 '/'
    0, 0, 0,
    0, 0, 1,
    0, 1, 0,
    1, 0, 0,
    0, 0, 0, 0,

    // 48 '0'
    1, 1, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 49 '|'
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 50 '2'
    1, 1, 1,
    0, 0, 1,
    1, 1, 1,
    1, 0, 0,
    1, 1, 1, 0,

    // 51 '3'
    1, 1, 1,
    0, 0, 1,
    0, 1, 1,
    0, 0, 1,
    1, 1, 1, 0,

    // 52 '4'
    1, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 0, 1,
    0, 0, 1, 0,

    // 53 '5'
    1, 1, 1,
    1, 0, 0,
    1, 1, 1,
    0, 0, 1,
    1, 1, 1, 0,

    // 54 '6'
    1, 1, 1,
    1, 0, 0,
    1, 1, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 55 '7'
    1, 1, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1, 0,

    // 56 '8'
    1, 1, 1,
    1, 0, 1,
    1, 1, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 57 '9'
    1, 1, 1,
    1, 0, 1,
    1, 1, 1,
    0, 0, 1,
    1, 1, 1, 0,

    // 58 ':'
    0, 0, 0,
    0, 1, 0,
    0, 0, 0,
    0, 1, 0,
    0, 0, 0, 0,

    // 59 ';'
    0, 0, 0,
    0, 1, 0,
    0, 0, 0,
    0, 1, 0,
    1, 0, 0, 0,

    // 60 '<'
    0, 0, 1,
    0, 1, 0,
    1, 0, 0,
    0, 1, 0,
    0, 0, 1, 0,

    // 61 '='
    0, 0, 0,
    1, 1, 1,
    0, 0, 0,
    1, 1, 1,
    0, 0, 0, 0,

    // 62 '>'
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
    0, 1, 0,
    1, 0, 0, 0,

    // 63 '?'
    1, 1, 1,
    0, 0, 1,
    0, 1, 0,
    0, 0, 0,
    0, 1, 0, 0,

    // 64 'e'
    0, 1, 0,
    1, 0, 1,
    1, 1, 1,
    1, 0, 0,
    0, 1, 1, 0,

    // 65 'A'
    0, 1, 0,
    1, 0, 1,
    1, 1, 1,
    1, 0, 1,
    1, 0, 1, 0,

    // 66 'B'
    1, 1, 0,
    1, 0, 1,
    1, 1, 0,
    1, 0, 1,
    1, 1, 0, 0,

    // 67 'C'
    0, 1, 1,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    0, 1, 1, 0,

    // 68 'D'
    1, 1, 0,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 1, 0, 0,

    // 69 'R'
    1, 1, 1,
    1, 0, 0,
    1, 1, 1,
    1, 0, 0,
    1, 1, 1, 0,

    // 70 'F'
    1, 1, 1,
    1, 0, 0,
    1, 1, 1,
    1, 0, 0,
    1, 0, 0, 0,

    // 71 'G'
    0, 1, 1,
    1, 0, 0,
    1, 1, 1,
    1, 0, 1,
    0, 1, 1, 0,

    // 72 'H'
    1, 0, 1,
    1, 0, 1,
    1, 1, 1,
    1, 0, 1,
    1, 0, 1, 0,

    // 73 'I'
    1, 1, 1,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    1, 1, 1, 0,

    // 74 'J'
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    1, 0, 1,
    0, 1, 0, 0,

    // 75 'K'
    1, 0, 1,
    1, 0, 1,
    1, 1, 0,
    1, 0, 1,
    1, 0, 1, 0,

    // 76 'L'
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 1, 1, 0,

    // 77 'H'
    1, 0, 1,
    1, 1, 1,
    1, 1, 1,
    1, 0, 1,
    1, 0, 1, 0,

    // 78 'N'
    1, 0, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 0, 1, 0,

    // 79 'O'
    0, 1, 0,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    0, 1, 0, 0,

    // 80 'P'
    1, 1, 0,
    1, 0, 1,
    1, 1, 0,
    1, 0, 0,
    1, 0, 0, 0,

    // 81 'Q'
    0, 1, 0,
    1, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1, 0,

    // 82 'R'
    1, 1, 0,
    1, 0, 1,
    1, 1, 1,
    1, 1, 0,
    1, 0, 1, 0,

    // 83 'S'
    0, 1, 1,
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
    1, 1, 0, 0,

    // 84 'T'
    1, 1, 1,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 85 'U'
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    0, 1, 1, 0,

    // 86 'V'
    1, 0, 1,
    1, 0, 1,
    1, 0, 1,
    0, 1, 0,
    0, 1, 0, 0,

    // 87 'W'
    1, 0, 1,
    1, 0, 1,
    1, 1, 1,
    1, 1, 1,
    1, 0, 1, 0,

    // 88 'X'
    1, 0, 1,
    1, 0, 1,
    0, 1, 0,
    1, 0, 1,
    1, 0, 1, 0,

    // 89 'Y'
    1, 0, 1,
    1, 0, 1,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 90 'Z'
    1, 1, 1,
    0, 0, 1,
    0, 1, 0,
    1, 0, 0,
    1, 1, 1, 0,

    // 91 '['
    1, 1, 1,
    1, 0, 0,
    1, 0, 0,
    1, 0, 0,
    1, 1, 1, 0,

    // 92 '\'
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    0, 0, 1,
    0, 0, 0, 0,

    // 93 ']'
    1, 1, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    1, 1, 1, 0,

    // 94 '^'
    0, 1, 0,
    1, 0, 1,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,

    // 95 '_'
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    1, 1, 1, 0,

    // 96 '`'
    1, 0, 0,
    0, 1, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,

    // 97 'a'
    0, 0, 0,
    0, 0, 0,
    0, 1, 1,
    1, 0, 1,
    1, 1, 1, 0,

    // 98 'b'
    1, 0, 0,
    1, 0, 0,
    1, 1, 0,
    1, 0, 1,
    1, 1, 0, 0,

    // 99 'c'
    0, 0, 0,
    0, 0, 0,
    0, 1, 1,
    1, 0, 0,
    0, 1, 1, 0,

    // 100 'd'
    0, 0, 1,
    0, 0, 1,
    0, 1, 1,
    1, 0, 1,
    0, 1, 1, 0,

    // 101 'e'
    0, 0, 0,
    0, 1, 1,
    1, 0, 1,
    1, 1, 0,
    0, 1, 1, 0,

    // 102 'f'
    0, 0, 1,
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    0, 1, 0, 0,

    // 103 'g'
    0, 0, 0,
    0, 1, 1,
    1, 1, 1,
    0, 0, 1,
    1, 1, 0, 0,

    // 104 'h'
    1, 0, 0,
    1, 0, 0,
    1, 1, 0,
    1, 0, 1,
    1, 0, 1, 0,

    // 105 'i'
    0, 1, 0,
    0, 0, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 106 'j'
    0, 0, 1,
    0, 0, 0,
    0, 0, 1,
    0, 0, 1,
    1, 1, 0, 0,

    // 107 'k'
    0, 0, 0,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    1, 0, 1, 0,

    // 108 'l'
    1, 1, 0,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0,
    1, 1, 1, 0,

    // 109 'm'
    0, 0, 0,
    0, 0, 0,
    1, 1, 1,
    1, 1, 1,
    1, 0, 1, 0,

    // 110 'n'
    0, 0, 0,
    0, 0, 0,
    1, 1, 0,
    1, 0, 1,
    1, 0, 1, 0,

    // 111 'o'
    0, 0, 0,
    0, 0, 0,
    0, 1, 0,
    1, 0, 1,
    0, 1, 0, 0,

    // 112 'p'
    0, 0, 0,
    0, 1, 0,
    1, 0, 1,
    1, 1, 0,
    1, 0, 0, 0,

    // 113 'q'
    0, 0, 0,
    0, 1, 0,
    1, 0, 1,
    0, 1, 1,
    0, 0, 1, 0,

    // 114 'r'
    0, 0, 0,
    1, 0, 0,
    1, 1, 1,
    1, 0, 0,
    1, 0, 0, 0,

    // 115 's'
    0, 0, 0,
    0, 1, 1,
    1, 1, 0,
    0, 1, 1,
    1, 1, 0, 0,

    // 116 't'
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 117 'u'
    0, 0, 0,
    0, 0, 0,
    1, 0, 1,
    1, 0, 1,
    0, 1, 1, 0,

    // 118 'v'
    0, 0, 0,
    0, 0, 0,
    1, 0, 1,
    1, 1, 1,
    0, 1, 0, 0,

    // 119 'w'
    0, 0, 0,
    0, 0, 0,
    1, 0, 1,
    1, 1, 1,
    1, 1, 1, 0,

    // 120 'x'
    0, 0, 0,
    0, 0, 0,
    1, 0, 1,
    0, 1, 0,
    1, 0, 1, 0,

    // 121 'y'
    0, 0, 0,
    1, 0, 1,
    0, 1, 1,
    0, 0, 1,
    1, 1, 0, 0,

    // 122 'z'
    0, 0, 0,
    1, 1, 1,
    0, 1, 1,
    1, 1, 0,
    1, 1, 1, 0,

    // 123 '{'
    0, 1, 1,
    0, 1, 0,
    1, 0, 0,
    0, 1, 0,
    0, 1, 1, 0,

    // 124 '|'
    0, 1, 0,
    0, 1, 0,
    0, 0, 0,
    0, 1, 0,
    0, 1, 0, 0,

    // 125 '}'
    1, 1, 0,
    0, 1, 0,
    0, 0, 1,
    0, 1, 0,
    1, 1, 0, 0,

    // 126 '~'
    0, 1, 1,
    1, 1, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,

    // 127 ''
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1, 0,

}; // font_4_5