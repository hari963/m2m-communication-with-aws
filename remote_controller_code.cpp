#include <pgmspace.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>        // Lib for esp266
#include <DNSServer.h>          //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>   //Local WebServer used to serve the configuration portal
#include <PubSubClient.h>       // Lib for for MQTT protocols , and PUB/SUB cilent
#include <NTPClient.h>          // Lib for internet Time
#include <WiFiUdp.h>            // Lib for UDP protocols
//cert certificate from amazon aws
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
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
-----END CERTIFICATE-----)EOF";
//aws server private policy
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAz+VO6AAOtVHYklYmnHO8eetMfNRbwVm//5tktYEgr/5vBa3m
tLevo1ZTZ9l6qE5cjRtW8ZaKZqRRVo6DiyLWFXlsl/2spPNmeK0VvdH9AzgpC7Nx
8vokkA7T/U/Rj5Hd5BiUsnaNB/2qSTQCM9cgD6jDWyLrRRrZyndlgu4m6TcYKfPg
ymjH6TSlrNXJydoqITVLFBn5lClZM1IR8YWUglpAoFdJnTQ9HPoN2GTWzL4sIRoZ
BHFC7jpE+tokbiSpx6adwioJKxhM58Uc1xYx4FOPVYDBbKxwTpoRnIvcbLOKpnEz
97y6Psw+VNNWQaa9vrmMqaSVsMEOGWh7JrDYSQIDAQABAoIBADPOFXKUiRW/1nLp
5SWnwadKQbhCO2JDUPajUvahPCYwOXIz/iIpGTFYsJS/7h2ToLJ1OaCbEBu8p1Fk
h2P5qICeckfklLH8tebRB9evlryoTfDsLm7b6fo31xmE1cC3Bog4bW0qdzmmve/Y
MyEmAw87MhDukIE6HsHp/Xavn+ehJEAVJGdP5iYZj/s+xZUdIWwLAvvCPucIJxCb
CwGEsFYRIyrVSCaDLHPTsnuD+Jsf2WPZ/Xt+KaNrW0ZqZQPMbkLVFgxRK+Suu39n
Y7DZErECgYEA7k8kN/TcH1kNYoGhvJNtd48OhIaZ9BTJPUb0NdwiDvLDFhTzM2Ys
ZkZYMFl4n9dJSfEC4GXmD2wzFZhGQl8GPh1jPBanKyzEDAKZEXeKtTx1q281PrUM
/o7lJf35tWVRqV41W6ih2GlqD6VlFLVh67/JkEz2Us9ZTLMCm1Z2ME0CgYEA31Qv
ru5v4WKuVcEofo4WLe9UGX/RHxgauBtcOtZ22Lt+gO5rkqrtUO+bjsMEQIBXtoMG
VK/4bBzombtqcG15tR5l0hWFvbp/3J8ZlaPNO6sJ72/3uWW6VCcyBfuNVo5b1jhF
GqQasSa9k5foGLmwpw5TVx45qXr0aSjHTBz3Je0CgYBA13fbYdyjCvABZZREYzjd
1yb0GEu7hgxbh/razNLXjmZWk8y4p4xdJ7bHMREJLOLcI5/Mvu0iNSaRMaWsWX6x
V/2wU9mYfc26FtrfTc+IELNTuiaasVifyvG+rKRNHwxhMSkDj5a587TQHQir4c6K
/twxTz+WuKRroimLarMt4QKBgQDPohKvHgmwz7FsEyjV+L5QOLnE91vy/9Mfpnhq
T6ixOiiUMJadBwObAxpqcY/LdEeYbVr+c5iQ05PCEkgUfQPxy3A5ET+FtRZZhSYh
ROhz+EJFdaEoopbI1KQigczXU4W4thoxea9DUtRgm5PzIPlHZ/89+OD9XE5OUtZw
QmHZxQKBgHt6L/6EKWeNkTfWjruKosKSbUhc+2p0zM1kUndpDvOD6yXokSRQcOSE
VVGXpjpOgY6Yutc2W55ADOh5BZvCig3vj/Q+AhCoIPRokxblP0JXkqukPwqf1tHV
I2MCxBbHzxp6mQQecWCm/7SNrCnKW3dDmtvxYQxFfxH+hRsAHYGC
-----END RSA PRIVATE KEY-----
)KEY";
//aws cert thing certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUYd7t7UJddZfRLDdnbnrVbk82tXkwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIwMTAwNjAzNTU1
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAM/lTugADrVR2JJWJpxz
vHnrTHzUW8FZv/+bZLWBIK/+bwWt5rS3r6NWU2fZeqhOXI0bVvGWimakUVaOg4si
1hV5bJf9rKTzZnitFb3R/QM4KQuzcfL6JJAO0/1P0Y+R3eQYlLJ2jQf9qkk0AjPX
IA+ow1si60Ua2cp3ZYLuJuk3GCnz4Mpox+k0pazVycnaKiE1SxQZ+ZQpWTNSEfGF
lIJaQKBXSZ00PRz6Ddhk1sy+LCEaGQRxQu46RPraJG4kqcemncIqCSsYTOfFHNcW
MeBTj1WAwWyscE6aEZyL3GyziqZxM/e8uj7MPlTTVkGmvb65jKmklbDBDhloeyaw
2EkCAwEAAaNgMF4wHwYDVR0jBBgwFoAU/daCQWtmWz3PQSptY2g0l5GmDUgwHQYD
VR0OBBYEFKELISlsYyPpRs3zP9YS5V4gtsm5MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB+BVhUZu+oGxFxNxcJf76hOAni
CKzMVcz0WdPfBvfLniCbvfye7mKPpkcgDPawZd/iXSNfN5MGrv8i/b4TH62rKlEw
GlZlwwkghmYCty4L8p/Q13CMNXzm73lhS4DTtsBot5F15HKd11VnNqWRPNGQAZm4
z0Ug5brAzHQ640Tm3Iv+UypiSqWMkz/MtK1lF6ZauF/OHmjzOf/mZ0tyRSFjV4aU
mvjya1L8Lwb2Pf/9H60njjEJjgwPVMXm+bIMt/n7x/35DXwETlmBmCihnIcE+4Wg
D/2ZZZq0/z4AfuJI+s/+hbSA5yRqWadWFx8qcwO6wIjOCGbh5sL/G8lJ7OPr
-----END CERTIFICATE-----
)KEY";
WiFiClientSecure net = WiFiClientSecure();
// Wifi SSID and Password
const char *ssid = "hari";
const char *password = "12345678";
// declaration 
const int led = 2;
const int button = 3;
const int feedback=4;
int msg='0';
char *s;
//aws end point adress
const char *AWS_endpoint ="a3vpnn****pdfy-ats.iot.us-east-2.amazonaws.com";
//wifi UDP client
WiFiUDP ntpUDP;
//time client to fetch internet time
NTPClient timeClient(ntpUDP, "pool.ntp.org");
//connecting wifi client with aws
WiFiClientSecure espClient;
//wifi connection setup
void setup_wifi(){
WiFi.begin(ssid, password);

while(WiFi.status()!= WL_CONNECTED){
  digitalWrite(led,LOW);
delay(2000);//wait 2 secs
}
digitalWrite(led,HIGH);//turn on led if wifi connected 
}
//this is used to read data from aws server
void callback(char* topic, byte* payload, unsigned int length);
//creation of client for aws server
PubSubClient client(AWS_endpoint, 8883, callback, espClient);
//aws connection setup
void awsconnect(){
  while(!client.connected()){
    //this line will blink the led while connecting to aws
    client.connect("ESP8266");
    digitalWrite(led,HIGH);
    delay(500);
    digitalWrite(led,LOW);
    delay(500);
    }
   if(client.connected()){
    client.subscribe("ESP8266");//subscribe to ESP8266 thing in aws
    }
  }
