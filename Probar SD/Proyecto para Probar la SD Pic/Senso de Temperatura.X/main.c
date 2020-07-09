#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#define _XTAL_FREQ 4000000
#include "config.h"
#include"UART.h"
#include "lcd.h"
#include "sd.h"

#define TAMANO_CADENA 20
#define TAMANO_SALIDA 50

void dameTemperaturaLm35(float* temperatura);
void configurarAdc(void);


void configurarAdc(void) {
    ADCON0bits.ADON = 1; //Encendiendo ADC
    ADCON1 = 0b00001110; //VSS REFERENCIA|TODOS DIGITALES MENOS AN0
    ADCON2 = 0b10100101; //TIEMPO DE ADQUISICION 8 TAD, JUSTIFICADO A LA DERECHA, FOSC/16
    TRISA = 0b000001;
}

void dameTemperaturaLm35(float* temperatura) {
    ADCON0bits.CHS = 0b0000;
    ADCON0bits.GO = 1;
    ADCON0bits.GO_DONE = 1; //Bandera en 1

    while (ADCON0bits.GO_DONE);

    *temperatura = (ADRESH << 8) + ADRESL;

    *temperatura *= 0.48877; //10 mv por cada grado

}

void main(void) {

    float temperatura;
    char buffer [TAMANO_CADENA];
    char datoSalida[TAMANO_SALIDA];

    Direccion = 4608;

    configurarAdc();
    SPI_Init();
    Lcd_Init();
    UART_init(9600);

    while (1) {

        __delay_ms(1000);

        dameTemperaturaLm35(&temperatura);

        Lcd_Clear();
        Lcd_Set_Cursor(2, 1);
        sprintf(buffer, "TEMP= %.2f", temperatura); //Copiar cadena
        Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1

        sprintf(datoSalida, "%.2f\n", temperatura);
        escribeSD(datoSalida);

    }

}

