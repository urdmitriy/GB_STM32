/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "crypto.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TAG_LENGTH 4
#define PLAINTEXT_LENGTH 4
#define CIPHER_TEXT_LENGTH PLAINTEXT_LENGTH + TAG_LENGTH
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

/* USER CODE BEGIN PV */
/* Header message, will be authenticated but not encrypted */
const uint8_t HeaderMessage[] =
        {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        };

uint32_t HeaderLength = sizeof (HeaderMessage) ;

/* string length only, without '\0' end of string marker */


/* Payload message, will be authenticated and encrypted */
const uint8_t Plaintext[] =
        {
                0x20, 0x21, 0x22, 0x23
        };

/* string length only, without '\0' end of string marker */
uint32_t InputLength = sizeof (Plaintext) ;

/* Key to be used for AES encryption/decryption */
uint8_t Key[CRL_AES128_KEY] =
        {
                0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
        };

/* Initialization Vector, used only in non-ECB modes */
uint8_t IV[] =
        {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
        };

/* NIST example 1 ciphertext vector: in encryption we expect this vector as result */
const uint8_t Expected_Ciphertext[CIPHER_TEXT_LENGTH] =
        {
                0x71, 0x62, 0x01, 0x5b, 0x4d, 0xac, 0x25, 0x5d
        };

/* Buffer to store the output data and the authentication TAG */
uint8_t encrypt_OutputMessage[64];
uint8_t decrypt_OutputMessage[64];
int32_t encrypt_OutputMessageLength = 0;
int32_t decrypt_OutputMessageLength = 0;
int32_t AuthenticationTAGLength = 0;

uint8_t tag[64];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
int32_t STM32_AES_CCM_Encrypt(uint8_t*  HeaderMessage,
                              uint32_t  HeaderMessageLength,
                              uint8_t*  InputMessage,
                              uint32_t  InputMessageLength,
                              uint8_t  *AES128_Key,
                              uint8_t  *InitializationVector,
                              uint32_t  IvLength,
                              uint8_t  *OutputMessage,
                              int32_t *OutputMessageLength,
                              int32_t *AuthenticationTAGLength
);


int32_t STM32_AES_CCM_Decrypt(uint8_t*  HeaderMessage,
                              uint32_t  HeaderMessageLength,
                              uint8_t*  InputMessage,
                              uint32_t  InputMessageLength,
                              uint8_t  *AES128_Key,
                              uint8_t  *InitializationVector,
                              uint32_t  IvLength,
                              uint8_t  *OutputMessage,
                              int32_t *OutputMessageLength,
                              int32_t  AuthenticationTAGLength);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    volatile int32_t status = AES_SUCCESS;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */


    status = STM32_AES_CCM_Encrypt( (uint8_t *) HeaderMessage, HeaderLength , (uint8_t *) Plaintext,
                                    InputLength, Key, IV, sizeof(IV), encrypt_OutputMessage,
                                    &encrypt_OutputMessageLength, &AuthenticationTAGLength);

    status = STM32_AES_CCM_Decrypt( (uint8_t *) HeaderMessage, sizeof(HeaderMessage), (uint8_t *) encrypt_OutputMessage,
                                    encrypt_OutputMessageLength, Key, IV, sizeof(IV), decrypt_OutputMessage,
                                    &decrypt_OutputMessageLength, AuthenticationTAGLength);

    __IO uint32_t test = status;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
  * @brief  AES CCM Authenticated Encryption example.
  * @param  HeaderMessage: pointer to the header message. It will be authenticated but not encrypted.
  * @param  HeaderMessageLength: header message length in byte.
  * @param  Plaintext: pointer to input message to be encrypted.
  * @param  PlaintextLength: input data message length in byte.
  * @param  AES128_Key: pointer to the AES key to be used in the operation
  * @param  InitializationVector: pointer to the Initialization Vector (IV)
  * @param  IvLength: IV length in bytes.
  * @param  OutputMessage: pointer to output parameter that will handle the encrypted message and TAG
  * @param  OutputMessageLength: pointer to encrypted message length.
  * @param  AuthenticationTAGLength: authentication TAG length.
  * @retval error status: can be AES_SUCCESS if success or one of
  *         AES_ERR_BAD_OPERATION, AES_ERR_BAD_CONTEXT
  *         AES_ERR_BAD_PARAMETER if error occured.
  */
