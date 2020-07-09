#include <SD.h>
#include  <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>//incluimos TinyGPS

TinyGPS gps;//Declaramos el objeto gps
SoftwareSerial serialgps(1, 0); //Declaramos el pin 1 Tx y 0 Rx


const int chipSelect = 10;
char horaCompleta[9] = "00:00:00";
char fechaCompleta[11] = "00/00/0000";
char latitudCompleta[17] = "00.00";
char longitudCompleta[17] = "00.00";
char datoSalida[50];

int year;
byte month, day, hour, minute, second, hundredths;
float latitude, longitude;
float temperaturaAmbiente;
unsigned long chars;
unsigned short sentences, failed_checksum;
unsigned char variableMostrar = 0;
File dataFile;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {

  //Serial.begin(9600);//Iniciamos el puerto serie
  serialgps.begin(9600);//Iniciamos el puerto serie del gps

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, INPUT);
  pinMode(chipSelect, OUTPUT); // Definimos el módulo SD como salida

  // INDICAMOS QUE TENEMOS CONECTADA UNA PANTALLA DE 16X2
  lcd.begin(16, 2);
  SD.begin(chipSelect);

  /*if (SD.begin(chipSelect))
      Serial.println("Tarjeta detectada");
    else
      Serial.println("No se ha detectado tarjeta");
      */
}

void loop() {

  while ( serialgps.available()) {

    gps.encode (serialgps.read ());

  }


  gps.f_get_position(&latitude, &longitude);
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);
  temperaturaAmbiente = analogRead(0);
  temperaturaAmbiente *= 0.48877;

  if (digitalRead(8)) {

    lcd.clear();
    lcd.setCursor ( 1, 1);
    sprintf(latitudCompleta, "LATI= %.4f", latitude);
    lcd.print(latitudCompleta);
    lcd.setCursor ( 2, 1);
    sprintf(longitudCompleta, "LONG= %.4f", longitude);
    lcd.print(longitudCompleta);

  }
  else {

    lcd.clear();
    lcd.setCursor ( 1, 1);
    sprintf(horaCompleta, "%d:%d:%d", hour, minute, second);
    lcd.print(horaCompleta);
    lcd.setCursor ( 2, 1);
    lcd.print("TEMP= " + String(temperaturaAmbiente));

    delay(500);

    lcd.clear();
    lcd.setCursor ( 1, 1);
    sprintf(fechaCompleta, "%d/%d/%d", day, month, year);
    lcd.print(fechaCompleta);
    lcd.setCursor ( 2, 1);
    lcd.print("TEMP= " + String(temperaturaAmbiente));

  }

  gps.stats(&chars, &sentences, &failed_checksum);

  dataFile = SD.open("sensoTemp.txt", FILE_WRITE); //Crea el archivo  o si ya existe escribe dentro de el

  if (dataFile) {
    //Serial.println("Bien");
    sprintf(datoSalida, "%s,%s,%.4f,%.4f,%.2f\r", fechaCompleta, horaCompleta, latitude, longitude, temperaturaAmbiente);
    dataFile.print(datoSalida);
    dataFile.close();
  }

  else {
    //Serial.println("Mal");

  }


}
