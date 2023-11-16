/**
 * Capteur Générique avec une carte Arduino
 */
#include "capteur_generique_esp_now.h"

#define BOARD_ID 3 // Mettre l'identidiant du capteur ici

int getBoardId(){
  return BOARD_ID;
}
//*******************************************
const int PLUVIOMETRE = 15;
volatile unsigned int countPluviometre = 0; //variable pour l'interruption du pluviomètre pour compter les impulsions



unsigned long debutObservation = millis();   // Spour stocker le moment de la première  mesure d'une période d'obervation

float pluie = 0;
bool envoiMessage=false;
bool demarrage=true;

#define FAIBLE 2.5
#define FORTE 7.5
#define DUREE_OBSERVATION 60*60*1000
/*
 * 
 * L'unité de mesure des précipitations est le millimètre (1 mm = 1 litre d'eau / m²)
 * Pluie faible niveau inférieur à 2.5
 * Pluie modérée niveau compris en 2.5 et 7.5
 * Pluie forte niveau supérieur à 7.5
 * 
 */
String getStringNiveauPluie(float _pluie){
  String _s="Pluie ";
  if(_pluie>0&&_pluie<FAIBLE){
    _s+="faible";
  } else if(_pluie>0&&_pluie<=FORTE){
    _s+="forte";
  } else if(_pluie>0){
    _s+="modérée";
  } else {
    _s="Pas de pluie";
  }
  return _s;
}
void interruptPluviometre(){
  countPluviometre++;

}
/** Fonction setup() */
void setup() {
  /* Initialise le port série */
  Serial.begin(115200);
  /* Initialise le réseau ESP-NOW pour échanger avec le serveur */
  setup_ESP_NOW();
  pinMode(PLUVIOMETRE, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PLUVIOMETRE),interruptPluviometre,RISING) ; // CHANGE: Déclenche de HIGH à LOW ou de LOW à HIGH - FALLING : HIGH à LOW - RISING : LOW à HIGH.
}

/** Fonction loop() */
void loop() {
  if (autoPairing() == PAIR_PAIRED&&envoiMessage) {
    prepareMessage();
    addMesure(pluie,getStringNiveauPluie(pluie).c_str(),0);
    envoiMessage=false;
    sendMessage();
  } else {
    if(millis()-debutObservation>DUREE_OBSERVATION){
      pluie = countPluviometre / 4 * 0.2794; // Nombre d'impulsions multiplié par la hauteur d'eau en mm d'un godet
      countPluviometre=0;
      envoiMessage=true;
      debutObservation=millis();
    } else if(demarrage){
      demarrage=false;
      envoiMessage=true;
    }
    delay(500);
  }
  
}
 