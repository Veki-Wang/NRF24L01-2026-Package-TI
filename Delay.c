#include "Delay.h"
#include "ti_msp_dl_config.h"

/**
  * 函    数：微秒级延时
  * 参    数：us 延时的微秒数
  * 返 回 值：无
  * 说    明：基于CPUCLK_FREQ(32MHz)的delay_cycles实现
  *           每微秒 = CPUCLK_FREQ / 1000000 = 32个时钟周期
  */
void Delay_us(uint32_t us)
{
	delay_cycles(us * (CPUCLK_FREQ / 1000000));
}

/**
  * 函    数：毫秒级延时
  * 参    数：ms 延时的毫秒数
  * 返 回 值：无
  * 说    明：基于CPUCLK_FREQ(32MHz)的delay_cycles实现
  *           每毫秒 = CPUCLK_FREQ / 1000 = 32000个时钟周期
  */
void Delay_ms(uint32_t ms)
{
	delay_cycles(ms * (CPUCLK_FREQ / 1000));
}
