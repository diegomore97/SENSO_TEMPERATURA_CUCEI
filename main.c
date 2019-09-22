#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#define _XTAL_FREQ 4000000
#include "config.h"
#include"UART.h"
#include "I2C.h"
#include "lcd.h"


#define RETARDO 5
#define TAMANO_FECHA 9
#define TAMANO_CADENA 20
#define TAMANO_HORA 9
#define TAMANO_CADENA_GPS 13
#define POSICION_FECHA 8
#define TAMANO_TRAMA 35

void dameTemperaturaLm35(float* temperatura);
void configurarAdc(void);
void dameCoordenadas(float* latitud, float* longitud);
void mensajeEspera(short segundos);
void leerGps(char* cadenaInicial);
void dameHora(char* cadenaFragmentada, unsigned char* hora);
float dameGrados(char* cadenaFragmentada);
void dameDato(char* cadenaFragmentada, int numComas, char* variable);
void dameFecha(char* cadenaAux);
void convierteFecha(char* cadenaAux, char* fecha);

char diaSiguiente;
unsigned char numeros[] = {63, 6, 91, 79, 102, 109, 125, 71, 127, 103};
// 0  1   2   3   4    5    6    7   8    9

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

void mensajeEspera(short segundos) {
    char buffer[TAMANO_CADENA];

    while (segundos) {
        sprintf(buffer, "Espere: %d seg", segundos);
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String("Conectando GPS...");
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String(buffer);
        __delay_ms(1000);
        segundos--;
    }

}

void leerGps(char* cadenaInicial) {

    unsigned char leerByte;
    int cont;
    int ok = 0, i = 0;
    *cadenaInicial = '\0';


    CREN = 1;


    while (!ok) {

        while (leerByte != '$') {
            leerByte = EUSART1_Read();
        }

        cont = 0;

        while (leerByte != '\r') {

            leerByte = EUSART1_Read();

            if (leerByte != ',') {
                cadenaInicial[cont++] = leerByte;

            } else {
                if (cadenaInicial[0] == 'G' && cadenaInicial[1] == 'P' &&
                        cadenaInicial[2] == 'G' && cadenaInicial[3] == 'G' &&
                        cadenaInicial[4] == 'A') {

                    cont = 0;

                    while (cont < 34) { //Capturando los primeros 35 caracteres
                        leerByte = EUSART1_Read();
                        UART_write(leerByte);
                        cadenaInicial[cont++] = leerByte;

                    }

                    ok = 1;

                }

            }

        }

    }


    UART_write('\r');
    cadenaInicial[cont - 1] = '\0';
    CREN = 0;

}

void dameHora(char* cadenaFragmentada, unsigned char* hora) {

    short horaGMT = 0;
    char decenaHora[2];
    char unidadHora[2];


    for (int i = 0; i <= 7; i++) {

        switch (i) {

            case 0:
                hora[i] = cadenaFragmentada[i];
                break;

            case 1:
                //CONVIRTIENDO HORA GMT A GMT-5 
                hora[i] = cadenaFragmentada[i];
                horaGMT = (hora[0]) - 48;
                horaGMT *= 10;
                horaGMT += (hora[i]) - 48;
                horaGMT -= 5;

                if (horaGMT < 0) {
                    horaGMT = 24 - (horaGMT * -1);
                    diaSiguiente = 1;
                } else
                    diaSiguiente = 0;

                sprintf(decenaHora, "%d", (horaGMT / 10) % 10);
                sprintf(unidadHora, "%d", (horaGMT % 10));

                hora[0] = decenaHora[0];
                hora[1] = unidadHora[0];
                break;

            case 2:
                hora[i] = ':';
                hora[i + 1] = cadenaFragmentada[i];
                break;

            case 4:
                hora[i] = cadenaFragmentada[i - 1];
                break;

            case 5:
                hora[i] = ':';
                hora[i + 1] = cadenaFragmentada[i - 1];
                break;

            case 7:
                hora[i] = cadenaFragmentada[i - 2];
                break;

        }

    }

    hora[TAMANO_HORA - 1] = '\0';


}

float dameGrados(char *cadenaFragmentada) {
    float grados = atof(cadenaFragmentada) / 100;
    int16_t gradosDecimal = grados;
    grados -= gradosDecimal;
    grados = grados * 5 / 3 + gradosDecimal;
    return grados;
}

