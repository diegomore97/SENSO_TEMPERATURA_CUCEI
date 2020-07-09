
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include "SPI.h"

int16_t Respuesta;
int8_t i;
int16_t x;
int32_t Direccion = 0; // N�mero de Direcci�n para comenzar a leer y escribir


///// Funciones  ////////////////////////////////////////////////////////////////////////////
void Pruebas(void);
int8_t InicializarSD(); // Rutina de Inicializaci�n
int8_t Escribir(int8_t Caracter); // Rutina de escritura
int8_t Leer(); // Rutina de lectura

int8_t Comando(int8_t NumeroComando, int32_t Direccion, int8_t CRC) // Para enviar comandos
{
    SPI_Write(0xFF); // Manda 8 pulsos de clock
    SPI_Write(NumeroComando); // Manda n�mero que identifica al comando
    SPI_Write((unsigned char) (Direccion >> 24));
    SPI_Write((unsigned char) (Direccion >> 16));
    SPI_Write((unsigned char) (Direccion >> 8));
    SPI_Write((unsigned char) Direccion);
    SPI_Write(CRC); // Manda el CRC
    SPI_Write(0xFF); // Manda 8 pulsos de clock
    return SPI_Read(); // Lee la respuesta de la SD
} // Fin de Comando 

int8_t InicializarSD() {
    int8_t com40, com41; // Enviamos un m�nimo de 80 clocks para inicializar la SD 
    CS = 1; // Deshabilitamos la SD
    for (i = 0; i < 10; i++) {
        SPI_Write(0xFF);
    }

    CS = 0; // Habilitamos la Tarjeta SD
    //LED1=1;while(Pulsador ==1); delay_ms(500);LED1=0;
    while (Comando(0x40, 0, 0x95) != 1);
    i = SPI_Read();

    com40 = 7;

    while (Comando(0x41, 0, 0xFF) != 0);
    i = SPI_Read();

    com41 = 21;
    if ((com40 + com41) == 28)
        return 1;

    CS = 1; // Deshabilitamos la SD

} // Fin de inicializar

int8_t Leer() {
    int8_t com51, yalei;
    CS = 0; // Habilitamos la Tarjeta SD
    // Enviamos el Comando de Lectura de Bloque

    while (Comando(0x51, Direccion, 0xFF) != 0);
    i = SPI_Read();
    com51 = 7;

    while (SPI_Read() != 0xFE); // Espera aqu� hasta que recibimos 0xFE

    for (i = 0; i < 30; i++) {
        SPI_Write(0xFF);
        Respuesta = SPI_Read();
    }// Leer parte del bloque
    yalei = 21;
    if ((com51 + yalei) == 28)
        return 0;

    CS = 1; // Deshabilitamos la SD

} // Fin de Leer

int8_t Escribir(int8_t Caracter) {
    int8_t com58, yaescribi;

    CS = 0; // Habilitamos la Tarjeta SD
    // Enviamos el Comando de Escritura, junto con la direcci�n del sector
    while (Comando(0x58, Direccion, 0xFF) != 0);
    i = SPI_Read();
    com58 = 7;

    SPI_Write(0xFE); // Byte Inicial Escritura (Token)

    for (x = 0; x < 512; x++) {
        SPI_Write(Caracter);
    }
    SPI_Write(0xFF); // CRC
    for (i = 0; i < 64; i++) {
        SPI_Write(0xFF);
        Respuesta = SPI_Read();
        if (Respuesta == 0xE5) {
            yaescribi = 21;
            break;
        }
    }

    while (SPI_Read() != 0xFF); // Esperamos hasta que la tarea de grabaci�n se termine completamente y la memoria se ponga Idle.
    if ((com58 + yaescribi) == 28)
        return 0;
    else
        return 10;
    CS = 1; // Deshabilitamos la SD

} // Fin de Escribir()


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

