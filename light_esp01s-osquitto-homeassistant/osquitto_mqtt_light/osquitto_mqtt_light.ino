/**********************************************************************
项目名称/Project          : 内网osquitto服务器用esp连接并且控制灯泡
程序名称/Program name     : osquitto_mqtt_light

-----------------------------------------------------------------------
本程序是在用巴法云过程中，总是有一台莫名的设备给人家发qos2消息经常被禁，所以在找不到
源头的情况下，着手在自家服务器搭建osquitto服务器，并且用esp连接。

如有疑问，请前往以下页面寻找解决方案
http://www.taichi-maker.com/public-mqtt-broker/
***********************************************************************/
#include <ESP8266WiFi.h>                 //默认，加载WIFI头文件
#include "PubSubClient.h"                //默认，加载MQTT库文件
 
// 设置wifi接入信息，以及服务器地址
const char* ssid = "BV_02_2.4";
const char* password = "04171228";
const char* mqttServer = "192.168.2.81";
const char* osquitto_user = "user2"; 
const char* osquitto_pass = "user2"; 

//实例化
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//变量
const char*  topic = "home/light/llws";        //主题名字，可创建，名称随意
const int B_led = 0;       //单片机LED引脚值，D系列是NodeMcu引脚命名方式，其他esp8266型号将D2改为自己的引脚

/**
 * 连服务器
 */
void connectMQTTServer(){
  // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
  String clientId = "esp8266-" + WiFi.macAddress(); 
  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str(),osquitto_user,osquitto_pass)) { 
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientId:");
    Serial.println(clientId);
    // 发送主题，指定文字
    //mqttClient.publish("outTopic", "hello world");
    // 订阅主题
    mqttClient.subscribe(topic);
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }   
}
 
/**
 * 连wifi
 */
void connectWifi(){
  WiFi.begin(ssid, password);
  //等待WiFi连接,成功连接后输出成功信息
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");  
  Serial.println(""); 
}

/**
 * 接收到消息回调
 */
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic:");
  Serial.println(topic);
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Msg:");
  Serial.println(msg);
  if (msg == "on") {//如果接收字符on，亮灯
    turnOnLed();//开灯函数
  } else if (msg == "off") {//如果接收字符off，亮灯
    turnOffLed();//关灯函数
  }
  msg = "";
}

/**
 * 开灯
 */
void turnOnLed() {
  Serial.println("turn on light");
  digitalWrite(B_led, LOW);
}

/**
 * 关灯
 */
void turnOffLed() {
  Serial.println("turn off light");
  digitalWrite(B_led, HIGH);
}

/**
 * 主set
 */
void setup() {
  Serial.begin(115200);
  pinMode(B_led, OUTPUT);  //设置引脚为输出模式
  digitalWrite(B_led, LOW); //默认引脚上电高电平
  WiFi.mode(WIFI_STA); //设置ESP8266工作模式为无线终端模式
  //设置好了先连一遍
  connectWifi(); // 连接WiFi
  mqttClient.setServer(mqttServer, 1883); // 设置MQTT服务器和端口号
  mqttClient.setCallback(callback); //设置订阅消息回调
  connectMQTTServer(); // 连接MQTT服务器
}

/**
 * 主loop
 */
void loop() { 
  // 检查断开重连服务器
  if (!mqttClient.connected()) {
    connectMQTTServer();
  }

  mqttClient.loop();
  
}
