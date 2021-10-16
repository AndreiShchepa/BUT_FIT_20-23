; Vernamova sifra na architekture DLX
; Andrei Shchapaniak xshcha00

        .data 0x04          ; zacatek data segmentu v pameti
login:  .asciiz "xshcha00"  ; <-- nahradte vasim loginem
cipher: .space 9 ; sem ukladejte sifrovane znaky (za posledni nezapomente dat 0)

        .align 2            ; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login         ; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher        ; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40          ; adresa zacatku programu v pameti
        .global main        ; 

; registers : r4-r10-r17-r22-r30-r0

main: 

addi r4, r0, 0   ; r4 = 0 - index in login

loop_for:
	lb r17, login(r4)        ; r17 = login[r4]
	slti r22, r17, 97        ; r22 = r17 < 97 ? 1 : 0
	bnez r22, end_func       ; jump r22 != 0 ? end func
	nop
	nop

	andi r22, r4, 1          ; r22 = r4%2 == 0 ? 0 : 1;
	beqz r22, shift_forward  ; jump r22 == 0 ? shift_forward : shift_back
	nop
	nop
	
	j shift_back
	nop
	nop

add_26:
	addi r17, r17, 26     ; r17 += 26
	j set_letter_to_cipher
	nop
	nop

shift_back: 
	addi r22, r0, 2
	lb r10, login(r22)
	subi r10, r10, 96   ; r10 = login[2] - 96 (shift size)

	sub r17, r17, r10   ; shift back 

	sgei r22, r17, 97   ; r22 = r17 >= 'a' ? 1 : 0
	beqz r22, add_26    ; jump r22 == 0 ? add_26
	nop
	nop

	j set_letter_to_cipher
	nop
	nop

sub_26:
	subi r17, r17, 26     ; r17 -= 26
	j set_letter_to_cipher
	nop
	nop
	
shift_forward:
	addi r22, r0, 1
	lb r10, login(r22)
	subi r10, r10, 96    ; r10 = login[1] - 96 (shift size)
	
	add r17, r17, r10    ; shift forward

	sgti r22, r17, 122   ; r22 = r17 > 'z' ? 1 : 0
	bnez r22, sub_26     ; jump r22 != 0 ? sub_26
	nop
	nop
	
set_letter_to_cipher: 
	sb cipher(r4), r17  ; cipher[r4] = r17
	addi r4, r4, 1      ; r4 += 1
	j loop_for
	nop
	nop

end_func: 
	addi r30, r0, 0     ; r30 = 0
	sb cipher(r4), r30  ; cipher[r4] = '\0'
	nop
	nop

end:    addi r14, r0, caddr ; <-- pro vypis sifry nahradte laddr adresou caddr
        trap 5  ; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0  ; ukonceni simulace
