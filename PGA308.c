 /*
 * PGA308.c
 *
 * Created on: Apr 29, 2025
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


#include <PGA308.h>

/**
 * @brief Writes a 16-bit value to a single PGA308 RAM register.
 * 
 * This function assembles and sends a 4-byte UART transmission to the PGA308.
 * The protocol includes a sync byte followed by the write command and data.
 * 
 * @param PGA308_Handler Pointer to hardware configuration structure.
 * @param PGA308_Register_Addr Address of the register to write.
 * @param PGA308_Write_Data 16-bit data to write into the register.
 * @retval PGA308_Error_TypeDef Execution status.
 */
PGA308_Error_TypeDef PGA308_SingleRegister_Write(PGA308_HW_InitTypeDef *PGA308_Handler, uint8_t PGA308_Register_Addr, uint16_t PGA308_Write_Data){

	uint8_t UARTTransmitData_PGA[4];

    // UART is half-duplex: must explicitly enable transmit mode
    if(HAL_HalfDuplex_EnableTransmitter(PGA308_Handler->UARThandler) != HAL_OK) return PGA_HAL_ERROR; 

    UARTTransmitData_PGA[0] = PGA_SYNC; // Start sync byte (0x55)
    UARTTransmitData_PGA[1] = PGA_ACCESS_RAM | PGA_ACCESS_WRITE | PGA308_Register_Addr;  // Write command + RAM access + register address
	
	// Send data LSB first, then MSB (as per PGA308 protocol)
    UARTTransmitData_PGA[2] = PGA308_Write_Data & 0xFF;  // LSB
    UARTTransmitData_PGA[3] = PGA308_Write_Data >> 8; // MSB

    if(HAL_UART_Transmit(PGA308_Handler->UARThandler, UARTTransmitData_PGA, 4, PGA308_Handler->USART_Timeout) != HAL_OK) return PGA_HAL_ERROR; // Transfer via uart

    return PGA_NO_ERROR;
}

/**
 * @brief Initializes and configures PGA308 for operation.
 * 
 * This function writes the necessary registers to configure the PGA308 
 * for analog output and signal conditioning. Verifies written values via read-back.
 * 
 * @param PGA308_Handler Pointer to hardware configuration structure.
 * @param PGA308_Registers Structure containing configuration values to be written.
 * @retval PGA308_Error_TypeDef Execution status.
 */
PGA308_Error_TypeDef PGA308_Enable(PGA308_HW_InitTypeDef *PGA308_Handler, PGA308_Registers_InitTypeDef PGA308_Registers){

	uint16_t PGA_Reg_Return = 0;

	// Enter software lock mode before configuration (required by PGA308)
	PGA308_SingleRegister_Write(PGA308_Handler, PGA_SFTC_REG, PGA_SFTC_SWL_SOFTLOCK);  
	PGA308_SingleRegister_Read(PGA308_Handler, PGA_SFTC_REG, &PGA_Reg_Return);
	if(PGA_Reg_Return != PGA_SFTC_SWL_SOFTLOCK) return PGA_INV_REG_DATA_READ;

    // Write and verify each configuration register
	// ZDAC
	PGA308_SingleRegister_Write(PGA308_Handler, PGA_ZDAC_REG, PGA308_Registers.REG_ZDAC);
	PGA308_SingleRegister_Read(PGA308_Handler, PGA_ZDAC_REG, &PGA_Reg_Return);
	if(PGA_Reg_Return != PGA308_Registers.REG_ZDAC) return PGA_INV_REG_DATA_READ;

	// GDAC
	PGA308_SingleRegister_Write(PGA308_Handler, PGA_GDAC_REG, PGA308_Registers.REG_GDAC);
	PGA308_SingleRegister_Read(PGA308_Handler, PGA_GDAC_REG, &PGA_Reg_Return);
	if(PGA_Reg_Return != PGA308_Registers.REG_GDAC) return PGA_INV_REG_DATA_READ;

	// CFG0
	PGA308_SingleRegister_Write(PGA308_Handler, PGA_CFG0_REG, PGA308_Registers.REG_CFG0);
	PGA308_SingleRegister_Read(PGA308_Handler, PGA_CFG0_REG, &PGA_Reg_Return);
	if(PGA_Reg_Return != PGA308_Registers.REG_CFG0) return PGA_INV_REG_DATA_READ;

    // Enable alarms CFG1
	PGA308_SingleRegister_Write(PGA308_Handler, PGA_CFG1_REG, PGA308_Registers.REG_CFG1);
	PGA308_SingleRegister_Read(PGA308_Handler, PGA_CFG1_REG, &PGA_Reg_Return);
	if(PGA_Reg_Return != PGA308_Registers.REG_CFG1) return PGA_INV_REG_DATA_READ;

    // Enable output
	PGA308_SingleRegister_Write(PGA308_Handler, PGA_CFG2_REG, PGA308_Registers.REG_CFG2);
	PGA308_SingleRegister_Read(PGA308_Handler, PGA_CFG2_REG, &PGA_Reg_Return);
	if(PGA_Reg_Return != PGA308_Registers.REG_CFG2) return PGA_INV_REG_DATA_READ;

	return PGA_NO_ERROR;
}

