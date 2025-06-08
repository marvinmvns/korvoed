#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_board_init.h"
#include "esp_log.h"
#include "esp_wn_iface.h"
#include "esp_wn_models.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"

#define MOUNT_POINT "/sdcard"
static const char *TAG = "wake_player";

static esp_err_t sdcard_init(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
    };
    sdmmc_card_t *card;
    return esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
}

static char *find_random_wav(void)
{
    DIR *dir = opendir(MOUNT_POINT"/audio");
    if (!dir) return NULL;
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
    for (int i = 0; i <= target; i++) {
        entry = readdir(dir);
    }
    char *path = malloc(64);
    snprintf(path, 64, MOUNT_POINT"/audio/%s", entry->d_name);
    closedir(dir);
    return path;
}

static void play_wav(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open %s", path);
        return;
    }
    fseek(f, 44, SEEK_SET);
    int16_t buf[512];
    size_t r;
    while ((r = fread(buf, sizeof(int16_t), 512, f)) > 0) {
        esp_audio_play(buf, r, portMAX_DELAY);
    }
    fclose(f);
}

void app_main(void)
{
    ESP_ERROR_CHECK(sdcard_init());
    ESP_ERROR_CHECK(esp_board_init(16000, 1, 16));

    srmodel_list_t *models = esp_srmodel_init("model");
    char *model_name = esp_srmodel_filter(models, ESP_WN_PREFIX, "hiesp");
    esp_wn_iface_t *wakenet = (esp_wn_iface_t *)esp_wn_handle_from_name(model_name);
    model_iface_data_t *model = wakenet->create(model_name, DET_MODE_90);
    int chunk = wakenet->get_samp_chunksize(model) * sizeof(int16_t);
    int16_t *audio = malloc(chunk);

    while (1) {
        esp_get_feed_data(true, audio, chunk);
        if (wakenet->detect(model, audio) == WAKENET_DETECTED) {
            ESP_LOGI(TAG, "Wake word detected");
            char *file = find_random_wav();
            if (file) {
                ESP_LOGI(TAG, "Playing %s", file);
                play_wav(file);
                free(file);
            }
        }
    }
}
