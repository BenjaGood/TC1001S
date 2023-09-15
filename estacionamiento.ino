#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID "iPhone"
#define WIFI_PASSWORD "0123456789"
#define API_KEY "AIzaSyC63Xp49NNunPnxdBqw-yaoAKSWqDIZ37o"
#define DATABASE_URL "https://tc1001s-d0002-default-rtdb.firebaseio.com"
#define USER_EMAIL "pruebasemana5iot@gmail.com"
#define USER_PASSWORD "012345678"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int DISTANCIA = 0;
int pinLed = D2;
int pinEco = D0;
int pinGatillo = D1;

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void setup()
{
  pinMode(pinLed, OUTPUT);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
}

void loop()
{
  if (Firebase.ready())
  {
    DISTANCIA = 0.01723 * readUltrasonicDistance(pinGatillo, pinEco);

    // Actualizar la distancia en Firebase
    if (Firebase.setInt(fbdo, F("/sensor/ultrasonido"), DISTANCIA))
    {
      Serial.printf("Distancia actualizada en Firebase: %d cm\n", DISTANCIA);
    }
    else
    {
      Serial.printf("Error al actualizar la distancia en Firebase: %s\n", fbdo.errorReason().c_str());
    }

    // Leer el estado del LED desde Firebase
    int ledState = 0;
    Firebase.getInt(fbdo, F("/test/LED"), &ledState);

    // Actualizar el estado del LED en Firebase
    if (Firebase.setInt(fbdo, F("/sensor/led"), ledState))
    {
      Serial.printf("Estado del LED actualizado en Firebase: %d\n", ledState);
    }
    else
    {
      Serial.printf("Error al actualizar el estado del LED en Firebase: %s\n", fbdo.errorReason().c_str());
    }
  }

  delay(1000);
}
