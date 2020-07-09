#include <SD.h>
#include <SPI.h>
#include  <LiquidCrystal.h>

float temperaturaAmbiente;
const int chipSelect = 4;
File dataFile;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(chipSelect, OUTPUT); // Definimos el módulo SD como salida
  
 // INDICAMOS QUE TENEMOS CONECTADA UNA PANTALLA DE 16X2
  lcd.begin(16, 2);
  
  if (SD.begin(chipSelect))
    Serial.println("Tarjeta detectada");
  else
    Serial.println("No se ha detectado tarjeta");
}

void loop() {
  // put your main code here, to run repeatedly:

    temperaturaAmbiente=analogRead(0);
    temperaturaAmbiente*=0.48877;

    lcd.clear();
    lcd.setCursor ( 2, 1);
    lcd.print("TEMP= "+String(temperaturaAmbiente));

    dataFile = SD.open("sensoTemp.txt", FILE_WRITE); //Crea el archivo  o si ya existe escribe dentro de el

    if (dataFile) {
      Serial.println("Bien");
      dataFile.println(temperaturaAmbiente);
      dataFile.close();
    }

    else
      Serial.println("Mal");

      delay(1000);

}
