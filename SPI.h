/*
 *Funcion que se encarga de iniciar el Bus SPI
 */
void SPI_Init(void) {

    TRISBbits.RB0 = 1; // SDI
    TRISBbits.RB1 = 0; //SCK
    TRISCbits.RC7 = 0; //SDO
    TRISAbits.RA5 = 0; //SS

    SSPSTAT = 0x40; //Data change on rising edge of clk , BF=0
    SSPCON1 = 0x20; // Master mode,Serial enable,
    CS = 1; // Deshabilitamos la SD
    PIR1bits.SSPIF = 0;

}

void SPI_Write(unsigned char x) {

    SSPBUF = x; /* Copy data in SSBUF to transmit */
    while (!SSPSTATbits.BF); /* Wait for complete 1 byte transmission */

}

unsigned char SPI_Read() {
    while (!SSPSTATbits.BF); /* Wait for complete 1 byte transmission */
    return (SSPBUF); /* Return received byte */
}

void SPI_Write_String(char *text) {
    uint16_t i;
    for (i = 0; text[i] != '\0'; i++)
        SPI_Write(text[i]);
}