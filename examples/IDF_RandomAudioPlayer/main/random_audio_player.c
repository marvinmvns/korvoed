#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/i2s_std.h"

#define BUTTON_GPIO    GPIO_NUM_0
#define SD_MOUNT_POINT "/sdcard"

static i2s_chan_handle_t tx_chan;

static esp_err_t sdcard_mount(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
    };
    sdmmc_card_t *card;
    return esp_vfs_fat_sdmmc_mount(SD_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
}

static void i2s_init(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_chan, NULL));
    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_0,
            .bclk = GPIO_NUM_26,
            .ws   = GPIO_NUM_25,
            .dout = GPIO_NUM_22,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &std_cfg));
}

static void play_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("Failed to open %s\n", path);
        return;
    }
    fseek(f, 44, SEEK_SET); // skip WAV header
    uint8_t buf[1024];
    size_t r;
    ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    while ((r = fread(buf, 1, sizeof(buf), f)) > 0) {
        size_t w;
        i2s_channel_write(tx_chan, buf, r, &w, portMAX_DELAY);
    }
    fclose(f);
    i2s_channel_disable(tx_chan);
}

static char* random_audio_file(void)
{
    DIR *dir = opendir(SD_MOUNT_POINT "/audio");
    if (!dir) {
        return NULL;
    }
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) count++;
    }
    if (count == 0) {
        closedir(dir);
        return NULL;
    }
    int target = esp_random() % count;
    rewinddir(dir);
    for (int i = 0; i <= target; ++i) {
        entry = readdir(dir);
    }
    char *path = malloc(64);
    snprintf(path, 64, SD_MOUNT_POINT "/audio/%s", entry->d_name);
    closedir(dir);
    return path;
}

void app_main(void)
{
    ESP_ERROR_CHECK(sdcard_mount());
    i2s_init();
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON_GPIO);

    while (1) {
        if (gpio_get_level(BUTTON_GPIO) == 0) {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (gpio_get_level(BUTTON_GPIO) == 0) {
                char *file = random_audio_file();
                if (file) {
                    printf("Playing %s\n", file);
                    play_file(file);
                    free(file);
                }
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
