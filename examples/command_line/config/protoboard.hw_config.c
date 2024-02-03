
/* hw_config.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/

/*

This file should be tailored to match the hardware design.

There should be one element of the spi[] array for each RP2040 hardware SPI used.

There should be one element of the spi_ifs[] array for each SPI interface object.
* Each element of spi_ifs[] must point to an spi_t instance with member "spi".

There should be one element of the sdio_ifs[] array for each SDIO interface object.

There should be one element of the sd_cards[] array for each SD card slot.
* Each element of sd_cards[] must point to its interface with spi_if_p or sdio_if_p.
*/

/* Hardware configuration for Pico SD Card Development Board
See https://oshwlab.com/carlk3/rp2040-sd-card-dev

See https://docs.google.com/spreadsheets/d/1BrzLWTyifongf_VQCc2IpJqXWtsrjmG7KnIbSBy-CPU/edit?usp=sharing,
tab "Monster", for pin assignments assumed in this configuration file.
*/

//
#include "hw_config.h"
#include "my_debug.h"

// Hardware Configuration of SPI "object"
static spi_t spi  = {  // One for each RP2040 SPI component used
    .hw_inst = spi0,  // SPI component
    .sck_gpio = 2,  // GPIO number (not Pico pin number)
    .mosi_gpio = 3,
    .miso_gpio = 4,
    .set_drive_strength = true,
    .mosi_gpio_drive_strength = GPIO_DRIVE_STRENGTH_2MA,
    .sck_gpio_drive_strength = GPIO_DRIVE_STRENGTH_12MA,
    .no_miso_gpio_pull_up = true,
    // .baud_rate = 125 * 1000 * 1000 / 8  // 15625000 Hz
    .baud_rate = 125 * 1000 * 1000 / 6  // 20833333 Hz
    //.baud_rate = 125 * 1000 * 1000 / 4  // 31250000 Hz
};

/* SPI Interface */
static sd_spi_if_t spi_if = {
        .spi = &spi,  // Pointer to the SPI driving this card
        .ss_gpio = 7,     // The SPI slave select GPIO for this SD card
        .set_drive_strength = true,
        .ss_gpio_drive_strength = GPIO_DRIVE_STRENGTH_2MA
};

/* SDIO Interfaces */
/*
Pins CLK_gpio, D1_gpio, D2_gpio, and D3_gpio are at offsets from pin D0_gpio.
The offsets are determined by sd_driver\SDIO\rp2040_sdio.pio.
    CLK_gpio = (D0_gpio + SDIO_CLK_PIN_D0_OFFSET) % 32;
    As of this writing, SDIO_CLK_PIN_D0_OFFSET is 30,
        which is -2 in mod32 arithmetic, so:
    CLK_gpio = D0_gpio -2.
    D1_gpio = D0_gpio + 1;
    D2_gpio = D0_gpio + 2;
    D3_gpio = D0_gpio + 3;
*/
static sd_sdio_if_t sdio_ifs[] = {
    {   // sdio_ifs[0]
        .CMD_gpio = 3,
        .D0_gpio = 4,
        .SDIO_PIO = pio0,
        .DMA_IRQ_num = DMA_IRQ_0,
        .baud_rate = 125 * 1000 * 1000 / 5  // 25 MHz
    },
    {   // sdio_ifs[1]
        .CMD_gpio = 17,
        .D0_gpio = 18,
        .SDIO_PIO = pio1,
        .DMA_IRQ_num = DMA_IRQ_1,
        .baud_rate = 125 * 1000 * 1000 / 5  // 25 MHz
        //.baud_rate = 125 * 1000 * 1000 / 4
    }
};

/* Hardware Configuration of the SD Card "objects"
    These correspond to SD card sockets
*/
static sd_card_t sd_cards[] = {  // One for each SD card
#if SPI_SD0
    {
        .device_name = "sd0",
        .mount_point = "/sd0",
        .type = SD_IF_SPI,
        .spi_if_p = &spi_if,  // Pointer to the SPI interface driving this card

        // SD Card detect:
        .use_card_detect = true,
        .card_detect_gpio = 9,  
        .card_detected_true = 0, // What the GPIO read returns when a card is present.
    },
#else
    {   // sd_cards[0]
        // "device_name" is arbitrary:
        .device_name = "sd0", 
		// "mount_point" must be a directory off the file system's root directory and must be an absolute path:
        .mount_point = "/sd0",
        .type = SD_IF_SDIO,
        .sdio_if_p = &sdio_ifs[0],  // Pointer to the SPI interface driving this card
        // SD Card detect:
        .use_card_detect = true,
        .card_detect_gpio = 9,  
        .card_detected_true = 0, // What the GPIO read returns when a card is
                                 // present.
        .card_detect_use_pull = true,
        .card_detect_pull_hi = true                                 
    },
#endif
    {   // sd_cards[1]
        // "device_name" is arbitrary:
        .device_name = "sd3", 
        // "mount_point" must be a directory off the file system's root directory and must be an absolute path:
		.mount_point = "/sd3",
        .type = SD_IF_SDIO,
        .sdio_if_p = &sdio_ifs[1],
        // SD Card detect:
        .use_card_detect = true,
        .card_detect_gpio = 22,  
        .card_detected_true = 1, // What the GPIO read returns when a card is
                                 // present.
        .card_detect_use_pull = true,
        .card_detect_pull_hi = true
    }
};

/* ********************************************************************** */

size_t sd_get_num() { return count_of(sd_cards); }

sd_card_t *sd_get_by_num(size_t num) {
    myASSERT(num < sd_get_num());
    if (num < sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}

/* [] END OF FILE */