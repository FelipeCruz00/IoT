//bibliotecas utilizadas
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>

//Login de rede e conexão com mqtt
const char* ssid = "Seu_Wifi";
const char* password = "Sua_senha";
const char* mqtt_server = "broker.mqtt-dashboard.com";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
String inString = "";

//pinos utilizados
const int umidade = 36;
int rele = 23;

//MQTT
const char* umidadea = "home/umidadea";
const char* umidades = "home/umidades";
const char* temperatura = "home/temperatura";

//pino para receber os dados do DHT
#define DHTPIN 27

//Tipo de sensor de umidade e temperatura(pode variar entres os 3 modelos: DHT 11, DHT 22, DHT 21)
#define DHTTYPE DHT22

//Criando instancia do DHT
DHT dht(DHTPIN, DHTTYPE);

//variáveis utilizadas para o sensor de umidade de solo
float umidadePorcentagem;
int valor;

//Setup do wifi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  inString = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    inString += (char)payload[i];
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("home/temperatura", "ESP Conectado");
      // ... and resubscribe
      client.subscribe("home/temperatura");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(rele, OUTPUT);
  pinMode(4, OUTPUT);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  //Lê a umidade do ar
  float h = dht.readHumidity();
  // Lê a temperatura em Celsius
  float t = dht.readTemperature();

  //imprime valores
  Serial.print("Humidade: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C ");

  //Verifica valor da umidade do solo
  valor = analogRead(umidade);

  //converte para porcentagem
  umidadePorcentagem = map(valor, 4096, 0, 0, 100);

  //imprime valores
  Serial.print("Umidade do solo  = ");
  Serial.print(umidadePorcentagem);
  Serial.println("%");

  //Enviando dados para mqtt
  char tempString[8];
  dtostrf(t, 1, 2, tempString);
  client.publish(temperatura, tempString);
  dtostrf(h, 1, 2, tempString);
  client.publish(umidadea, tempString);
  dtostrf(umidadePorcentagem, 1, 2, tempString);
  client.publish(umidades, tempString);

  //relé
  if (umidadePorcentagem <= 45) {
    digitalWrite(rele, LOW);
  } else {
    digitalWrite(rele, HIGH);
  }
  delay(1000);
  client.loop();
}
