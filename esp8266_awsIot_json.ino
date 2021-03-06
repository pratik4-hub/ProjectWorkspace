/* ESP8266 AWS IoT*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "libb64/cdecode.h"
}

#define ssid "HomeLander"
#define password "HelloHomelander"
#define deviceName "Stonecold_ESP"
IPAddress staticIP(192, 168, 43, 90); //ESP static ip
IPAddress gateway(192, 168, 43, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS

/*This Endpoint is in Settings option in AWS IoT console*/

#define awsEndpoint "a13w8xiv7tj0cl-ats.iot.us-west-2.amazonaws.com"

/* Three Certificates are obtained while creating new Certificate, one is certificate, another is private key and last one is root CA cert(2048 bit)
 * Neglect or comment out "----BEGIN CERTIFICATE-----" and "----END CERTIFICATE-----" lines.
 * Copy/Paste the certificates in between "EOF(" and ")EOF"
*/ 
const String certificatePemCrt = \
R"EOF(

MIIDWjCCAkKgAwIBAgIVAL2yC1n4T9sRdOQhKZg8skqt+uI3MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDA4MTYxMzMx
MThaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHxmZNXcp9wNX/BEfb
ujK/GmV6s1SxHYC4+Qb0Vy0JPRUkgGd0sw2BQbEOYEHwgCzkRz6x5dq2znCXX+jl
exYzFN0fn4tb+Qkf6vngNfoMWz9UjzITgJ8cgNXMmuQZh/Oss2oi2Ig7BEAHOP/U
6TsFzX14nEBA9mN+p+R4rUdmhfESCabCZGQOTTXfetZy1LsvMCXfrAD/ou7hXtgQ
P4634uSQBrb7qcKa5h4I0FwOLD7RtUq/3DGxI5+6N2UNVWm2lLYvQPjbv0SNQ6a1
qX8tcZ3MHYCEfJ2+EkAXaw1STExJi1OA964nze1un0W+PH5zyUtWmQ98QL6vS/rT
LkY7AgMBAAGjYDBeMB8GA1UdIwQYMBaAFFjGlZktBL1m9T2k6GaYga3jfnh8MB0G
A1UdDgQWBBQwZBJLHovv/I5G3BQ4SOY8TB6CQzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEATw6+W1kDfM762y+imJBrXUeL
VIJbm8SIZD6rctK/T+Ul6XcbgHONkzsUdFiqUdZws9xMIYSqP3ypykRyapd8QVbG
BdvnaPWV0Xtk5G2rdXrREvFxbj4TxaedMxTtgKXhUlE7538wUZPUs1OWCSYnLtyn
+Qht4D0YoJibj8GRpiHkrc8vE3U/L6ffzoI9qFtoUQSGZOxKw3Sa//ADs9GfQ0Y6
/2JNOZF0u1ojowAgdZZzn8KEiYQDNz4mW9tTCvEV3YIHy48sXCJHTWKURpg0iOyh
NeSMLGCEtP3I1IMnhW/N9FE+6LN/4UkudNKlUpVIXEsIxPMc6JCnLjJW6sHgmA==

)EOF"; 

const String privatePemKey = \

R"EOF(

