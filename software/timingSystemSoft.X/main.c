/*
 * UFSC- Universidade Federal de Santa Catarina
 * Projeto: Trabalho Processo Industrial de programação C para PIC
 * File:   main.c
 * Author: Ítalo Manzine e Yan Bentes
 * Sistema embarcado para um sistema de saída e cronometragem em competições de natação
 * 
 * Created on 04 de Março de 2022, 19:02
 */

// PIC16F877A Configuration Bit Settings
// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h> //include padrão do compilador XC8

#define _XTAL_FREQ 4000000 //Define a frequencia do clock em 4MHz

#define START RB0
#define LIGHT RC0
#define BUZZER RC2

void main(void) {
    //inicialização das portas todas em saída
    TRISB = 0xFF;
    TRISC = 0x00;
    
    // Resistores Pull-up ativados
    OPTION_REG = 0b01111111;
    
    LIGHT = 0;
    BUZZER = 0;
    
    while (1) // LOOP PRINCIPAL
    {
        if (START == 0) 
        {
            LIGHT = 1;
            BUZZER = 1;
            __delay_ms(200);
            LIGHT = 0;
            BUZZER = 0;
        }
    }
    
    return;
}
