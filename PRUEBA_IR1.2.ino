/*
 * Programa: Control de LED y Lectura ADC con Temporizador
 * Descripción: Este programa controla un LED en el pin 13 del Arduino Uno
 *              basado en la lectura del valor del ADC en el pin A0.
 *              Utiliza un temporizador para implementar una pausa de 3 segundos
 *              sin bloquear la ejecución del sistema.
 *              Este programa constituye una parte importante de la creación de un robot sumo 
 *              puesto que es la parte del inicio y se han implementado registros y timmers para lograrlo de la mejor manera.
 *
 *
 *
 * Autor: [Angel Vallejo]
 * Fecha: [10/06/2024]
 * Versión: 1.2
 *
 *
 * Historial de Versiones:
 * - 1.0: [10/06/2024] Versión inicial.
 * - 1.1: [11/06/2024] 
 *
 * Referencias:
 * - Documentación de Arduino: https://www.arduino.cc/reference/en/
 * - Hoja de datos del ATmega328P: http://ww1.microchip.com/downloads/en/DeviceDoc/40002061A.pdf
 *
 * Notas:
 * - Este código está diseñado para ser ejecutado en un Arduino Uno.
 * - Asegúrese de que los componentes de hardware están conectados correctamente.
 */
 

//Programa de Robot Sumo 

volatile bool alpha = true;
volatile bool lambda = true;
volatile unsigned int timerCounter = 0;

void setup() 
{
  Serial.begin(9600);                                  // Inicializar la comunicación serial a 9600 baudios
                    
  DDRB |= (1 << DDB5);                                 // Configurar el pin 13 como salida                                   
  ADMUX |= (1 << REFS0);                               // Configurar el ADC en modo de conversión libre; Seleccionar AREF como referencia de voltaje                             
  DIDR0 |= (1 << ADC0D);                               // Configurar el pin A0 como entrada

                                                       // Habilitar el ADC, configurar el prescaler a 128 y habilitar la conversión libre
  ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  ADCSRA |= (1 << ADSC);                               // Iniciar la conversión libre

                                                       // Configurar Timer1
  cli();                                               // Desactivar interrupciones globales
  TCCR1A = 0;                                          // Configuración de control del temporizador
  TCCR1B = 0;                                          // Configuración de control del temporizador
  TCNT1 = 0;                                           // Reiniciar contador del temporizador

                                                       // Configurar el valor de comparación para 1 segundo
  OCR1A = 15624;                                       // Con un prescaler de 1024 y reloj de 16 MHz (16*10^6 / 1024 / 1Hz - 1)
  TCCR1B |= (1 << WGM12);                              // Modo CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << CS12) | (1 << CS10);                 // Prescaler de 1024
  TIMSK1 |= (1 << OCIE1A);                             // Habilitar interrupción por comparación A
  sei();                                               // Habilitar interrupciones globales
}

void loop() 
{
  if (!alpha) 
  {
    if (lambda) 
    {
      while (!(ADCSRA & (1 << ADIF)));                 // Esperar a que la conversión se complete
       
      ADCSRA |= (1 << ADIF);                           // Limpiar el bit de interrupción de conversión completa

      int valorADC = ADC;                              // Leer el resultado de la conversión
      Serial.println(valorADC);                        // Imprimir el resultado de la conversión

      if (valorADC < 35) 
      {
        PORTB |= (1 << PORTB5);                        // Poner en alto el pin 13
        lambda = false;
        Serial.println("Ha terminado el programa");
      }
    }
  }
}
                                                       // ISR para el Timer1
ISR(TIMER1_COMPA_vect) 
{
  timerCounter++;
  if (timerCounter >= 3) 
  {
    alpha = false;
  }
}