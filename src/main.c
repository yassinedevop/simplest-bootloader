/*! \file   main.c
 *  \brief  Custom Bootloader
 *  \author Seghrouchni Yassine
 *  \date   2024-08-20
 *  \copyright Copyright (C) 2024 Seghrouchni Yassine
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "stm32f4xx.h"                  // Device header
#include "HAL_GPIO_Driver.h"
#include "HAL_RCC_Driver.h"
#include "HAL_CRC_Driver.h"
#include "HAL_UART_Driver.h"
#include "HAL_Flash_Driver.h"
#include <string.h>


// Define the memory address where the main application is located
#define APP_ADDRESS 0x08004000

// Forward declaration of the functions
void Bootloader_Init(void);
void Bootloader_BlinkLED(void);
int Bootloader_CheckApp(void);
void Bootloader_JumpToApp(void);


// Reset Handler - Entry point of the bootloader

void Reset_Handler(void) {
    Bootloader_Init();

    if (Bootloader_CheckApp()) {
        Bootloader_BlinkLED();
        Bootloader_JumpToApp();
    } else {
        // Handle the error: Blinking LED continuously if no valid app is found
        while (1) {
            GPIOA->ODR ^= GPIO_ODR_OD5;  // Toggle LED
            for (volatile int j = 0; j < 1000000; j++);  // Delay
        }
    }
}

// Bootloader Initialization
void Bootloader_Init(void) {
    // Set up system clock (assuming default clock settings)
    SystemInit();

    // Optionally configure GPIOs, LEDs, etc.
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock
    GPIOA->MODER |= GPIO_MODER_MODER5_0;  // Set PA5 as output (e.g., onboard LED)
}

// Blink LED to indicate bootloader is running
void Bootloader_BlinkLED(void) {
    for (int i = 0; i < 5; i++) {
        GPIOA->ODR ^= GPIO_ODR_OD5;  // Toggle LED
        for (volatile int j = 0; j < 1000000; j++);  // Delay
    }
}

// Check if a valid application is present at the specified memory address
int Bootloader_CheckApp(void) {
    uint32_t app_reset_vector = *((volatile uint32_t *)(APP_ADDRESS + 4));
    return (app_reset_vector != 0xFFFFFFFF);  // Check if reset vector is valid
}

// Jump to the main application
void Bootloader_JumpToApp(void) {
    // Get the application's reset vector (second word in the vector table)
    uint32_t app_reset_vector = *((volatile uint32_t *)(APP_ADDRESS + 4));

    // Set the application's stack pointer (first word in the vector table)
    __set_MSP(*((volatile uint32_t *)APP_ADDRESS));

    // Jump to the application's reset vector
    ((void (*)(void))app_reset_vector)();
}

void main(){
    // Initialize the bootloader
    Bootloader_Init();

    // Check if a valid application is present
    if (Bootloader_CheckApp()) {
        // Blink the LED to indicate bootloader is running
        Bootloader_BlinkLED();

        // Jump to the main application
        Bootloader_JumpToApp();
    } else {
        // Handle the error: Blinking LED continuously if no valid app is found
        while (1) {
            GPIOA->ODR ^= GPIO_ODR_OD5;  // Toggle LED
            for (volatile int j = 0; j < 1000000; j++);  // Delay
        }
    }
}






