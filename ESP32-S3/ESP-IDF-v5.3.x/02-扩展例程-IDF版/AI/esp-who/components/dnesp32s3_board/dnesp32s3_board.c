/**
 ****************************************************************************************************
 * @file        dnesp32s3_board.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2025-01-01
 * @brief       板载驱动
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
  * 实验平台:正点原子 ESP32-S3 开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "bsp_err_check.h"
#include "bsp/display.h"
#include "bsp/dnesp32s3_board.h"

static const char *TAG = "S3-EYE";

/* I2C总线控制块 */
static i2c_master_bus_handle_t i2c_handle = NULL;
/* XL955设备控制块 */
i2c_master_dev_handle_t xl9555_handle = NULL;
/* i2c是否初始化 */
static bool i2c_initialized = false;
/* SD / MMC卡结构 */
sdmmc_card_t *bsp_sdcard = NULL; 
/* SD卡设备句柄 */
spi_device_handle_t MY_SD_Handle = NULL;

/* 摄像头控制宏定义 */
#define CAM_PWDN(x)         do{ x ?             \
    (xl9555_pin_write(OV_PWDN_IO, 1)):          \
    (xl9555_pin_write(OV_PWDN_IO, 0));          \
}while(0)

#define CAM_RST(x)          do{ x ?             \
    (xl9555_pin_write(OV_RESET_IO, 1)):         \
    (xl9555_pin_write(OV_RESET_IO, 0));         \
}while(0)

/* LCD控制宏定义 */
#define LCD_PWR(x)          do { x ?            \
    xl9555_pin_write(SLCD_PWR_IO, 1):           \
    xl9555_pin_write(SLCD_PWR_IO, 0);           \
} while(0)

#define LCD_RST(x)          do { x ?            \
    xl9555_pin_write(SLCD_RST_IO, 1):           \
    xl9555_pin_write(SLCD_RST_IO, 0);           \
} while(0)

/**
 * @brief       读取XL9555的IO值
 * @param       data:读取数据的存储区
 * @param       len:读取数据的大小
 * @retval      ESP_OK:读取成功; 其他:读取失败
 */
esp_err_t xl9555_read_byte(uint8_t *data, size_t len)
{
    uint8_t reg_addr = XL9555_INPUT_PORT0_REG;
    
    return i2c_master_transmit_receive(xl9555_handle, &reg_addr, 1, data, len, -1);
}

/**
 * @brief       向XL9555寄存器写入数据
 * @param       reg:寄存器地址
 * @param       data:要写入数据的存储区
 * @param       len:要写入数据的大小
 * @retval      ESP_OK:读取成功; 其他:读取失败
 */
esp_err_t xl9555_write_byte(uint8_t reg, uint8_t *data, size_t len)
{
    esp_err_t ret;

    uint8_t *buf = malloc(1 + len);
    if (buf == NULL)
    {
        ESP_LOGE(TAG, "%s memory failed", __func__);
        return ESP_ERR_NO_MEM;      /* 分配内存失败 */
    }

    buf[0] = reg;                   /* 0号元素为寄存器数值 */
    memcpy(buf + 1, data, len);     /* 拷贝数据至存储区中 */

    ret = i2c_master_transmit(xl9555_handle, buf, len + 1, -1);

    free(buf);                      /* 发送完成释放内存 */

    return ret;
}

/**
 * @brief       控制某个IO的电平
 * @param       pin     : 控制的IO
 * @param       val     : 电平
 * @retval      返回所有IO状态
 */
uint16_t xl9555_pin_write(uint16_t pin, int val)
{
    uint8_t w_data[2];
    uint16_t temp = 0x0000;

    xl9555_read_byte(w_data, 2);

    if (pin <= 0x0080)
    {
        if (val)
        {
            w_data[0] |= (uint8_t)(0xFF & pin);
        }
        else
        {
            w_data[0] &= ~(uint8_t)(0xFF & pin);
        }
    }
    else
    {
        if (val)
        {
            w_data[1] |= (uint8_t)(0xFF & (pin >> 8));
        }
        else
        {
            w_data[1] &= ~(uint8_t)(0xFF & (pin >> 8));
        }
    }

    temp = ((uint16_t)w_data[1] << 8) | w_data[0]; 

    xl9555_write_byte(XL9555_OUTPUT_PORT0_REG, w_data, 2);
    
    return temp;
}

/**
 * @brief       获取某个IO状态
 * @param       pin : 要获取状态的IO
 * @retval      此IO口的值(状态, 0/1)
 */
