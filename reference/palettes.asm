; palette is initialized from 838851-838979

; palette indexes referred to here start at 1

ORG $838851
    lda #$CA4E ; 83:8851
    sta $00 ; 83:8854
    sep #$20 ; 83:8856
    lda $07D0 ; 83:8858
    jsl $80A89D ; 83:885B
    lda #$89 ; 83:885F
    sta $02 ; 83:8861
    rep #$20 ; 83:8863
    inc $00 ; 83:8865
    lda [$00] ; 83:8867
    clc ; 83:8869
    adc #$CB00 ; 83:886A
    sta $0759 ; 83:886D
    dec $00 ; 83:8870
    lda [$00] ; 83:8872
; holds a to a table which is made of sets of two colors, which are written to the 8th + 9th indexes of palette 4
; the value goes in steps of 2 from 0 - $1E
; this is used on levels like 1-18 with firey terrain that changes brightness
; the first offset is the brightest, then it decreases, then increases again
; the pointer used is [$0758] + $3000 + ([$074F] & $001E)
    sta $0758 ; 83:8874
    lda #$C9EB ; 83:8877
    sta $00 ; 83:887A
    sep #$20 ; 83:887C
    lda $07D0 ; 83:887E
    jsl $80A89D ; 83:8881
    lda #$89 ; 83:8885
    sta $02 ; 83:8887
    rep #$20 ; 83:8889
    inc $00 ; 83:888B
    lda [$00] ; 83:888D
    clc ; 83:888F
    adc #$CB00 ; 83:8890
    sta $04 ; 83:8893
    dec $00 ; 83:8895
    lda [$00] ; 83:8897
    sta $03 ; 83:8899
; fills all of cgram 
    ldy #$0100 ; 83:889B
    ldx #$0000 ; 83:889E
-:
    lda [$03] ; 83:88A1
    sta $7F5130,X ; 83:88A3
    inc $03 ; 83:88A7
    inc $03 ; 83:88A9
    inx ; 83:88AB
    inx ; 83:88AC
    dey ; 83:88AD
    bne - ; 83:88AE
; overwrites palettes 6 - 8 ?
    ldy #$0060 ; 83:88B0
    ldx #$0000 ; 83:88B3
-:
    lda $95F800,X ; 83:88B6
    sta $7F51D0,X ; 83:88BA
    inx ; 83:88BE
    dey ; 83:88BF
    inx ; 83:88C0
    dey ; 83:88C1
    bne - ; 83:88C2
; overwrites the first palette
    ldy #$0020 ; 83:88C4
    ldx #$0000 ; 83:88C7
-:
    lda $94D080,X ; 83:88CA
    sta $7F5130,X ; 83:88CE
    inx ; 83:88D2
    dey ; 83:88D3
    inx ; 83:88D4
    dey ; 83:88D5
    bne - ; 83:88D6
; mirror palette 6 > 13
    ldy #$0020 ; 83:88D8
    ldx #$0000 ; 83:88DB
-:
    lda $7F51D0,X ; 83:88DE
    sta $7F52B0,X ; 83:88E2
    inx ; 83:88E6
    dey ; 83:88E7
    inx ; 83:88E8
    dey ; 83:88E9
    bne - ; 83:88EA
; mirror palette 7 > 15
    ldy #$0020 ; 83:88EC
    ldx #$0000 ; 83:88EF
-:
    lda $7F51F0,X ; 83:88F2
    sta $7F52F0,X ; 83:88F6
    inx ; 83:88FA
    dey ; 83:88FB
    inx ; 83:88FC
    dey ; 83:88FD
    bne - ; 83:88FE
; mirror palette 8 > 14
    ldy #$0020 ; 83:8900
    ldx #$0000 ; 83:8903
-:
    lda $7F5210,X ; 83:8906
    sta $7F52D0,X ; 83:890A
    inx ; 83:890E
    dey ; 83:890F
    inx ; 83:8910
    dey ; 83:8911
    bne - ; 83:8912
; copied palette 1's 4th color to the last color of palettes 13, 14, 15
    lda $7F5136 ; 83:8914
    sta $7F52CE ; 83:8918
    sta $7F52EE ; 83:891C
    sta $7F530E ; 83:8920
; overwrite pallete 9
    ldy #$0020 ; 83:8924
    ldx #$0000 ; 83:8927
-:
    lda $94D000,X ; 83:892A
    sta $7F5230,X ; 83:892E
    inx ; 83:8932
    dey ; 83:8933
    inx ; 83:8934
    dey ; 83:8935
    bne - ; 83:8936
; overwrite pallete 12
    ldy #$0020 ; 83:8938
    ldx #$0000 ; 83:893B
-:
    lda $94D020,X ; 83:893E
    sta $7F5290,X ; 83:8942
    inx ; 83:8946
    dey ; 83:8947
    inx ; 83:8948
    dey ; 83:8949
    bne - ; 83:894A
; overwrie palette 16
    ldy #$0020 ; 83:894C
    ldx #$0000 ; 83:894F
-:
    lda $94D060,X ; 83:8952
    sta $7F5310,X ; 83:8956
    inx ; 83:895A
    dey ; 83:895B
    inx ; 83:895C
    dey ; 83:895D
    bne - ; 83:895E
; 1st color of palette 1 is always 1
    lda #$0000 ; 83:8960
    sta $7F5130 ; 83:8963
    lda #$8300 ; 83:8967
    sta $01 ; 83:896A
    lda #$897A ; 83:896C
    sta $00 ; 83:896F
    jsl $808EAD ; 83:8971
    jsl $93E007 ; 83:8975
    rtl ; 83:8979
