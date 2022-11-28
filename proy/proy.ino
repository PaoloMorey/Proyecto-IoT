#include <ESP8266WiFi.h>
#include <PubSubClient.h> // WiFi

const char *ssid = "dlink";         // Enter your WiFi name
const char *pass = "Aveces.com.es"; // Enter WiFi password
// MQTT Broker
const char *mqtt_broker = "192.168.0.9"; // Enter your WiFi or Ethernet IP

const char *topic = "sensor/1";
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
int SECONDS = 0;
int MAX_SECONDS = 1*5; // 5 minutes
char msg[50];

void setup()
{
  int total = 10;
  randomSeed(total);
  Serial.begin(9600),
      pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  currentCar = false;
  contCars = 0;
  prevDistance = -1;

  // connecting to a WiFi network
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // client connecting to mosquitto mqtt broker
  while (!client.connected())
  {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());

    Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str()))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  // publish and subscribe
  client.publish(topic, "Hello From ESP8266");
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println(" - - - - - - - - - - - -");
  delay(1000);
}

void incrementCars()
{
  digitalWrite(trig, LOW);

  delayMicroseconds(4);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  timeInterval = pulseIn(echo, HIGH);
  // distance = timeInterval / 58.4;
  distance = random(20);
  // Serial.println(currentCar);
  if (distance <= 10)
    contCars++;
  String distStr = "Distancia: " + String(distance) + " cm";
  Serial.println(contCars);
}

void loop()
{
  incrementCars();
  SECONDS++;
  if (SECONDS == MAX_SECONDS)
  {
    SECONDS = 0;
    String carsStr = "Publish número de carros: " + String(contCars);
    sprintf(msg, "%ld", contCars);
    Serial.println(carsStr);
    client.publish(topic, msg);
    contCars = 0;
  }
  delay(1000);
}