int xl9555_pin_read(uint16_t pin)
{
    uint16_t ret;
    uint8_t r_data[2];

    xl9555_read_byte(r_data, 2);

    ret = r_data[1] << 8 | r_data[0];

    return (ret & pin) ? 1 : 0;
}

/**
 * @brief       XL9555的IO配置
 * @param       config_value：IO配置输入或者输出
 * @retval      返回设置的数值
 */
void xl9555_ioconfig(uint16_t config_value)
{
    /* 从机地址 + CMD + data1(P0) + data2(P1) */
    /* P10、P11、P12、P13和P14为输入，其他引脚为输出 -->0001 1111 0000 0000 注意：0为输出，1为输入*/
    uint8_t data[2];
    esp_err_t err;

    data[0] = (uint8_t)(0xFF & config_value);
    data[1] = (uint8_t)(0xFF & (config_value >> 8));

    do
    {
        err = xl9555_write_byte(XL9555_CONFIG_PORT0_REG, data, 2);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "%s configure %X failed, ret: %d", __func__, config_value, err);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
        
    } while (err != ESP_OK);
}

/**
 * @brief       外部中断服务函数
 * @param       arg：中断引脚号
 * @note        IRAM_ATTR: 这里的IRAM_ATTR属性用于将中断处理函数存储在内部RAM中，目的在于减少延迟
 * @retval      无
 */
static void IRAM_ATTR xl9555_exit_gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    
    if (gpio_num == XL9555_INT_IO)
    {
        esp_rom_delay_us(20000);

        if (gpio_get_level(XL9555_INT_IO) == 0)
        {
            /* 中断处理 */
            // ESP_DRAM_LOGI("xl9555", "xl9555 int");
        }
    }
}

/**
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void xl9555_int_init(void)
{
    gpio_config_t gpio_init_struct;

    /* 配置XL9555器件的INT中断引脚 */
    gpio_init_struct.mode         = GPIO_MODE_INPUT;        /* 选择为输入模式 */
    gpio_init_struct.pull_up_en   = GPIO_PULLUP_ENABLE;     /* 上拉使能 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* 下拉失能 */
    gpio_init_struct.intr_type    = GPIO_INTR_NEGEDGE;      /* 下降沿触发 */
    gpio_init_struct.pin_bit_mask = 1ull << XL9555_INT_IO;  /* 设置的引脚的位掩码 */
    gpio_config(&gpio_init_struct);                         /* 配置使能 */
    
    /* 注册中断服务 */
    gpio_install_isr_service(0);
    
    /* 设置GPIO的中断回调函数 */
    gpio_isr_handler_add(XL9555_INT_IO, xl9555_exit_gpio_isr_handler, (void*)XL9555_INT_IO);
}

/**
 * @brief       初始化XL9555
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t xl9555_init(void)
{
    uint8_t r_data[2];

    i2c_device_config_t xl9555_i2c_dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  /* 从机地址长度 */
        .scl_speed_hz    = 400000,              /* 传输速率 */
        .device_address  = XL9555_ADDR,         /* 从机7位的地址 */
    };
    /* I2C总线上添加XL9555设备 */
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_handle, &xl9555_i2c_dev_conf, &xl9555_handle));

    /* 输入模式下，中断才有效（读取IO电平） */
    // xl9555_int_init();

    /* 上电先读取一次清除中断标志 */
    xl9555_read_byte(r_data, 2);
    /* 配置那些扩展管脚为输入输出模式 */
    xl9555_ioconfig(0xF003);
    /* 关闭蜂鸣器 */
    xl9555_pin_write(BEEP_IO, 1);
    /* 关闭喇叭 */
    xl9555_pin_write(SPK_EN_IO, 1);

    return ESP_OK;
}

/**
 * @brief       按键扫描函数
 * @param       mode:0->不连续;1->连续
 * @retval      键值, 定义如下:
 *              KEY0_PRES, 1, KEY0按下
 *              KEY1_PRES, 2, KEY1按下
 *              KEY2_PRES, 3, KEY2按下
 *              KEY3_PRES, 4, KEY3按下
 */
