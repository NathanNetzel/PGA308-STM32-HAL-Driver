 /*
 * PGA308.h
 *
 * Created on: April 29, 2025
 * Author: Nathan Netzel
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 Nathan Netzel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the conditions of the MIT License.
 */

#ifndef INC_PGA308_H_
#define INC_PGA308_H_


#include "stm32f3xx_hal.h"

// --- Register Addresses ---
#define PGA_ZDAC_REG    0x00  /**< Zero DAC register */
#define PGA_GDAC_REG    0x01  /**< Gain DAC register */
#define PGA_CFG0_REG    0x02  /**< Configuration register 0 */
#define PGA_CFG1_REG    0x03  /**< Configuration register 1 */
#define PGA_CFG2_REG    0x04  /**< Configuration register 2 */
#define PGA_CHKS_REG    0x05  /**< Checksum register */
#define PGA_CHSR_REG    0x06  /**< Status register */
#define PGA_SFTC_REG    0x07  /**< Software test/control register */
#define PGA_OENC_REG    0x08  /**< Output enable configuration */
#define PGA_ALMR_REG    0x09  /**< Alarm register */
#define PGA_OTPS_REG    0x0A  /**< OTP status register */

// --- Operation Mode Values ---
#define	PGA_SFTC_SWL_SOFTLOCK 0x0050  /**< Software lock enable value for PGA_SFTC_REG */

// --- Command Masks ---
#define PGA_SYNC           0x55  /**< Sync byte for UART communication */
#define PGA_ACCESS_READ    0x80  /**< Bitmask for read operation */
#define PGA_ACCESS_WRITE   0x00  /**< Bitmask for write operation */
#define PGA_ACCESS_OTP     0x40  /**< Bitmask for accessing OTP registers */
#define PGA_ACCESS_RAM     0x00  /**< Bitmask for accessing RAM registers */

// --- Error Status Enumeration ---
/**
 * @brief Status return values for PGA308 operations
 */
typedef enum {
    PGA_NO_ERROR            = 0x00U,  /**< No error */
    PGA_ERROR               = 0x01U,  /**< Generic error */
    PGA_HAL_ERROR           = 0x02U,  /**< HAL communication failure */
    PGA_INV_REG_DATA_READ   = 0x03U,  /**< Register readback mismatch */
    PGA_ID_VERIFY_ERROR     = 0x04U   /**< ID mismatch (unused in current implementation) */
} PGA308_Error_TypeDef;


// --- Hardware Interface Structure ---
/**
 * @brief Structure for initializing communication with the PGA308
 */
typedef struct {
    UART_HandleTypeDef *UARThandler; /**< Pointer to UART handler (half-duplex mode) */
    uint8_t USART_Timeout;           /**< UART timeout in milliseconds */
} PGA308_HW_InitTypeDef;

// --- Register Map Structure ---
/**
 * @brief Structure for storing PGA308 RAM register values
 */
typedef struct {
    uint16_t REG_ZDAC;  /**< Zero DAC */
    uint16_t REG_GDAC;  /**< Gain DAC */
    uint16_t REG_CFG0;  /**< Configuration 0 */
    uint16_t REG_CFG1;  /**< Configuration 1 */
    uint16_t REG_CFG2;  /**< Configuration 2 */
    uint16_t REG_CHKS;  /**< Checksum */
    uint16_t REG_CHSR;  /**< Status */
    uint16_t REG_SFTC;  /**< Software test/control */
    uint16_t REG_OENC;  /**< Output enable config */
    uint16_t REG_ALMR;  /**< Alarm */
    uint16_t REG_OTPS;  /**< OTP status */
} PGA308_Registers_InitTypeDef;

// --- Function Prototypes ---
/**
 * @brief Write a 16-bit value to a PGA308 RAM register
 */
PGA308_Error_TypeDef PGA308_SingleRegister_Write(PGA308_HW_InitTypeDef *, uint8_t, uint16_t);

/**
 * @brief Configure PGA308 registers for measurement operation
 */
PGA308_Error_TypeDef PGA308_Enable(PGA308_HW_InitTypeDef *, PGA308_Registers_InitTypeDef);

/**
 * @brief Read a 16-bit value from a PGA308 RAM register
 */
PGA308_Error_TypeDef PGA308_SingleRegister_Read(PGA308_HW_InitTypeDef *, uint8_t, uint16_t *);

/**
 * @brief Read all major PGA308 RAM registers
 */
PGA308_Error_TypeDef PGA308_All_Registers_Read(PGA308_HW_InitTypeDef *, PGA308_Registers_InitTypeDef *);

#endif /* INC_PGA308_H_ */