MIIEowIBAAKCAQEAx8ZmTV3KfcDV/wRH27oyvxplerNUsR2AuPkG9FctCT0VJIBn
dLMNgUGxDmBB8IAs5Ec+seXats5wl1/o5XsWMxTdH5+LW/kJH+r54DX6DFs/VI8y
E4CfHIDVzJrkGYfzrLNqItiIOwRABzj/1Ok7Bc19eJxAQPZjfqfkeK1HZoXxEgmm
wmRkDk0133rWctS7LzAl36wA/6Lu4V7YED+Ot+LkkAa2+6nCmuYeCNBcDiw+0bVK
v9wxsSOfujdlDVVptpS2L0D4279EjUOmtal/LXGdzB2AhHydvhJAF2sNUkxMSYtT
gPeuJ83tbp9Fvjx+c8lLVpkPfEC+r0v60y5GOwIDAQABAoIBAQCHiFRzfIQaGK+W
GaNesM4o1JGwUuXMW4Wu+FP9hP8q22PypaKKi8dHv/eNF0TlEzBhluwz6QsfJqNA
L3Nj7MO82BTElYsIVMYZ/2KS+EuyAl3O0m9Yx9QqlEV4UEUcBR4ONpJ1HY4e6908
iTEXkzvVzfmnz+nPqNkLXoAkVvqv1E7HgDfR0KZyPmtaQzfvpbb4LYTU+Aw7tzi9
nGxQkRLxgKmBEQUeks14EDPXpHgvGIP24OWfFecBEOFypmatQ4Wn2yUFrwVgFuUG
Q37ZSpOt/5b3bH85sQSPqi50lFNwlarIm/Ba7nkqp4ZfzVpx9FUH0rvpZY90/kF9
K2LXhyrxAoGBAPmLRkkCDJ/sDijyKcwLpU7RyQzldZc8tavZTOevaN6wWVOeqkYd
0aCmBy4Y1AnVRlyiKDao52twsaEQZtvN2p0Lc82FHTsq/SApPv0u8dLO9Fyrf9x+
rZqs4XyALX6U+w+BpUdoNRdUUdykTP8prbqigovAPYeB5g0SjOhEgBenAoGBAMzx
gXG1Gd58zH9Cj+nykf30kDC+EuR+lfL1tS60qcHUOg7GEk2nmG3NxxzBqDyBsdPd
MAffM3m8zSL5aQcRVgxjFVG6ELMosLOLS19IFpbvfvzYsYBKBlefBaNR8o6tPEKJ
eB191oWXrlAm9vrS+nx9aSKSynFtcPwkwwijJK9NAoGAB5uVYvhrBh0eOWVYnN3m
BkcjB/+e3CN/rmOLkqyoV9Gsh34Y1tZl463sHh2nqT0VThGrhC8WDxJ6d9yWJjFa
bttAho1+XKVs3iQjxXg+6xxgGssntMMv671FipQlzuwf01MyhMDKjZGGhWGE72qH
317A+B2+QnlVWCSuow4jkZUCgYAzbDWKjmJbTP0SwWTS0qaHDiJP2KFs68Y8HCLG
AVbG57VZl6FoErFQe92F/aF07SXtfwZaM0r7NMBUEjikKVlp6kKjKwidq4wKwNy8
n53raVR3pPeEtQ46VhBYFmvCGP2RxGR7IeP9yT1Xpy85aE6phd4naqWZKFi9K4w0
UpsV0QKBgHq8E5mQ8hkOp84Ourxzq3JKzuuI8oSSVAWQoMMx4Hhj7uuutdHvIrMD
brWz5817VgyAyJp73YAqY/igIMLaQwgnJdxJNJAh3S9vhaXy9qV3/lbp+JqFMisV
m1/KcITfOFAhiZOucQ4tnNUSKTl0SkZxZZMA8BqkLfEbAPk4+Oh0


)EOF";

const String caPemCrt = \
R"EOF(
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5

)EOF";

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(115200); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  
  WiFi.disconnect();  //Prevent connecting to wifi based on previous configuration
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
  WiFi.config(staticIP, subnet, gateway, dns);
  
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  uint8_t binaryCert[certificatePemCrt.length() * 3 / 4];
  int len = b64decode(certificatePemCrt, binaryCert);
  wiFiClient.setCertificate(binaryCert, len);
  
  uint8_t binaryPrivate[privatePemKey.length() * 3 / 4];
  len = b64decode(privatePemKey, binaryPrivate);
  wiFiClient.setPrivateKey(binaryPrivate, len);

  uint8_t binaryCA[caPemCrt.length() * 3 / 4];
  len = b64decode(caPemCrt, binaryCA);
  wiFiClient.setCACert(binaryCA, len);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  //Add a JSON package of fake data to deliver to AWS IoT
  //Uses snprintf but other viable options are: sprintf, strcpy, strncpy, or
  //Use the ArduinoJson library for Efficient JSON serialization
  //If you need to increase buffer size, then you need to change MQTT_MAX_PACKET_SIZE in PubSubClient.h
  char fakeData[128];

  float var1 =  random(55,77); //fake number range, adjust as you like
  float var2 =  random(77,99);
  sprintf(fakeData,  "{\"uptime\":%lu,\"temp\":%f,\"humid\":%f}", millis() / 1000, var1, var2);


  

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;

   pubSubClient.publish("outTopic", fakeData);
   Serial.print("Published: "); Serial.println(fakeData);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESPthing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

int b64decode(String b64Text, uint8_t* output) {
  base64_decodestate s;
  base64_init_decodestate(&s);
  int cnt = base64_decode_block(b64Text.c_str(), b64Text.length(), (char*)output, &s);
  return cnt;
}

void setCurrentTime() {
  configTime(3 * 3600, 0,"pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}
