#include "pedestrian_detect.hpp"
#include "who_cam_lcd.hpp"
#include "who_detect.hpp"

using namespace who::app;
using namespace who::lcd;
using namespace who::cam;
using namespace dl::detect;
extern "C" void app_main(void)
{
    auto cam = new S3Cam(PIXFORMAT_RGB565, FRAMESIZE_QVGA, 2, true);
    auto who_cam_lcd = new WhoCamLCD(cam);

#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif

#if !CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    PedestrianDetect *detect = new PedestrianDetect();
#else
    char dir[64];
    snprintf(dir, sizeof(dir), "%s/espdl_models/s3", CONFIG_BSP_SD_MOUNT_POINT);
    PedestrianDetect *detect = new PedestrianDetect(dir);
#endif
    auto who_detect = new WhoDetect(detect, cam, "PedestrianDet");

    who_cam_lcd->run();

    // delay to ensure who_cam_lcd is ready.
    vTaskDelay(pdMS_TO_TICKS(100));
    who_detect->run();
#if CONFIG_PEDESTRIAN_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
