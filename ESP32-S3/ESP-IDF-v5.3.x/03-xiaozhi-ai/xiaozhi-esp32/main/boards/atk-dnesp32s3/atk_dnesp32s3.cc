#include "wifi_board.h"
#include "es8388_audio_codec.h"
#include "display/ssd1306_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "i2c_device.h"
#include "iot/thing_manager.h"
#include "led/single_led.h"

#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <driver/i2c_master.h>
#include <wifi_station.h>

#define TAG "atk_dnesp32s3"

/* XL9555设备涉及到SPILCD的引脚和喇叭的控制 */
class XL9555 : public I2cDevice {
public:
    XL9555(i2c_master_bus_handle_t i2c_bus, uint8_t addr) : I2cDevice(i2c_bus, addr) {
        WriteReg(0x06, 0x03);   /* config IO */
        WriteReg(0x07, 0xF0);   /* config IO */
    }

    /* XL9555芯片外扩16个IO */
    void SetOutputState(uint8_t bit, uint8_t level) {
        uint16_t data;
        if (bit < 8) {
            data = ReadReg(0x02);   /* 读取第一个输出寄存器（0~7引脚）*/
        } else {
            data = ReadReg(0x03);   /* 读取第二个输出寄存器（8~15引脚）*/
            bit -= 8;               /* 将bit转换为0~7的范围 */
        }

        /* 设置指定bit的输出状态 */ 
        data = (data & ~(1 << bit)) | (level << bit);

        /* 写回对应的输出寄存器 */ 
        if (bit < 8) {
            WriteReg(0x02, data);
        } else {
            WriteReg(0x03, data);
        }
    }
};


class atk_dnesp32s3 : public WifiBoard {
private:
    i2c_master_bus_handle_t i2c_bus_;
    Button boot_button_;
    Ssd1306Display* display_;
    XL9555* xl9555_;

    /* 初始化I2C总线 */
    void InitializeI2c()
    {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port           = (i2c_port_t)I2C_NUM_0,
            .sda_io_num         = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num         = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source         = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt  = 7,
            .intr_priority      = 0,
            .trans_queue_depth  = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_));

        /* 初始化XL9555设备 */
        xl9555_ = new XL9555(i2c_bus_, 0x20);
    }

    /* OLED(SSD1306) 走 I2C，无需初始化 SPI 总线 */

    /* 初始化BOOT引脚 */
    void InitializeButtons()
    {
        /* 点击事件 */
        boot_button_.OnClick([this]()
        {
            auto& app = Application::GetInstance();         /* 获取对象 */
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected())
            {
                ResetWifiConfiguration();                   /* 在设备开始阶段或链接WiFi阶段，进行wifi复位 */
            }
        });
        /* 按下事件 */
        boot_button_.OnPressDown([this]()
        {
            Application::GetInstance().StartListening();    /* 应用开始聆听语音 */
        });
        /* 释放事件 */
        boot_button_.OnPressUp([this]()
        {
            Application::GetInstance().StopListening();     /* 应用结束聆听语音 */
        });
    }

    /* 初始化 SSD1306 OLED（I2C0，地址 0x3C，分辨率 128x64）
       接线：OLED 的 SDA -> 板子 IIC_SDA(GPIO41)，SCL -> IIC_SCL(GPIO42)，VCC -> 3.3V，GND -> GND
       若你的 OLED 是 128x32 或地址 0x3D，改下方 new Ssd1306Display(..., 128, 64/32, ...) 与
       ssd1306_display.cc 中的 .dev_addr 即可 */
    void InitializeSsd1306Display()
    {
        display_ = new Ssd1306Display(i2c_bus_, 128, 64, false, false);
        xl9555_->SetOutputState(2, 0);  /* 打开喇叭（若已接喇叭） */
    }

    /* 物联网初始化，添加对 AI 可见设备 */ 
    void InitializeIot()
    {
        auto& thing_manager = iot::ThingManager::GetInstance();
        thing_manager.AddThing(iot::CreateThing("Speaker"));
    }

public:
    atk_dnesp32s3() : boot_button_(BOOT_BUTTON_GPIO)
    {
        InitializeI2c();
        InitializeSsd1306Display();
        InitializeButtons();
        InitializeIot();
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    /* 获取音频设备 */ 
    virtual AudioCodec* GetAudioCodec() override {
        static Es8388AudioCodec* audio_codec = nullptr;
        if (audio_codec == nullptr) {
            audio_codec = new Es8388AudioCodec(i2c_bus_, I2C_NUM_0, AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
                AUDIO_I2S_GPIO_MCLK, AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN,
                GPIO_NUM_NC, AUDIO_CODEC_ES8388_ADDR);

            audio_codec->SetOutputVolume(AUDIO_DEFAULT_OUTPUT_VOLUME);  /* 设置默认音量 */
        }
        return audio_codec;
    }

    /* 获取显示设备 */ 
    virtual Display* GetDisplay() override {
        return display_;
    }
};

DECLARE_BOARD(atk_dnesp32s3);
