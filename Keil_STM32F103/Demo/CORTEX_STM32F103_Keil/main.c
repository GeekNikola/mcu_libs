
/* Standard includes. */
#include <stdio.h>

/* Library includes. */
#include "stm32f10x_it.h"
#include "mcu_libs.h"
extern void UART_Init(unsigned long ulWantedBaud);

/* Demo app includes. */
static void prvSetupHardware(void);

#define mainDELAY_LOOP_COUNT 100000

int main(void)
{
	prvSetupHardware();

	mcu_loger_init();
	mcu_lib_init();

	MCU_OBJ_TYPE(ring_buf) * ringBuffer;
	int *data;
	size_t len = 5;

	// 创建环形缓冲区
	mcu_ring_buf_new(int, ringBuffer, data, len);
	log_i("Ring buffer created.");
	printf("Initial state - front: %d, rear: %d, max: %d\n", ringBuffer->front, ringBuffer->rear, ringBuffer->max_len);

	// 测试入队
	for (int i = 0; i < 7; i++)
	{
		log_i("Trying to enqueue: %d", i);
		mcu_ring_buf_enqueue(ringBuffer, data, i);
		log_i("Enqueued value: %d", i);
		printf("After enqueue - front: %d, rear: %d, length: %zu\n", ringBuffer->front, ringBuffer->rear, mcu_ring_buf_len(ringBuffer));
	}

	// 测试获取队头和队尾
	log_i("Front value:");
	int frontValue = mcu_ring_buf_front(ringBuffer, int, data);
	printf("Front value: %d\n", frontValue);

	log_i("Rear value:");
	int rearValue = mcu_ring_buf_rear(ringBuffer, int, data);
	printf("Rear value: %d\n", rearValue);

	// 遍历当前队列
	log_i("Current buffer contents:");

	mcu_ring_buf_forech(ringBuffer, n)
	{
		log_i("n: %d", n);
	}
	printf("State after enqueues - front: %d, rear: %d, max: %d\n", ringBuffer->front, ringBuffer->rear, ringBuffer->max_len);

	// 测试出队
	for (int i = 0; i < 3; i++)
	{
		log_i("Trying to dequeue:");
		mcu_ring_buf_dequeue(ringBuffer);
		log_i("Dequeued value.");
		printf("After dequeue - length: %zu\n", mcu_ring_buf_len(ringBuffer));
	}

	// 遍历当前队列
	log_i("Current buffer contents after dequeues:");
	mcu_ring_buf_forech(ringBuffer, n)
	{
		log_i("n: %d", n);
	}
	printf("State after dequeues - front: %d, rear: %d, max: %d\n", ringBuffer->front, ringBuffer->rear, ringBuffer->max_len);

	// 测试查询
	for (int pos = 0; pos < len; pos++)
	{
		log_i("Trying to peek at position: %d", pos);
		int peekValue = mcu_ring_buf_peek(ringBuffer, int, data, pos);
		printf("Peeked value at position %d: %d\n", pos, peekValue);
	}

	// 清理环形缓冲区
	mcu_ring_buf_clear(ringBuffer);
	log_i("Ring buffer clean.");
	// 遍历当前队列
	log_i("Current buffer contents:");

	mcu_ring_buf_forech(ringBuffer, n)
	{
		log_i("n: %d", n);
	}
	printf("State after enqueues - front: %d, rear: %d, max: %d\n", ringBuffer->front, ringBuffer->rear, ringBuffer->max_len);

	while (1)
	{
		// log_i("test1\n");
	}

	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware(void)
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready. */
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
	{
	}

	/* 2 wait states required on the flash. */
	*((unsigned long *)0x40022000) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);

	/* PCLK2 = HCLK */
	RCC_PCLK2Config(RCC_HCLK_Div1);

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config(RCC_HCLK_Div2);

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	/* Enable PLL. */
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready. */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	/* Wait till PLL is used as system clock source. */
	while (RCC_GetSYSCLKSource() != 0x08)
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	// vParTestInitialise();

	UART_Init(115200);
}
/*-----------------------------------------------------------*/

#ifdef DEBUG
/* Keep the linker happy. */
void assert_failed(unsigned char *pcFile, unsigned long ulLine)
{
	for (;;)
	{
	}
}
#endif
