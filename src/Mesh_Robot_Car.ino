/*
 * Project Mesh_Robot_Car
 * Description: Program that each Particle Mesh robot car runs
 * Author: Arduino "having11" Guy
 * Date: 7/13/2019
 */


#include "RoboCar.h"
//SYSTEM_THREAD(ENABLED);

#define UID "CAR2" //Change for each car

RoboCar car = RoboCar();

void setup() {
  Serial.begin(115200);
  waitFor(Serial.isConnected, 30000);
  waitUntil(Mesh.ready);
  Mesh.subscribe(UID, handle_data);
  Mesh.subscribe("GAME", handle_game_data);
  Serial.println("Ready");
}


void loop() {
  car.run();

}

//Must use this to pass new data

void handle_data(const char *event, const char *data){
  Serial.printlnf("Event: %s | Data: %s", event, data ? data : "NULL");
  car.new_mesh_msg(event, data);
}

void handle_game_data(const char* event, const char* data){
  car.show_congrats(data);
}
