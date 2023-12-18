//
// Created by Ewout on 14-12-2023.
//

#ifndef MOTORDRIVE_INCLUDE_COUNTER_H
#define MOTORDRIVE_INCLUDE_COUNTER_H
#include <SPI.h>

/**
 * \defgroup error_code Error Code
 * \{
 */
#define COUNTER_RETVAL  uint8_t

#define COUNTER_OK           0x00
#define COUNTER_INIT_ERROR   0xFF
/** \} */

/**
 * \defgroup register_commands Register commands
 * \{
 */
#define COUNTER_CMD_CLR               0x00
#define COUNTER_CMD_WR                0x80
#define COUNTER_CMD_LOAD              0xC0
#define COUNTER_CMD_RD                0X40
/** \} */

/**
 * \defgroup counter_click_registers Counter click registers
 * \{
 */
#define COUNTER_MDR0                  0x08
#define COUNTER_MDR1                  0x10
#define COUNTER_DTR                   0x18
#define COUNTER_CNTR                  0x20
#define COUNTER_OTR                   0x28
#define COUNTER_STR                   0x30
/** \} */

/**
 * \defgroup mdr0_count_mode_setting MDR0 count mode setting
 * \{
 */
#define COUNTER_NON_QUAD              0x00
#define COUNTER_1X_QUAD               0x01
#define COUNTER_2X_QUAD               0x02
#define COUNTER_4X_QUAD               0x03
/** \} */

/**
 * \defgroup  mdr0_count_type_setting MDR0 count type setting
 * \{
 */
#define COUNTER_FREE_RUN              0x00
#define COUNTER_SINGLE_COUNT          0x04
#define COUNTER_RANGE_LIMIT           0x08
#define COUNTER_N_MODULE              0x0C
/** \} */

/**
 * \defgroup mdr0_index_setting MDR0 index setting
 * \{
 */
#define COUNTER_INDEX_DISABLED        0x00
#define COUNTER_INDEX_LOAD_CNTR       0x10
#define COUNTER_INDEX_RESET_CNTR      0x20
#define COUNTER_INDEX_LOAD_OTR        0x30
/** \} */

/**
 * \defgroup mdr0_index_sign_setting MDR0 index sign setting
 * \{
 */
#define COUNTER_INDEX_NEG             0x00
#define COUNTER_INDEX_POS             0x40
/** \} */

/**
 * \defgroup mdr0_clock_filter_division_factor MDR0 clock filter division factor
 */
#define COUNTER_FILTER_CLOCK_DIV1     0x00
#define COUNTER_FILTER_CLOCK_DIV2     0x80
/** \} */

/**
 * \defgroup mdr1_buffer_size_setting MDR1 buffer size setting
 * \{
 */
#define COUNTER_MODE_32               0x00
#define COUNTER_MODE_24               0x01
#define COUNTER_MODE_16               0x02
#define COUNTER_MODE_8                0x03
/** \} */

/**
 * \defgroup mdr1_enable_disable_counting_setting MDR1 enable/disable counting setting
 * \{
 */
#define COUNTER_ENABLE                0x00
#define COUNTER_DISABLE               0x04

/** \} */

/**
 * \defgroup mdr1_flags_settings MDR1 flags settings
 * \{
 */
#define COUNTER_FLAG_DISABLE          0x00
#define COUNTER_FLAG_IDX              0x10
#define COUNTER_FLAG_CMP              0x20
#define COUNTER_FLAG_BW               0x40
#define COUNTER_FLAG_FY               0x80
/** \} */

/** \} */ // End group macro



class Counter {

public:
  explicit Counter(const SPIClass &spi, uint8_t cs_pin = -1);
  Counter() : Counter(SPIClass()) {};
  ~Counter();

  /**
* @brief Generic transfer function.
*
* @param wr_buf       Write data buffer
* @param wr_len       Number of byte in write data buffer
* @param rd_buf       Read data buffer
* @param rd_len       Number of byte in read data buffer
*
* @description Generic SPI transfer, for sending and receiving packages
*/
  bool generic_transfer(uint8_t *wr_buf, uint16_t wr_len, uint8_t *rd_buf, uint16_t rd_len);

  /**
  * @brief Writes data into MDR0
  *
  * @param settings     MDR0 settings.
  *
  * @note The MDR0 (Mode Register 0) is an 8-bit read/write register that sets up the
  * operating mode for the LS7366. Upon power up MDR0 is cleared to zero.
  *
  * @description This function writes data into MDR0
  */
  bool writeMdr0(uint8_t settings);

  /**
  * @brief Writes data into MDR1
  *
  * @param settings     MDR1 settings.
  *
  * @note The MDR1 (Mode Register 1) is an 8-bit read/write register which is appended
  * to MDR0 for additional modes. Upon power-up MDR1 is cleared to zero.
  *
  * @description This function writes data into MDR1
  */
  bool writeMdr1(uint8_t settings);

