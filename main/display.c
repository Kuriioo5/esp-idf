#include "display.h"

#include <driver/gpio.h>

#include <esp_log.h>
#include <esp_heap_caps.h>

uint16_t buffer[HEIGHT][WIDTH] = {};
spi_device_handle_t display_handle = NULL;

void display_send_cmd(uint8_t cmd)
{
    gpio_set_level(DISPLAY_DC, 0);
    spi_transaction_t t = 
    {
        .flags = SPI_TRANS_USE_TXDATA,
        .length = 8,
        .tx_data = { cmd }
    };
    spi_device_polling_transmit(display_handle, &t);
    gpio_set_level(DISPLAY_DC, 1);
}

void display_send_data(uint8_t *data, uint16_t size)
{
    gpio_set_level(DISPLAY_DC, 1);
    spi_transaction_t t = 
    {
        .length = size * 8,
        .tx_buffer = data,
    };
    spi_device_polling_transmit(display_handle, &t);
    gpio_set_level(DISPLAY_DC, 0);
}

void display_init(void)
{
    memset(buffer, 0xFF, sizeof(buffer));

    gpio_set_level(DISPLAY_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(DISPLAY_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(120));

    display_send_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));
 
    uint8_t colmod = 0x55;
    display_send_cmd(0x3A);
    display_send_data(&colmod, 1);

    display_send_cmd(0x29);
    vTaskDelay(pdMS_TO_TICKS(120));

    display_send_cmd(0x21);

    uint8_t madctl = 0xA0;
    display_send_cmd(0x36);
    display_send_data(&madctl, 1);
}

void display_write_buffer(void)
{
    uint8_t column[4] = { 0x00, 0x00, 0x01, 0x3F };
    uint8_t row[4] = { 0x00, 0x00, 0x00, 0xEF };

    display_send_cmd(0x2A);
    display_send_data(column, 4);

    display_send_cmd(0x2B);
    display_send_data(row, 4);

    display_send_cmd(0x2C);

    int total = WIDTH * HEIGHT;

    uint8_t *dma_buf = heap_caps_malloc(
        CHUNK * 2,
        MALLOC_CAP_DMA
    );

    if (!dma_buf)
        return;

    uint16_t *src = &buffer[0][0];

     for (int sent = 0; sent < total; sent += CHUNK)
    {
        int n = total - sent;

        if (n > CHUNK)
            n = CHUNK;

        for (int i = 0; i < n; i++)
        {
            uint16_t pixel = src[sent + i];

            dma_buf[i * 2]     = pixel >> 8;
            dma_buf[i * 2 + 1] = pixel & 0xFF;
        }

        spi_transaction_t t = {
            .length = n * 16,
            .tx_buffer = dma_buf
        };

        spi_device_polling_transmit(display_handle, &t);
    }

    heap_caps_free(dma_buf);
}

void display_draw(int x, 
              int y, 
              int width, 
              int height, 
              uint16_t color)
{
    for (int h = y ; h < y + height ; h++)
        for (int w = x; w < x + width ; w++)
            buffer[h][w] = color;
}