void setup(){
Serial.begin(9600);
pinMode(led,OUTPUT);//decalre led pin to output in
digitalWrite(led,LOW);//write led to low
pinMode(feedback,OUTPUT);//declare feedback pin to output pin
digitalWrite(feedback,LOW);//write feedback to low
pinMode(button,INPUT);//read input from button
setup_wifi();
// start time and gets current time
timeClient.begin();
//updates current time
while(!timeClient.update()){
timeClient.forceUpdate();
}

espClient.setX509Time(timeClient.getEpochTime());
//uploading certificates to aws server
//net.setCACert(AWS_CERT_CA);
//net.setCertificate(AWS_CERT_CRT);
//net.setPrivateKey(AWS_CERT_PRIVATE);
}
void loop(){
  //if aws is client is not connected it calles awsconnects function
  if(!client.connected()){
    awsconnect();
  }
  //read data from button
  if(digitalRead(button)==HIGH){
    if(msg==0){
      msg=1;//if smart device is turned on it will turn off device 
      digitalWrite(feedback,HIGH);//this will show the current state of device
      }
    if(msg==1){
      msg=0;//if smart device is turned off it will turn on device
      digitalWrite(feedback,LOW);}}
   if(WiFi.status()== WL_CONNECTED){
    digitalWrite(led,HIGH);}//this will state the current state of wifi
     else{
      digitalWrite(led,LOW);
      setup_wifi();
        } 
   snprintf(s,24,"",msg);
   client.publish("ESP8266",s);// this will upload data to aws serever
 
  }

