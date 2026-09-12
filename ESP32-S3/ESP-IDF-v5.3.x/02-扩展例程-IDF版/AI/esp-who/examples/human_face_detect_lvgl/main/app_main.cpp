#include "human_face_detect.hpp"
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
#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_mount());
#endif
#if !CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    HumanFaceDetect *detect = new HumanFaceDetect();
#else
    char dir[64];
    snprintf(dir, sizeof(dir), "%s/espdl_models/s3", CONFIG_BSP_SD_MOUNT_POINT);
    HumanFaceDetect *detect = new HumanFaceDetect(dir);
#endif
    auto who_detect = new WhoDetect(detect, cam, "HumanFaceDet", (1 << 0));

    who_cam_lcd->run();
    who_detect->run();
#if CONFIG_HUMAN_FACE_DETECT_MODEL_IN_SDCARD
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
#endif
}
