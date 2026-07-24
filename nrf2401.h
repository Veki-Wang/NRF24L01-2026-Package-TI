#ifndef __NRF2401_H__
#define __NRF2401_H__

#include "ti_msp_dl_config.h"

/* ========== nRF24L01 引脚定义 ========== */
// CE:  PA31
#define NRF24L01_CE_PORT    GPIOA
#define NRF24L01_CE_PIN     DL_GPIO_PIN_31
#define NRF24L01_CE_IOMUX   IOMUX_PINCM6

// CSN: PC12
#define NRF24L01_CSN_PORT   GPIOC
#define NRF24L01_CSN_PIN    DL_GPIO_PIN_12
#define NRF24L01_CSN_IOMUX  IOMUX_PINCM61

// SCK: PC13
#define NRF24L01_SCK_PORT   GPIOC
#define NRF24L01_SCK_PIN    DL_GPIO_PIN_13
#define NRF24L01_SCK_IOMUX  IOMUX_PINCM62

// MOSI: PC15
#define NRF24L01_MOSI_PORT  GPIOC
#define NRF24L01_MOSI_PIN   DL_GPIO_PIN_15
#define NRF24L01_MOSI_IOMUX IOMUX_PINCM64

// MISO: PC14
#define NRF24L01_MISO_PORT  GPIOC
#define NRF24L01_MISO_PIN   DL_GPIO_PIN_14
#define NRF24L01_MISO_IOMUX IOMUX_PINCM63

/* ========== NRF24L01 指令宏定义 ========== */
#define NRF24L01_R_REGISTER         0x00
#define NRF24L01_W_REGISTER         0x20
#define NRF24L01_R_RX_PAYLOAD       0x61
#define NRF24L01_W_TX_PAYLOAD       0xA0
#define NRF24L01_FLUSH_TX           0xE1
#define NRF24L01_FLUSH_RX           0xE2
#define NRF24L01_REUSE_TX_PL        0xE3
#define NRF24L01_R_RX_PL_WID        0x60
#define NRF24L01_W_ACK_PAYLOAD      0xA8
#define NRF24L01_W_TX_PAYLOAD_NOACK 0xB0
#define NRF24L01_NOP                0xFF

/* ========== NRF24L01 寄存器地址 ========== */
#define NRF24L01_CONFIG             0x00
#define NRF24L01_EN_AA              0x01
#define NRF24L01_EN_RXADDR          0x02
#define NRF24L01_SETUP_AW           0x03
#define NRF24L01_SETUP_RETR         0x04
#define NRF24L01_RF_CH              0x05
#define NRF24L01_RF_SETUP           0x06
#define NRF24L01_STATUS             0x07
#define NRF24L01_OBSERVE_TX         0x08
#define NRF24L01_RPD                0x09
#define NRF24L01_RX_ADDR_P0         0x0A
#define NRF24L01_RX_ADDR_P1         0x0B
#define NRF24L01_RX_ADDR_P2         0x0C
#define NRF24L01_RX_ADDR_P3         0x0D
#define NRF24L01_RX_ADDR_P4         0x0E
#define NRF24L01_RX_ADDR_P5         0x0F
#define NRF24L01_TX_ADDR            0x10
#define NRF24L01_RX_PW_P0           0x11
#define NRF24L01_RX_PW_P1           0x12
#define NRF24L01_RX_PW_P2           0x13
#define NRF24L01_RX_PW_P3           0x14
#define NRF24L01_RX_PW_P4           0x15
#define NRF24L01_RX_PW_P5           0x16
#define NRF24L01_FIFO_STATUS        0x17
#define NRF24L01_DYNPD              0x1C
#define NRF24L01_FEATURE            0x1D

/* ========== 全局变量声明 ========== */
extern uint8_t NRF24L01_TxAddress[];
extern uint8_t NRF24L01_TxPacket[];
extern uint8_t NRF24L01_RxAddress[];
extern uint8_t NRF24L01_RxPacket[];

/* ========== GPIO 操作 ========== */
void NRF24L01_GPIO_Init(void);
void NRF24L01_W_CE(uint8_t BitValue);
void NRF24L01_W_CSN(uint8_t BitValue);
void NRF24L01_W_SCK(uint8_t BitValue);
void NRF24L01_W_MOSI(uint8_t BitValue);
uint8_t NRF24L01_R_MISO(void);

/* ========== SPI 通信 ========== */
uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte);

/* ========== 指令操作 ========== */
uint8_t NRF24L01_ReadReg(uint8_t RegAddress);
void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data);
void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count);
void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count);
void NRF24L01_FlushTx(void);
void NRF24L01_FlushRx(void);
uint8_t NRF24L01_ReadStatus(void);

/* ========== 功能函数 ========== */
void NRF24L01_PowerDown(void);
void NRF24L01_StandbyI(void);
void NRF24L01_Rx(void);
void NRF24L01_Tx(void);
void NRF24L01_Init(void);
uint8_t NRF24L01_Send(void);
uint8_t NRF24L01_Receive(void);
void NRF24L01_UpdateRxAddress(void);

/* ========== 测试函数 ========== */
uint8_t NRF24L01_Check(void);  // 检查 SPI 通信是否正常

#endif
