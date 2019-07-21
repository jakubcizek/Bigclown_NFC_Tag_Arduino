#ifndef BC_TAG_NFC_H
#define BC_TAG_NFC_H

#include <Arduino.h>
#include <Wire.h>

#define _BC_TAG_NFC_TNF_WELL_KNOWN 0x01
#define _BC_TAG_NFC_BLOCK_SIZE 16
#define BC_TAG_NFC_I2C_ADDRESS_DEFAULT 0x08
#define BC_TAG_NFC_BUFFER_SIZE 864

typedef struct{
    uint8_t _i2c_address;
} bc_tag_nfc_t;

typedef struct{
    size_t _length;
    int _last_tnf_pos;
    uint16_t _encoded_size;
    uint8_t _buffer[BC_TAG_NFC_BUFFER_SIZE];

} bc_tag_nfc_ndef_t;

bool bc_tag_nfc_init(bc_tag_nfc_t *self, uint8_t i2c_address);
bool bc_tag_nfc_memory_read(bc_tag_nfc_t *self, void *buffer, size_t length);
bool bc_tag_nfc_memory_write(bc_tag_nfc_t *self, void *buffer, size_t length);
bool bc_tag_nfc_memory_write_ndef(bc_tag_nfc_t *self, bc_tag_nfc_ndef_t *ndef);
void bc_tag_nfc_ndef_init(bc_tag_nfc_ndef_t *self);
bool bc_tag_nfc_ndef_add_text(bc_tag_nfc_ndef_t *self, const char *text, const char *encoding);
bool bc_tag_nfc_ndef_add_uri(bc_tag_nfc_ndef_t *self, const char *uri);
#endif
