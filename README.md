# Firebase_send_realtime
IOT 

Bu çalışmada esp32 de verilerin zaman bilgisi ile firebase veritabanına gönderilmesi ve firebase 
veritabanının nasıl yazılması gerektiğinden bahsedilmiştir. 
Daha önce nasıl firebase veri tabanına veri gönderilmesi gerektiğini anlatılmıştır. O yüzden bu kısımlar atlanmıştır. 


### Firebase veri düzeni

Firebase veri depolanması başlıklar halinde yapılabilir. 
İlk önce ana başlık daha sonra alt başlıklar oluşturarak düzeninizi oluşturabilirsiniz. 

> Örneğin 
1. Ana başlık
   - Ana alt başlık
     - alt başlık
  <br>
 Böyle bir veri yapısı elde etmek istiyoruz. Bunun için: 
 
  `````````
  adres="1. Ana başlık/Ana alt başlık/alt başlık";
    
    if(Firebase.setInt(fbdo,adres,t ))
  {
    //Success
     Serial.println("Set int data success");

  }else{
    //Failed?, get the error reason from fbdo

    Serial.print("Error in setInt, ");
    Serial.println(fbdo.errorReason());
  }
 
 `````````
 adres isminde bir string ifade tanımlanır, bu ifadeye verinin gönderilmek istenen veri tabanındaki yerini belirtmektedir. 
 Örnekte veridiği gibi bir veritabanının oluşmasını isteniyorsa adres içerisine ilk önce ana başlık yazılmalıdır. 
 daha sonra alt başlıkların oluşması isteniyorsa her başlık arasına "/" işaret konulmalıdır. En sonda da belirtilen alt başlığa göndermek istenen değer yazılmaktadır.
 
 
 
 ### Real time ile  veri gönderme
 
  Esp32 ile firebase veritabanına gönderilen verilerin hangi saatte hangi verinin geldiğini takip etmek için esp'nin internetten tarih ve saat verisini çekip daha sonra 
  gönderilmek istenen veri ile tarih ve saat bilgisi gönderilir. Böylece sadece verilerin değerleri değil zaman bilgisi ile daha nitelikli verileri elde edilir. 
   <br>
   
  Saat ve tarih bilgisini nasıl çekildiği daha önceki çalışmada anlatılmıştı.  <br>
  
  Önceki çalışmada tarih ve saat bilgisi aynı anda alınmaktadır ancak database de karmaşıklık çıkarmamak ve kolay erişilebilir yapmak için tarih ve saat bilgileri ayrı ayrı alınmaktadır. 
  
  `````````
  strftime(day_output, 9, "%d", localtime(&now));
    strftime(mount_output, 9, "%m", localtime(&now));
    strftime(year_output, 9, "%y", localtime(&now));
    strftime(time_output, 6, "%T", localtime(&now));
  `````````
 day_output ile gün bilgisini , mount_output ile ay bilgisini , year_output ile yıl bilgisini ve time_output ile dakika ve saat bilgisini elde edilmektedir. 
 <br> 
 1. Sensor ID:1234
   - Data
     - Year:22
       - Mount:02
         - Day:15
           - Time:11:25
              - Value:00
 
 şeklinde bir veritabanı oluşturmak için adres bölümüne 
 
  `````````
  adres="/Sensor ID: "+sensorId+ /Data"+"/Year: "+year_output+"/Mount: "+mount_output+"/Day: "+day_output+"/Time: " +time_output+ "  Value ";
   `````````
   
   adres değişkeni string bir değişkendir ve string değişkenlere tırnak içeretleri içerisinde eklemeler ve + ile string ifade diğer string ifadeler toplanarak tek bir string ifade ortaya çıkarılabilir. 
   database'e Ana başlık olarak sensor ID yazılmış ve bunun değerini yazmak için sensorId eklenmiştir. Daha sonra alt ana başlığa geçmek için "/" ifadesi kullanılmış ve Data adında bir alt ana başlık oluşturulmuştur. 
   Diğer yıl, ay, gün ve zaman bilgiside alt başlıklar olarak eklenmiştir. en sonda ise value ifadesine gönderilmek istenen değer ifadesi yazılmıştır.
   
   
  
  ## Veri gönderme sıklığı
  
  Verilerin sürekli olarak gönderilmesi esp32'nin ısınmasına ve veritabanın şişmesine sebep olabilir. Bunun için verilerin belirli aralıklar ile gönderilmesi daha doğru olacaktır. <br>
  Esp32 nin veri göndermedi zaman aralıklarında deepsleep moduna girerek daha az enerji harcaması ve esp'nin uzun ömürlü kullanılması sağlanabilir. <br>
  Esp32 modüllerinde deepsleep moduna aşağıdaki kod ile girmektedir. 
  ```
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
  ```
  
   ```
   #define TIME_TO_SLEEP  frequency
   #define uS_TO_S_FACTOR 59700000 
  ```
  uS_TO_S_FACTOR ile mikrosaniyeyi dakikaya çevirmek için gerekli katsayıyı tanımladık, TIME_TO_SLEEP değerine ise frequency tanımlayarak kaç dakika boyunca uykuya gireceğini belirmektedir. 
    
  ## database'den veri çekme
  
  frequency bilgisini database'den çekerek kaç dakika bir veri göndermesini gerektiğini belirlenmektedir. 
  
  ```
    
   adres="/Sensor ID: "+sensorId+ " Sensor Type: "+sensorType+"/frequency";
if(Firebase.getInt(fbdo, adres))
  {
    //Success
    Serial.print("Get int data success, int = ");
    Serial.println("anahtarDeger:");
   Serial.println( fbdo.intData());
    frequency=fbdo.intData();

  }
  ```
  adres değişkeninde yer alan frequency girilen bir int değerin veritanından çekilmesi yukarudaki kod ile sağlanmakdır. 
  böylelikle veritabanından istenilen zaman aralığı ayarlanabilir. 
