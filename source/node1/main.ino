#include <pt.h>
#include <RF24Network.h>
#include <RF24.h>

RF24 radio(7, 8);               //Cria um objeto RF24 chamado radio, 7 e 8 são os pinos CE e CSN
RF24Network network(radio);      //Cria uma network e adiciona o objeto radio na mesma

const uint16_t self = 00; //Endereço deste nodo em octal
const uint16_t next_node = 01; //Endereço em octal do nodo que receberá a mensagem

int indicador = 0;
unsigned long incomingData0 ;
unsigned long incomingData1 ;

unsigned long test0 = 111 ;
unsigned long test1 = 222 ;


void setup() {
  Serial.begin(500000);
  radio.begin();
  network.begin(90, self);  //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MIN);
}

void ouvindo_thread1(int interval) {
  static long next_time = 0; 
  if(millis()<next_time) return;
  next_time += interval;

  Serial.print("Thread 1 - ouvindo \n");
  network.update();
  while ( network.available() ){
    if (indicador == 0){
      RF24NetworkHeader headerIn;   
      network.read(headerIn, &incomingData0, sizeof(incomingData0));
      indicador = 1;
      Serial.println("Recebido: self = "+String(self)+String(incomingData0));
    }else{
      RF24NetworkHeader headerIn;   
      network.read(headerIn, &incomingData1, sizeof(incomingData1));
      indicador = 0;
      Serial.println("Recebido: self = "+String(self)+String(incomingData1));

    }       
  }
}

void enviando_thread2(int interval) {
  static long next_time = 0;
  if(millis()<next_time) return;
  next_time += interval;

  network.update();
  RF24NetworkHeader headerOut(next_node); // (Endereço de envio)
  if (indicador == 0){
      Serial.print("Thread 2 - enviando \n");
      if(sizeof(test1) > 0){
        network.write(headerOut, &test1, sizeof(incomingData1));
      }
      indicador = 1;
   }else{
      Serial.print("Thread 2 - enviando \n");
      if(sizeof(test0) > 0){
        network.write(headerOut, &test0, sizeof(incomingData0));
      }
   }      
}

void loop() {
ouvindo_thread1(100);
enviando_thread2(100);
}