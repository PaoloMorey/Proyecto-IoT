#include <ESP8266WiFi.h>
#include <PubSubClient.h> // WiFi

const char *ssid =  "GERBI"; // Enter your WiFi name
const char *pass =  "mama20ld"; // Enter WiFi password 
// MQTT Broker
const char *mqtt_broker = "192.168.1.9"; // Enter your WiFi or Ethernet IP

const char *topic = "test/topic";
const int mqtt_port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

long timeInterval;
long distance;
long prevDistance;
long contCars;
bool currentCar;
int echo = 9;
int trig = 10;

void setup()
{
  Serial.begin(9600),
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  currentCar = false;
  contCars = 0;
  prevDistance = -1;

  // connecting to a WiFi network
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // client connecting to mosquitto mqtt broker
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    
    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and subscribe
  client.publish(topic, "Hello From ESP8266");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println(" - - - - - - - - - - - -");
  delay(1000);
}

void loop()
{
  digitalWrite(trig, LOW);

  delayMicroseconds(4);
  
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  timeInterval = pulseIn(echo, HIGH);
  
  distance = timeInterval/58.4;
  //Serial.println(currentCar);
  if (distance <= 10) 
  {
    if (!currentCar)
      contCars++; 
    //Serial.println("SET TRUE");
    currentCar = true;
    //prevDistance = distance;
  }
  else {
    //Serial.println("SET FALSE");
    currentCar = false;
  }
  String distStr = "Distancia: " + String(distance) + " cm";
  String carsStr = "Número de carros: " + String(contCars);
  client.publish(topic, distStr.c_str());
  client.publish(topic, carsStr.c_str());
/*

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Número de carros: ");
  Serial.println(contCars);*/

  delay(1000);
}