void dameDato(char* cadenaFragmentada, int numComas, char* variable) {

    if (cadenaFragmentada[0] != 'G') {
        int comasContadas = 0;
        int i = 0;
        int j = 0;

        while (comasContadas != numComas) {
            if (cadenaFragmentada[i++] == ',') {
                comasContadas++;
            }
        }

        while (cadenaFragmentada[i] != ',') {
            variable[j++] = cadenaFragmentada[i++];
        }

        variable[j] = '\0';
    }
}

void dameFecha(char* cadenaAux) {


    unsigned char leerByte;
    int cont;
    int ok = 0, i = 0;
    *cadenaAux = '\0';


    CREN = 1;


    while (!ok) {

        while (leerByte != '$') {
            leerByte = EUSART1_Read();
        }

        cont = 0;

        while (leerByte != '\r') {

            leerByte = EUSART1_Read();

            if (leerByte != ',') {
                cadenaAux[cont++] = leerByte;

            } else {
                if (cadenaAux[0] == 'G' && cadenaAux[1] == 'P' &&
                        cadenaAux[2] == 'R' && cadenaAux[3] == 'M' &&
                        cadenaAux[4] == 'C') {

                    cont = 0;

                    while (cont < 57) { //Capturando los primeros 35 caracteres
                        leerByte = EUSART1_Read();
                        UART_write(leerByte);
                        cadenaAux[cont++] = leerByte;

                    }

                    ok = 1;

                }

            }

        }

    }


    UART_write('\r');
    cadenaAux[cont - 1] = '\0';
    CREN = 0;

}

void convierteFecha(char* cadenaAux, char* fecha) {
    int contComas = 0;
    int cont = 0;
    int i = 0;

    while (contComas < POSICION_FECHA) {
        if (cadenaAux[i++] == ',')
            contComas++;
    }


    while (cont < 6)
        fecha[cont++] = cadenaAux[i++];

    fecha[cont] = '\0';
}

void main(void) {

    float temperatura;
    char buffer [TAMANO_CADENA];
    unsigned char horaCompleta[TAMANO_HORA] = "00:00:00";
    char fecha[TAMANO_FECHA] = "00/00/00";
    unsigned char variableMostrar = 0;

    char longitud[TAMANO_CADENA_GPS];
    char latitud[TAMANO_CADENA_GPS];

    float latitudMostrar;
    float longitudMostrar;

    char cadenaFragmentada[TAMANO_TRAMA];
    char cadenaAux[TAMANO_TRAMA];


    TRISD = 0;
    TRISBbits.RB2 = 1;

    configurarAdc();
    Lcd_Init();
    UART_init(9600);
    leerGps(cadenaFragmentada);
    mensajeEspera(15); //Esperar 15 segundos para que el gps reciba informacion


    while (1) {

        dameTemperaturaLm35(&temperatura);

        leerGps(cadenaFragmentada);
        dameDato(cadenaFragmentada, 1, latitud);
        dameDato(cadenaFragmentada, 3, longitud);

        latitudMostrar = dameGrados(latitud);
        longitudMostrar = dameGrados(longitud);
        dameHora(cadenaFragmentada, horaCompleta);
        dameFecha(cadenaAux);
        convierteFecha(cadenaAux, fecha);


        if (PORTBbits.RB2) {
            while (PORTBbits.RB2); //ANTIREBOTE
            variableMostrar = ~variableMostrar;
        }


        if (variableMostrar) {
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            sprintf(buffer, "LATI= %.4f", latitudMostrar); //Copiar cadena
            Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1
            Lcd_Set_Cursor(2, 1);
            sprintf(buffer, "LONG= %.4f", (longitudMostrar*-1)); //Copiar cadena
            Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1


        } else {
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            sprintf(buffer, "HORA= %s", horaCompleta); //Copiar cadena
            Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1
            Lcd_Set_Cursor(2, 1);
            sprintf(buffer, "TEMP= %.2f", temperatura); //Copiar cadena
            Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1

            __delay_ms(500);
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            sprintf(buffer, "FECHA= %c%c/%c%c/%c%c", fecha[0], fecha[1], fecha[2],
                    fecha[3], fecha[4], fecha[5]); //Copiar cadena
            Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1sprintf(buffer, "FECHA= %c%c/%c%c/%c%c", fecha[0], fecha[1], fecha[2],
            Lcd_Set_Cursor(2, 1);
            sprintf(buffer, "TEMP= %.2f", temperatura); //Copiar cadena
            Lcd_Write_String(buffer); //Imprimir la cadena en la posicion 1,1
        }


    }

}