/**
 * @brief Reads a 16-bit value from a single PGA308 register.
 * 
 * This function sends a read command and waits for the UART reply.
 * 
 * @param PGA308_Handler Pointer to hardware configuration structure.
 * @param PGA308_Register_Addr Address of the register to read.
 * @param PGA308_Read_Data Pointer to store the received 16-bit data.
 * @retval PGA308_Error_TypeDef Execution status.
 */
PGA308_Error_TypeDef PGA308_SingleRegister_Read(PGA308_HW_InitTypeDef *PGA308_Handler, uint8_t PGA308_Register_Addr, uint16_t *PGA308_Read_Data){
    
	uint8_t UARTTransmitData_PGA[2];
    uint8_t UARTReceiveData_PGA[2];

    UARTTransmitData_PGA[0] = PGA_SYNC;  // Start sync byte
    UARTTransmitData_PGA[1] = PGA_ACCESS_RAM | PGA_ACCESS_READ | PGA308_Register_Addr; // Read command + register address

	// Enable transmit mode to send read command
    if(HAL_HalfDuplex_EnableTransmitter(PGA308_Handler->UARThandler) != HAL_OK) return PGA_HAL_ERROR; 
	
    if(HAL_UART_Transmit(PGA308_Handler->UARThandler, UARTTransmitData_PGA, 2, PGA308_Handler->USART_Timeout) != HAL_OK) return PGA_HAL_ERROR;
	
	// Switch to receive mode
    if(HAL_HalfDuplex_EnableReceiver(PGA308_Handler->UARThandler) != HAL_OK) return PGA_HAL_ERROR; 

    if(HAL_UART_Receive(PGA308_Handler->UARThandler, UARTReceiveData_PGA, 2, PGA308_Handler->USART_Timeout) != HAL_OK) return PGA_HAL_ERROR;

	// Combine LSB and MSB
    *PGA308_Read_Data = ((((uint16_t)UARTReceiveData_PGA[1]) << 8) + UARTReceiveData_PGA[0]);

    return PGA_NO_ERROR;
}

/**
 * @brief Reads all relevant PGA308 configuration registers.
 * 
 * This function reads a group of internal RAM registers for debugging
 * or configuration verification purposes.
 * 
 * @param PGA308_Handler Pointer to hardware configuration structure.
 * @param PGA308_Registers Pointer to structure that will receive register values.
 * @retval PGA308_Error_TypeDef Always returns PGA_NO_ERROR (errors ignored internally).
 */
PGA308_Error_TypeDef PGA308_All_Registers_Read(PGA308_HW_InitTypeDef *PGA308_Handler, PGA308_Registers_InitTypeDef *PGA308_Registers){

		PGA308_SingleRegister_Read(PGA308_Handler, PGA_ZDAC_REG, &(PGA308_Registers->REG_ZDAC));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_GDAC_REG, &(PGA308_Registers->REG_GDAC));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_CFG0_REG, &(PGA308_Registers->REG_CFG0));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_CFG1_REG, &(PGA308_Registers->REG_CFG1));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_CFG2_REG, &(PGA308_Registers->REG_CFG2));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_CHKS_REG, &(PGA308_Registers->REG_CHKS));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_CHSR_REG, &(PGA308_Registers->REG_CHSR));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_SFTC_REG, &(PGA308_Registers->REG_SFTC));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_OENC_REG, &(PGA308_Registers->REG_OENC));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_ALMR_REG, &(PGA308_Registers->REG_ALMR));
		PGA308_SingleRegister_Read(PGA308_Handler, PGA_OTPS_REG, &(PGA308_Registers->REG_OTPS));

		return PGA_NO_ERROR;
}
