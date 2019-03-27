//Acionamento de lampada com MQTT Dash
//Por: Fábio Souza
//Data: 19/03/18

#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define INTERVALO_ENVIO       10000

#define DHTPIN 4      // o sensor dht11 foi conectado ao pino 2( D4 do node MCU)
#define DHTTYPE DHT22
 
DHT dht(DHTPIN, DHTTYPE);

#define DEBUG

#define L1 4   //pino de saida para acionamento da Lampada L1

//informações da rede WIFI
const char* ssid = "AndroidAP";                 //SSID da rede WIFI
const char* password =  "kizi3184";    //senha da rede wifi
 
//informações do broker MQTT - Verifique as informações geradas pelo CloudMQTT
const char* mqttServer = "m16.cloudmqtt.com";   //server
const char* mqttUser = "shfldxvc";              //user
const char* mqttPassword = "SVLnPuDrNmlb";      //password
const int mqttPort = 19759;                     //port
const char* mqttTopicSub ="weather/L1";            //tópico que sera assinado

int ultimoEnvioMQTT = 0;
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
  pinMode(L1, OUTPUT);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG
    Serial.println("Conectando ao WiFi..");
    #endif
  }
  #ifdef DEBUG
  Serial.println("Conectado na rede WiFi");
  #endif
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.println("Conectando ao Broker MQTT...");
    #endif
 
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      #ifdef DEBUG
      Serial.println("Conectado");  
      #endif
 
    } else {
      #ifdef DEBUG 
      Serial.print("falha estado  ");
      Serial.print(client.state());
      #endif
      delay(2000);
 
    }
  }

  //subscreve no tópico
  client.subscribe(mqttTopicSub);

  dht.begin();
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

  #ifdef DEBUG
  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
  #endif

  //aciona saída conforme msg recebida 
  if (strMSG == "1"){         //se msg "1"
     digitalWrite(L1, LOW);  //coloca saída em LOW para ligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }else if (strMSG == "0"){   //se msg "0"
     digitalWrite(L1, HIGH);   //coloca saída em HIGH para desligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }
 
}

//função pra reconectar ao servido MQTT
void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) {
    #ifdef DEBUG
    Serial.print("Tentando conectar ao servidor MQTT");
    #endif
     
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");

    if(conectado) {
      #ifdef DEBUG
      Serial.println("Conectado!");
      #endif
      //subscreve no tópico
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
      #ifdef DEBUG
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      #endif
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}
 
void loop() {
  if (!client.connected()) {
    reconect();
  }

  //envia a cada X segundos
  if ((millis() - ultimoEnvioMQTT) > INTERVALO_ENVIO)
  {
      enviaDHT();
      ultimoEnvioMQTT = millis();
  }
  
  client.loop();
}

//função para leitura do DHT11
void enviaDHT(){

  char MsgUmidadeMQTT[10];
  char MsgTemperaturaMQTT[10];
  
  float umidade = dht.readHumidity();
  float temperatura = dht.readTemperature();
  

  if (isnan(temperatura) || isnan(umidade)) 
  {
    #ifdef DEBUG
    Serial.println("Falha na leitura do dht11...");
    #endif
  } 
  else 
  {
    #ifdef DEBUG
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.print(" \n"); //quebra de linha
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");
    #endif

    sprintf(MsgUmidadeMQTT,"%f",umidade);
    client.publish("casa/umidade", MsgUmidadeMQTT);
    sprintf(MsgTemperaturaMQTT,"%f",temperatura);
    client.publish("casa/temperatura", MsgTemperaturaMQTT);
  }
}
