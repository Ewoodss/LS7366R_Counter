//
// Created by Ewout on 14-12-2023.
//

#include "LS7366R_Counter.h"
#include "Arduino.h"

Counter::Counter(const SPIClass &spi, uint8_t cs_pin) : spi(spi), cs_pin(cs_pin) {
  spiSettings = SPISettings(100000, SPI_MSBFIRST, SPI_MODE0);
}

void Counter::begin() {
  spi.begin();
  if (this->cs_pin == (uint8_t)-1)  // if no pin is specified, use the default SS pin
	this->cs_pin = this->spi.pinSS();

  pinMode(this->cs_pin, OUTPUT);
  digitalWrite(this->cs_pin, HIGH);
}

void Counter::end() {
  //disable counter
  this->clearCntr();
  this->disable();
  spi.end();
  //reset pin to default state
  pinMode(this->cs_pin, INPUT);
}


void Counter::defaultCfg() {
  uint8_t tmp[4] = {0};
  initAdvanced(COUNTER_4X_QUAD | COUNTER_FREE_RUN | COUNTER_INDEX_DISABLED | COUNTER_FILTER_CLOCK_DIV1,
			   COUNTER_MODE_32 | COUNTER_ENABLE | COUNTER_FLAG_DISABLE );

  this->writeDtr(tmp);
  this->clearCntr();
  this->loadCntr();
  this->enable();
}

bool Counter::generic_transfer(uint8_t *wr_buf, uint16_t wr_len, uint8_t *rd_buf, uint16_t rd_len) {
  if (wr_buf == nullptr || rd_buf == nullptr)
	return false;

  memset(rd_buf, 0, rd_len);

  this->startTransaction();

  for (int i = 0; i < wr_len; i++) {
	spi.transfer(wr_buf[i]);
  }

  for (int i = 0; i < rd_len; i++) {
	rd_buf[i] = spi.transfer(0x00);
  }

  this->endTransaction();
  return true;
}

bool Counter::writeMdr0(uint8_t settings) {
  return this->writeData(COUNTER_CMD_WR | COUNTER_MDR0, &settings, 1);
}

bool Counter::writeMdr1(uint8_t settings) {
  return this->writeData(COUNTER_CMD_WR | COUNTER_MDR1, &settings, 1);
}
bool Counter::writeDtr(uint8_t *buffer) {
  return this->writeData(COUNTER_CMD_WR | COUNTER_DTR, buffer, this->bufferSize);
}

bool Counter::loadCntr() {
  return this->writeCommand(COUNTER_CMD_LOAD | COUNTER_CNTR);
}
bool Counter::loadOtr() {
  return this->writeCommand(COUNTER_CMD_LOAD | COUNTER_OTR);
}

uint8_t Counter::readMdr0() {
  uint8_t result;
  result = this->readRegister(COUNTER_CMD_RD | COUNTER_MDR0);
  return result;
}

uint8_t Counter::readMdr1() {
  uint8_t result;
  result = this->readRegister(COUNTER_CMD_RD | COUNTER_MDR0);
  return result;
}

int32_t Counter::readOtr() {
  uint8_t data_buf[4];
  uint32_t result;

  this->readData(COUNTER_CMD_RD | COUNTER_OTR, data_buf, this->bufferSize);

  result = data_buf[0];

  for (uint8_t cnt = 1; cnt < this->bufferSize; cnt++) {
	result = result << 8;
	result |= data_buf[cnt];
  }

  return (int32_t)result;
}

uint32_t Counter::readCntr() {
  uint8_t data_buf[4];
  uint32_t result;

  this->readData(COUNTER_CMD_RD | COUNTER_CNTR, data_buf, this->bufferSize);

  result = data_buf[0];

  for (uint8_t cnt = 1; cnt < this->bufferSize; cnt++) {
	result = result << 8;
	result |= data_buf[cnt];
  }

  return result;
}

int32_t Counter::readDtr() {
  uint8_t data_buf[4];
  uint32_t result;

  this->readData(COUNTER_CMD_RD | COUNTER_DTR, data_buf, this->bufferSize);

  result = data_buf[0];

  for (uint8_t cnt = 1; cnt < this->bufferSize; cnt++) {
	result = result << 8;
	result |= data_buf[cnt];
  }

  return (int32_t)result;
}

uint8_t Counter::readStr() {
  uint8_t result;

  result = this->readRegister(COUNTER_CMD_RD | COUNTER_STR);

  return result;
}

bool Counter::clearMdr0() {
  return this->writeMdr0(COUNTER_CMD_CLR | COUNTER_MDR0);
}
bool Counter::clearMdr1() {
  return this->writeMdr1(COUNTER_CMD_CLR | COUNTER_MDR1);
}
bool Counter::clearCntr() {
  return this->writeCommand(COUNTER_CMD_CLR | COUNTER_CNTR);
}
bool Counter::clearStr() {
  return this->writeCommand(COUNTER_CMD_CLR | COUNTER_STR);
}

void Counter::initAdvanced(uint8_t mdr0_set, uint8_t mdr1_set) {
  writeMdr0(mdr0_set);
  writeMdr1(mdr1_set);

  switch (mdr1_set & 0x03) {
  case 0 : {
	this->bufferSize = 4;
	break;
  }
  case 1 : {
	this->bufferSize = 3;
	break;
  }
  case 2 : {
	this->bufferSize = 2;
	break;
  }
  case 3 : {
	this->bufferSize = 1;
	break;
  }
  }
}

bool Counter::enable() {
  uint8_t mdr1_temp = 0;

  mdr1_temp = this->readMdr1() & 0xFB;
  return this->writeMdr1(mdr1_temp);
}

bool Counter::disable() {
  uint8_t mdr1_temp = 0;

  mdr1_temp = this->readMdr1() | 0x04;
  return this->writeMdr1(mdr1_temp);
}

bool Counter::writeCommand(uint8_t command) {
  this->startTransaction();
  spi.transfer(&command, 1);
  this->endTransaction();
  return true;
}

uint8_t Counter::readRegister(int command) {
  uint8_t tx_buff[1];
  uint8_t rx_buff[1];

  tx_buff[0] = command;

  this->generic_transfer(tx_buff, 1, rx_buff, 1);

  return rx_buff[0];
}

bool Counter::writeData(uint8_t command, uint8_t *data_buff, uint8_t count) {
  uint8_t cmnd[1];
  uint8_t temp[4];

  cmnd[0] = command;
  memcpy(temp, data_buff, count);

  this->startTransaction();
  spi.transfer(&cmnd, 1);
  spi.transfer(&temp, count);
  this->endTransaction();

  return true;
}

bool Counter::readData(uint8_t command, uint8_t *data_buff, uint8_t count) {
  uint8_t tx_buff[1];
  uint8_t rx_buff[4];

  tx_buff[0] = command;

  auto result = generic_transfer(tx_buff, 1, rx_buff, count);

  for (uint8_t cnt = 0; cnt < count; cnt++) {
	data_buff[cnt] = rx_buff[cnt];
  }
  return result;
}

void Counter::startTransaction() {
  digitalWrite(this->cs_pin, LOW);
  spi.beginTransaction(spiSettings);
}

void Counter::endTransaction() {
  digitalWrite(this->cs_pin, HIGH);
  spi.endTransaction();
}

Counter::~Counter() {
  this->end();
}

