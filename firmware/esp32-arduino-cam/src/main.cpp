#include "esp_camera.h"
#include <WiFi.h>
#include "credentials.h"
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h" // Ensure this file is correctly set up for your camera model

// ===========================
// WiFi Credentials
// ===========================
const char *ssid = MY_SSID;
const char *password = MY_PASSWORD;

// Function declarations
void startCameraServer();
void setupLedFlash(int pin);

void setup() {
    Serial.begin(115200);
    Serial.println();

    // Camera configuration
    camera_config_t config = {
      .pin_pwdn       = PWDN_GPIO_NUM,
      .pin_reset      = RESET_GPIO_NUM,
      .pin_xclk       = XCLK_GPIO_NUM,
      .pin_sccb_sda   = SIOD_GPIO_NUM,
      .pin_sccb_scl   = SIOC_GPIO_NUM,
      .pin_d7         = Y9_GPIO_NUM,
      .pin_d6         = Y8_GPIO_NUM,
      .pin_d5         = Y7_GPIO_NUM,
      .pin_d4         = Y6_GPIO_NUM,
      .pin_d3         = Y5_GPIO_NUM,
      .pin_d2         = Y4_GPIO_NUM,
      .pin_d1         = Y3_GPIO_NUM,
      .pin_d0         = Y2_GPIO_NUM,
      .pin_vsync      = VSYNC_GPIO_NUM,
      .pin_href       = HREF_GPIO_NUM,
      .pin_pclk       = PCLK_GPIO_NUM,

      .xclk_freq_hz   = 20000000,
      .ledc_timer     = LEDC_TIMER_0,
      .ledc_channel   = LEDC_CHANNEL_0,
      .pixel_format   = PIXFORMAT_JPEG,
      .frame_size     = FRAMESIZE_UXGA,
      .jpeg_quality   = 12,
      .fb_count       = 1,
      .grab_mode      = CAMERA_GRAB_WHEN_EMPTY
    };

    if (config.pixel_format == PIXFORMAT_JPEG && psramFound()) {
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else if (!psramFound()) {
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // Initialize camera
    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("Camera initialization failed");
        return;
    }

    // Camera sensor settings
    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1);
        s->set_brightness(s, 1);
        s->set_saturation(s, -2);
    }

    // Setup LED flash (if applicable)
    #if defined(LED_GPIO_NUM)
    setupLedFlash(LED_GPIO_NUM);
    #endif

    // WiFi setup
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    // Start the camera server
    startCameraServer();
}

void loop() {
    // Placeholder loop. Camera server runs in another task.
    delay(10000);
}
