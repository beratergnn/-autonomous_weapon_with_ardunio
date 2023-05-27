#include<stdlib.h> //C++ dilinde kullanılan standart kütüphaneyi içerir.
#include <Wire.h> //Arduino için kullanılan bir seri haberleşme kütüphanesi.
#include <Adafruit_GFX.h> //Adafruit tarafından sağlanan grafik arabirimi kütüphanesi.
#include <WiFi.h> //WiFi bağlantısı için Arduino kütüphanesi.
#include "WiFiClientSecure.h" //Güvenli bir WiFi bağlantısı için kullanılan kütüphane.
#include "Adafruit_MQTT.h" //Adafruit tarafından sağlanan MQTT protokolü kütüphanesi.
#include "Adafruit_MQTT_Client.h" //Adafruit tarafından sağlanan MQTT istemcisi kütüphanesi.
#include <ESP32Servo.h>//Servo tanıtımı için kütüphane.

#define CLK 4 //Saat Sinyali
#define DT 15 //Veri Sinyali

#define WLAN_SSID "baris"
#define WLAN_PASS "12345678"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  8883

#define IO_USERNAME  "barisbalcikoca"
#define IO_KEY       "aio_rCYA21ZrDCUJH6e1idyVGk2jlqaq"

#define servoPress 27   // Press butonunu tetikleyen servoMotor PIN tanımı.
#define servoTarama 26  // 90 derecelik alanda tarama yapan servoMotor PIN tanımı.
#define servoDerece 25  // İlgili mesafeyi ölçüp eğik atış için gerekli açıyı veren servoMotor PIN tanımı.

Servo servoPress27;   // Pressi karşılaması için Servo türünde nesne tanımı.
Servo servoTarama26;  // Taramayı karşılaması için Servo türünde nesne tanımı.
Servo servoDerece25;  // Dereceyi karşılaması için Servo türünde nesne tanımı.

WiFiClientSecure client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);

const char* adafruitio_root_ca = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
    "-----END CERTIFICATE-----\n";


Adafruit_MQTT_Publish mesafe = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/tof10120"); //Mesafe için
Adafruit_MQTT_Publish ruzgar = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/rotaryencoder"); // rüzgar için

 
// "unsigned" ifadesi, değişkenin sadece pozitif değerleri içerebileceğini gösterir
unsigned short lenth_val = 0; //ReadDistance() değişkeninde tutulan mesafe verisi
unsigned char i2c_rx_buf[16];

int counter = 0; //Rotary Encoder içerinde tutulan veri
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
int distance =-1;


