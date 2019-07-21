#include "bc_tag_nfc.h"

// NFC cip
bc_tag_nfc_t nfc;
bool nfc_ok = false;
String zprava;

typedef enum {
  URI,
  PLAINTEXT
} ndef_typ_t;

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

void setup() {
  Serial.begin(115200);
  zprava.reserve(500);

  // Incializace I2C NFC cipu na adrese 0x08
  if ((nfc_ok = bc_tag_nfc_init(&nfc, 0x08)) == false) {
    Serial.println("Nepodarilo se inicializovat NFC cip :-(");
  }
  else {
    Serial.println("NFC cip v poradku inicializovan!");
  }
}

void loop() {
  while (Serial.available()) {
    zprava = Serial.readStringUntil('\n');
    zprava.trim();
    if(!nfc_ok){
      Serial.println("NNFC cip neni inicializovany!");
      break;
    }
    if (zprava[0] == 'U') {
      zprava.remove(0, 2);
      Serial.print("Vytvarim NDEF zpravu s URL: ");
      Serial.println(zprava);
      if (ulozDoNFC(zprava.c_str(), URI)) {
        Serial.println("Zapsano do NFC cipu!");
      }
      else {
        Serial.println("Nelze zapsat do NFC cipu!");
      }
    }
    else if (zprava[0] == 'T') {
      zprava.remove(0, 2);
      Serial.print("Vytvarim NDEF zpravu: ");
      Serial.println(zprava);
      if (ulozDoNFC(zprava.c_str(), PLAINTEXT)) {
        Serial.println("Zapsano do NFC cipu!");
      }
      else {
        Serial.println("Nelze zapsat do NFC cipu!");
      }
    }
  }
}
