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
Hardware configuration for Adafruit Feather RP2040 with Adalogger FeatherWing

This configuration matches the EXACT Arduino pin layout from pins_arduino.h:
- SPI0 interface (__SPI1_DEVICE spi0)
- CLK: GPIO 18 (PIN_SD_CLK)
- MOSI: GPIO 19 (PIN_SD_CMD_MOSI)
- MISO: GPIO 20 (PIN_SD_DAT0_MISO)  
- CS: GPIO 23 (PIN_SD_DAT3_CS)
- Card Detect: GPIO 16 (PIN_CARD_DETECT)
*/

#include <assert.h>
//
#include "hw_config.h"

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects (or "chip selects").
static spi_t spis[] = {  // One for each RP2040 SPI component used
    {   // spis[0]: SPI0 for Adafruit RP2040 Adalogger (EXACT Arduino configuration)
        .hw_inst = spi0,  // RP2040 SPI component (spi0 like Arduino __SPI1_DEVICE)
        .sck_gpio = 18,   // SCK pin (PIN_SD_CLK)
        .mosi_gpio = 19,  // MOSI pin (PIN_SD_CMD_MOSI) 
        .miso_gpio = 20,  // MISO pin (PIN_SD_DAT0_MISO)
        .set_drive_strength = true,
        .mosi_gpio_drive_strength = GPIO_DRIVE_STRENGTH_4MA,  // MOSI
        .sck_gpio_drive_strength = GPIO_DRIVE_STRENGTH_8MA,   // SCK (clock needs good edges)
        .no_miso_gpio_pull_up = true,  // Standard configuration
        .DMA_IRQ_num = DMA_IRQ_0,
        .baud_rate = 125 * 1000 * 1000 / 8  // 15625000 Hz
    }
};

/* SPI Interfaces */
static sd_spi_if_t spi_ifs[] = {
    {   // spi_ifs[0]: SD card interface for Adafruit RP2040 Adalogger
        .spi = &spis[0],  // Pointer to the SPI driving this card
        .ss_gpio = 23,    // CS pin (PIN_SD_DAT3_CS - EXACT Arduino configuration)
        .set_drive_strength = true,
        .ss_gpio_drive_strength = GPIO_DRIVE_STRENGTH_4MA
    }
};

/* Hardware Configuration of the SD Card "objects" */
static sd_card_t sd_cards[] = {  // One for each SD card
    {   // sd_cards[0]: Socket sd0 - Adafruit RP2040 Adalogger
        .device_name = "sd0", 
        .mount_point = "/sd0",
        .type = SD_IF_SPI,
        .spi_if_p = &spi_ifs[0],  // Pointer to the SPI interface driving this card
        // SD Card detect: ENABLE with proper debouncing
        .use_card_detect = true,
        .card_detect_gpio = 16,   // PIN_CARD_DETECT from Arduino (GPIO 16)
        .card_detected_true = 1,  // Active high: 1 = card present, 0 = card absent
        .card_detect_use_pull = true,
        .card_detect_pull_hi = true  // Pull-up resistor for stable detection                          
    }
};

/* ********************************************************************** */

size_t sd_get_num() { return count_of(sd_cards); }

sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}

size_t spi_get_num() { return count_of(spis); }

spi_t *spi_get_by_num(size_t num) {
    if (num <= spi_get_num()) {
        return &spis[num];
    } else {
        return NULL;
    }
}

/* [] END OF FILE */