uint8_t xl9555_key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    static uint8_t key_up = 1;                                          /* 按键按松开标志 */

    if (mode)
    {
        key_up = 1;                                                     /* 支持连按 */
    }
    
    if (key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0  || KEY3 == 0 )) /* 按键松开标志为1, 且有任意一个按键按下了 */
    {
        esp_rom_delay_us(100000);                                  /* 去抖动 */
        key_up = 0;

        if (KEY0 == 0)
        {
            keyval = KEY0_PRES;
        }

        if (KEY1 == 0)
        {
            keyval = KEY1_PRES;
        }

        if (KEY2 == 0)
        {
            keyval = KEY2_PRES;
        }

        if (KEY3 == 0)
        {
            keyval = KEY3_PRES;
        }
    }
    else if (KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && KEY3 == 1)          /* 没有任何按键按下, 标记按键松开 */
    {
        key_up = 1;
    }

    return keyval;                                                      /* 返回键值 */
}

/**
 * @brief       初始化I2C总线
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_i2c_init(void)
{
    if (i2c_initialized)
    {
        return ESP_OK;
    }

    const i2c_master_bus_config_t i2c_config = {
        .i2c_port = BSP_I2C_NUM_0,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .flags.enable_internal_pullup = true,
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&i2c_config, &i2c_handle));

    xl9555_init();

    if (BSP_CAMERA_PWDN == GPIO_NUM_NC)
    {
        CAM_PWDN(0);
    } 

    if (BSP_CAMERA_RESET == GPIO_NUM_NC)
    { 
        CAM_RST(0);
        vTaskDelay(pdMS_TO_TICKS(20));
        CAM_RST(1);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    
    i2c_initialized = true;
    return ESP_OK;
}

/**
 * @brief       卸载I2C总线
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_i2c_deinit(void)
{
    BSP_ERROR_CHECK_RETURN_ERR(i2c_del_master_bus(i2c_handle));
    i2c_initialized = false;
    return ESP_OK;
}

/**
 * @brief       获取I2C总线句柄
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    bsp_i2c_init();

    return i2c_handle;
}

/**
 * @brief       挂载spiffs
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_BSP_SPIFFS_MOUNT_POINT,
        .partition_label = CONFIG_BSP_SPIFFS_PARTITION_LABEL,
        .max_files = CONFIG_BSP_SPIFFS_MAX_FILES,
#ifdef CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
    };

    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

    BSP_ERROR_CHECK_RETURN_ERR(ret_val);

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);

    if (ret_val != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    return ret_val;
}

/**
 * @brief       卸载spiffs
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_spiffs_unmount(void)
{
    return esp_vfs_spiffs_unregister(CONFIG_BSP_SPIFFS_PARTITION_LABEL);
}

/**
 * @brief       挂载SD卡
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_sdcard_mount(void)
{
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_BSP_SD_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    /* SD卡引脚配置 */
    sdspi_device_config_t slot_config = {0};
    slot_config.host_id   = host.slot;
    slot_config.gpio_cs   = BSP_SD_CS;
    slot_config.gpio_cd   = GPIO_NUM_NC;
    slot_config.gpio_wp   = GPIO_NUM_NC;
    slot_config.gpio_int  = GPIO_NUM_NC;

#if !CONFIG_FATFS_LONG_FILENAMES
    ESP_LOGW(TAG, "Warning: Long filenames on SD card are disabled in menuconfig!");
#endif

    return esp_vfs_fat_sdmmc_mount(BSP_SD_MOUNT_POINT, &host, &slot_config, &mount_config, &bsp_sdcard);
}

/**
 * @brief       卸载SD卡
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_sdcard_unmount(void)
{
    return esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, bsp_sdcard);
}

#define LCD_CMD_BITS        (8)
#define LCD_PARAM_BITS      (8)
#define LCD_LEDC_CH         (CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH)
#define LVGL_TICK_PERIOD_MS (CONFIG_BSP_DISPLAY_LVGL_TICK)
#define LVGL_MAX_SLEEP_MS   (CONFIG_BSP_DISPLAY_LVGL_MAX_SLEEP)

/**
 * @brief       打开背光
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_display_backlight_off(void)
{
    LCD_PWR(0);
    return ESP_OK;
}

/**
 * @brief       关闭背光
 * @param       无
 * @retval      ESP_OK:初始化成功
 */
esp_err_t bsp_display_backlight_on(void)
{
    LCD_PWR(1);
    return ESP_OK;
}

/**
 * @brief       新建显示对象
 * @param       config      :   配置结构体
 * @param       ret_panel   :   控制句柄
 * @param       ret_io      :   IO控制句柄
 * @retval      esp_err_t
 */
