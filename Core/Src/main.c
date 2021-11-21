/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "can.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "BMP280_simple.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define STACK_SIZE 500
#define TAB_SIZE 10
TaskHandle_t xHandleT = NULL;
TaskHandle_t xHandleP = NULL;
TaskHandle_t xHandleC = NULL;

int val[TAB_SIZE][2];
int val_cpt = 0;

int it_uart = 0;
int it_uart4 = 0;
int cpt = 0;
uint8_t uart_rx_buffer[64];
uint8_t uart_tx_buffer[64];
uint8_t uart4_rx_buffer[64];
uint8_t uart4_tx_buffer[64];
uint8_t uart4_rx2_buffer[64];

uint8_t i2c_t_buffer[1] = {0xD0} ;
uint8_t i2c_r_buffer[1];
uint8_t i2c_t_conf_buffer[2] = {0xF4, 0x57} ;
uint8_t i2c_t_calibration_buffer[1]={0x88}; /*creating buffer to get calibration values with I2C HAL functions*/
uint8_t i2c_r_conf_buffer[1];
uint8_t i2c_t_data_buffer[1]= {0xF7};
uint8_t i2c_r_data_buffer[6];

uint32_t pres, temp;
uint8_t pres_val[64], temp_val[64];
uint32_t p;
uint32_t t,t_comp;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//buffer communication CAN
uint8_t message[2]={0x54,0x01};
uint8_t zero[2]={0x54,0x00};
uint32_t mail;
//buffer communication I2C
uint8_t buff_temp;
uint8_t buff_press;

int calcul(){
	if (val_cpt<TAB_SIZE){
		temp = BMP280_get_temperature();
		pres = BMP280_get_pressure();

		val[val_cpt][0] = pres;
		val[val_cpt][1] = temp;
		val_cpt++;
	} else {
		printf("Tableau plein, voici le tableau dans son int\351gralit\351. \r\n");
		printf("Pres1 = %x, Pres2 = %x, Pres3 = %x, Pres4 = %x, Pres5 = %x, Pres6 = %x, Pres7 = %x, Pres8 = %x, Pres9 = %x, Pres10 = %x \r\n",
				val[0][0], val[1][0], val[2][0], val[3][0], val[4][0], val[5][0], val[6][0], val[7][0], val[8][0], val[9][0]);
		printf("Temp1 = %x, Temp2 = %x, Temp3 = %x, Temp4 = %x, Temp5 = %x, Temp6 = %x, Temp7 = %x, Temp8 = %x, Temp9 = %x, Temp10 = %x \r\n",
				val[0][1], val[1][1], val[2][1], val[3][1], val[4][1], val[5][1], val[6][1], val[7][1], val[8][1], val[9][1]);
		printf("Tableau r\351initialis\351... \r\n");
		val_cpt = 0;
		temp = BMP280_get_temperature();
		pres = BMP280_get_pressure();

		pres = 3;
		temp = 5;
		val[val_cpt][0] = pres;
		val[val_cpt][1] = temp;
		val_cpt++;
	}

	return 0;
}

int get_pres(int argc, char ** argv) {
	if(argc==2){
		if(strncmp("all", argv[1], 1)==0){
			for(int k=0; k<val_cpt; k++){
				//printf("all\r\n");
				printf("Pres%d = %x \r\n",k+1, val[k][0]);
			}
		}
		if(strncmp("now", argv[1], 1)==0){
			int c = calcul();
			//printf("P=\r\n");
			printf("P = 0X%05lX \r\n", val[val_cpt-1][0]);
		}
	}
	else{
		printf("Erreur: nombre d'arguments incorrect\r\n");
		printf("Usage : %s <'now' pour avoir la valeur du moment ou 'all' pour avoir toutes les valeurs>\r\n", argv[0]);
	}
	return 0;
}

int get_temp(int argc, char ** argv) {
	if(argc==2){
		if(strncmp("all", argv[1], 1)==0){
			for(int k=0; k<val_cpt; k++){
				//printf("all\r\n");
				printf("Temp%d = %x \r\n",k+1, val[k][0]);
			}
		}
		if(strncmp("now", argv[1], 1)==0){
			int c = calcul();
			//printf("P=\r\n");
			printf("T =0X%05lX \r\n", val[val_cpt-1][0]);
		}
	}
	else{
		printf("Erreur: nombre d'arguments incorrect\r\n");
		printf("Usage : %s <'now' pour avoir la valeur du moment ou 'all' pour avoir toutes les valeurs>\r\n", argv[0]);
	}
	return 0;
}

