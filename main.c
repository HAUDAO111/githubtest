
/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"
/* Private function prototypes -----------------------------------------------------------------------------*/
void CKCU_Configuration(void);
void GPIO_IN_Configuration(void);
void GPIO_OUT_Configuration(void);
void GPIO_MainRoutine(void);
void BFTM_Configuration(void);

/* Private variables ---------------------------------------------------------------------------------------*/
FlagStatus TmpStatus = RESET;
FlagStatus TmpStatus1 = RESET;
FlagStatus TmpStatus2 = RESET;
int main(void)
{
  CKCU_Configuration();               /* System Related configuration                                       */

  /* Configure WAKEUP, KEY1, KEY2 pins as the input function                                                */
  GPIO_IN_Configuration();

  /* Configure LED1, LED2, LED3 pins as output function                                                     */
  GPIO_OUT_Configuration();
	BFTM_Configuration();
	HT_GPIOC->DOUTR  = 0xffff;
  /* Infinite loop to read data from input pin and then output to LED                                       */
  while (1)
  {
		GPIO_MainRoutine();
  }
}

/*********************************************************************************************************//**
  * @brief  Configure the system clocks.
  * @retval None
  ***********************************************************************************************************/
void CKCU_Configuration(void)
{
#if 1
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  HTCFG_OUTPUT_LED0_CLK(CKCUClock) = 1;
  HTCFG_OUTPUT_LED1_CLK(CKCUClock) = 1;
  HTCFG_OUTPUT_LED2_CLK(CKCUClock) = 1;
  HTCFG_INPUT_WAKE_CLK(CKCUClock)  = 1;
  HTCFG_INPUT_KEY1_CLK(CKCUClock)  = 1;
  HTCFG_INPUT_KEY2_CLK(CKCUClock)  = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
#endif
}

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports for input.
  * @retval None
  ***********************************************************************************************************/
void GPIO_IN_Configuration(void)
{
  AFIO_GPxConfig(HTCFG_INPUT_WAKE_ID, HTCFG_INPUT_WAKE_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_INPUT_KEY1_ID, HTCFG_INPUT_KEY1_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_INPUT_KEY2_ID, HTCFG_INPUT_KEY2_AFIO_PIN, AFIO_FUN_GPIO);

  /* Configure GPIO direction of input pins                                                                 */
  GPIO_DirectionConfig(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN, GPIO_DIR_IN);
  GPIO_DirectionConfig(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN, GPIO_DIR_IN);
  GPIO_DirectionConfig(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN, GPIO_DIR_IN);

  /* Configure GPIO pull resistor of input pins                                                             */
  GPIO_PullResistorConfig(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN, GPIO_PR_DISABLE);
  GPIO_PullResistorConfig(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN, GPIO_PR_DISABLE);
  GPIO_PullResistorConfig(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN, GPIO_PR_DISABLE);

  GPIO_InputConfig(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN, ENABLE);
  GPIO_InputConfig(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN, ENABLE);
  GPIO_InputConfig(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  Configure the GPIO ports for output.
  * @retval None
  ***********************************************************************************************************/
void GPIO_OUT_Configuration(void)
{
  /* Configure LED1, LED2, LED3 pins as output function                                                     */
  /* Configure AFIO mode of output pins                                                                     */
  AFIO_GPxConfig(HTCFG_OUTPUT_LED0_ID, HTCFG_OUTPUT_LED0_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_OUTPUT_LED1_ID, HTCFG_OUTPUT_LED1_AFIO_PIN, AFIO_FUN_GPIO);
  AFIO_GPxConfig(HTCFG_OUTPUT_LED2_ID, HTCFG_OUTPUT_LED2_AFIO_PIN, AFIO_FUN_GPIO);

  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HTCFG_LED0, HTCFG_OUTPUT_LED0_GPIO_PIN, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HTCFG_LED1, HTCFG_OUTPUT_LED1_GPIO_PIN, GPIO_DIR_OUT);
  GPIO_DirectionConfig(HTCFG_LED2, HTCFG_OUTPUT_LED2_GPIO_PIN, GPIO_DIR_OUT);
}

/*********************************************************************************************************//**
  * @brief  GPIO Main Routine.
  * @retval None
  ***********************************************************************************************************/
void GPIO_MainRoutine(void)
{
	
  /* Read WEAKUP and B1,B2                                                                 */
	TmpStatus = GPIO_ReadInBit(HTCFG_WAKE, HTCFG_INPUT_WAKE_GPIO_PIN);
	TmpStatus1 = GPIO_ReadInBit(HTCFG_KEY1, HTCFG_INPUT_KEY1_GPIO_PIN);
	TmpStatus2 = GPIO_ReadInBit(HTCFG_KEY2, HTCFG_INPUT_KEY2_GPIO_PIN);
	/*b1 led0 sang, b2 led chop tat, b3 chay led */
	if (TmpStatus == SET ) HT_GPIOC->DOUTR = 0xbfff;
	else if (TmpStatus1 == RESET || TmpStatus2 == RESET )BFTM_EnaCmd(HT_BFTM0, ENABLE);
	else {
		BFTM_EnaCmd(HT_BFTM0, DISABLE);
		HT_GPIOC->DOUTR = 0xffff;
		}
}

void BFTM_Configuration(void)
{
  #if 0 // Use following function to configure the IP clock speed.
  CKCU_SetPeripPrescaler(CKCU_PCLK_BFTM0, CKCU_APBCLKPRE_DIV2);
  #endif

  { /* Enable peripheral clock                                                                              */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
    CKCUClock.Bit.BFTM0 = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* BFTM as Repetitive mode, every 0.2 second to trigger the match interrupt                               */
  BFTM_SetCompare(HT_BFTM0, SystemCoreClock /5);
  BFTM_SetCounter(HT_BFTM0, 0);
	BFTM_IntConfig(HT_BFTM0, ENABLE);
  NVIC_EnableIRQ(BFTM0_IRQn);
}


#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
