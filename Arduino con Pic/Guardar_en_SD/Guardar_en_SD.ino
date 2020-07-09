#include <SD.h>

bool finMensaje = false;
String guardar;
const int chipSelect = 4;
File dataFile;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(chipSelect, OUTPUT); // Definimos el módulo SD como salida
  if (SD.begin(chipSelect))
    Serial.println("Tarjeta detectada");
  else
    Serial.println("No se ha detectado tarjeta");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (finMensaje) {

    dataFile = SD.open("sensoTemp.txt", FILE_WRITE); //Crea el archivo  o si ya existe escribe dentro de el

    if (dataFile) {
      //Serial.println("Bien");
      dataFile.print(guardar);
      dataFile.close();
    }

    else
      //Serial.println("Mal");

    //Limpiar variables
    finMensaje = false;
    guardar = "";

  }

}

void serialEvent() {
  //Mientras haya bytes sin leer en el buffer de entrada serial, el while sera valid
  while (Serial.available()) {
    char byteRecibido = Serial.read();
    guardar += byteRecibido;
    if (byteRecibido == '\r') {
      finMensaje = true;
      Serial.print(guardar);
    }

  }

}
