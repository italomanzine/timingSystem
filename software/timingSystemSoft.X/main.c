/*
 * UFSC- Universidade Federal de Santa Catarina
 * Projeto: Trabalho Processo Industrial de programa��o C para PIC
 * File:   main.c
 * Author: �talo Manzine e Yan Bentes
 * Sistema embarcado para um sistema de sa�da e cronometragem em competi��es de nata��o
 * 
 */

// PIC16F877A Configuration Bit Settings
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)

#include <xc.h>  // biblioteca compilador XC8
#include <pic16f877a.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000 // define a frequencia do clock em 20MHz

// Definindo entradas e sa�das
#define START RB0
#define FINISH_LINE RB1
#define LIGHT RC0
#define BUZZER RC2
#define LED_AD RC3

// define pinos referentes a interface com LCD
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include "lcd.h" // biblioteca do lcd

void setCronometro(void);
void cronometro(void);
void __interrupt() contaSegundos(void);

__bit startTimer;
__bit inverte;
int minutos=0, segundos=0, centesimos=0, contador=0, valor;
char buffer[10];

void main(void)
{   
    // inicializa��o das portas todas em sa�da *********************************
    TRISB = 0b11111111; //PORT B em entrada
    TRISC = 0b00000000; //PORT C em sa�da
    TRISD = 0b00000000; //PORT D em sa�da
    
    // resistores Pull-up ativados *********************************************
    OPTION_REGbits.nRBPU = 0;
    
    // Configurando interrup��es ***********************************************
    INTCONbits.GIE = 1;    // habilita int global
    INTCONbits.PEIE = 1;   // habilita int dos perifericos
    PIE1bits.TMR1IE = 1;   // habilita int do timer1
    
    // Configurando o TIMER 1 **************************************************
    T1CONbits.TMR1CS = 0;   // Define o timer1 como temporizador (FOSC/4)
    T1CONbits.T1CKPS0 = 1;  // Bit para configurar pre-escaler
    T1CONbits.T1CKPS1 = 1;  // Bit para configurar pre-escaler
    
    // TMR1 recebe o valor 59285(65536 - 59285), que em Hexadecimal vale 0xE795
    TMR1L = 0x95;           // Carga inicial no contador (65536 - 6250)
    TMR1H = 0xE7;           // 59285. Quando estourar contou 6250, passou 0.1s
    
    T1CONbits.TMR1ON = 0;   // Liga o TIMER 1
    
    // Configura conversor A/D *************************************************
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 1;
    ADCON1bits.PCFG2 = 1;
    ADCON1bits.PCFG3 = 1;
    
    // define o clock de convers�o
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 0;
    
    ADCON1bits.ADFM = 0;
    
    ADRESL = 0x00;
    ADRESH = 0x00;
    
    ADCON0bits.ADON = 1;
    
    // Inicialializando sa�das *************************************************
    LIGHT = 0;
    BUZZER = 0;
    LED_AD = 0;
    
    // Inicialializando o LCD *************************************************
    Lcd_Init();                 // Inicia m�dulo LCD
    Lcd_Clear();                // Limpa display
    
    // loop principal
    while(1)
    {
        CLRWDT();
        
        if(!START)
        {
            TMR1ON = 1;
            
            LIGHT = 1;
            BUZZER = 1;
            //setCronometro();
            __delay_ms(300);
            LIGHT = 0;
            BUZZER = 0;
            //__delay_ms(1000);
        }
        
        if(!FINISH_LINE) TMR1ON = 0;
        
        setCronometro();
        
        ADCON0bits.CHS0 = 0;
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS2 = 0;

        ADCON0bits.GO = 1;
        __delay_us(10);
        valor = ADRESH;
        
        if(valor == 0)
            LED_AD = 1;
        else
            LED_AD = 0;
    }
    
    return;
}

// Subrotina pra tratar a interrup��o ******************************************
void __interrupt() ContaSegundos(void)
{
    if(TMR1IF)
    {
        PIR1bits.TMR1IF = 0;    // Reseta o flag da interrup��o
        TMR1L = 0x95;           // Reinicia a contagem com 59285
        TMR1H = 0xE7;           // 

        // Comandos para tratar a interrup��o
        cronometro();
        ///* teoricamente quando o contador chegasse em 100 passaria 1 segundo
        contador++;
        if(contador == 100)
        {
            cronometro();
            contador = 0;
        }
    }
}

void setCronometro(void)
{
    sprintf(buffer,"1-      %02d:%02d:%02d", minutos, segundos, centesimos);    //Armazena em buffer os conte�dos de tempo
    Lcd_Set_Cursor(1,1);        // P�e cursor na linha 1 coluna 1
    Lcd_Write_String(buffer);   // Escreve o conte�do de buffer no LCD
}

void cronometro(void)
{  
    centesimos++;

    if(centesimos == 100)
    {
        centesimos = 0;
        segundos++;

        if(segundos == 60)
        {
           segundos = 0;
           minutos++;

           if(minutos == 60) minutos = 0;
        }
    }
} 