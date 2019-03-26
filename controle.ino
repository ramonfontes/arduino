#include <IRremote.h>                              // Biblioteca IRemote
int RECV_PIN = 15;                                 // Arduino pino D11 conectado no Receptor IR
IRrecv irrecv(RECV_PIN);                           // criando a instância
decode_results results;                            // declarando os resultados
bool LED5, LED6, LED7  = false;                    // estado dos LEDs
int atraso = 250;                                  // atraso após ligar LED
 
void setup()
{
  Serial.begin(9600);                              // Monitor velocidade 9600 bps
  pinMode(19, OUTPUT);                              // LED vermelho no pino D07
  pinMode(18, OUTPUT);                              // LED azul no pino D06
  pinMode(4, OUTPUT);                              // LED verde no pino D05
  irrecv.enableIRIn();                             // Inicializa a recepção de códigos
}
 
void loop()
{
  results.value = 0;                               // zera os registradores
  if (irrecv.decode(&results))                     // se algum código for recebido
  {
    Serial.println(results.value, HEX);            // imprime o HEX Code
    irrecv.resume();                               // reinicializa o receptor
  }
  if (results.value == 0xFFB04F)                   // pressione tecla 3 para controlar LED vermelho (D07)
  {
    LED7 = !LED7;                                  // alterna o estado do LED D07
    digitalWrite(19, LED7);                         // acende ou apaga LED vermelho (D07)
    delay(atraso);                                 // atraso de 250 ms
  }
 
  if (results.value == 0xFF18E7)                   // pressione tecla 2 para controlar LED azul (D06)
  {
    LED6 = !LED6;                                  // alterna o estado do LED D06
    digitalWrite(18, LED6);                         // acende ou apaga LED vermelho (D07)
    delay(atraso);                                 // atraso de 250 ms
  }
 
  if (results.value == 0xFF30CF)                   // pressione tecla 1 para controlar LED verde (D05)
  {
    LED5 = !LED5;                                  // alterna o estado do LED D05
    digitalWrite(4, LED5);                         // acende ou apaga LED verde (D05)
    delay(atraso);                                 // atraso de 250 ms
  }
}
