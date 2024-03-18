//https://www.haruirosoleil.com/entry/2020/02/02/101937
//→【arduino_IDE】googlesheetに書き込んでみた

#include <WiFi.h>
#include <WiFiClientSecure.h>
//#include <OneWire.h> //library manager 
//#include <DallasTemperature.h>
#include <DHT20.h>
#include <Wire.h>

DHT20 DHT(&Wire1);

//#define ONE_WIRE_BUS 32 //INA:33 INB:32
//#define SENSER_BIT    10     // 精度の設定bit

//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

// 接続先のSSIDとパスワード
const char* ssid = "AP01-01"; //無線ルーターのssidを入力
const char* password = "1qaz2wsx"; //無線ルーターのパスワードを入力


void setup() {
  Serial.begin(115200);
  Wire1.begin(33, 32);  //  select your pin numbers here
  //sensors.begin();
  //sensors.setResolution(SENSER_BIT);
}


void loop() {

  //sensors.requestTemperatures();              // 温度取得要求
  //float temperature = sensors.getTempCByIndex(0);
  //Serial.println(sensors.getTempCByIndex(0)); //温度の取得&シリアル送信
  int status = DHT.read();  //温湿を読み取り
  float Temperature = DHT.getTemperature(); //温度をdoubleで受ける
  float Humidity = DHT.getHumidity();    //湿度をdobleで受ける
  Serial.println(DHT.getTemperature()); 
  Serial.println(DHT.getHumidity());
  sendData(Temperature, Humidity); // 温度と湿度の両方を sendData() 関数に送信
  //sendData(Temperature);
  //sendData(Humidity);
  delay(10000);
}


//WiFiに接続
void connectWiFi(){

  Serial.print("ssid:");
  Serial.print(ssid);
  Serial.println(" に接続します。");

  WiFi.begin(ssid, password);

  Serial.print("WiFiに接続中");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("接続しました。");

  //IPアドレスの表示
  Serial.print("IPアドレス:");
  Serial.println(WiFi.localIP());

}

 

//測定データの送信
void sendData(float Temperature,float Humidity){

  WiFiClientSecure sslclient;

  const char* server = "script.google.com";
  String url = "https://script.google.com/macros/s/AKfycby24JbDOJ6uqefprmSN2XSKoaigN2nSZdP399pxEkeIy7Nqw82MRMfPg_6MSOyLHMI/exec";
  //String url = "https://script.google.com/macros/s/1QgnbTi0ThkOPV9EIgXohMit4JPTNltmk3qKyYilbhi12Cyj2tOha6mHa/Aexec";  //googlescript web appのurlを入力
  //String url = "https://docs.google.com/spreadsheets/d/17W3hxEHz6z_ZXpjdkSnRMX1UfqoCVBSksPv-d8BR4B4/edit?pli=1#gid=0";  //googlescript web appのurlを入力
//測定値を準備
/*digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); //超音波を出力
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  Duration = pulseIn(echoPin, HIGH); //センサからの入力
  Duration = Duration / 2; //往復距離を半分
  Distance = Duration * 340 * 100 / 1000000; //音速を340m/sに設定 
  delay(500);

double sensor_data1 = Distance;*/


  //wifiに接続
  connectWiFi();

  //測定値の表示
  Serial.print("Temperature: ");
  Serial.print(Temperature);
  Serial.println("℃");

  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.println("%");

  //urlの末尾に測定値を加筆
  url += "?&1_cell=";
  url +=  Temperature;
  url += "&2_cell=";
  url +=  Humidity;
  
  //url += "?&1_cell_temperature=";
  //url += Temperature;
  //url += "&1_cell_humidity=";
  //url += Humidity;

  //url += "?&1_cell=A1&2_cell=B1";
  //url += "&1_value=" + String(Temperature) + "&2_value=" + String(Humidity);


  // サーバーにアクセス
  Serial.println("サーバーに接続中...");
  sslclient.setInsecure();//skip verification
  
  //データの送信
  if (!sslclient.connect(server, 443)) {
    Serial.println("接続に失敗しました");
    Serial.println("");//改行
    return;
  }

  Serial.println("サーバーに接続しました");

  sslclient.println("GET " + url);
  delay(1000); //私の環境ではここに待ち時間を入れないとデータが送れないことがある
  sslclient.stop();

  Serial.println("データ送信完了");
  Serial.println("");//改行

  //WiFiを切断
  WiFi.mode(WIFI_OFF);
}