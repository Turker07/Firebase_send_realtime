
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt
 * 
 * Copyright (c) 2020 mobizt
 *
*/
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <time.h>
#include <WiFi.h>

#include <FirebaseESP32.h>

#define uS_TO_S_FACTOR 59700000  /* Conversion factor for micro seconds to seconds */



#define DHTPIN 13  

//1. Change the following info
#define WIFI_SSID "your wifi name"
#define WIFI_PASSWORD "your wifi password"
#define FIREBASE_HOST "https://-------(your firebase host)-----.firebaseio.com"
#define FIREBASE_AUTH "your firebase auth"

FirebaseData fbdo;


const char* NTP_SERVER = "ch.pool.ntp.org";  // zamanın çekildiği site
const char* TZ_INFO    = "EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00";  // zaman diliminizi seçin enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php) // 
char time_output[30];
char date_output[30];
char day_output[30];
char mount_output[30];
char year_output[30];
int temp,frequency;
tm timeinfo;
time_t now;
long unsigned lastNTPtime;
unsigned long lastEntryTime;
String adres,sensorId,temparature,sensorType;



void setup()
{

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


 dht.begin();
  //3. Set your Firebase info

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //4. Enable auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

  //5. Try to set int data to Firebase
  //The set function returns bool for the status of operation
  //fbdo requires for sending the data
  


configTime(0, 0, NTP_SERVER);
  // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv  Bölgeniz için saat dilimi kodlarına buradan ulaşabilirsiniz.
  setenv("TZ", TZ_INFO, 1);

  
//  showTime(timeinfo);
  lastNTPtime = time(&now);
  lastEntryTime = millis();

sensorId="123456";
sensorType="temparature";
}

void loop()
{
   
  getNTPtime(10);  // oluşturulan fonksiyonda 10 saniyede bir veriler çekliyor burada fonksiyonun içerisini değiştirerek güncellemeleri değişterebilirsiniz. 
//  showTime(timeinfo); // zaman bilgisini gösteren fonksiyon 
  delay(1000);


    
    strftime(day_output, 9, "%d", localtime(&now));
    strftime(mount_output, 9, "%m", localtime(&now));
    strftime(year_output, 9, "%y", localtime(&now));
    strftime(time_output, 6, "%T", localtime(&now));
    
    adres="Sensor ID: "+sensorId+ " Sensor Type: "+sensorType+"/Data"+"/Year: "+year_output+"/Mount: "+mount_output+"/Day: "+day_output+"/Time: " +time_output+ "  Value ";
    
    if(Firebase.setInt(fbdo,adres,t ))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
  
   adres="/ORCAQS/Sensor ID: "+sensorId+ " Sensor Type: "+sensorType+"/frequency";
if(Firebase.getInt(fbdo, adres))
  {
    //Success
    Serial.print("Get int data success, int = ");
    Serial.println("anahtarDeger:");
   Serial.println( fbdo.intData());
    frequency=fbdo.intData();

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in getInt, ");
    Serial.println(fbdo.errorReason());
  }
  #define TIME_TO_SLEEP  frequency
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
  
}

bool getNTPtime(int sec) {

  {
    uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &timeinfo);
      delay(10);
    } 
    while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    if (timeinfo.tm_year <= (2016 - 1900)) 
    return false; 

     
    Serial.print("now ");  Serial.println(now);
    
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now)); // time_output içerisinde bulunan verileri istenen %a  %d-%m-%y %T düzenler. 
    Serial.println(time_output);
    Serial.println();
  }
  return true;
}
