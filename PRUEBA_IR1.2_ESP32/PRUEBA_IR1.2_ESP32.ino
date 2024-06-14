/*
 * Programa: Control de LED y Lectura ADC con Temporizador para ESP32
 * Descripción: Este programa controla un LED en el pin 13 del ESP32
 *              basado en la lectura del valor del ADC en el pin A0.
 *              Utiliza un temporizador para implementar una pausa de 3 segundos
 *              sin bloquear la ejecución del sistema.
 *
 * Autor: [Angel Vallejo]
 * Fecha: [10/06/2024]
 * Versión: 1.2
 *
 * Historial de Versiones:
 * - 1.0: [10/06/2024] Versión inicial.
 * - 1.1: [11/06/2024] 
 *
 * Referencias:
 * - Documentación de Arduino: https://www.arduino.cc/reference/en/
 * - Documentación del ESP32: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html
 *
 * Notas:
 * - Este código está diseñado para ser ejecutado en un ESP32.
 * - Asegúrese de que los componentes de hardware están conectados correctamente.
 */
#include <esp_adc_cal.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#define DEFAULT_VREF    1100                              // Valor típico para la referencia de voltaje del ADC (1.1V)
esp_adc_cal_characteristics_t adc_chars;


volatile bool alpha = true;
volatile bool lambda = true;
volatile byte timerCounter = 0;

hw_timer_t *timer = NULL;

void IRAM_ATTR onTimer() 
{
  timerCounter = timerCounter + 1;
  if (timerCounter >= 3) 
  {
    alpha = false;
  }
}
                                                          /*
                                                          void IRAM_ATTR onTimer(): Declara una función llamada onTimer(). La etiqueta IRAM_ATTR indica que la función se almacena 
                                                          en la memoria IRAM, que es más rápida de acceder en comparación con la memoria flash estándar.
                                                          timerCounter = timerCounter + 1;: Incrementa el valor de timerCounter en 1 cada vez que se ejecuta la rutina de 
                                                          interrupción. Este contador generalmente se utiliza para llevar la cuenta del número de veces que se ha ejecutado 
                                                          la rutina de interrupción del temporizador.
                                                          if (timerCounter >= 3): Comprueba si timerCounter ha alcanzado el valor de 3. Esto se usa para implementar una pausa 
                                                          o temporización en el programa. En este caso, después de que el temporizador haya interrumpido tres veces (es decir, 
                                                          después de aproximadamente 3 segundos, dependiendo de la configuración del temporizador), se realiza la acción siguiente.
                                                          alpha = false;: Desactiva la variable alpha. alpha es una variable volátil (volatile) que generalmente se usa como una 
                                                          bandera o indicador en el programa principal. La desactivación de alpha podría indicar que se ha completado un período 
                                                          de tiempo determinado o una tarea que se estaba realizando.
                                                          */
void setup() 
{
  Serial.begin(921600);                                   // Inicializar la comunicación serial a 9600 baudios
  pinMode(15, OUTPUT);                                    // Configurar el pin 13 como salida
  analogReadResolution(10);                               // Configurar la resolución del ADC a 12 bits
  analogWriteResolution(15, 8);

  adc_chars = esp_adc_cal_characteristics_t();
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_10, DEFAULT_VREF, &adc_chars);
                                                          /*
                                                          ADC_UNIT_1: Indica qué ADC del ESP32 se está configurando. En el ESP32, normalmente hay dos unidades de ADC disponibles: 
                                                          ADC_UNIT_1 y ADC_UNIT_2.
                                                          ADC_ATTEN_DB_0: Especifica la atenuación del canal ADC. En este caso, ADC_ATTEN_DB_0 indica que no se aplica atenuación 
                                                          al canal, lo que significa que el rango completo de voltaje de entrada se utiliza para la conversión.
                                                          ADC_WIDTH_BIT_10: Especifica la resolución del ADC. Aquí, se configura el ADC para tener una resolución de 10 bits, lo 
                                                          que permite representar valores de voltaje en un rango de 0 a 1023.
                                                          DEFAULT_VREF: Es el voltaje de referencia del ADC, que en este caso es 1100 mV (1.1V).
                                                          &adc_chars: Es un puntero a la estructura adc_chars que hemos inicializado previamente. Después de que 
                                                          esp_adc_cal_characterize() se ejecuta, adc_chars contendrá los parámetros de calibración específicos del ADC configurado.
                                                          */
  // Configurar Timer 
  timer = timerBegin(1000000);                            // Configurar el timer para 1 MHz (1000000 Hz)
  timerAttachInterrupt(timer, &onTimer);                  // Adjuntar la interrupción del temporizador
  timerAlarm(timer, 1000000, true, 0);                    // Configurar el temporizador para 1 segundo, con autoreload
  timerStart(timer);                                      // Habilitar la alarma del temporizador
}     

void loop() 
{
  if (!alpha) 
  {
    if (lambda) 
    {
      uint16_t valorADC = analogRead(4);                 // Leer el resultado de la conversión
      Serial.println(valorADC);                          // Imprimir el resultado de la conversión

      if (valorADC < 35) 
      {
        for(uint8_t i = 0; i<255 ; i++)
        {
          analogWrite(15,i);                             // Poner en alto el pin 13
          delay(100);
        }
        lambda = false;
        Serial.println("Ha terminado el programa");
      }
    }
  }
}
