#pragma once

#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/i2c.h"

/* I2C master config */
#define I2C_SDA_IO           21
#define I2C_SCL_IO           22 
#define I2C_FREQ_HZ          100000
#define I2C_TX_BUF_DISABLE   0                          
#define I2C_RX_BUF_DISABLE   0 
#define I2C_PORT_NUM         I2C_NUM_1

/* I2C common protocol defines */
#define WRITE_BIT           I2C_MASTER_WRITE              
#define READ_BIT            I2C_MASTER_READ               
#define ACK_CHECK_EN        0x1                        
#define ACK_CHECK_DIS       0x0                       
#define ACK_VAL             0x0                            
#define NACK_VAL            0x1                 

/* TCS34725 color sensor defines */
#define TCS_ADDR            0x29 /* Slave address */
#define TCS_ID              0x12 /* Read ID of IC, 0x44 = TCS34721/TCS34725*/
#define TCS_STATUS          0x13 /* Device status */
#define TCS_COMMAND_BIT     0x80 /* Command Bit (for sending commands to the sensor) */

#define TCS_ENABLE          0x00 /* Interrupt enable register */
#define TCS_ENABLE_PON      0x01 /* Power on - Writing 1 activates the internal oscillator, 0 disables it */
#define TCS_ENABLE_AEN      0x02 /* RGBC Enable - Writing 1 actives the ADC, 0 disables it */
#define TCS_ENABLE_WEN      0x08 /* Wait Enable - Writing 1 activates the wait timer */
#define TCS_ENABLE_AIEN     0x10 /* RGBC Interrupt Enable */

#define TCS_CONTROL         0x0F /* Control Bit (Gain adjust) */
#define TCS_ATIME           0x01 /* Integration time */

#define TCS_WTIME           0x03 /* Wait time (if TCS_ENABLE_WEN is asserted) */
#define TCS_WTIME_2_4MS     0xFF /* WLONG0 = 2.4ms   WLONG1 = 0.029s */
#define TCS_WTIME_204MS     0xAB /* WLONG0 = 204ms   WLONG1 = 2.45s  */
#define TCS_WTIME_614MS     0x00 /* WLONG0 = 614ms   WLONG1 = 7.4s   */

/* TCS34725 color registers */
#define TCS_CDATAL          0x14 /* clear */
#define TCS_RDATAL          0x16 /* red */
#define TCS_GDATAL          0x18 /* green */
#define TCS_BDATAL          0x1A /* blue */

/* TCS34725 gain settings */
typedef enum {
  TCS_GAIN_1X = 0x00,  /*  No gain  */
  TCS_GAIN_4X = 0x01,  /*  4x gain  */
  TCS_GAIN_16X = 0x02, /*  16x gain */
  TCS_GAIN_60X = 0x03  /*  60x gain */
} tcs_gain_t;

/* TCS34725 integration time settings
Increasing the integration time improves the sensitivity at the cost of time */
typedef enum {
  TCS_INT_TIME_2_4MS = 0xFF, /* <2.4ms */
  TCS_INT_TIME_24MS = 0xF6, /* <24ms */
  TCS_INT_TIME_50MS = 0xEB, /* <50ms */
  TCS_INT_TIME_101MS = 0xD5, /* <101ms */
  TCS_INT_TIME_154MS = 0xC0, /* <154ms */
  TCS_INT_TIME_700MS = 0x00 /* <700ms */
} tcs_int_t;


esp_err_t i2c_init(void);

esp_err_t tcs_init(void);

esp_err_t wr_tcs(uint8_t reg_addr, uint8_t* data_wr, size_t size);

esp_err_t rd_tcs(uint8_t reg_addr, uint8_t* data_wr, size_t size);

// uint16_t rd_16_tcs(uint8_t reg_addr);

void get_color(float r, float g, float b);