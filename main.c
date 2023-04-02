//PMPS projekat
//PAMETNA KANTA ZA SMECE
//Home automation

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stdlib.h"

TIM_HandleTypeDef TIM_HandleStruct;
GPIO_InitTypeDef GPIO_InitStructure;
//Ultrasonic
void GPIO_Init(void){
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	//Inicijaliiramo PA2 i PA3
	GPIO_InitStructure.Pin=GPIO_PIN_2;
	GPIO_InitStructure.Mode=GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull=GPIO_NOPULL;
	GPIO_InitStructure.Speed=GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_3;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

}

void delay_us(uint32_t us)
{/// delay in us (1,2^16);
 /// TIM4 runs on APB1 @ 42MHz with multiplier 2 which makes it 84MHz
 /// since it takes some time to setup timer the actual delay will be us + const!
 /// error < 0.1004%
    
	__HAL_RCC_TIM2_CLK_ENABLE();

	TIM_Base_InitTypeDef TIM_Base_InitStruct;
	TIM_Base_InitStruct.Prescaler = 84-1;
	TIM_Base_InitStruct.CounterMode = TIM_COUNTERMODE_UP;
	TIM_Base_InitStruct.Period = 0x0054;
	TIM_Base_InitStruct.ClockDivision =	TIM_CLOCKDIVISION_DIV1;

	TIM_HandleStruct.Instance = TIM2;
	TIM_HandleStruct.Init = TIM_Base_InitStruct;

	HAL_TIM_Base_Init(&TIM_HandleStruct);
	HAL_TIM_Base_Start(&TIM_HandleStruct);

	while(us > 0)
	{
		while(__HAL_TIM_GET_FLAG(&TIM_HandleStruct,TIM_FLAG_UPDATE) == 0x0000);
		__HAL_TIM_CLEAR_FLAG(&TIM_HandleStruct,TIM_FLAG_UPDATE);
		us--;
	} 

	HAL_TIM_Base_Stop(&TIM_HandleStruct);
	__HAL_RCC_TIM2_CLK_DISABLE();
}

//HAL_Init();
volatile uint32_t msTicks; //broji 1ms timeTicks
void SysTick_Handler(void){
	msTicks++;
}

//Delays broj sysTickova(svake 1ms)
//static void Delay(__IO uint32_t dlyTicks){
//	uint32_t curTicks=msTicks;
//	while ((msTicks - curTicks)<dlyTicks);
//}

void setSysTick(void){
	//-----sysTick timer 1ms-----//
	if(SysTick_Config(SystemCoreClock / 1000)){
		//capture error//
		while(1){};
	}
}

TIM_HandleTypeDef TIM4_HandleStruct;