void setup(){
  Wire.begin();
  Serial.begin(115200); // Başlatma işlemleri, seri haberleşme 115200 baud hızında yapılıyor.
  pinMode(CLK,INPUT); // CLK pininin giriş olarak ayarlanması
  pinMode(DT,INPUT); // DT pininin giriş olarak ayarlanması
  Serial.println(F("Adafruit IO MQTTS (SSL/TLS) Example"));

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID); // WLAN_SSID bilgisi ekrana yazdırılıyor

  WiFi.begin(WLAN_SSID, WLAN_PASS); // WiFi bağlantısı WLAN_SSID ve WLAN_PASS bilgileri ile yapılıyor
  delay(2000); // 2 saniye gecikme
  while (WiFi.status() != WL_CONNECTED) { // WiFi bağlantı kontrolü, bağlantı sağlandığında döngüden çıkılıyor
  delay(500); // 0.5 saniye gecikme
  Serial.print("."); // Bağlantı yapılırken ekrana nokta işareti yazdırılıyor

  Serial.println();

  Serial.println("WiFi connected"); // Bağlantı sağlandıysa ekrana "WiFi connected" yazdırılıyor
  Serial.println("IP address: "); Serial.println(WiFi.localIP()); // Ekrana IP adresi yazdırılıyor
  client.setCACert(adafruitio_root_ca); // client'ın root CA sertifikası ayarlanıyor

  lastStateCLK = digitalRead(CLK); // CLK pininin son durumunun okunması

  attachInterrupt(4, updateEncoder, CHANGE); // 4 numaralı pin için kesinti servis rutini olarak updateEncoder fonksiyonu atanıyor
  attachInterrupt(15, updateEncoder, CHANGE); // 15 numaralı pin için kesinti servis rutini olarak updateEncoder fonksiyonu atanıyor
  delay(500);

  servoPress27.attach(servoPress);    // Press için ilgili servo nun pin ile eşleşmesi
  servoTarama26.attach(servoTarama);  // Tarama için ilgili servo nun pin ile eşleşmesi
  servoDerece25.attach(servoDerece);  // Derece için ilgili servo nun pin ile eşleşmesi
}
}
int eskiDeger=0; //Verinin gönderilmesi kısmında adafruite sadece değişen verileri gönder.
void mesafeOkuma()
{
    distance = ReadDistance();
    delay(15);
}
uint32_t x=0;
int mutlakpozisyon;
void loop(){
  MQTT_connect();

//----------------------------------------
  for (int pos = 45; pos <= 90; pos += 5) {
    mesafeOkuma();
    Serial.print(distance);
  Serial.println(" mm");
    delay(15);
    if(distance<2000){
      mutlakpozisyon=pos;
      hesaplaVeAt();
    }
    servoTarama26.write(pos);  
  }
    adafruitPush();

  
  for (int pos = 90; pos >= 45; pos -= 5) {
    mesafeOkuma();
    Serial.print(distance);
  Serial.println(" mm");
    delay(15); 
    if(distance<2000){
      mutlakpozisyon=pos;
      hesaplaVeAt();
    }
    servoTarama26.write(pos);  
  }
  adafruitPush();
  for (int pos = 45; pos >= 0; pos -= 5) {
    mesafeOkuma();
    Serial.print(distance);
  Serial.println(" mm");
    delay(15); 
    if(distance<2000){
      mutlakpozisyon=pos;
      hesaplaVeAt();
    }
    servoTarama26.write(pos);  
  }
    adafruitPush();

  for (int pos = 0; pos <= 45; pos += 5) { 
    mesafeOkuma();
    Serial.print(distance);
  Serial.println(" mm");
    delay(15); 
     if(distance<2000){
      mutlakpozisyon=pos;
      hesaplaVeAt();
    }
    servoTarama26.write(pos);  
  }
    adafruitPush();

}
//-----------------------------------------
int derece=10;
bool atisYapildi=false;
int hesaplaVeAt(){

if(mutlakpozisyon<=45){
  if(counter<0 && counter>-10){
    for (int pos = mutlakpozisyon; pos >= mutlakpozisyon-10; pos -= 1) {
      servoTarama26.write(pos);
      delay(15);    
    }
  }else if(counter<-10 && counter>-20){
    for (int pos = mutlakpozisyon; pos >= mutlakpozisyon-20; pos -= 1) {
      servoTarama26.write(pos);
      delay(15);    
    }
  }

}else if(mutlakpozisyon>45){
  if(counter<10 && counter>0){
    for (int pos = mutlakpozisyon; pos <= mutlakpozisyon+10; pos += 1) {
      servoTarama26.write(pos);
      delay(15);  
    }
  }else if(counter<20 && counter>10){
    for (int pos = mutlakpozisyon; pos <= mutlakpozisyon+20; pos += 1) {
      servoTarama26.write(pos);
      delay(15);  
    }
  }
}

  
  if(distance<2000 && distance>1350){
  derece=10;
  for (int pos = 80; pos >=10; pos -= 1) {
      servoDerece25.write(pos);
      delay(15);      
  }
  atisYapildi=true;
// 30 derece
}else if(distance<=1350 && distance>700){
  derece=30;
   for (int pos = 80; pos >=30; pos -= 1) {
      servoDerece25.write(pos);
      delay(15);      
  }
  atisYapildi=true;
// 15 derece
}else if(distance<=700 && distance>0){
  derece=40;
 for (int pos = 80; pos >=40; pos -= 1) {
      servoDerece25.write(pos);
      delay(15);      
  }
  atisYapildi=true;
}
// Atışı yapma
if(atisYapildi){
  for (int pos = 0; pos <= 27; pos += 1) {
      servoPress27.write(pos);
      delay(15);      
  }
  for (int pos = 27; pos >= 0; pos -= 1) {
      servoPress27.write(pos);
      delay(15);      
  }
  atisYapildi=false;
}
for (int pos =derece; pos <= 80; pos += 1) {
      servoDerece25.write(pos);
      delay(15);      
}
distance = 2001;
return 0;
}

void adafruitPush(){
  if(distance!=eskiDeger)
  {
    mesafe.publish(distance);//mesafe verisinin adafruite gönderildiği yer
    ruzgar.publish(counter);//rotary encoder(rüzgar) verisinin adafruite gönderildiği yer
    Serial.println("Gönderildi");
    eskiDeger=distance;
  }
  Serial.print(distance);
  Serial.println(" mm");
}

//--------------------------------------------------

void updateEncoder(){
	// CLK Değerini Oku
	currentStateCLK = digitalRead(CLK);

  // Eğer CLK'nın son durumu ve mevcut durumu farklıysa
	if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){

  //DT durumu CLK durumundan farklıysa, o zaman
		if (digitalRead(DT) != currentStateCLK) {
			counter --;
			currentDir ="CCW";//saat yönü tersine
		} else {
			// Enkoder CW(saat yönü) yönünde dönüyorsa
			counter ++;
			currentDir ="CW";
		}
		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(counter);
	}
	// Son CLK durumunu tut
	lastStateCLK = currentStateCLK;
}

//haberleşme protokolü ile bağlı olan bir sensörden veri okur
void SensorRead(unsigned char addr,unsigned char* datbuf,unsigned char cnt) 
{
  unsigned short result=0;
  Wire.beginTransmission(82); 
  Wire.write(byte(addr));      
  Wire.endTransmission();     
  delay(1);                   
  Wire.requestFrom(82, cnt);    
  if (cnt <= Wire.available()) { 
    *datbuf++ = Wire.read();  
    *datbuf++ = Wire.read(); 
  }
}
int ReadDistance(){
    SensorRead(0x00,i2c_rx_buf,2);//i2c_rx_buf (alınan verilerin saklanacağı dizi veya tamponun bir göstericisi) 2 bayt sayısı
    lenth_val=i2c_rx_buf[0];
    lenth_val=lenth_val<<8;
    lenth_val|=i2c_rx_buf[1];
    delay(100); 
    return lenth_val;
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }

  Serial.println("MQTT Connected!");
}
