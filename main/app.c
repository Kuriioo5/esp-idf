#include <freertos/FreeRTOS.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include "display.h"

void app_main(void)
{
    gpio_config_t gpio = {
        .pin_bit_mask = (1ULL << DISPLAY_DC) | (1ULL << DISPLAY_RST),
        .mode = GPIO_MODE_OUTPUT
    };
    gpio_config(&gpio);

    gpio_set_direction(15, GPIO_MODE_INPUT);

    spi_bus_config_t bus_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = DISPLAY_DIN,
        .sclk_io_num = DISPLAY_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = CHUNK * 2
    };

    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t device_cfg = {
        .clock_speed_hz = 40 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = DISPLAY_CS,
        .queue_size = 3,
        .flags = SPI_DEVICE_HALFDUPLEX
    };

    esp_err_t ret = spi_bus_add_device(SPI2_HOST, &device_cfg, &display_handle);
    if (ret != ESP_OK) 
    {
        ESP_LOGE("SPI", "spi_bus_add_device failed: %s", esp_err_to_name(ret));
        return;
    }

    display_init();

    int selected = 0;
    while (1)
    {
        display_draw(0, 0, WIDTH, HEIGHT, 0x0000);
        //display_draw(0, 0, WIDTH, 100, 0xF800);
        //display_draw(0, 100, WIDTH, 100, 0x001F);
        display_draw_text("Patrick Mahomes est le meilleur joueur de foot americain", 0, 0, 0xF800);
        display_draw_text("Infiniment meilleur que le nullos de tom brindille", 0, 10, 0xF800);

        display_write_buffer(); 
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
