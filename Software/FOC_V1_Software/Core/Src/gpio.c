/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SENS_DIR_GPIO_Port, SENS_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SENS_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SENS_DIR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = DRV_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DRV_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = SENS_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SENS_OUT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/* USER CODE BEGIN 2 */

void led_blink(uint16_t blink_speed, uint16_t blink_time)
{
  for (unsigned int i = 0; i < blink_time * 2; i++)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(blink_speed);
  }
}



void key_read(void)
{
  static uint8_t hold_time, double_time, click_time;

  if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == KEY_PRESS)
  { // �ٴ��ж�

    while ((HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == KEY_PRESS) && (hold_time < KEY_HOLD))
    { // ѭ���жϳ�������ʱ��ת

      hold_time++;
      HAL_Delay(10); // �����жϵļ�ʱ
    }

    if (hold_time >= KEY_HOLD)
    { // ��������
      /* ������ִ�еĳ���ŵ��˴� */

      key_read_hold();

      /* ������ִ�еĳ���ŵ��˴� */
      while (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == KEY_PRESS)
        ;
    }
    else
    { // ��������

      for (double_time = 0; double_time < KEY_DOUBLE; double_time++)
      { // ���˫��
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == KEY_PRESS)
        {
          click_time = 1;
          /* ˫����ִ�еĳ���ŵ��˴� */

          key_read_double_click();

          /* ˫����ִ�еĳ���ŵ��˴� */
          while (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == KEY_PRESS);
        }
      }
      if (click_time == 0)
      { // �жϵ���
        /* ������ִ�еĳ���ŵ��˴� */
        
        key_read_click();

        /* ������ִ�еĳ���ŵ��˴� */
      }
    }
    click_time = 0;
    hold_time = 0; // ������0
  }

} // �����ж��ڴ˽���

void key_read_click(void)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

extern sdo_typedef* oSdo;
extern state_typedef* oState;
extern pdo_typedef* oPdo;

void key_read_double_click(void)
{
  for (unsigned int i = 0; i < 10; i++)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(100);
  }

  // �ֶ���Ƕȶ���
  calib_elec_angle(oState, oSdo);

  // �ֶ�ADC����
  calib_adc_offset(oPdo, oSdo);
  
  // ��������
  save_config(oSdo);
}
void key_read_hold(void)
{
  for (unsigned int i = 0; i < 10; i++)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(500);
  }
  save_config(oSdo);
}


/* USER CODE END 2 */