int32_t STM32_AES_CCM_Encrypt(uint8_t*  HeaderMessage,
                              uint32_t  HeaderMessageLength,
                              uint8_t*  Plaintext,
                              uint32_t  PlaintextLength,
                              uint8_t  *AES128_Key,
                              uint8_t  *InitializationVector,
                              uint32_t  IvLength,
                              uint8_t  *OutputMessage,
                              int32_t *OutputMessageLength,
                              int32_t *AuthenticationTAGLength
)
{
    AESCCMctx_stt AESctx;

    uint32_t error_status = AES_SUCCESS;

    /* Set flag field to default value */
    AESctx.mFlags = E_SK_DEFAULT;

    /* Set key size to 16 (corresponding to AES-128) */
    AESctx.mKeySize = 16;

    /* Set nonce size field to IvLength, note that valid values are 7,8,9,10,11,12,13*/
    AESctx.mNonceSize = IvLength;

    /* Size of returned authentication TAG */
    AESctx.mTagSize = 4;

    /* Set the size of the header */
    AESctx.mAssDataSize = HeaderMessageLength;

    /* Set the size of thepayload */
    AESctx.mPayloadSize = PlaintextLength;

    /* Initialize the operation, by passing the key and IV */
    error_status = AES_CCM_Encrypt_Init(&AESctx, AES128_Key, InitializationVector );

    /* check for initialization errors */
    if (error_status == AES_SUCCESS)
    {
        /* Process Header */
        error_status = AES_CCM_Header_Append(&AESctx,
                                             HeaderMessage,
                                             HeaderMessageLength);
        if (error_status == AES_SUCCESS)
        {
            /* Encrypt Data */
            error_status = AES_CCM_Encrypt_Append(&AESctx,
                                                  Plaintext,
                                                  PlaintextLength,
                                                  OutputMessage,
                                                  OutputMessageLength);

            if (error_status == AES_SUCCESS)
            {
                /* Do the Finalization, write the TAG at the end of the encrypted message */
                error_status = AES_CCM_Encrypt_Finish(&AESctx, OutputMessage + *OutputMessageLength, AuthenticationTAGLength);
            }
        }
    }

    return error_status;
}


/**
  * @brief  AES CCM Authenticated Decryption example.
  * @param  HeaderMessage: pointer to the header message. It will be authenticated but not Decrypted.
  * @param  HeaderMessageLength: header message length in byte.
  * @param  Plaintext: pointer to input message to be Decrypted.
  * @param  PlaintextLength: input data message length in byte.
  * @param  AES128_Key: pointer to the AES key to be used in the operation
  * @param  InitializationVector: pointer to the Initialization Vector (IV)
  * @param  IvLength: IV length in bytes.
  * @param  OutputMessage: pointer to output parameter that will handle the Decrypted message and TAG
  * @param  OutputMessageLength: pointer to Decrypted message length.
  * @param  AuthenticationTAGLength: authentication TAG length.
  * @retval error status: can be AUTHENTICATION_SUCCESSFUL if success or one of
  *         AES_ERR_BAD_OPERATION, AES_ERR_BAD_CONTEXT
  *         AES_ERR_BAD_PARAMETER, AUTHENTICATION_FAILED if error occured.
  */
int32_t STM32_AES_CCM_Decrypt(uint8_t*  HeaderMessage,
                              uint32_t  HeaderMessageLength,
                              uint8_t*  Plaintext,
                              uint32_t  PlaintextLength,
                              uint8_t  *AES128_Key,
                              uint8_t  *InitializationVector,
                              uint32_t  IvLength,
                              uint8_t  *OutputMessage,
                              int32_t *OutputMessageLength,
                              int32_t  AuthenticationTAGLength
)
{
    AESCCMctx_stt AESctx;

    uint32_t error_status = AES_SUCCESS;

    /* Set flag field to default value */
    AESctx.mFlags = E_SK_DEFAULT;

    /* Set key size to 16 (corresponding to AES-128) */
    AESctx.mKeySize = 16;

    /* Set nonce size field to IvLength, note that valid values are 7,8,9,10,11,12,13*/
    AESctx.mNonceSize = IvLength;

    /* Size of returned authentication TAG */
    AESctx.mTagSize = 4;

    /* Set the size of the header */
    AESctx.mAssDataSize = HeaderMessageLength;

    /* Set the size of thepayload */
    AESctx.mPayloadSize = PlaintextLength;

    /* Set the pointer to the TAG to be checked */
    AESctx.pmTag = Plaintext + PlaintextLength;

    /* Size of returned authentication TAG */
    AESctx.mTagSize = AuthenticationTAGLength;


    /* Initialize the operation, by passing the key and IV */
    error_status = AES_CCM_Decrypt_Init(&AESctx, AES128_Key, InitializationVector );

    /* check for initialization errors */
    if (error_status == AES_SUCCESS)
    {
        /* Process Header */
        error_status = AES_CCM_Header_Append(&AESctx,
                                             HeaderMessage,
                                             HeaderMessageLength);
        if (error_status == AES_SUCCESS)
        {
            /* Decrypt Data */
            error_status = AES_CCM_Decrypt_Append(&AESctx,
                                                  Plaintext,
                                                  PlaintextLength,
                                                  OutputMessage,
                                                  OutputMessageLength);

            if (error_status == AES_SUCCESS)
            {
                /* Do the Finalization, check the authentication TAG*/
                error_status = AES_CCM_Decrypt_Finish(&AESctx, NULL, &AuthenticationTAGLength);
            }
        }
    }

    return error_status;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
