# PGA308 STM32 HAL Driver

C driver library for the **PGA308** programmable analog signal conditioner from Texas Instruments.  
Designed for STM32 microcontrollers using the **STM32 HAL library** and **UART half-duplex interface**.

---

## **Features**

- Write to internal RAM configuration registers  
- Read back PGA308 register values  
- Enable analog output and alarm configuration  
- Verify proper register configuration via read-back  
- UART half-duplex support for TX/RX switching  
- Basic error handling with return status codes

---

## **Requirements**

- STM32 MCU with STM32 HAL library  
- UART peripheral configured in **half-duplex mode**  
- PGA308 sensor connected according to TI guidelines  
- Familiarity with embedded C and STM32CubeIDE (or similar)  

---

**Note:**  
This driver includes `#include "stm32f3xx_hal.h"` in the header file and is currently configured for the STM32F3 family.  
To use it with a different STM32 family, update the HAL include and USART settings accordingly.

---

## **Quick Start**

1. **Add driver files to your project**

Include `PGA308.c` and `PGA308.h` in your STM32 project source folder.

2. **Configure UART in half-duplex mode**

In `CubeMX`, enable UART in **half-duplex** mode and generate the code.  
Make sure to properly configure the TX pin used for communication with the PGA308.

3. **Initialize driver handle structure**

```c
PGA308_HW_InitTypeDef PGA308_Handle = {
    .UARThandler = &huart1,              // Your UART handle
    .USART_Timeout = 100                 // Timeout in milliseconds
};
```

4. **Set desired register values**

```c
PGA308_Registers_InitTypeDef PGA308_Registers = {
    .REG_ZDAC = 0x1234,
    .REG_GDAC = 0x5678,
    .REG_CFG0 = 0x00C3,
    .REG_CFG1 = 0x0080,
    .REG_CFG2 = 0x0002
};
```

5. **Enable PGA308 with configuration**

```c
if (PGA308_Enable(&PGA308_Handle, PGA308_Registers) != PGA_NO_ERROR) {
    // Handle error (e.g., invalid read-back)
}
```

6. **Read all registers (optional)**

```c
PGA308_Registers_InitTypeDef PGA_Readback;
PGA308_All_Registers_Read(&PGA308_Handle, &PGA_Readback);
// Use the values for validation or debugging
```

---

## **API Overview**

* `PGA308_SingleRegister_Write()` — Write to a single RAM register  
* `PGA308_SingleRegister_Read()` — Read from a single RAM register  
* `PGA308_Enable()` — Configure and enable analog output  
* `PGA308_All_Registers_Read()` — Read full register map into a structure  

---

## **License**

This project is licensed under the MIT License. See the `LICENSE` file for details.

---

## **Author**

Nathan Netzel  
Electrical Engineering Student — State University of Londrina (UEL)
