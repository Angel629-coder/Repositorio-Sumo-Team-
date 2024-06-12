volatile bool shouldRunPWM = false; // Variable para controlar la ejecución del PWM

// Definir constantes para la configuración de temporizadores
const int pwmPin = 9;
const int inputPin = 2; // Pin de interrupción externa
const int pwmResolution = 1023;
const int pwmSteps = 1023;
const int duration = 1000;  // 1 segundo

void setup() 
{
    // Inicializar comunicación serial
    Serial.begin(9600);

    // Configurar el pin 7 como entrada usando manipulación directa de puertos
    DDRD &= ~(1 << DDD7);  // DDRD es el registro de dirección de datos para el puerto D

    // Configurar el pin 9 (OC1A) como salida
    DDRB |= (1 << DDB1);  // DDB1 corresponde al pin 9 en el puerto B

    // Configurar el Timer 1 para Fast PWM, 10-bit, sin prescaler
    TCCR1A = _BV(COM1A1) | _BV(WGM11);  // Fast PWM, 10-bit
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);  // No prescaling
    ICR1 = pwmResolution;  // TOP value for 10-bit resolution
    OCR1A = 0;  // Inicialmente, el ciclo de trabajo es 0%

    // Configurar interrupción externa para el pin 2
    attachInterrupt(digitalPinToInterrupt(inputPin), pin2Change, HIGH);
}

// ISR (Interrupt Service Routine) para manejar cambios en el pin 2
void pin2Change()
{
    // Leer el estado del pin 2
    bool pinState = digitalRead(inputPin);
    
    // Si el pin está en estado alto, establecer la bandera para ejecutar el PWM
    if (pinState == HIGH) 
    {
        shouldRunPWM = true;
    }
}

void increasePWMStepwise()
{
    int stepDelay = duration / pwmSteps;  // Calcular el tiempo entre cada incremento
    
    for (int i = 0; i <= pwmSteps; i++) 
    {
        OCR1A = i;  // Incrementar el ciclo de trabajo del PWM
        delayMicroseconds(stepDelay);  // Esperar el tiempo necesario entre cada incremento
        Serial.println(i);
    }
    OCR1A = 0;
}

void loop() 
{
    // Si la bandera está establecida, ejecutar la función de PWM
    if (shouldRunPWM) 
    {
        increasePWMStepwise();
        shouldRunPWM = false; // Restablecer la bandera
    }
}