esp_err_t bsp_display_new(const bsp_display_config_t *config,
                          esp_lcd_panel_handle_t *ret_panel,
                          esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    assert(config != NULL && config->max_transfer_sz > 0);
    /* 复位 */
    LCD_RST(0);
    vTaskDelay(pdMS_TO_TICKS(100));
    LCD_RST(1);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = BSP_LCD_SPI_CLK,
        .mosi_io_num = BSP_LCD_SPI_MOSI,
        .miso_io_num = BSP_LCD_SPI_MISO,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = config->max_transfer_sz,
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");

    /* SPI驱动接口配置,SPISD卡时钟是20-25MHz */
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20 * 1000 * 1000, /* SPI时钟 */
        .mode = 0,                          /* SPI模式0 */
        .spics_io_num = BSP_SD_CS,          /* 片选引脚 */
        .queue_size = 7,                    /* 事务队列尺寸 7个 */
    };

    /* 添加SPI总线设备 */
    ESP_ERROR_CHECK(spi_bus_add_device(BSP_LCD_SPI_NUM, &devcfg, &MY_SD_Handle));
    
    ESP_LOGD(TAG, "Install panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = BSP_LCD_DC,
        .cs_gpio_num = BSP_LCD_CS,
        .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 7,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_config, ret_io),
                      err,
                      TAG,
                      "New panel IO failed");

    ESP_LOGD(TAG, "Install LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_LCD_RST,
        .rgb_ele_order  = COLOR_RGB_ELEMENT_ORDER_RGB,  /* RGB颜色格式 */
        .bits_per_pixel = 16,                           /* 颜色深度 */
        .data_endian    = LCD_RGB_DATA_ENDIAN_BIG,      /* 大端顺序 */
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(*ret_io, &panel_config, ret_panel), err, TAG, "New panel failed");

    esp_lcd_panel_reset(*ret_panel);
    esp_lcd_panel_init(*ret_panel);
    esp_lcd_panel_invert_color(*ret_panel, true);

    esp_lcd_panel_swap_xy(*ret_panel, true);
    esp_lcd_panel_mirror(*ret_panel, true, false);
    return ret;
err:
    if (*ret_panel) {
        esp_lcd_panel_del(*ret_panel);
    }
    if (*ret_io) {
        esp_lcd_panel_io_del(*ret_io);
    }
    spi_bus_free(BSP_LCD_SPI_NUM);
    return ret;
}

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
static lv_display_t *bsp_display_lcd_init(const bsp_display_cfg_t *cfg)
{
    assert(cfg != NULL);
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = BSP_LCD_DRAW_BUFF_SIZE * sizeof(uint16_t),
    };
    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_new(&bsp_disp_cfg, &panel_handle, &io_handle));

    esp_lcd_panel_disp_on_off(panel_handle, true);

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = cfg->buffer_size,
        .double_buffer = cfg->double_buffer,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation =
            {
                .swap_xy = true,
                .mirror_x = true,
                .mirror_y = false,
            },
        .flags = {
            .buff_dma = cfg->flags.buff_dma,
            .buff_spiram = cfg->flags.buff_spiram,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = (BSP_LCD_BIGENDIAN ? true : false),
#endif
        }};

    return lvgl_port_add_disp(&disp_cfg);
}

lv_display_t *bsp_display_start(void)
{
    bsp_display_cfg_t cfg = {.lvgl_port_cfg =
                                 {
                                     .task_priority = CONFIG_BSP_DISPLAY_LVGL_TASK_PRIORITY,
                                     .task_stack = 6144,
                                     .task_affinity = 1,
                                     .timer_period_ms = LVGL_TICK_PERIOD_MS,
                                     .task_max_sleep_ms = LVGL_MAX_SLEEP_MS,
                                 },
                             .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
                             .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
                             .flags = {
#if (CONFIG_BSP_DISPLAY_LVGL_BUFFER_IN_PSRAM && CONFIG_SPIRAM)
                                 .buff_dma = false,
                                 .buff_spiram = true,
#else
                                 .buff_dma = true,
                                 .buff_spiram = false,
#endif
                             }};
    return bsp_display_start_with_config(&cfg);
}

lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg)
{
    lv_display_t *disp;
    assert(cfg != NULL);
    BSP_ERROR_CHECK_RETURN_NULL(lvgl_port_init(&cfg->lvgl_port_cfg));
    BSP_NULL_CHECK(disp = bsp_display_lcd_init(cfg), NULL);

    return disp;
}

lv_indev_t *bsp_display_get_input_dev(void)
{
    return NULL;
}

