/*
 * This intend to connect to a Wifi Access Point
 * and a rosserial socket server.
 * You can launch the rosserial socket server with
 * roslaunch rosserial_server socket.launch
 * The default port is 11411
 */
#include <ESP8266WiFi.h>
#include "WiFiLogins.h"
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <sensor_msgs/Imu.h>
#include <rosserial_msgs/RPY.h>


// Set the rosserial socket server IP address
// TIP: Use ifconfig in Linux terminal to find your computers local WiFi IP 
// WiFi looks something like this 'wlo1' then use the 'inet' ip
// TIP: Use nmap -sn 192.168.1.0/24 in Linux terminal to help debug
IPAddress server(192,168,1,167);// for BearCave Network
//IPAddress server(192,168,43,164);// for Tripp Network

// Set the rosserial socket server port
const uint16_t serverPort = 11411;

// Create Node Handler
ros::NodeHandle nh;

// Make a chatter publisher
std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);


// Make Roll Pitch Yaw Publisher
rosserial_msgs::RPY rpy_msg;
ros::Publisher rpy("rpy", &rpy_msg);

// Create Global RPY Command for use by controller
rosserial_msgs::RPY rpy_command_global;

// Make a RPY Command Callback
void rpy_command_callback(const rosserial_msgs::RPY& rpy_command){
  // Store message in global msg for availability
  rpy_command_global = rpy_command;
  char charVal[20];
  dtostrf(rpy_command.roll, 20, 4, charVal);
  str_msg.data = charVal;
  //MC.debugPWM();
};

// Create RPY Command Subscriber
ros::Subscriber<rosserial_msgs::RPY> rpy_command_sub("commands/rpy_command", &rpy_command_callback);

void setupConnection()
{
  // Use ESP8266 serial to monitor the process
  const char* ssid = BearCave.ssid;
  const char* password = BearCave.password;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect the ESP8266 the the wifi AP
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  // Set the connection to rosserial socket server
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();

  // Another way to get IP
  Serial.print("IP = ");
  Serial.println(nh.getHardware()->getLocalIP());
  
  
  // Advertise Published Topics
  nh.advertise(chatter);
  nh.advertise(rpy);

  // Subscribe to Topics
  nh.subscribe(rpy_command_sub);
}