void config_PWM(void){
	//Strukture za konfiguraciju
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_Base_InitTypeDef TIM_TimeBaseStructure;
	TIM_OC_InitTypeDef TIM_OCInitStructure;
	
	//TIM4 Clock Enaable
	
	//GPIO Clock Enable
	__HAL_RCC_GPIOB_CLK_ENABLE();
	//Inicijaliiramo PB6(TIM4 Ch1)
	GPIO_InitStruct.Pin=GPIO_PIN_6;
	GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed=GPIO_SPEED_HIGH;
	GPIO_InitStruct.Pull=GPIO_PULLUP;
	GPIO_InitStruct.Alternate= GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	//Setup TIM / PWM values
	//Servo reqirements:
	// 50Hz (==20ms)
	//1-2 ms Duty Cycle
	
//1. Determine Required Timer_Freq.
//TIM_Period = (Timer_Freq./PWM_Freq)-1
//-treba nam period od 20ms(ili 20000us) i nas PWM_Freq = 50 Hz (1/20ms)
//	TIM_Period = 20000 - 1 = 19999 (0 offset)
//Timer_Freq = (TIM_Period + 1 )*PWM_Freq.
//Timer_Freq=(20000*50)
//Timer_Freq=1000000=1MHz

//2. Determine Pre-Scaler
/* APB1 clock frequency:
	-SYS_CLK/4 kada je prescaler =1 (npr. 168MHZ/4=42MHz)
	-SYS_CLK/2 kada je preskaliran !=1 (168MHz/2=84MHz)
	
Prescaler=APB1_Freq / Timer_Freq = 84 MHz/1= 84MHz

*/
uint16_t PrescalerValue = (uint16_t) 84;
//Time Base Configuration
	__HAL_RCC_TIM4_CLK_ENABLE();
TIM_TimeBaseStructure.Prescaler=PrescalerValue;
TIM_TimeBaseStructure.Period=19999;
TIM_TimeBaseStructure.ClockDivision=0;
TIM_TimeBaseStructure.CounterMode=TIM_COUNTERMODE_UP; //Count 0 -> CNT
TIM4_HandleStruct.Instance = TIM4;
TIM4_HandleStruct.Init = TIM_TimeBaseStructure;

HAL_TIM_PWM_Init(&TIM4_HandleStruct);

TIM_OCInitStructure.OCMode =TIM_OCMODE_PWM1;
//TIM_OCInitStructure.OCIdleState = TIM_OutputState_Enable;
TIM_OCInitStructure.Pulse=0;
TIM_OCInitStructure.OCPolarity=TIM_OCPOLARITY_HIGH;
TIM_OCInitStructure.OCFastMode= TIM_OCFAST_DISABLE;

HAL_TIM_PWM_ConfigChannel(&TIM4_HandleStruct, &TIM_OCInitStructure, TIM_CHANNEL_1);
HAL_TIM_PWM_Start(&TIM4_HandleStruct,TIM_CHANNEL_1);
}

void setPWM(uint32_t ccr1)
{
	__HAL_TIM_SET_COMPARE(&TIM4_HandleStruct,TIM_CHANNEL_1,ccr1);
	
	
}

//volatile uint8_t g_pb_state;
int main(void)
{

HAL_Init();
setSysTick();
config_PWM();
GPIO_Init();
	uint32_t numTicks;
	const float brzSvjetlosti=0.0343/2;
	float udaljenost;
	
	/*GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);*/
	
void ServoOpen(void){

setPWM(2000);
//HAL_Delay(10000);

}

void ServoClose(void){

	setPWM(1000);
	//HAL_Delay(10000);


}

while(1)
{

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0x00);
		delay_us(3);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0x01);
		delay_us(10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0x00);
		
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==0);
		
		numTicks=0;
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)==1)
		{
			numTicks++;
			delay_us(2);
		};
		
		udaljenost=(numTicks+0.0f)*brzSvjetlosti;
		
		
		if(udaljenost < 0.4 ){
			HAL_Delay(3000);
			if(udaljenost < 0.5){
				ServoOpen();
				
			} else ServoClose();
			}else
			ServoClose(); 
	//Control Servo
	
	
}

}

/*void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
	if (GPIO_Pin == GPIO_PIN_0)// PA0 or PB0 or PC0, all GPIO 0 are multiplexed
	{
		setPWM(1500);			
		HAL_Delay(7000);
		setPWM(2000);
	}

}*/
//Skontati kako preko inerrupta tacno pokrenuti ugao od 90stepeni()
//Skontati zasto se nekad vrti u suprotnu stranu (ako ne stavimo delay ili aako nema delaya uopste cudno se ponasa)
//Napraiti 2 funkcije ServoOpen() i ServoClose() koje ce imati usporenje pri dolasku ka zeljenom uglu(devide by 2 metoda)
//Skonati da li GPIO pin moze raditi pod custom napon i kako taj napon napraviti (ideja je preko ADC)
//na 3V sporije otvara, ali bi bilo bolje na jos nizem naponu da radi
//Ako bude bilo vremena dodati sleepmode i watchdog timer, bilo bi pozeljno 