bool bsp_display_lock(uint32_t timeout_ms)
{
    return lvgl_port_lock(timeout_ms);
}

void bsp_display_unlock(void)
{
    lvgl_port_unlock();
}

void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation)
{
    lv_disp_set_rotation(disp, rotation);
}
#endif // (BSP_CONFIG_NO_GRAPHIC_LIB == 0)

esp_err_t bsp_leds_init(void)
{
    const gpio_config_t led_io_config = {.pin_bit_mask = BIT64(BSP_LED_RED),
                                         .mode = GPIO_MODE_OUTPUT_OD, // GPIO3 is connected directly to the LED (on
                                                                      // board revision 2.1), so we use Open-drain here
                                         .pull_up_en = GPIO_PULLUP_DISABLE,
                                         .pull_down_en = GPIO_PULLDOWN_DISABLE,
                                         .intr_type = GPIO_INTR_DISABLE};
    BSP_ERROR_CHECK_RETURN_ERR(gpio_config(&led_io_config));
    return ESP_OK;
}

esp_err_t bsp_led_set(const bsp_led_t led_io, const bool on)
{
    BSP_ERROR_CHECK_RETURN_ERR(gpio_set_level((gpio_num_t)led_io, (uint32_t)on));
    return ESP_OK;
}

esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    const audio_codec_data_if_t *i2s_data_if = bsp_audio_get_codec_itf();
    if (i2s_data_if == NULL) {
        /* Configure I2S peripheral and Power Amplifier */
        BSP_ERROR_CHECK_RETURN_NULL(bsp_audio_init(NULL));
        i2s_data_if = bsp_audio_get_codec_itf();
    }
    assert(i2s_data_if);

    esp_codec_dev_cfg_t codec_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN,
        .codec_if = NULL,
        .data_if = i2s_data_if,
    };
    return esp_codec_dev_new(&codec_dev_cfg);
}

static adc_oneshot_unit_handle_t bsp_adc_handle = NULL;

static const button_config_t bsp_button_config[BSP_BUTTON_NUM] = {
    {.type = BUTTON_TYPE_ADC,
     .adc_button_config.adc_handle = &bsp_adc_handle,
     .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
     .adc_button_config.button_index = BSP_BUTTON_MENU,
     .adc_button_config.min = 2310, // middle is 2410mV
     .adc_button_config.max = 2510},
    {.type = BUTTON_TYPE_ADC,
     .adc_button_config.adc_handle = &bsp_adc_handle,
     .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
     .adc_button_config.button_index = BSP_BUTTON_PLAY,
     .adc_button_config.min = 1880, // middle is 1980mV
     .adc_button_config.max = 2080},
    {.type = BUTTON_TYPE_ADC,
     .adc_button_config.adc_handle = &bsp_adc_handle,
     .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
     .adc_button_config.button_index = BSP_BUTTON_DOWN,
     .adc_button_config.min = 720, // middle is 820mV
     .adc_button_config.max = 920},
    {.type = BUTTON_TYPE_ADC,
     .adc_button_config.adc_handle = &bsp_adc_handle,
     .adc_button_config.adc_channel = ADC_CHANNEL_0, // ADC1 channel 0 is GPIO1
     .adc_button_config.button_index = BSP_BUTTON_UP,
     .adc_button_config.min = 280, // middle is 380mV
     .adc_button_config.max = 480},
    {.type = BUTTON_TYPE_GPIO,
     .gpio_button_config.active_level = 0,
     .gpio_button_config.gpio_num = BSP_BUTTON_BOOT_IO}};
     
esp_err_t bsp_iot_button_create(button_handle_t btn_array[], int *btn_cnt, int btn_array_size)
{
    esp_err_t ret = ESP_OK;
    if ((btn_array_size < BSP_BUTTON_NUM) || (btn_array == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    /* Initialize ADC and get ADC handle */
    BSP_ERROR_CHECK_RETURN_NULL(bsp_adc_initialize());
    bsp_adc_handle = bsp_adc_get_handle();

    if (btn_cnt) {
        *btn_cnt = 0;
    }
    for (int i = 0; i < BSP_BUTTON_NUM; i++) {
        btn_array[i] = iot_button_create(&bsp_button_config[i]);
        if (btn_array[i] == NULL) {
            ret = ESP_FAIL;
            break;
        }
        if (btn_cnt) {
            (*btn_cnt)++;
        }
    }
    return ret;
}
