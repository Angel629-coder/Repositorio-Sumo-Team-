// Clase SensorInfrarrojo
class SensorInfrarrojo 
{
  private:
    int pin;
    volatile bool estado;
  
  public:
    // Constructor que inicializa el pin y lo configura como entrada
    SensorInfrarrojo(int p) : pin(p), estado(true) 
    {
      pinMode(pin, INPUT);
    }
    
    // Método para obtener el estado del sensor
    bool get_estado() 
    { 
      estado = digitalRead(pin);
      return estado;
    }
};

// Clase Ultrasonico
class Ultrasonico 
{
  private:
    int trig_pin;
    int echo_pin;
  
  public:
    // Constructor que inicializa los pines y los configura
    Ultrasonico(int trig, int echo) : trig_pin(trig), echo_pin(echo) 
    {
      pinMode(trig_pin, OUTPUT);
      pinMode(echo_pin, INPUT);
    }
    
    // Método para medir la distancia utilizando el sensor ultrasonico
    long medir_distancia() 
    {
      digitalWrite(trig_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(trig_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig_pin, LOW);
      
      long duration = pulseIn(echo_pin, HIGH);
      long distance = duration * 0.034 / 2;
      return distance;
    }
};

// Clase Motor
class Motor 
{
  private:
    int pin1;
    int pin2;
    int enable_pin;
  
  public:
    // Constructor que inicializa los pines del motor y los configura
    Motor(int p1, int p2, int en) : pin1(p1), pin2(p2), enable_pin(en) 
    {
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      pinMode(enable_pin, OUTPUT);
    }
    
    // Método para avanzar
    void avanzar() 
    {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(enable_pin, HIGH);
    }
    
    // Método para retroceder
    void retroceder() 
    {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(enable_pin, HIGH);
    }

};

// Clase RobotSUMO
class RobotSUMO 
{
  private:
    Motor* motor_izq;
    Motor* motor_der;
    Ultrasonico* ultrasonico_izq;
    Ultrasonico* ultrasonico_der;
    SensorInfrarrojo* infrarrojo_izq;
    SensorInfrarrojo* infrarrojo_der;
    SensorInfrarrojo* infrarrojo_tras;
    
    const int umbral = 50; // Umbral de distancia para detectar oponente

    volatile bool bandera_infrarrojo_izq = true;
    volatile bool bandera_infrarrojo_der = true;
    volatile bool bandera_infrarrojo_tras = true;
  
  public:
    // Constructor que inicializa los componentes del robot
    RobotSUMO(Motor* mi, Motor* md, Ultrasonico* ui, Ultrasonico* ud, SensorInfrarrojo* ii, SensorInfrarrojo* id, SensorInfrarrojo* it)
      : motor_izq(mi), motor_der(md), ultrasonico_izq(ui), ultrasonico_der(ud), infrarrojo_izq(ii), infrarrojo_der(id), infrarrojo_tras(it) {}
    
    // Método para actualizar el estado del robot
    void actualizar() 
    {

      bandera_infrarrojo_izq = infrarrojo_izq->get_estado();
      bandera_infrarrojo_der = infrarrojo_der->get_estado();
      bandera_infrarrojo_tras = infrarrojo_tras->get_estado();
      
      if (! bandera_infrarrojo_tras) 
      {
        Serial.println("Flag 3");
      }else if (! bandera_infrarrojo_izq)
        {
          Serial.println("Flag 2");
        }else if (! bandera_infrarrojo_der)
          {
            Serial.println("Flag 1");
          }
    }
};

// Creación de instancias de los motores
Motor motor_izq(15, 2, 4); // Pines del motor izquierdo
Motor motor_der(5, 18, 10); // Pines del motor derecho

// Creación de instancias de los sensores ultrasonicos
Ultrasonico ultrasonico_izq(13, 12); // Pines del sensor ultrasonico izquierdo
Ultrasonico ultrasonico_der(14, 27); // Pines del sensor ultrasonico derecho

// Creación de instancias de los sensores infrarrojos
SensorInfrarrojo infrarrojo_izq(26); // Pin del sensor infrarrojo izquierdo
SensorInfrarrojo infrarrojo_der(25); // Pin del sensor infrarrojo derecho
SensorInfrarrojo infrarrojo_tras(33); // Pin del sensor infrarrojo trasero

// Creación de la instancia del robot SUMO
RobotSUMO robot(&motor_izq, &motor_der, &ultrasonico_izq, &ultrasonico_der, &infrarrojo_izq, &infrarrojo_der, &infrarrojo_tras);

// Variables para multitarea
unsigned long previousMillisUltrasonico = 0;
unsigned long previousMillisInfrarrojos = 0;
const long intervalUltrasonico = 10; // Intervalo para sensores ultrasonicos
const long intervalInfrarrojos = 5; // Intervalo para sensores infrarrojos


void setup() 
{
  Serial.begin(115200);
}

void loop() 
{
  unsigned long currentMillis = millis();

  // Leer sensores ultrasonicos
  if (currentMillis - previousMillisUltrasonico >= intervalUltrasonico) 
  {
    previousMillisUltrasonico = currentMillis;
    
  }

  // Leer sensores infrarrojos
  if (currentMillis - previousMillisInfrarrojos >= intervalInfrarrojos) 
  {
    previousMillisInfrarrojos = currentMillis;
    robot.actualizar();
  }

  delay(5); // Pequeño retraso para estabilidad
}