#include "RF69.h"

RF69::RF69(Module* module) {
  _mod = module;
}

uint8_t RF69::begin(float freq, uint32_t br) {
  // set module properties
  _mod->init(USE_SPI, INT_0);
  
  // try to find the RF69 chip
  uint8_t i = 0;
  bool flagFound = false;
  while((i < 10) && !flagFound) {
    uint8_t version = _mod->SPIreadRegister(RF69_REG_VERSION);
    if(version == 0x24) {
      flagFound = true;
    } else {
      #ifdef KITELIB_DEBUG
        Serial.print("RF69 not found! (");
        Serial.print(i + 1);
        Serial.print(" of 10 tries) RF69_REG_VERSION == ");
        
        char buffHex[5];
        sprintf(buffHex, "0x%02X", version);
        Serial.print(buffHex);
        Serial.println();
      #endif
      delay(1000);
      i++;
    }
  }
  
  if(!flagFound) {
    DEBUG_PRINTLN_STR("No RF69 found!");
    SPI.end();
    return(ERR_CHIP_NOT_FOUND);
  } else {
    DEBUG_PRINTLN_STR("Found RF69! (match by RF69_REG_VERSION == 0x24)");
  }
  
  return(config(freq, br));
}

uint8_t RF69::transmit(Packet& pack) {
  // check packet length
  if(pack.length >= 256) {
    return(ERR_PACKET_TOO_LONG);
  }
  
  // set mode to standby
  setMode(RF69_STANDBY);
  
  // set DIO pin mapping
  _mod->SPIsetRegValue(RF69_REG_DIO_MAPPING_1, RF69_DIO0_PACK_PACKET_SENT, 7, 6);
  
  // clear interrupt flags
  clearIRQFlags();
  
  // set packet length
  _mod->SPIwriteRegister(RF69_REG_FIFO, pack.length);
  
  // write packet to FIFO
  _mod->SPIwriteRegisterBurstStr(RF69_REG_FIFO, pack.source, 8);
  _mod->SPIwriteRegisterBurstStr(RF69_REG_FIFO, pack.destination, 8);
  _mod->SPIwriteRegisterBurstStr(RF69_REG_FIFO, pack.data, pack.length - 16);
  
  // set mode to transmit
  setMode(RF69_TX);
  _mod->SPIsetRegValue(RF69_REG_TEST_PA1, RF69_PA1_20_DBM);
  _mod->SPIsetRegValue(RF69_REG_TEST_PA2, RF69_PA2_20_DBM);
  
  // wait for transmission end
  while(!_mod->getInt0State());
  
  // clear interrupt flags
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t RF69::receive(Packet& pack) {
  // set mode to standby
  setMode(RF69_STANDBY);
  
  // set DIO pin mapping
  _mod->SPIsetRegValue(RF69_REG_DIO_MAPPING_1, RF69_DIO0_PACK_PAYLOAD_READY | RF69_DIO1_PACK_TIMEOUT, 7, 4);
  
  // clear interrupt flags
  clearIRQFlags();
  
  // set mode to receive
  setMode(RF69_RX);
  _mod->SPIsetRegValue(RF69_REG_TEST_PA1, RF69_PA1_NORMAL);
  _mod->SPIsetRegValue(RF69_REG_TEST_PA2, RF69_PA2_NORMAL);
  
  // wait for packet reception or timeout
  while(!_mod->getInt0State()) {
    if(_mod->getInt1State()) {
      clearIRQFlags();
      return(ERR_RX_TIMEOUT);
    }
  }
  
  // read packet length
  pack.length = _mod->SPIreadRegister(RF69_REG_FIFO);
  
  // read packet addresses
  _mod->SPIreadRegisterBurstStr(RF69_REG_FIFO, 8, pack.source);
  _mod->SPIreadRegisterBurstStr(RF69_REG_FIFO, 8, pack.destination);
  
  // read packet data
  delete[] pack.data;
  pack.data = new char[pack.length - 15];
  _mod->SPIreadRegisterBurstStr(RF69_REG_FIFO, pack.length - 16, pack.data);
  pack.data[pack.length - 16] = 0;
  
  // clear interrupt flags
  clearIRQFlags();
  
  return(ERR_NONE);
}

uint8_t RF69::sleep() {
  return(setMode(RF69_SLEEP));
}

uint8_t RF69::standby() {
  return(setMode(RF69_STANDBY));
}

uint8_t RF69::setFrequency(float freq) {
  uint8_t state = RF69::config(freq, _br);
  if(state == ERR_NONE) {
    RF69::_freq = freq;
  }
  return(state);
}

uint8_t RF69::setBitRate(uint32_t br) {
  uint8_t state = RF69::config(_freq, br);
  if(state == ERR_NONE) {
    RF69::_br = br;
  }
  return(state);
}

uint8_t RF69::config(float freq, uint32_t br) {
  uint8_t status = ERR_NONE;
  
  // check supplied settings
  if(!((freq > 290.0) && (freq < 340.0) ||
       (freq > 431.0) && (freq < 510.0) ||
       (freq > 862.0) && (freq < 1020.0))) {
    return(ERR_INVALID_FREQUENCY);
  }
  
  if((br < 1200) || (br > 300000)) {
    return(ERR_INVALID_BIT_RATE);
  }
  
  //set mode to STANDBY
  status = setMode(RF69_STANDBY);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set operation modes
  status = _mod->SPIsetRegValue(RF69_REG_OP_MODE, RF69_SEQUENCER_ON | RF69_LISTEN_OFF, 7, 6);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //enable over-current protection
  status = _mod->SPIsetRegValue(RF69_REG_OCP, RF69_OCP_ON, 4, 4);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set data mode and modulation type
  status = _mod->SPIsetRegValue(RF69_REG_DATA_MODUL, RF69_PACKET_MODE | RF69_FSK, 6, 3);
  status = _mod->SPIsetRegValue(RF69_REG_DATA_MODUL, RF69_NO_SHAPING, 1, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set bit rate
  uint16_t bitRate = 32000000 / br;
  status = _mod->SPIsetRegValue(RF69_REG_BITRATE_MSB, (bitRate & 0xFF00) >> 8, 7, 0);
  status = _mod->SPIsetRegValue(RF69_REG_BITRATE_LSB, bitRate & 0x00FF, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set allowed frequency deviation (5 kHz by default)
  status = _mod->SPIsetRegValue(RF69_REG_FDEV_MSB, RF69_FDEV_MSB, 5, 0);
  status = _mod->SPIsetRegValue(RF69_REG_FDEV_LSB, RF69_FDEV_LSB, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set carrier frequency
  uint32_t base = 1;
  uint32_t FRF = (freq * (base << 19)) / 32.0;
  status = _mod->SPIsetRegValue(RF69_REG_FRF_MSB, (FRF & 0xFF0000) >> 16, 7, 0);
  status = _mod->SPIsetRegValue(RF69_REG_FRF_MID, (FRF & 0x00FF00) >> 8, 7, 0);
  status = _mod->SPIsetRegValue(RF69_REG_FRF_LSB, FRF & 0x0000FF, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set Rx bandwidth
  status = _mod->SPIsetRegValue(RF69_REG_RX_BW, RF69_DCC_FREQ | RF69_RX_BW_MANT_16 | RF69_RX_BW_EXP, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set RSSI threshold
  status = _mod->SPIsetRegValue(RF69_REG_RSSI_THRESH, RF69_RSSI_THRESHOLD, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //reset FIFO flags
  status = _mod->SPIsetRegValue(RF69_REG_IRQ_FLAGS_2, RF69_IRQ_FIFO_OVERRUN, 4, 4);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //disable ClkOut on DIO5
  status = _mod->SPIsetRegValue(RF69_REG_DIO_MAPPING_2, RF69_CLK_OUT_OFF, 2, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set synchronization
  status = _mod->SPIsetRegValue(RF69_REG_SYNC_CONFIG, RF69_SYNC_ON | RF69_FIFO_FILL_CONDITION_SYNC | RF69_SYNC_SIZE | RF69_SYNC_TOL, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set sync word
  status = _mod->SPIsetRegValue(RF69_REG_SYNC_VALUE_1, 0x2D, 7, 0);
  status = _mod->SPIsetRegValue(RF69_REG_SYNC_VALUE_2, 100, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set packet configuration and disable encryption
  status = _mod->SPIsetRegValue(RF69_REG_PACKET_CONFIG_1, RF69_PACKET_FORMAT_VARIABLE | RF69_DC_FREE_NONE | RF69_CRC_ON | RF69_CRC_AUTOCLEAR_ON | RF69_ADDRESS_FILTERING_OFF, 7, 1);
  status = _mod->SPIsetRegValue(RF69_REG_PACKET_CONFIG_2, RF69_INTER_PACKET_RX_DELAY, 7, 4);
  status = _mod->SPIsetRegValue(RF69_REG_PACKET_CONFIG_2, RF69_AUTO_RX_RESTART_ON | RF69_AES_OFF, 1, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set payload length
  status = _mod->SPIsetRegValue(RF69_REG_PAYLOAD_LENGTH, RF69_PAYLOAD_LENGTH, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set FIFO threshold
  status = _mod->SPIsetRegValue(RF69_REG_FIFO_THRESH, RF69_TX_START_CONDITION_FIFO_NOT_EMPTY | RF69_FIFO_THRESHOLD, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set output power
  status = _mod->SPIsetRegValue(RF69_REG_PA_LEVEL, RF69_PA0_ON | RF69_PA1_OFF | RF69_PA2_OFF | RF69_OUTPUT_POWER, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //set Rx timeouts
  status = _mod->SPIsetRegValue(RF69_REG_RX_TIMEOUT_1, RF69_TIMEOUT_RX_START, 7, 0);
  status = _mod->SPIsetRegValue(RF69_REG_RX_TIMEOUT_2, RF69_TIMEOUT_RSSI_THRESH, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  //enable improved fading margin
  status = _mod->SPIsetRegValue(RF69_REG_TEST_DAGC, RF69_CONTINUOUS_DAGC_LOW_BETA_OFF, 7, 0);
  if(status != ERR_NONE) {
    return(status);
  }
  
  return(ERR_NONE);
}

uint8_t RF69::setMode(uint8_t mode) {
  _mod->SPIsetRegValue(RF69_REG_OP_MODE, mode, 4, 2);
  return(ERR_NONE);
}

void RF69::clearIRQFlags() {
  _mod->SPIwriteRegister(RF69_REG_IRQ_FLAGS_1, 0b11111111);
  _mod->SPIwriteRegister(RF69_REG_IRQ_FLAGS_2, 0b11111111);
}
