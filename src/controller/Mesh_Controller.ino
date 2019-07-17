/*
 * Project Mesh_Controller
 * Description: Controller prograam that runs on Argon. Handles communication between clients and Xenon-powered robot cars
 * Author: Arduino "having11" Guy
 * Date: 7/13/2019
 */

#define WEBDUINO_SERIAL_DEBUGGING 1
#include "WebServer.h"

#define PREFIX "/cars"
WebServer webserver(PREFIX, 80);

struct car_life
{
  bool car1 = true;
  const char* car1_uid = "CAR1";
  bool car2 = true;
  const char* car2_uid = "CAR2";
  bool car3 = true;
  const char* car3_uid = "CAR3";
  uint8_t num_alive = 3;
} cars_alive;

void carCmd(WebServer &server, WebServer::ConnectionType type, char*, bool){
  if(type == WebServer::POST){
    bool repeat;
    char uid_name[4], uid[8], command_name[10], command[20], value_name[10], value[48];
    server.readPOSTparam(uid_name, 4, uid, 8);
    server.readPOSTparam(command_name, 10, command, 20);
    server.readPOSTparam(value_name, 10, value, 48);
    Serial.printlnf("UID: %s, command: %s, value: %s", uid, command, value);
    String dir_val;
    String data;
    if(!strcmp(command, "drive")){
      if(!strcmp(value, "forward")) dir_val = "01500150";
      if(!strcmp(value, "right")) dir_val = "0100-100";
      if(!strcmp(value, "back")) dir_val = "-150-150";
      if(!strcmp(value, "left")) dir_val = "-1000100";
      data = "02";
      data.concat(dir_val);
    }
    if(!strcmp(command, "name")){
      data = "01";
      data.concat(value);
    }
    if(!strcmp(command, "start")){
      data = "03";
    }
    if(!strcmp(command, "end")){
      data = "04";
    }
    if(!strcmp(command, "laser")){
      data = "05";
      data.concat(value);
    }
    if(!strcmp(command, "servo")){
      data = "06";
      data.concat(value); //Needs to have 3 digits per servo deg ex: 090170
    }
    if(!strcmp(command, "free")){
      data = "07";
    }
    if(!strcmp(command, "color")){
      data = "08";
      data.concat(value);
    }
    Mesh.publish(uid, data.c_str());
    //Particle.publish(uid, data.c_str());
    server.httpSeeOther(PREFIX);
    return;
  }
  server.httpSuccess();
    if(type == WebServer::GET){
      P(message) = 
      //HTML here
      "<html><body>hi</body></html>"

      ;
      server.printP(message);
    }
}

void handle_out(const char* event, const char* data){
  cars_alive.num_alive --;
  if(data == cars_alive.car1_uid){
    cars_alive.car1 = false;
  }
  if(data == cars_alive.car2_uid){
    cars_alive.car2 = false;
  }
  if(data == cars_alive.car3_uid){
    cars_alive.car3 = false;
  }
  if(cars_alive.num_alive==1){
    const char* used_uid;
    if(cars_alive.car1){
      used_uid = cars_alive.car1_uid;
    }
    else if(cars_alive.car2){
      used_uid = cars_alive.car2_uid;
    }
    Mesh.publish("GAME", used_uid);
    cars_alive.num_alive = 3;
    cars_alive.car1 = true;
    cars_alive.car2 = true;
    cars_alive.car3 = true;
  }
}

void setup() {
  Serial.begin(115200);
  waitFor(Serial.isConnected, 30000);
  Serial.println(WiFi.localIP());
  webserver.setDefaultCommand(&carCmd);
  webserver.begin();
  Mesh.subscribe("OUT", handle_out);

}


void loop() {
  webserver.processConnection();

}