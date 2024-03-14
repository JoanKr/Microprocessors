 LJMP START
 ORG 100H

START:
    LCALL LCD_CLR
    LCALL WAIT_KEY
    MOV R0, A
    LCALL BCD
    LCALL WAIT_KEY
    LCALL DODAWANIE

BCD:
    MOV B, #100 ; Dzielnik dla setek
    DIV AB ; Podziel przez setki, wynik w A, reszta w B
    ORL A, #48 ; zamiana wyniku na cyfrę ASCII (dodaj 48 do A)
    LCALL WRITE_DATA ; Wyświetl cyfrę setek

    MOV A, B ; Skopiuj resztę z dzielenia (dziesiątki + jedności) do A
    MOV B, #10 ; Dzielnik dla dziesiątek
    DIV AB ; Podziel przez dziesiątki, wynik w A, reszta w B
    ORL A, #48 ; zamiana wyniku na cyfrę ASCII (dodaj 48 do A)
    LCALL WRITE_DATA ; Wyświetl cyfrę dziesiątek

    MOV A, B ; Skopiuj resztę (jedności) do A
    ORL A, #48 ; zamiana jedności na cyfrę ASCII (dodaj 48 do A)
    LCALL WRITE_DATA ; Wyświetl cyfrę jedności

    RET


DODAWANIE:
    CJNE A, #10, ODEJMOWANIE;    sprawdz czy dodawanie
    MOV A, #'+'
    LCALL WRITE_DATA;    wyswietli sie plus

    LCALL WAIT_KEY
    MOV R1, A
    LCALL BCD
    MOV A, #'='
    LCALL WRITE_DATA;    wyswietli sie =
    MOV A, R1
    ADD A, R0
    MOV R0, A;    zapisze do R0
    LCALL BCD    
    MOV A, #30
    LCALL DELAY_100MS
    LCALL LCD_CLR
    RET

    

ODEJMOWANIE:
    CJNE A, #11, MNOZENIE;    sprawdź, czy to operacja odejmowania

    MOV A, #'-' ; Wyświetl znak minusa
    LCALL WRITE_DATA
    LCALL WAIT_KEY
    MOV R1, A
    LCALL BCD
    MOV A, #'='
    LCALL WRITE_DATA
    MOV A, R0 
    CLR C ; Wyczyść flagę przeniesienia, przygotowując się do odejmowania
    SUBB A, R1 ; Odejmij
    JNC ODEJM_DOD ; nie było przeniesienia
    MOV A, #'-' ; było przeniesienie
    LCALL WRITE_DATA
    MOV A, R1 ; zamieniamy liczby bo było przeniesienie
    SUBB A, R0
    ADD A, #1
    LCALL BCD 
    MOV A, #30
    LCALL DELAY_100MS
    LCALL LCD_CLR
    RET

ODEJM_DOD:
    LCALL BCD ; Wyświetl wynik odejmowania
    MOV A, #30
    LCALL DELAY_100MS
    LCALL LCD_CLR
    RET


MNOZENIE:
    CJNE A, #12, DZIELENIE_CZY_ZERO;    sprawdz czy mnozenie
    MOV A, #'*'
    LCALL WRITE_DATA;    wyswietli sie gwiazdka

    LCALL WAIT_KEY
    MOV R1, A
    LCALL BCD
    MOV A, #'='
    LCALL WRITE_DATA;    wyswietli sie =
    MOV A, R0
    MOV B, R1
    MUL AB
    MOV R1, A
    LCALL BCD
    MOV A, #30
    LCALL DELAY_100MS
    LCALL LCD_CLR
    RET

DZIELENIE_CZY_ZERO:
    CJNE A, #13, BRAK;    sprawdz czy dzielenie
    MOV A, #'/'
    LCALL WRITE_DATA;    wyswietli sie ukosnik 
    LCALL WAIT_KEY
    MOV R1, A
    LCALL BCD
    MOV A, #'='
    LCALL WRITE_DATA;    wyswietli sie =
    MOV A, R1
    CJNE A, #0, DZIELENIE
    MOV A, #'!'
    LCALL WRITE_DATA
    MOV A, #30
    LCALL DELAY_100MS
    LJMP START
    RET

DZIELENIE:
    MOV A, R0
    MOV B, R1
    DIV AB ; Podziel A przez B, wynik w A, reszta w B
    MOV R1, A ; Skopiuj wynik dzielenia z A do R1 (całkowita część wyniku)
    LCALL BCD ; Wyświetl całą część wyniku
    MOV A, #30
    LCALL DELAY_100MS
    LCALL LCD_CLR
    RET

BRAK:
    LCALL LCD_CLR
    LJMP START
