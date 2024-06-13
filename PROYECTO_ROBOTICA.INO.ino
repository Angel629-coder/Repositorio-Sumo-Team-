#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;

void setup() {
  // Inicializar la comunicación serial a 9600 baudios
  Serial.begin(9600);

  // Conectar los servos a los pines digitales de Arduino
  servo1.attach(9);  // Conectar el primer servo al pin 9
  servo2.attach(10); // Conectar el segundo servo al pin 10
  servo3.attach(11); // Conectar el tercer servo al pin 11
}

void loop() {
  // Verificar si hay datos disponibles para leer
  if (Serial.available() > 0) {
    // Leer la cadena de caracteres enviada desde el otro dispositivo
    String data = Serial.readStringUntil('\n');

    // Separar la cadena de datos en los ángulos q1, q2, q3
    int commaIndex1 = data.indexOf(',');
    int commaIndex2 = data.indexOf(',', commaIndex1 + 1);

    String q1String = data.substring(0, commaIndex1);
    String q2String = data.substring(commaIndex1 + 1, commaIndex2);
    String q3String = data.substring(commaIndex2 + 1);

    // Convertir los ángulos de cadena a enteros
    int q1 = q1String.toInt();
    int q2 = q2String.toInt();
    int q3 = q3String.toInt();

    // Mover los servos a los ángulos especificados
    servo1.write(q1);
    servo2.write(q2);
    servo3.write(q3);
  }
}
