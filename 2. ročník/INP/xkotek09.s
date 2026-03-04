; Autor reseni: Jiří Kotek xkote09
; Pocet cyklu k serazeni puvodniho retezce:             3553
; Pocet cyklu razeni sestupne serazeneho retezce:       3889
; Pocet cyklu razeni vzestupne serazeneho retezce:      2874
; Pocet cyklu razeni retezce s vasim loginem:           808
; Implementovany radici algoritmus:                     Bubble Sort
; ------------------------------------------------

; DATA SEGMENT
                .data
;login:          .asciiz "vitejte-v-inp-2023"    ; puvodni uvitaci retezec
 ;login:          .asciiz "vvttpnjiiee3220---"  ; sestupne serazeny retezec
 ;login:          .asciiz "---0223eeiijnpttvv"  ; vzestupne serazeny retezec
 login:          .asciiz "xkotek09"            ; SEM DOPLNTE VLASTNI LOGIN
                                                ; A POUZE S TIMTO ODEVZDEJTE

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize - "funkce" print_string)

; CODE SEGMENT
                .text
main:
        ; SEM DOPLNTE VASE RESENI
        sub r0,r0, r0          ;nastaveni registru r0 na hodnotu 0

        daddi   r4, r0, login   ; vozrovy vypis: adresa login: do r4

        daddi r1, r0, 1         ;KONSTANTA      Vlozeni 1 pro inkrementaci
        daddi r2, r0, -1        ;KONSTANTA      Vlozeni -1 pro potrebnou dekrementaci v cyklech for
        daddi r3, r0, 0         ;Vlozeni 0 do reg R3 ktery bude pocitat POSUN v ramci retezce
        ;R4                     slozi pro docasne ulozeni symbolu
        daddi r5, r0, 0         ;Vlozeni 0 do reg R5 ktery bude pocitat POCET ZNAKU v retezci
         daddi r8, r0, 1        ;KONSTANTA      Vlozeni 1 do reg R8 ktery bude inkrementontovat o 1
        ;R14                    slouzi pro docasne ulozeni symbolu (Vetsinou R14 = [R4+1])
        

        ;Cyklus pro vypocet delky retezce
        lenght_loop:
                nop
                

                lb r4, login(r3)        ;do registru r4 nahrajeme znak z adresy login (+offset)
                slti r10, r4, 1         ;porovname s r4 s 1, pokud bude v r4 ,mensi hodnota (0 - ukoncovac), v r10 bude hodnota 1, jinak se nastavi hodnota r10 na 0
                
                beq r10, r1, konec_vypoctu_delky  ;pokud r10 je 1 vyskocime z cyklu

                add r3, r3, r8          ;Pokud jsme nanarazili na ukoncovac, posuneme se o jeden znak doprava
                add r5, r5, r1          ;Inkrementace pocitadla delky retezce

        B lenght_loop
        
        
        konec_vypoctu_delky:

        ;R5 = delka retezce
        dsub r5, r5, r1
        ;R5 = delka retezce-1
        daddi r3, r0, 0         ;Vlozeni 0 do reg R3

        daddi r11, r0, 0        ;R11 bude slouzit pro cyklus for1 (int R11 =0; R11<R5; R11++)

         ;cyklus for1 (int R11 =0; R11<R5; R11++)
        for1:                  
                BEQ r11, r5, konec_for1 ;kontrola podminky opusteni cyklu
                
                daddi r12, r0, 0        ;;R12 bude slouzit pro cyklus for1 (int R12 =0; R12<(R5-R11); R12++)
                dsub r16, r5, r11 
                ;cyklus for2 (int R12 =0; R12<R5-R11; R12++)
                for2:
                        
                        BEQ r12, r16, konec_for2 ;kontrola podminky opusteni cyklu
                        lb r4, login(r12)       ;prvni porovnavany symbol
                        add r24, r12, r1
                        lb r14, login(r24)      ;druhy porovnavany symbol

                        dsub r18, r14, r4
                        ;add r18, r18, r1


                        bgez r18, skip_swap

                                ;swap
                                daddi r19, r14, 0       ;tmp = r14
                                daddi r14, r4, 0        ;r14 = r4
                                daddi r4, r19, 0        ;r4 = tmp
                                sb r4, login(r12)       ;ulozeni prohozene hodnoty
                                sb r14, login(r24)

                        skip_swap:

                        add r12, r12, r1
                        b for2
                konec_for2:
                

                add r11, r11, r1
                b for1
        konec_for1:


        daddi   r4, r0, login   ; vozrovy vypis: adresa login: do r4
        jal     print_string    ; vypis pomoci print_string - viz nize

        syscall 0   ; halt


print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address








                       
                        
              
                                      
