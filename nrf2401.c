#include "nrf2401.h"
#include "Delay.h"

/* ========== 全局变量 ========== */

// 发送参数
uint8_t NRF24L01_TxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
#define NRF24L01_TX_PACKET_WIDTH        4
uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH];

// 接收参数
uint8_t NRF24L01_RxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
#define NRF24L01_RX_PACKET_WIDTH        4
uint8_t NRF24L01_RxPacket[NRF24L01_RX_PACKET_WIDTH];

/* ========== GPIO 底层操作 ========== */

void NRF24L01_GPIO_Init(void)
{
    // CE(PA31): 推挽输出
    DL_GPIO_initDigitalOutput(NRF24L01_CE_IOMUX);

    // CSN(PC12): 推挽输出
    DL_GPIO_initDigitalOutput(NRF24L01_CSN_IOMUX);

    // SCK(PC13): 推挽输出
    DL_GPIO_initDigitalOutput(NRF24L01_SCK_IOMUX);

    // MOSI(PC15): 推挽输出
    DL_GPIO_initDigitalOutput(NRF24L01_MOSI_IOMUX);

    // MISO(PC14): 浮空输入 (NRF24L01 的 MISO 是推挽输出)
    DL_GPIO_initDigitalInputFeatures(NRF24L01_MISO_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    // 使能输出 (CE在GPIOA, 其余在GPIOC)
    DL_GPIO_enableOutput(GPIOA, NRF24L01_CE_PIN);
    DL_GPIO_enableOutput(GPIOC, NRF24L01_CSN_PIN |
        NRF24L01_SCK_PIN | NRF24L01_MOSI_PIN);

    // 默认电平 (必须在使能输出后设置)
    NRF24L01_W_CE(0);    // CE 默认低，退出收发模式
    NRF24L01_W_CSN(1);   // CSN 默认高，不选中
    NRF24L01_W_SCK(0);   // SCK 默认低，SPI 模式0
    NRF24L01_W_MOSI(0);  // MOSI 默认低
}

void NRF24L01_W_CE(uint8_t BitValue)
{
    if (BitValue)
        DL_GPIO_setPins(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
    else
        DL_GPIO_clearPins(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
}

void NRF24L01_W_CSN(uint8_t BitValue)
{
    if (BitValue)
        DL_GPIO_setPins(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN);
    else
        DL_GPIO_clearPins(NRF24L01_CSN_PORT, NRF24L01_CSN_PIN);
}

void NRF24L01_W_SCK(uint8_t BitValue)
{
    if (BitValue)
        DL_GPIO_setPins(NRF24L01_SCK_PORT, NRF24L01_SCK_PIN);
    else
        DL_GPIO_clearPins(NRF24L01_SCK_PORT, NRF24L01_SCK_PIN);
}

void NRF24L01_W_MOSI(uint8_t BitValue)
{
    if (BitValue)
        DL_GPIO_setPins(NRF24L01_MOSI_PORT, NRF24L01_MOSI_PIN);
    else
        DL_GPIO_clearPins(NRF24L01_MOSI_PORT, NRF24L01_MOSI_PIN);
}

uint8_t NRF24L01_R_MISO(void)
{
    return (DL_GPIO_readPins(NRF24L01_MISO_PORT, NRF24L01_MISO_PIN) != 0) ? 1 : 0;
}

/* ========== SPI 通信协议 (模式0) ========== */

uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte)
{
    uint8_t i;

    for (i = 0; i < 8; i ++)
    {
        // MOSI 输出最高位
        if (Byte & 0x80)
            NRF24L01_W_MOSI(1);
        else
            NRF24L01_W_MOSI(0);
        Byte <<= 1;

        // 延时 (约 200ns @80MHz)
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

        // SCK 上升沿
        NRF24L01_W_SCK(1);

        // 延时
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();

        // MISO 读取数据
        if (NRF24L01_R_MISO())
            Byte |= 0x01;

        // SCK 下降沿
        NRF24L01_W_SCK(0);

        // 延时
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    }

    return Byte;
}

/* ========== 指令操作 ========== */

uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
    Data = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);

    return Data;
}

void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
    uint8_t i;

    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
    for (i = 0; i < Count; i ++)
        DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);
}

void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
    NRF24L01_SPI_SwapByte(Data);
    NRF24L01_W_CSN(1);
}

void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
    uint8_t i;

    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
    for (i = 0; i < Count; i ++)
        NRF24L01_SPI_SwapByte(DataArray[i]);
    NRF24L01_W_CSN(1);
}

void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count)
{
    uint8_t i;

    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
    for (i = 0; i < Count; i ++)
        DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);
}

void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count)
{
    uint8_t i;

    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD);
    for (i = 0; i < Count; i ++)
        NRF24L01_SPI_SwapByte(DataArray[i]);
    NRF24L01_W_CSN(1);
}

void NRF24L01_FlushTx(void)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);
    NRF24L01_W_CSN(1);
}

