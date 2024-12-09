#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"

#define CAMERA_MODEL_AI_THINKER // ESP32-CAM model
#include "camera_pins.h"        // Pin definitions for AI-Thinker

static const char *TAG = "camera";

// Function to initialize the camera
esp_err_t camera_init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // Frame Size: UXGA, SVGA, VGA, etc.
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10; // 0-63 (lower is better quality)
    config.fb_count = 2;      // Number of frame buffers

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }
    return ESP_OK;
}

// HTTP handler for video streaming
esp_err_t stream_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    res = httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");
    if (res != ESP_OK) return res;

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        } else {
            char *part_header = "Content-Type: image/jpeg\r\nContent-Length: %zu\r\n\r\n";
            res = httpd_resp_send_chunk(req, part_header, strlen(part_header));
            res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
            res = httpd_resp_send_chunk(req, "\r\n--frame\r\n", 10);
            esp_camera_fb_return(fb);
        }
        if (res != ESP_OK) break;
    }
    return res;
}

// Main application
void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(camera_init());

     esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &config);

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL,
    };
    httpd_register_uri_handler(server, &stream_uri);

    ESP_LOGI(TAG, "Camera ready! Stream at http://<IP>/stream");
}