void vTaskCAN(void * p){


	vTaskDelete(NULL);
}

int thermo(int argc, char ** argv){
	//xTaskCreate(vTaskCAN, "", STACK_SIZE, (void *) NULL, 1, xHandleC);

	//configuring CAN
	HAL_CAN_Start (&hcan1);
	CAN_TxHeaderTypeDef TxHeader;
	TxHeader.StdId=0x61;
	TxHeader.IDE=CAN_ID_STD;
	TxHeader.RTR=CAN_RTR_DATA;
	TxHeader.DLC=2;
	TxHeader.TransmitGlobalTime=DISABLE;

	//Setting motor to zero
	HAL_CAN_AddTxMessage(&hcan1, &TxHeader, (uint8_t*) {0x00,0x00} , &mail);
	HAL_Delay(100);

	for(int l=0; l<10; l++){
		//printf("test\r\n");
		HAL_CAN_AddTxMessage(&hcan1, &TxHeader, message, &mail);
		//Send_CAN_Message(message,&hcan1,pHeader,mail);
		//to code with tasks
		HAL_Delay(1000);
		HAL_CAN_AddTxMessage(&hcan1, &TxHeader, zero, &mail);
		HAL_Delay(1000);
	}
	return 0;
}

int rasp(int argc, char ** argv){

	printf("Nous avons malheureusement eu beaucoup de mal avec le code Python de la Raspberry. \r\n");
	printf("Nous avons r\351ussi \340 faire une page web simple avec la Raspberry et nous avons \351galement \351crit un code python pour le protocole de commande. \r\n");
	printf("Notre probl\350me a \351t\351 de cr\351er une page web adapt\351e au sujet de TP (cr\351er toutes les fonctions demand\351es) et aussi de relier les codes pythons avec nos codes C de la STM. \r\n");
	printf("Nous avons alors pr\351f\351r\351 rendre un shell sur la STM, m\352me si cela implique un fonctionnement sans Raspberry.\r\n");
	printf("Gr\342ce \340 ce shell, vous pourrez faire des demandes de temp\351rature et de pressions et lancer le moteur pour 10 cycles.\r\n");
	printf("Vous trouverez tout de m\352me nos scripts python dans le git.\r\n");

	return 0;
}

void vTaskShell(void * p) {
	shell_init();
	shell_add('E', rasp, "*Ce qu'on aurait aim\351 faire avec la Raspberry* \r\n -->Commande : <E> \r\n -->Sortie : ce qu'on aurait aim\351 faire et explications sur pourquoi nous ne l'avons pas fait. \r\n");
	shell_add('T', get_temp, "*Renvoie la temp\351rature* \r\n -->Commande : <T> <'now' pour une temp\351rature ou 'all' pour avoir toutes les temp\351rature> \r\n -->Sortie : La ou les valeur(s) de temp\351rature demand\351e(s).\r\n");
	shell_add('P', get_pres, "*Renvoie la pression* \r\n -->Commande : <P> <'now' pour une pression ou 'all' pour avoir toutes les pressions> \r\n -->Sortie : La ou les valeur(s) de pression demand\351e(s).\r\n");
	shell_add('M', thermo, "*Lance le moteur* \r\n -->Commande : <M> \r\n -->Sortie : le moteur se met \340 tourner. \r\n");

	shell_run();
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

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
	MX_USART2_UART_Init();
	MX_CAN1_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	printf("\r\nChecking for BMP280\r\n");
	BMP280_check();
	printf("\r\nConfigure BMP280\r\n");
	BMP280_init();

	//Création du shell
	xReturned = xTaskCreate(
			vTaskShell,      	/* Function that implements the task. */
			"Shell",         	/* Text name for the task. */
			STACK_SIZE,      	/* Stack size in words, not bytes. */
			( void * ) NULL,    /* Parameter passed into the task. */
			1,					/* Priority at which the task is created. */
			&xHandle );      	/* Used to pass out the created task's handle. */

	if( xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY ) {
		printf("Task Shell creation error: Could not allocate required memory\r\n");
	}
	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();
	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
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

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
