#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Datos de su red WIFI
const char* ssid = "iPhone de Felipe";
const char* password = "1234abcd";

// Inicializar BOT Telegram
#define BOTtoken "6782731599:AAG9iS2bor4X-fb3UizgKoaB5AuSzo4fbZw"  // Tu Bot Token (Obtener de Botfather)

// Usa @myidbot para averiguar el chat ID
// Tambien necesita hacer click en "start" antes de enviarle mensajes el bot
#define CHAT_ID "1067649956"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int buzPin = 4;
const int trigPin = 22;
const int echoPin = 23;
const int led = 19;

// Variables para el cálculo de la distancia
long duration;
int distance;

//Identifica cuando  hay un nuevo mensaje por parte del usuario y define que quiere decir la instruccion
void handleNewMessages(int numNewMessages) {

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Usuario No Autorizado", "");
      continue;
    }

    String text = bot.messages[i].text;

    if (text == "/alarma_on") {
      bot.sendMessage(chat_id, "Alarma activada", "");
      digitalWrite(buzPin, HIGH);
      digitalWrite(led, HIGH);
    }
    
    if (text == "/alarma_off") {
      bot.sendMessage(chat_id, "Alarma desactivada", "");
      digitalWrite(buzPin, LOW);
      digitalWrite(led, LOW);
    }
  }
}

void setup() {
  Serial.begin(9600);

  // setup de los pines de cada componente
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzPin, OUTPUT);
  pinMode(led, OUTPUT);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while(numNewMessages) {
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
    
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Mide el tiempo que tarda en llegar el eco
  duration = pulseIn(echoPin, HIGH);
  
  // Calcula la distancia en centímetros
  distance = duration * 0.034 / 2;
  
  // Imprime la distancia en el monitor serial
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
  //define cuando se debe mandar una alerta
  if(distance < 10){
    bot.sendMessage(CHAT_ID, "Se Detecto un Movimiento, Alarma Activa!!", "");
    Serial.println("Movimiento Detectado");
    digitalWrite(buzPin, HIGH);
    digitalWrite(led, HIGH);
    delay(200);
    digitalWrite(buzPin, LOW);
    digitalWrite(led, LOW);
  } else {
    // Desactiva el buzzer
    digitalWrite(buzPin, LOW);
    digitalWrite(led, LOW);
  }
  delay(1000);
}