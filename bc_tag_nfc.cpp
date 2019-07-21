#include "bc_tag_nfc.h"

static bool _bc_tag_nfc_ndef_add_record_head(bc_tag_nfc_ndef_t *self, size_t payload_length);

bool bc_tag_nfc_init(bc_tag_nfc_t *self, uint8_t i2c_address) {
  memset(self, 0, sizeof(*self));
  self->_i2c_address = i2c_address;
  uint8_t config[_BC_TAG_NFC_BLOCK_SIZE];
  Wire.begin();

  Wire.beginTransmission(self->_i2c_address);
  Wire.write(0x00);
  Wire.endTransmission();

  size_t sz = 0;
  sz = Wire.requestFrom(self->_i2c_address, sizeof(config));
  if (sz == 0)
    return false;


  for (uint8_t i = 0; i < sz; i++) {
    config[i] = (uint8_t)Wire.read();
  }

  if ((config[12] | config[13] | config[14] | config[15]) == 0x00 ) {
    config[0] = self->_i2c_address << 1;
    config[12] = 0xE1;
    config[13] = 0x10;
    config[14] = 0x6D;
    config[15] = 0x00;
    Wire.beginTransmission(self->_i2c_address);
    Wire.write(config, sizeof(config));
    if (Wire.endTransmission() != 0) {
      return false;
    }
    delay(5);
  }
  return true;
}

bool bc_tag_nfc_memory_write(bc_tag_nfc_t *self, void *buffer, size_t length) {
  if ((length % _BC_TAG_NFC_BLOCK_SIZE != 0) || (length > BC_TAG_NFC_BUFFER_SIZE)) {
    return false;
  }
  size_t write_length = 0;
  uint8_t address = 0x01;
  uint8_t *buf;

  while ((write_length < length) && (address < 0x37)) {
    buf = (uint8_t *) buffer + write_length;
    Wire.beginTransmission(self->_i2c_address);
    Wire.write(address);
    Wire.write(buf, _BC_TAG_NFC_BLOCK_SIZE);
    if (Wire.endTransmission() != 0) {
      return false;
    }
    delay(5);
    address++;
    write_length += _BC_TAG_NFC_BLOCK_SIZE;
  }
  return true;
}

bool bc_tag_nfc_memory_write_ndef(bc_tag_nfc_t *self, bc_tag_nfc_ndef_t *ndef) {
  size_t length = ndef->_length;
  if (((length % _BC_TAG_NFC_BLOCK_SIZE) != 0)) {
    length = ((length / _BC_TAG_NFC_BLOCK_SIZE) + 1) * _BC_TAG_NFC_BLOCK_SIZE;
  }
  return bc_tag_nfc_memory_write(self, ndef->_buffer, length);
}

void bc_tag_nfc_ndef_init(bc_tag_nfc_ndef_t *self) {
  self->_buffer[0] = 0x03;
  self->_buffer[1] = 0x00;
  self->_buffer[2] = 0xFE;
  self->_length = 3;
  self->_encoded_size = 0;
  self->_last_tnf_pos = 0;
}

bool bc_tag_nfc_ndef_add_text(bc_tag_nfc_ndef_t *self, const char *text, const char *encoding) {
  size_t text_length = strlen(text);
  size_t encoding_length = strlen(encoding);
  if (!_bc_tag_nfc_ndef_add_record_head(self, encoding_length + 1 + text_length)) {
    return false;
  }
  self->_buffer[self->_length++] = 0x54; // type RTD_TEXT
  self->_buffer[self->_length++] = encoding_length;
  memcpy(self->_buffer + self->_length, encoding, encoding_length);
  self->_length += encoding_length;
  memcpy(self->_buffer + self->_length, text, text_length);
  self->_length += text_length;
  self->_buffer[self->_length++] = 0xFE; // terminator
  return true;
}

bool bc_tag_nfc_ndef_add_uri(bc_tag_nfc_ndef_t *self, const char *uri) {
  size_t uri_length = strlen(uri);
  if (!_bc_tag_nfc_ndef_add_record_head(self, 1 + uri_length)) {
    return false;
  }
  self->_buffer[self->_length++] = 0x55; // type RTD_TEXT
  self->_buffer[self->_length++] = 0;
  memcpy(self->_buffer + self->_length, uri, uri_length);
  self->_length += uri_length;
  self->_buffer[self->_length++] = 0xFE; // terminator
  return true;
}

static bool _bc_tag_nfc_ndef_add_record_head(bc_tag_nfc_ndef_t *self, size_t payload_length) {
  size_t head_length = 2 + (payload_length > 0xff ? 4 : 1) + 1; // tnf + type_length + type
  if ((head_length + payload_length > BC_TAG_NFC_BUFFER_SIZE)) {
    return false;
  }
  if (self->_last_tnf_pos != 0) {
    self->_buffer[self->_last_tnf_pos] &= ~0x40; // remove flag last record
  }
  if (self->_encoded_size == 0) {
    self->_buffer[1] = 0xff;
    self->_length = 4;
  }
  else {
    self->_length--;
  }
  self->_encoded_size += head_length + payload_length;
  self->_buffer[2] = self->_encoded_size >> 8;
  self->_buffer[3] = self->_encoded_size;
  self->_buffer[self->_length] = _BC_TAG_NFC_TNF_WELL_KNOWN | 0x40;
  if (self->_length == 4) {
    self->_buffer[self->_length] |= 0x80;
  }
  if (payload_length <= 0xff) {
    self->_buffer[self->_length] |= 0x10;
  }
  self->_last_tnf_pos = self->_length;
  self->_length++;
  self->_buffer[self->_length++] = 1; // type length
  if (payload_length <= 0xFF) { // short record{
    self->_buffer[self->_length++] = payload_length;
  }
  else {
    self->_buffer[self->_length++] = payload_length >> 24;
    self->_buffer[self->_length++] = payload_length >> 16;
    self->_buffer[self->_length++] = payload_length >> 8;
    self->_buffer[self->_length++] = payload_length & 0xff;
  }
  return true;
}
