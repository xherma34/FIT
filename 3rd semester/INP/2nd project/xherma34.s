; Vernamova sifra na architekture DLX
; Pavel Heømann xherma34

        .data 0x04          ; zacatek data segmentu v pameti
login:  .asciiz "xherma34"  ; <-- nahradte vasim loginem
cipher: .space 9 ; sem ukladejte sifrovane znaky (za posledni nezapomente dat 0)

        .align 2            ; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login         ; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher        ; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40          ; adresa zacatku programu v pameti
        .global main        ; 

main:   ; sem doplnte reseni Vernamovy sifry dle specifikace v zadani

	addi r25, r25, 1	;r0->indexovaci registr pro array, prictu abych mohl vytahnout 1. klic
	lb r20, login(r25)	;do registru r20 ulozim prvni klic
	subi r20, r20, 96	;"a" ma ascii hodnotu 97, odectu tedy od hodnoty 96, abych i pro a zachoval hodnotu klice +1

	addi r25, r25, 1		;r0->indexovaci registr pro array, prictu abych mohl vytahnout 2. klic
	lb r23, login(r25)	;do registru r23 ulozim druhy klic
	subi r23, r23, 96	;"a" ma ascii hodnotu 97, odectu tedy od hodnoty 96, abych i pro a zachoval hodnotu klice +1
	xor r25, r25, r25

getValue:
	lb r27, login(r25)	;nactu si prvni znak arraye login[0]
	j filter		;Prozenu hodnotu filtrem
	nop
	
filter:
	slti r9, r27, 58	;57->ascii 9, pokud je znak < 58, nastavim r9=1
	bnez r9, exitState	;pokud je znak cislo, jdu do exit state
	xor r9, r9, r9		;vynuluji registr

	andi r9, r25, 1		;operace index AND 0 -> pokud je posledni bit 0, tak ma znak sudy index => r9=1, else r9=0
	beqz r9, even		;r9=1, skacu na suda cisla
	bnez r9, odd		;r9=0, skacu na licha cisla
	nop

even:
	add r27, r27, r20	;prictu 1. klic ke znaku
	xor r9, r9, r9		;vynuluji registr
	sgti r9, r27, 122	;Kontroluji, zda jsem nepretekl pres "z"=>122
	bnez r9, overflow	;pokud r=1 skacu na stav preteceni	
	sb cipher(r25), r27	;Loadnu zasifrovany znak do cipher[r0]
	addi r25, r25, 1	;posunu index
	j getValue
	nop

odd:
	sub r27, r27, r23	;odectu 2. klic
	xor r9, r9, r9		;vynuluji registr
	slti r9, r27, 97	;Kontroluji, zda jsem nepodtekl pres "a"=>97
	bnez r9, underflow	;pokud r=1 skacu na stav podteceni
	sb cipher(r25), r27	;Loadnu zasifrovany znak do cipher[r0]
	addi r25, r25, 1	;posunu index
	j getValue
	nop

overflow:
	subi r27, r27, 26	;odectu hodnotu (z-a)+1
	sb cipher(r25), r27	;Loadnu zasifrovany znak do cipher[r0]
	addi r25, r25, 1	;posunu index
	j getValue
	nop

underflow:
	addi r27, r27, 26	;prictu hodnotu (z-a)+1
	sb cipher(r25), r27	;Loadnu zasifrovany znak do cipher[r0]
	addi r25, r25, 1	;posunu index
	j getValue
	nop

exitState:
	xor r9, r9, r9
	sb cipher(r25), r9	;Dle dokumentace -> musi koncit retezec hodnotou 0

end:    addi r14, r0, caddr ; <-- pro vypis sifry nahradte laddr adresou caddr
        trap 5  ; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0  ; ukonceni simulace
