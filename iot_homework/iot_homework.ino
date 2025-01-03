#define ID "iot421-001"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <PubSubClient.h>
#define DHTTYPE DHT11
#define DHTPIN 5
char msg[50];
const char* ssid = "iot421";
const char* password = "hzcuiot421";
const char* serverUrl = "http://175.24.228.169:8080/api/machines/check/";
const char* mqtt_server = "175.24.228.169";//   MQTT Broker地址
const float TEMP_MIN = -20.0;   // 最低温度
const float TEMP_MAX = 50.0;    // 最高温度
const float HUM_MIN = 0.0;      // 最低湿度
const float HUM_MAX = 80.0;    // 最高湿度
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
// 添加一个全局变量来跟踪验证状态
bool isVerified = false;
float humidity;    // 添加全局变量
float temperature; // 添加全局变量
StaticJsonDocument<200> doc;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("blockchain/msg", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


bool isDataValid(float temperature, float humidity) {
  // 检查温度是否在正常范围内
  if (temperature < TEMP_MIN || temperature > TEMP_MAX) {
    return false;
  }
  
  // 检查湿度是否在正常范围内
  if (humidity < HUM_MIN || humidity > HUM_MAX) {
    return false;
  }
  
  // 检查数据是否为 NaN（非数字）
  if (isnan(temperature) || isnan(humidity)) {
    return false;
  }
  
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void readDHT() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("从DHT传感器读取失败！");
    return;
  }
}
void sendGetRequest(String parameter1) {
  if (!WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi未连接！");
    return;
  }
  // 如果已经验证通过，不再发送请求
  if (isVerified) {
    readDHT();  // 只读取温湿度数据
    return;
  }

  Serial.println("\n开始发送验证请求...");
  WiFiClient client;
  HTTPClient http;
  String url = String(serverUrl) + parameter1;
  Serial.print("发送请求到: ");
  Serial.println(url);
  if(http.begin(client, url)) {
    int httpResponseCode = http.GET();
    Serial.print("HTTP响应代码: ");
    Serial.println(httpResponseCode);
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("服务器响应: " + payload);
      // 检查服务器响应
      if (payload == "1") {
        Serial.println("设备验证通过！");
        isVerified = true;  // 设置验证状态为已通过
      } else if (payload == "0") {
        Serial.println("此开发板不是受信设备");
        WiFi.mode(WIFI_OFF);
        WiFi.forceSleepBegin();
      }
    } else {
      Serial.println("请求失败！错误代码: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("HTTP请求初始化失败！");
  }
}

  void loop() {
  Serial.println("\n--- 开始新的循环 ---");
  sendGetRequest(ID);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  readDHT();
  
  doc.clear();
  doc["nodeName"] = "DHT11_Node";  // 可以根据需要修改节点名称
  doc["deviceId"] = "client-001";
  doc["ownerId"] = "owner001";     // 可以根据需要修改所有者ID
  doc["sensorType"] = "DHT11";


  // 添加错误信息
  if (!isDataValid(temperature, humidity)) {
    doc["message"] = "";
    if (isnan(temperature) || isnan(humidity)) {
      doc["message"] = "Failed to read data";
    } else if (temperature < TEMP_MIN) {
      doc["message"] = "temperature too low";
    } else if (temperature > TEMP_MAX) {
      doc["message"] = "temperature too high";
    } else if (humidity < HUM_MIN) {
      doc["message"] = "humidity too low";
    } else if (humidity > HUM_MAX) {
      doc["message"] = "humidity too high";
    }
  }
  
  char jsonBuffer[256];
  
  
  // 发送数据
  if (isDataValid(temperature, humidity)) {
    doc["humidity"] = String(humidity);
    doc["temperature"] = String(temperature);
    serializeJson(doc, jsonBuffer);
    client.publish("blockchain/msg", jsonBuffer);
  } else {
    serializeJson(doc, jsonBuffer);
    client.publish("blockchain/error", jsonBuffer);
    delay(20000);
  }
  
  delay(5000);
}