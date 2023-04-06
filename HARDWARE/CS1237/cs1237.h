/*
 * @Description: cd1237 驱动
 * @Author: TOTHTOT
 * @Date: 2023-03-23 16:42:25
 * @LastEditTime: 2023-03-28 16:11:18
 * @LastEditors: TOTHTOT
 * @FilePath: \MDK-ARMe:\Work\MCU\stm32\read_cs1237_STM32F103C8T6(HAL+FreeRTOS)\HARDWARE\CS1237\cs1237.h
 */
#ifndef __CS1237_H__
#define __CS1237_H__

#include "main.h"

#define CS1237_CIONFIG_WRITE_REGISTER 0x65 // 设置写入寄存器
#define CS1237_CIONFIG_READ_REGISTER 0x56  // 设置读取寄存器

#define CS1237_SCL_H HAL_GPIO_WritePin(CS1237_SCL_GPIO_Port, CS1237_SCL_Pin, GPIO_PIN_SET)
#define CS1237_SCL_L HAL_GPIO_WritePin(CS1237_SCL_GPIO_Port, CS1237_SCL_Pin, GPIO_PIN_RESET)

#define CS1237_SDA_H HAL_GPIO_WritePin(CS1237_DOUT_GPIO_Port, CS1237_DOUT_Pin, GPIO_PIN_SET)
#define CS1237_SDA_L HAL_GPIO_WritePin(CS1237_DOUT_GPIO_Port, CS1237_DOUT_Pin, GPIO_PIN_RESET)

#define CS1237_SDA_READ HAL_GPIO_ReadPin(CS1237_DOUT_GPIO_Port, CS1237_DOUT_Pin)

/* 设置IO方向 */
#define CS1237_SDA_IN                                    \
    {                                                    \
        CS1237_DOUT_GPIO_Port->CRL &= 0XFFFF0FFF;        \
        CS1237_DOUT_GPIO_Port->CRL |= (uint32_t)8 << 12; \
    }
#define CS1237_SDA_OUT                                   \
    {                                                    \
        CS1237_DOUT_GPIO_Port->CRL &= 0XFFFF0FFF;        \
        CS1237_DOUT_GPIO_Port->CRL |= (uint32_t)3 << 12; \
    }

/* cs1237 设备结构体 */
struct cs1237_device
{
#define CS1237_DEFAULT_FREQUENCY 10 // 默认输出频率
#define RAW_DATA_MAX_NUM 8			// 用于滤波, 连续读取 RAW_DATA_MAX_NUM 次数据然后去掉最高, 最低, 
									// 再计算平均值,平均值带入卡尔曼滤波器
    int32_t adc_data;                                 // 读取到的数据, 原始adc数据, 单次读取
    int32_t adc_calculate_raw_data[RAW_DATA_MAX_NUM]; // 读取数据后存入, 用于滤波, 连续采集12次
    int32_t adc_calculate_deal_data;                  // 读取数据后存入, 用于滤波
    uint8_t adc_config;                               // 模块配置参数
    uint16_t output_frequency;                        // 模块输出频率

    enum dev_ch
    {
        DEV_CH_NONE,          // 默认状态
        DEV_CH_A,             // 通道A
        DEV_CH_SAVE,          // 保留
        DEV_CH_TEMPERERATURE, // 温度
        DEV_CH_SHORT,         // 内部短路
        DEV_CH_TOTAL_STATE    // 状态统计
    } dev_ch_em;              // cs1237 通道选择

    enum dev_pga
    {
        DEV_PGA_NONE,
        DEV_PGA_1,
        DEV_PGA_2,
        DEV_PGA_64,
        DEV_PGA_128,
        DEV_PGA_TOTAL_STATE
    } dev_pga_em; // cs1237 pga选择, 在测温时选择 DEV_PGA_1

    enum dev_frequency
    {
        DEV_FREQUENCY_NONE,
        DEV_FREQUENCY_10,
        DEV_FREQUENCY_40,
        DEV_FREQUENCY_640,
        DEV_FREQUENCY_1280,
        DEV_FREQUENCY_TOTAL_STATE
    } dev_frequency_em; // cs1237 输出频率选择

    enum dev_state
    {
        DEV_NONE,       // 默认状态
        DEV_ONLINE,     // 设备在线
        DEV_OFFLINE,    // 设备掉线
        DEV_TOTAL_STATE // 所有状态数量
    } dev_state_em;     // cs1237 是否在线, 不在线就不读取数据

    int32_t (*get_adc_data)(struct cs1237_device *dev);   // 读取数据, 外部操作的函数
    uint8_t (*get_adc_config)(struct cs1237_device *dev); // 读取配置数据, 外部操作的函数
};
extern struct cs1237_device g_cs1237_device_st;
/* 外部调用函数 */
uint8_t cs1237_init(struct cs1237_device *dev, enum dev_frequency frequency, enum dev_pga pga, enum dev_ch ch);

#endif /* __CS1237_H__ */