  /**
  * @brief Writes data into DTR
  *
  * @param buffer       Data to be written into DTR.
  *
  * @note The DTR is a software configurable 8, 16, 24 or 32-bit input data register
  * which can be written into directly from MOSI, the serial input. The DTR data
  * can be transferred into the 32-bit counter (CNTR) under program control or
  * by hardware index signal.
  *
  * @description This function writes data into DTR
  */
  bool writeDtr(uint8_t *buffer);

  /**
* @brief Loads CNTR from DTR
*
* @note The CNTR is a software configurable 8, 16, 24 or 32-bit up/down counter which
* counts the up/down pulses resulting from the quadrature clocks applied at the
* A and B inputs, or alternatively, in non-quadrature mode, pulses applied at
* the A input.
*
* @description This function loads CNTR from DTR
*/
  bool loadCntr();

  /**
 * @brief Loads OTR from CNTR
 *
 * @note The OTR is a software configuration 8, 16, 24 or 32-bit register which can be
 * read back on the MISO output. Since instantaneous CNTR value is often needed
 * to be read while the CNTR continues to count, the OTR serves as a convenient
 * dump site for instantaneous CNTR data which can then be read without
 * interfering with the counting process.
 *
 * @description This function loads OTR from CNTR
 */
  bool loadOtr();

  /**
 * @brief Reads MDR0
 *
 * @returns            Data from MDR0.
 *
 * @description This function reads MDR0
 */
  uint8_t readMdr0();

  /**
 * @brief Reads MDR1
 *
 * @returns            Data from MDR1.
 *
 * @description This function reads MDR1
 */
  uint8_t readMdr1();

  /**
 * @brief Reads OTR
 *
 * @returns            OTR register data.
 *
 * @description This function reads OTR
 */
  int32_t readOtr();

  /**
 * @brief Reads CNTR
 *
 * @returns            CNTR register data.
 *
 * @description This function reads CNTR
 */
  uint32_t readCntr();

  /**
 * @brief Reads DTR
 *
 * @returns            DTR register data.
 *
 * @description This function reads DTR
 */
  int32_t readDtr();

  /**
 * @brief Reads STR
 *
 * @returns            Status register data.
 *
 * @note
 * <pre>
 * The STR is an 8-bit status register which stores count related status
 * information.
 *
 * B0 - Sign bit. 1: negative, 0: positive
 * B1 - Count direction indicator: 0: count down, 1: count up
 * B2 - Power loss indicator latch; set upon power up
 * B3 - Count enable status: 0: counting disabled, 1: counting enabled
 * B4 - Index latch
 * B5 - Compare (CNTR = DTR) latch
 * B6 - Borrow (CNTR underflow) latch
 * B7 - Carry (CNTR overflow) latch
 * </pre>
 *
 * @description This function reads STR
 */
  uint8_t readStr();

  /**
 * @brief Clears MDR0
 *
 * @description This function clears MDR0.
 */
  bool clearMdr0();

  /**
 * @brief Clears MDR1
 *
 * @description This function clears MDR1.
 */
  bool clearMdr1();

  /**
 * @brief Clears CNTR
 *
 * @description This function clears CNTR.
 */
  bool clearCntr();

  /**
 * @brief Clears STR
 *
 * @note
 * <pre>
 * Resets all status bits except CEN and U/D. In quadrature mode, if the
 * quadrature clocks have been halted, the status bits CY, BW and CMP are not
 * affected by a “CLR STR” command under the following conditions:\n
 * 	CY: If CNTR = FFFFFFFF with status bit U/D = 1\n
 * 	BW: If CNTR = 0 with status bit U/D = 0\n
 * 	CMP: If CNTR = DTR\n
 * In non-quadrature mode the same rules apply if input A is held at
 * logic low.
 * </pre>
 *
 * @description This function clears STR
 */
  bool clearStr();

  /**
* @brief Initialisation of click board with specific settings.
*
* @param mdr0_set     Settings for mdr0.
* @param mdr1_set     Settings for mdr1.
*
* @description This function initialize of click board with specific settings.
*/
  void initAdvanced(uint8_t mdr0_set, uint8_t mdr1_set);

/**
 * @brief Enables counting ( Software mode )
 *
 * @description This function enables counting in software mode.
 */
  bool enable();

  /**
 * @brief Disables counting ( Software mode )
 *
 * @description This function disables counting in software mode.
 */
  bool disable();

  void begin();
  void end();
  void defaultCfg();

private:
  SPIClass spi;
  uint8_t cs_pin;
  SPISettings spiSettings;
  uint8_t bufferSize = 0;

  bool readData(uint8_t command, uint8_t *data_buff, uint8_t count);
  bool writeData(uint8_t command, uint8_t *data_buff, uint8_t count);
  bool writeCommand(uint8_t command);
  uint8_t readRegister(int Command);

  void startTransaction();
  void endTransaction();
};

#endif //MOTORDRIVE_INCLUDE_COUNTER_H
