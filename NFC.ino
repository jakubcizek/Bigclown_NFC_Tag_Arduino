#include "bc_tag_nfc.h"

// NFC cip
bc_tag_nfc_t nfc;

// Je NFC cip v poradku inciializovany?
bool nfc_ok = false;

// Zprava ze seriove linky
String zprava;

// Pomocny enum pro typ NDEF zpravy (URL nebo text)
typedef enum {
  URI,
  PLAINTEXT
} ndef_typ_t;

// Funkce pro ulzoeni NDEF zpravy do EEPROM pameti NFC cipu
bool ulozDoNFC(char* txt, ndef_typ_t typ) {
  bc_tag_nfc_ndef_t zprava;
  bc_tag_nfc_ndef_init(&zprava);
  if (typ == URI) {
    bc_tag_nfc_ndef_add_uri(&zprava, txt);
  }
  else if (typ == PLAINTEXT) {
    bc_tag_nfc_ndef_add_text(&zprava, txt, "en");
  }
  if (bc_tag_nfc_memory_write_ndef(&nfc, &zprava)) {
    return true;
  }
  else {
    return false;
  }
}

// Hlavni funkce Arduina; spusti se po startu
void setup() {
  // Nastartovani seriove linky
  Serial.begin(115200);
  // Rezervace pameti pro dynamicky objekt String (vytvari se v heap)
  zprava.reserve(200);

  // Incializace BigClown NFC Tag na I2C adrese 0x08
  if ((nfc_ok = bc_tag_nfc_init(&nfc, 0x08)) == false) {
    Serial.println("Nepodarilo se inicializovat NFC cip :-(");
  }
  else {
    Serial.println("NFC cip v poradku inicializovan!");
  }
}

// Smycka loop se spusti po funkci setup a opakuje se stale dokola
void loop() {
  // Pokud na seriove lince dorazila nejaka data
  /*
   * Predpokladejme format zpravy:
   * URL: U xxxxxxxxx
   * Prosty text: T xxxxxxxxxx
  */
  while (Serial.available()) {
    // Precti je jako text az do zalomeni radku
    zprava = Serial.readStringUntil('\n');
    // Smaz ze zpravy pripadne neviditelne znaky (\r apod.)
    zprava.trim();
    
    // Pokud NFC neni pripraveny, napis do seriove linky varovani a ukonci smycku
    if(!nfc_ok){
      Serial.println("NFC cip neni inicializovany!");
      break;
    }
    
    // Pokud je prvni znak zpravy U, jedna se o WWW adresu
    if (zprava[0] == 'U') {
      // Odstran prvni dva znaky a pro kontrolu vypis do seriove lniky, co ulozim do EEPROM NFC cipu
      zprava.remove(0, 2);
      Serial.print("Vytvarim NDEF zpravu s URL: ");
      Serial.println(zprava);
      // Pokus se URL ulozit do EEPROM NFC cipu jako URI NDEF zpravu
      if (ulozDoNFC(zprava.c_str(), URI)) {
        Serial.println("Zapsano do NFC cipu!");
      }
      else {
        Serial.println("Nelze zapsat do NFC cipu!");
      }
    }
    // Pokud je prvni znak zpravy T, jedna se o plaintext zpravu
    else if (zprava[0] == 'T') {
      zprava.remove(0, 2);
      Serial.print("Vytvarim NDEF zpravu: ");
      Serial.println(zprava);
      // Pokud se prosty text ulozit do EEPROM NFC cipu jako NDEF zpravu s prostym textem
      if (ulozDoNFC(zprava.c_str(), PLAINTEXT)) {
        Serial.println("Zapsano do NFC cipu!");
      }
      else {
        Serial.println("Nelze zapsat do NFC cipu!");
      }
    }
  }
}