void NRF24L01_FlushRx(void)
{
    NRF24L01_W_CSN(0);
    NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);
    NRF24L01_W_CSN(1);
}

uint8_t NRF24L01_ReadStatus(void)
{
    uint8_t Status;

    NRF24L01_W_CSN(0);
    Status = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
    NRF24L01_W_CSN(1);

    return Status;
}

/* ========== 功能函数 ========== */

void NRF24L01_PowerDown(void)
{
    uint8_t Config;

    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    if (Config == 0xFF) return;
    Config &= ~0x02;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}

void NRF24L01_StandbyI(void)
{
    uint8_t Config;

    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    if (Config == 0xFF) return;
    Config |= 0x02;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}

void NRF24L01_Rx(void)
{
    uint8_t Config;

    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    if (Config == 0xFF) return;
    Config |= 0x03;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
    NRF24L01_W_CE(1);
}

void NRF24L01_Tx(void)
{
    uint8_t Config;

    NRF24L01_W_CE(0);
    Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
    if (Config == 0xFF) return;
    Config |= 0x02;
    Config &= ~0x01;
    NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
    NRF24L01_W_CE(1);
}

void NRF24L01_Init(void)
{
    // GPIO 初始化
    NRF24L01_GPIO_Init();

    // 配置寄存器
    NRF24L01_WriteReg(NRF24L01_CONFIG, 0x08);
    NRF24L01_WriteReg(NRF24L01_EN_AA, 0x3F);
    NRF24L01_WriteReg(NRF24L01_EN_RXADDR, 0x01);
    NRF24L01_WriteReg(NRF24L01_SETUP_AW, 0x03);
    NRF24L01_WriteReg(NRF24L01_SETUP_RETR, 0x03);
    NRF24L01_WriteReg(NRF24L01_RF_CH, 0x02);
    NRF24L01_WriteReg(NRF24L01_RF_SETUP, 0x0E);

    NRF24L01_WriteReg(NRF24L01_RX_PW_P0, NRF24L01_RX_PACKET_WIDTH);
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);

    NRF24L01_FlushTx();
    NRF24L01_FlushRx();
    NRF24L01_WriteReg(NRF24L01_STATUS, 0x70);

    NRF24L01_Rx();
}

uint8_t NRF24L01_Send(void)
{
    uint8_t Status;
    uint8_t SendFlag;
    uint32_t Timeout;

    NRF24L01_WriteRegs(NRF24L01_TX_ADDR, NRF24L01_TxAddress, 5);
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_TxAddress, 5);
    NRF24L01_WriteTxPayload(NRF24L01_TxPacket, NRF24L01_TX_PACKET_WIDTH);
    NRF24L01_Tx();

    Timeout = 10000;
    while (1)
    {
        Status = NRF24L01_ReadStatus();
        Timeout --;

        if (Timeout == 0)
        {
            SendFlag = 4;           // 发送超时
            NRF24L01_Init();
            break;
        }

        if ((Status & 0x30) == 0x30)
        {
            SendFlag = 3;           // 状态值不合法
            NRF24L01_Init();
            break;
        }
        else if ((Status & 0x10) == 0x10)
        {
            SendFlag = 2;           // 重发次数达上限
            NRF24L01_Init();
            break;
        }
        else if ((Status & 0x20) == 0x20)
        {
            SendFlag = 1;           // 发送成功
            break;
        }
    }

    NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);
    NRF24L01_FlushTx();
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
    NRF24L01_Rx();

    return SendFlag;
}

uint8_t NRF24L01_Receive(void)
{
    uint8_t Status;

    Status = NRF24L01_ReadStatus();

    // 检查 RX_DR (bit 6) 是否置位
    if ((Status & 0x40) == 0x40)
    {
        // 读取接收数据
        NRF24L01_ReadRxPayload(NRF24L01_RxPacket, NRF24L01_RX_PACKET_WIDTH);
        // 清除 RX_DR 标志
        NRF24L01_WriteReg(NRF24L01_STATUS, 0x40);
        NRF24L01_FlushRx();
        return 1;  // 接收成功
    }
    // 检查 TX_DS 和 MAX_RT 同时为1（异常）
    else if ((Status & 0x30) == 0x30)
    {
        NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);  // 清除异常标志
        return 2;  // 状态异常
    }

    // 未收到数据
    return 0;
}

void NRF24L01_UpdateRxAddress(void)
{
    NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
}

/* ========== 测试函数 ========== */

/**
 * @brief 检查 NRF24L01 SPI 通信是否正常
 * @return 0: 异常, 1: 正常
 */
uint8_t NRF24L01_Check(void)
{
    uint8_t status;

    // 读取 STATUS 寄存器 (上电默认值为 0x0E，bit1=1)
    status = NRF24L01_ReadReg(NRF24L01_STATUS);

    // 检查是否为 0xFF (SPI 无响应，MISO 悬空)
    if (status == 0xFF) return 0;

    // 检查是否为 0x00 (可能 MISO 接地或短路)
    if (status == 0x00) return 0;

    return 1;  // SPI 通信正常
}
