# BigClown NFC Tag (Arduino)

Bigclown NFC Tag: https://shop.bigclown.com/nfc-tag/

Prepis zakladnich funkci pro ulozeni NDEF zpravy do EEPROM pameti NFC cipu z BigClown SDK do Arduina. NFC tag komunikuje na I2C (3,3V) a prepis se tyka jen prace s I2C pomoci knihovny Wire. Vse ostatni zustalo as is.

Puvodni zdrojove kody pro BigClown:

bc_tag_nfc.c: https://github.com/bigclownlabs/bcf-sdk/blob/master/bcl/src/bc_tag_nfc.c

bc_tag_nfc.h: https://github.com/bigclownlabs/bcf-sdk/blob/master/bcl/inc/bc_tag_nfc.h

This project is licensed under the MIT License
