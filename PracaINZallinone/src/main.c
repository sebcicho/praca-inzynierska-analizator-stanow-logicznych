/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_flash.h"
#include "stm32f4xx_tim.h"

#include "tm_stm32f4_dma2d_graphic.h"
#include "tm_stm32f4_dma2d_graphic.h"
#include "tm_stm32f4_ili9341_ltdc.h"
#include "tm_stm32f4_fonts.h"
#include "tm_stm32f4_delay.h"

#include "tm_stm32f4_stmpe811.h"

#define DIVS_NUMBER		9
#define SAMPLE_PER_BIT	10
#define PIXELS_PER_DIV	20

  /* Zapalenie bitu n w zmiennej k */
 #define SET_BIT(k, n)        (k |= (1 << (n)))
 
 /* Zgaszenie bitu n w zmiennej k */
 #define RESET_BIT(k, n)      (k &= ~(1 << (n)))

  #define IS_BIT_SET(k, n)     ((k) & (1 << (n)))

void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void TIM_Configuration(void);
void TIM_Configuration3(int Period, int Prescaler);
void TIM_Configuration1(int Period, int Prescaler);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM9_IRQHandler(void);
void DMA2D_Configuration(void);
void fillChannelsText(void);
void GUIPreparation(void);
void DrawGUIBackground(void);
void DrawSignal(int startPosition);

void TouchDataConfiguration(void);
void StartMenu(void);
void fillTriggersText(void);
void infoPanel(void);
void infoPanel2(void);
void infoPanel3(void);
void infoPanel4(void);
void infoPanel5(void);
void resetInfoPanel(void);
void resetInfoPanel5(void);
void resetGUI(void);
void fillFreqList(void);
void generateDivision(void);
void DataMatching(int a);


TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
ErrorStatus HSEStartupStatus;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitDef;	//wyjscia
GPIO_InitTypeDef GPIO_InitDef1; //wejscia
GPIO_InitTypeDef GPIO_InitDef2; //wejscia

TM_STMPE811_TouchData touchData;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure3;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure1;

NVIC_InitTypeDef NVIC_InitStructure3;
NVIC_InitTypeDef NVIC_InitStructure1;


char Data[4096];
uint16_t  DataInt[4096];
unsigned short int LastData = 53;


//240x320
TM_DMA2DRAPHIC_Poly_t Coordinates2[] = {
       {25, 20},
       {25, 300},
       {226, 300},
       {226, 20},
       {25, 20}
   };


short int SamplingFreq = 1;
volatile int SamplingPause = 1;   //w mikrosekundach
char SamplingFreqDiscriminant[2] = "M";

int Division = 20;
int Division1 = 20;		//w mikrosekundach
char DivisionDiscriminant[3] = "u";

char text1[10];
char text2[10];
char text3[15];
char text4[10];
char text5[10];

char Channels[8][4];
char Triggers[2][20];
char Frequencies[12][7];
int divisions[5];

short int ChannelChosen=0;
short int TriggerChosen=0;

short int MenuLevel=0;	// 0 - wybieranie kanalu i wyzwalania/ 1 - wybieranie czestotliowsci / 2 wybieranie podziałki / 3 klikniecie pomiaru i koniec menu
bool ChannelFlag = false; // wybrano kanal
bool TriggerFlag = false; //wybrano wyzwalanie
bool FreqFlag = false; //wybrano czestotliwosc
bool DivFlag = false; //wybrano podzialke
bool MeasureFlag = false; //przycisk pomiar nacisniety

bool Menu = true;//wyjście z menu
bool Draw = false;// czy rysować ?
bool IsSignalDrawn = false;
bool FastFreq = false;// czy wybrana czestotliość próbkowanie >= 100kHz
bool SlowFreq = false;// czy wybrana czestotliość próbkowanie < 100kHz
volatile bool Download = false;// czy ma sie odbyś ściąganie danych z GPIO?
volatile bool Trigger = false;
volatile char Bufor[2];

bool DownloadFinished = false;

int StartPosition = 0;

volatile int k= 0;
volatile bool flaga = true;
volatile int SampleCount= 0; 

int shift= 0;

int main(void)
{
	int i = 0;
	int j = 0; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	RCC_Configuration();
	GPIO_Configuration();
	TIM_Configuration();
	NVIC_Configuration();
	DMA2D_Configuration();
	TouchDataConfiguration();

	TM_ILI9341_Layer1To2();

	fillChannelsText();
	fillTriggersText();
	fillFreqList();
	StartMenu();
	infoPanel();

	short int x1 = 20;
	short int y1 = 50;

	short int x2 = 70;
	short int y2 = 50;
	char str[30];


	
while(1)
	{
		if(Menu == true)
		{
			if(MenuLevel == 0)
			{
				if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed)
				{
					if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1 && touchData.y <= y1 + 20)
					{
						ChannelChosen = 0;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30 && touchData.y <= y1+30 + 20)
					{
						ChannelChosen = 1;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30*2 && touchData.y <= y1+30*2 + 20)
					{
						ChannelChosen = 2;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30*3 && touchData.y <= y1+30*3 + 20)
					{
						ChannelChosen = 3;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30*4 && touchData.y <= y1+30*4 + 20)
					{
						ChannelChosen = 4;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30*5 && touchData.y <= y1+30*5 + 20)
					{
						ChannelChosen = 5;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30*6 && touchData.y <= y1+30*6 + 20)
					{
						ChannelChosen = 6;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 30 && touchData.y >= y1+30*7 && touchData.y <= y1+30*7 + 20)
					{
						ChannelChosen = 7;
						infoPanel();
						ChannelFlag = true;
					}
					else if(touchData.x >= x2 && touchData.x <= x2 + 130 && touchData.y >= y2 && touchData.y <= y2 + 20)
					{
						TriggerChosen = 1;
						infoPanel();
						TriggerFlag = true;
						Bufor[1]= 1;
					}
					else if(touchData.x >= x2 && touchData.x <= x2 + 130 && touchData.y >= y2+30 && touchData.y <= y2+30 + 20)
					{
						TriggerChosen = 2;
						infoPanel();
						TriggerFlag = true;
						Bufor[1]=0;
					}
				}
			}
			else if(MenuLevel == 1)
			{
				if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed)
				{
					if(touchData.x >= x1 && touchData.x <= x1 + 50 && touchData.y >= y1 && touchData.y <= y1 + 20)
					{
						SamplingFreq = 1;
						SamplingPause = 1;
						TIM_Configuration3(4,45);
						TIM_Configuration1(4,45);
						strcpy(SamplingFreqDiscriminant ,"M");
						FreqFlag = true;
					}
					else if(touchData.x >= x1 + 75 && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 && touchData.y <= y1 + 20)
					{
						SamplingFreq = 500;
						SamplingPause = 2;
						TIM_Configuration3(4,90);
						TIM_Configuration1(4,90);
						strcpy(SamplingFreqDiscriminant ,"k");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1 + 75*2 && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 && touchData.y <= y1 + 20)
					{
						SamplingFreq = 250;
						SamplingPause = 4;
						TIM_Configuration3(4,180);
						TIM_Configuration1(4,180);
						strcpy(SamplingFreqDiscriminant ,"k");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1  && touchData.x <= x1 + 50 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
					{
						SamplingFreq = 100;
						SamplingPause = 10;
						TIM_Configuration3(4,450);
						TIM_Configuration1(4,450);
						strcpy(SamplingFreqDiscriminant ,"k");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1 + 75  && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
					{
						SamplingFreq = 50;
						SamplingPause = 20;
						TIM_Configuration3(8,450);
						TIM_Configuration1(8,450);
						strcpy(SamplingFreqDiscriminant ,"k");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1 + 75*2  && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
					{
						SamplingFreq = 10;
						SamplingPause = 100;
						TIM_Configuration3(40,450);
						TIM_Configuration1(40,450);
						strcpy(SamplingFreqDiscriminant ,"k");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1  && touchData.x <= x1 + 50 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
					{
						SamplingFreq = 1;
						SamplingPause = 1000;
						TIM_Configuration3(40,4500);
						TIM_Configuration1(40,4500);
						strcpy(SamplingFreqDiscriminant ,"k");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1 + 75  && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
					{
						SamplingFreq = 500;
						SamplingPause = 2000;
						TIM_Configuration3(40,9000);
						TIM_Configuration1(40,9000);
						strcpy(SamplingFreqDiscriminant ," ");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1 + 75*2  && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
					{
						SamplingFreq = 250;
						SamplingPause = 4000;
						TIM_Configuration3(40,18000);
						TIM_Configuration1(40,18000);
						strcpy(SamplingFreqDiscriminant ," ");
						FreqFlag = true;
					}
					else if(touchData.x >= x1  && touchData.x <= x1 + 50 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
					{
						SamplingFreq = 100;
						SamplingPause = 10000;
						TIM_Configuration3(40,45000);
						TIM_Configuration1(40,45000);
						strcpy(SamplingFreqDiscriminant ," ");
						FreqFlag = true;
					} 
					else if(touchData.x >= x1 + 75  && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
					{
						SamplingFreq = 50;
						SamplingPause = 20000;
						TIM_Configuration3(80,45000);
						TIM_Configuration1(80,45000);
						strcpy(SamplingFreqDiscriminant ," ");
						FreqFlag = true;
					}  
					else if(touchData.x >= x1 + 75*2  && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
					{
						SamplingFreq = 10;
						SamplingPause = 100000;
						TIM_Configuration3(400,45000);
						TIM_Configuration1(400,45000);
						strcpy(SamplingFreqDiscriminant ," ");
						FreqFlag = true;
					}
				}
			}
			else if(MenuLevel == 2)
			{
				if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed)
				{
					if(touchData.x >= x1 && touchData.x <= x1 + 70 && touchData.y >= y1 && touchData.y <= y1 + 20)
					{
						Division = divisions[0];
						DivFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 70 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
					{
						Division = divisions[1];
						DivFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 70 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
					{
						Division = divisions[2];
						DivFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 70 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
					{
						Division = divisions[3];
						DivFlag = true;
					}
					else if(touchData.x >= x1 && touchData.x <= x1 + 70 && touchData.y >= y1 + 40*4 && touchData.y <= y1 + 40*4 + 20)
					{
						Division = divisions[4];
						DivFlag = true;
					}
				}
			}
			else if(MenuLevel == 3)
			{
				if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed)
				{
					if(touchData.x >= 20 && touchData.x <= 20 + 120 && touchData.y >= 280 && touchData.y <=  280 + 20)
						MeasureFlag = true;

				}
			}

			if(MenuLevel == 0 && TriggerFlag == true && ChannelFlag == true)
			{
				MenuLevel = 1;
				TriggerFlag = false;
				ChannelFlag = false;
				Delayms(250);
				infoPanel2();
			} 
			else if(MenuLevel == 1 && FreqFlag == true)
			{
				MenuLevel = 2;
				FreqFlag = false;
				Delayms(250);
				infoPanel3();

			}
			else if (MenuLevel == 2 && DivFlag == true)
			{
				MenuLevel = 3;
				DivFlag = false;

				if(Division/1000000>=1)
				{
					strcpy(DivisionDiscriminant," s");
					Division1 = Division / 1000000;	
				}
				else if(Division/1000>=1)
				{
					strcpy(DivisionDiscriminant,"ms");
					Division1 = Division / 1000;
				}
				else
				{
					Division1 = Division;
				}

				Delayms(250);
				infoPanel4();


			}
			else if(MenuLevel == 3 && MeasureFlag == true)
			{
				MenuLevel = 0;
				Menu = false;
				MeasureFlag = false;
								

				if (SamplingPause == 1 || SamplingPause == 2 || SamplingPause == 4 || SamplingPause == 10 || SamplingPause == 20)
					FastFreq = true;
				else
					SlowFreq = true;

				resetGUI();	
				GUIPreparation();
				TIM_Cmd(TIM4, ENABLE); //włącz licznik timera numer 3 (tego od pobierania danych)
				Download = true;
			}
		}
		else if ( Download == true )
		{

			if (SampleCount >= 4096)
			{
				TIM_Cmd(TIM3, DISABLE);
				TIM_Cmd(TIM4, DISABLE);
				Download = false;
				DrawSignal(StartPosition);
				infoPanel5();
				DownloadFinished = true;
			}
			if(Draw == true)
			{
				DrawSignal(StartPosition);
				infoPanel5();
				Draw = false;

			}
		}
		else if (DownloadFinished == true)
		{
			if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed)
			{
				if(touchData.x >= 160 && touchData.x <= 240 && touchData.y >= 0 && touchData.y <= 320)
				{
					// prawo
					if( (StartPosition + (int)(5*(Division/SamplingPause))) <= 4096)
					{
						StartPosition = StartPosition + (int)(5*(Division/SamplingPause));
						DrawSignal(StartPosition);
						shift++;
						infoPanel5();
						Delayms(250);
					}	
				}
				else if(touchData.x >= 0 && touchData.x <= 80 && touchData.y >= 0 && touchData.y <= 320)
				{
					//lewo
					if(StartPosition != 0) // bo pozostałę parametry sa stałe i nie da rady czegoś zchrzanić przy powrocie
					{
						StartPosition = StartPosition - (int)(5*(Division/SamplingPause));
						DrawSignal(StartPosition);
						shift--;
						infoPanel5();
						Delayms(250);
					}
				}
			}
		}
	}
}

void RCC_Configuration(void){



	RCC_DeInit();
	//Reset ustawieñ RCC

	RCC_HSEConfig(RCC_HSE_ON);
	//w³¹czenie HSE

	HSEStartupStatus = RCC_WaitForHSEStartUp();
		//czekaj jak HSE bedzie gotowy
	if(HSEStartupStatus == SUCCESS)
	{

		RCC_PLLConfig(RCC_PLLSource_HSE, 8 , 360 , 2 , 8);
		//konfiguracja PLL
		RCC_PLLCmd(ENABLE);
		//PLL enable
		RCC_GetFlagStatus(RCC_FLAG_PLLRDY == RESET);
		//Poczekaj a¿ PLL wystawi flage

		FLASH_SetLatency(FLASH_Latency_5);

		// ustawienie opóŸnienia flasha


		//ahb apb1 apb2

		//AHB clock = SYSCLK  APB2 clock = HCLK/2  APB1 clock = HCLK/4
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK1Config(RCC_HCLK_Div1);

		//wybranie systemowego zegara
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		FLASH_SetLatency(FLASH_Latency_5);


	}

}

void NVIC_Configuration(void)
{


	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure1.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);




	NVIC_InitStructure3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure3);
}

void GPIO_Configuration(void)
{
	GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_9;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitDef);

	GPIO_InitDef1.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 ;
	GPIO_InitDef1.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitDef1.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitDef1);

	GPIO_InitDef2.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 ;
	GPIO_InitDef2.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitDef2.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitDef1);


//CH0 = G13 | CH1 = G14 | CH2 = E2 | CH3 = E3 | CH4 = E4 | CH5 = E5 | CH6 = E6 | CH7 = G9 |
}

void TIM_Configuration(void)
{

	TIM_TimeBaseStructure.TIM_Period = 4000 - 1;  // 4 kHz down to 1 Hz (1 s)
	TIM_TimeBaseStructure.TIM_Prescaler = 45000 - 1; // 180 MHz Clock down to 4 kHz (adjust per your clock)
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	  /* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	  /* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

void TIM_Configuration3(int Period, int Prescaler)
{

	TIM_TimeBaseStructure3.TIM_Period = Period - 1;  // 4 kHz down to 1 Hz (1 s)
	TIM_TimeBaseStructure3.TIM_Prescaler = Prescaler - 1; // 180 MHz Clock down to 4 kHz (adjust per your clock)
	TIM_TimeBaseStructure3.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure3.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure3);
	  /* TIM IT enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	  /* TIM2 enable counter */
	TIM_Cmd(TIM3, DISABLE);
}

void TIM_Configuration1(int Period, int Prescaler)
{

	TIM_TimeBaseStructure1.TIM_Period = Period - 1;  // 4 kHz down to 1 Hz (1 s)
	TIM_TimeBaseStructure1.TIM_Prescaler = Prescaler - 1; // 180 MHz Clock down to 4 kHz (adjust per your clock)
	TIM_TimeBaseStructure1.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure1.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure1);
	  /* TIM IT enable */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	  /* TIM2 enable counter */
	TIM_Cmd(TIM4, DISABLE);
}




void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//		GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
//		GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
		//if(SlowFreq == true && Download == true && SampleCount <= 4096)
		if(Download == true && SampleCount <= 4096)
			Draw = true;
	}
}


void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	//	if(Download == true && Trigger == false)
		if(Trigger == false)
		{
			switch (ChannelChosen)
			{
				case 0: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1);
					break;
				case 1: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3);
					break;
				case 2: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
					break;
				case 3: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
					break;
				case 4: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8);
					break;
				case 5: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11);
					break;
				case 6: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12);
					break;
				case 7: 
					Bufor[0] = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
					break;
				default:
					break;
			}

			if(Bufor[0] > Bufor[1] && TriggerChosen == 1)
			{						
				Trigger = true;
				TIM_Cmd(TIM4, DISABLE);
				TIM_Cmd(TIM3, ENABLE);
			}
			else if(Bufor[0] < Bufor[1] && TriggerChosen == 2)
			{
				Trigger = true;
				TIM_Cmd(TIM4, DISABLE);
				TIM_Cmd(TIM3, ENABLE);
			}

			Bufor[1] = Bufor[0];

		}
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

		if(Download == true && Trigger == true)
		{
			DataInt[SampleCount] = GPIO_ReadInputData(GPIOC);
			SampleCount++;
		}
	}
}

void DMA2D_Configuration(void)
{
	TM_DELAY_Init();

	    /* Initialize ILI9341 with LTDC */
	    /* By default layer 1 is used */
	TM_ILI9341_Init();
	    /* Initialize DMA2D GRAPHIC library */
	TM_DMA2DGRAPHIC_Init();

	    /* Set ILI9341 Orientation */
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Portrait_2);
	    /* Set DMA2D GRAPHIC library Orientation */
	TM_DMA2DGRAPHIC_SetOrientation((uint8_t)TM_ILI9341_Orientation_Portrait_2);

	TM_DMA2DGRAPHIC_Fill(GRAPHIC_COLOR_BLACK);





}

void GUIPreparation(void)
{
	char s[5];
	int i = 0;

	strncpy(text1, "fs= ", 10);
	itoa(SamplingFreq,s,10);
	strcpy(text1,strcat(text1, s));
	strcpy(text1,strcat(text1, SamplingFreqDiscriminant));
	strcpy(text1,strcat(text1, "Hz"));

	itoa(Division1,s,10);
	strcpy(text2,strcat(text2, s));
	strcpy(text2,strcat(text2, DivisionDiscriminant));
	strcpy(text2,strcat(text2, "/d"));



	TM_DMA2DGRAPHIC_Fill(GRAPHIC_COLOR_BLACK);
	TM_DMA2DGRAPHIC_DrawPolyLine(Coordinates2, 5, GRAPHIC_COLOR_WHITE);
	TM_ILI9341_Puts( 0,10, text1, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 80,10, text2, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);


	for(i=0; i<8; i++)
		TM_ILI9341_Puts( 1,40+i*35, Channels[i], &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	for(i=0;i<10; i++)
		TM_DMA2DGRAPHIC_DrawVerticalLine(46+20*i,21, 277,GRAPHIC_COLOR_WHITE);
}

void DrawGUIBackground(void)
{
	int i =  0;

	TM_DMA2DGRAPHIC_Fill(GRAPHIC_COLOR_BLACK);
	TM_DMA2DGRAPHIC_DrawPolyLine(Coordinates2, 5, GRAPHIC_COLOR_WHITE);
	TM_ILI9341_Puts( 0,10, text1, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 70,10, text2, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 130,10, text3, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	


}

void DrawSignal(int startPosition)
{
	

	float a = (float)SamplingPause;
	float b = (float)Division;
	int j = 0;
	int i = 0;
	float c = 20/(b/a); // ile pixeli co próbke
	char s[5];


	TM_DMA2DGRAPHIC_DrawFilledRectangle(26, 21,  200, 279, GRAPHIC_COLOR_BLACK);
	for(i=0;i<10; i++)
		TM_DMA2DGRAPHIC_DrawVerticalLine(46+20*i,21, 278,GRAPHIC_COLOR_WHITE);

	for(i = 0; i < SampleCount; ++i)
	{
		DataMatching(i);
	}

// b/a = ile próbek na działke    10 to ilość działek
	//if( b/a*10<SampleCount)   //maluj do oporu
	if( (20/c)*10<SampleCount)   //maluj do oporu
	{
		for(i = 0; i< ((int)b/a)*10 ; i++)
		{
			if(i == startPosition)
			{
				for(j = 0; j<8; j++)
				{
					
					 
					if((int)(Data[i] & (1<< j) ) == 0)
						TM_DMA2DGRAPHIC_DrawHorizontalLine(26,50+j*35,c,GRAPHIC_COLOR_RED);
					else
						TM_DMA2DGRAPHIC_DrawHorizontalLine(26,30+j*35,c,GRAPHIC_COLOR_RED);
				}
			}
			else
			{

				for(j = 0; j<8; j++)
				{
					if(j == 0)
					{	
						if(((int)( Data[i+startPosition] & (1<< j)) == 0) && ((int)(Data[i-1+startPosition] & (1<< j)) == 0))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j)) == 1) && ((int)(Data[i-1+startPosition] & (1<< j)) == 1))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j)) == 0) && ((int)(Data[i-1+startPosition] & (1<< j)) == 1 ))
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);							
						}
						else 
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);	
						}
					}
					else
					{
						if(( (int)( Data[i+startPosition] & (1<< j))>>j == 0) && ((int)(Data[i-1+startPosition] & (1<< j))>>j == 0))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j))>>j == 1) && ((int)(Data[i+startPosition-1] & (1<< j))>>j == 1))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j))>>j == 0) && ((int)(Data[i-1+startPosition] & (1<< j))>>j == 1 ))
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);							
						}
						else 
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);	
						}
					}
				}
			}
		}
	}
	else	//maluj tyle ile masz próbek
	{
		for(i = 0; i< SampleCount ; i++)
		{
			if(i == startPosition)
			{
				for(j = 0; j<8; j++)
				{
					
					 
					if((int)(Data[i] & (1<< j) ) == 0)
						TM_DMA2DGRAPHIC_DrawHorizontalLine(26,50+j*35,c,GRAPHIC_COLOR_RED);
					else
						TM_DMA2DGRAPHIC_DrawHorizontalLine(26,30+j*35,c,GRAPHIC_COLOR_RED);
				}
			}
			else
			{

				for(j = 0; j<8; j++)
				{
					if(j == 0)
					{	
						if(((int)( Data[i+startPosition] & (1<< j)) == 0) && ((int)(Data[i-1+startPosition] & (1<< j)) == 0))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j)) == 1) && ((int)(Data[i-1+startPosition] & (1<< j)) == 1))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j)) == 0) && ((int)(Data[i-1+startPosition] & (1<< j)) == 1 ))
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);							
						}
						else 
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);	
						}
					}
					else
					{
						if(( (int)( Data[i+startPosition] & (1<< j))>>j == 0) && ((int)(Data[i-1+startPosition] & (1<< j))>>j == 0))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[i+startPosition] & (1<< j))>>j == 1) && ((int)(Data[i-1+startPosition] & (1<< j))>>j == 1))
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
						else if(((int)(Data[+startPosition] & (1<< j))>>j == 0) && ((int)(Data[i-1] & (1<< j))>>j == 1 ))
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,50+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);							
						}
						else 
						{
							TM_DMA2DGRAPHIC_DrawHorizontalLine(26+i*c,30+j*35,c,GRAPHIC_COLOR_RED);
							TM_DMA2DGRAPHIC_DrawVerticalLine(26+i*c,30+j*35,20,GRAPHIC_COLOR_RED);	
						}
					}
				}
			}
		}	

	}

	LastData = Data[i];
	itoa(LastData,s,16);
	strcpy(text3, "LastValue=0x");
	strcpy(text3,strcat(text3,s));
	TM_ILI9341_Puts( 140,10, text3, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);


}


void StartMenu(void)
{
	char s[5];
	int i = 0;
	short int x1 = 20;
	short int y1 = 50;

	short int x2 = 70;
	short int y2 = 50;

	TM_ILI9341_Puts( 0,10, " Wybierz zrodlo kanalu i krawedz", &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 0,20, "     sygnalu wyzwalajacego", &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 0,35, "kanaly", &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 100,35, "wyzwalanie", &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);

	for(i = 0; i<8; i++)
		TM_ILI9341_Puts( x1+4,y1+7+i*30, Channels[i], &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);

	for(i = 0; i<8; i++)
		TM_DMA2DGRAPHIC_DrawRectangle(x1, y1 + 30*i, 30, 20, GRAPHIC_COLOR_WHITE);

	for (i = 0; i <2; i++)
		TM_DMA2DGRAPHIC_DrawRectangle(x2, y2 + 30*i, 130, 20, GRAPHIC_COLOR_WHITE);		
	
	for(i = 0; i<2; i++)
		TM_ILI9341_Puts( x2+4,y1+7+i*30, Triggers[i], &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);



}

void fillChannelsText(void)
{
	strncpy(Channels[0], "Ch0",4);
	strncpy(Channels[1], "Ch1",4);
	strncpy(Channels[2], "Ch2",4);
	strncpy(Channels[3], "Ch3",4);
	strncpy(Channels[4], "Ch4",4);
	strncpy(Channels[5], "Ch5",4);
	strncpy(Channels[6], "Ch6",4);
	strncpy(Channels[7], "Ch7",4);
}

void fillTriggersText(void)
{
	strncpy(Triggers[0], "Krawedz rosnaca",20);
	strncpy(Triggers[1], "Krawedz opadajaca",20);
}

void infoPanel(void)
{

	resetInfoPanel();
	char s[5];

	itoa(ChannelChosen,s,10);
	strcpy(text4,"kanal: ");
	strcpy(text4,strcat(text4, s));

	itoa(TriggerChosen,s,10);
	strcpy(text5,"trigger: ");
	strcpy(text5,strcat(text5, s));

	TM_ILI9341_Puts( 10,290, text4, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 100,290, text5, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);

}

void resetInfoPanel(void)
{
	TM_DMA2DGRAPHIC_DrawFilledRectangle(10, 290 , 200, 20, GRAPHIC_COLOR_BLACK);
}

void resetGUI(void)
{
	TM_DMA2DGRAPHIC_DrawFilledRectangle(0,0 , 240, 320, GRAPHIC_COLOR_BLACK);
}


void TouchDataConfiguration(void)
{
	TM_STMPE811_Init();
	touchData.orientation = TM_STMPE811_Orientation_Portrait_2;
}

void infoPanel2(void)
{
	char s[5];
	int i = 0;
	int j = 0;
	int k = 0;
	short int x1 = 20;
	short int y1 = 50;
	resetGUI();

	TM_ILI9341_Puts( 0,10, " Wybierz czestotliwosc probkowania", &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 3; ++j)
		{	
			TM_DMA2DGRAPHIC_DrawRectangle(x1 + 75*j, y1 +40*i , 50, 20, GRAPHIC_COLOR_WHITE);
			TM_ILI9341_Puts( x1+4 + 75*j , y1+7 + 40*i, Frequencies[k], &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
			k++;

		}
	}
}

void fillFreqList(void)
{
	strcpy(Frequencies[0], "1MHz");
	strcpy(Frequencies[1], "500kHz");
	strcpy(Frequencies[2], "250kHz");
	strcpy(Frequencies[3], "100kHz");
	strcpy(Frequencies[4], "50kHz");
	strcpy(Frequencies[5], "10kHz");
	strcpy(Frequencies[6], "1kHz");
	strcpy(Frequencies[7], "500Hz");
	strcpy(Frequencies[8], "250Hz");
	strcpy(Frequencies[9], "100Hz");
	strcpy(Frequencies[10], "50Hz");
	strcpy(Frequencies[11], "10Hz");

}

void infoPanel3(void)
{
	char s[15];
	int i = 0;
	int j = 0;
	int k = 0;
	short int x1 = 20;
	short int y1 = 50;
	resetGUI();

	strncpy(text1, "fs= ", 10);
	itoa(SamplingFreq,s,10);
	strcpy(text1,strcat(text1, s));
	strcpy(text1,strcat(text1, SamplingFreqDiscriminant));
	strcpy(text1,strcat(text1, "Hz"));


	TM_ILI9341_Puts( 0,10, "Wybierz podstawe czasu", &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 0,20, "Wybrana czestotliwosc probkowania:" , &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	TM_ILI9341_Puts( 75,30, text1 , &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	
	generateDivision();

}

void generateDivision(void)
{

	long int maxDivision; 
	maxDivision = 20*SamplingPause; //w mikrosekundach
	int i = 0;
	
	short int x1 = 20;
	short int y1 = 50;
	char s[15];
	int temp; //tymczasowa zamiana na inne jednostki czasu z divisions[i]
	char tempDis[4]; //tymczasowa zamiana na inny wyroznik niz usekundy

	divisions[0] = maxDivision;

	for(i = 0; i <5; i++)
	{
		if(i>0)
			divisions[i]=divisions[i-1]/2;

		if(divisions[i]/1000000>=1)
		{
			strcpy(tempDis," s");
			itoa(divisions[i]/1000000,s,10);	
		}
		else if(divisions[i]/1000>=1)
		{
			strcpy(tempDis,"ms");
			itoa(divisions[i]/1000,s,10);
		}
		else
		{
			strcpy(tempDis,"us");
			itoa(divisions[i],s,10);
		}


			
		
			
		strcpy(s,strcat(s,tempDis));
		strcpy(s,strcat(s,"/d"));

		TM_DMA2DGRAPHIC_DrawRectangle(x1 , y1 +40*i , 70, 20, GRAPHIC_COLOR_WHITE);
		TM_ILI9341_Puts( x1+4  , y1+7 + 40*i, s , &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
	}
}

void infoPanel4(void)
{
	char s[15];

	strcpy(s,"pomiar");
	TM_DMA2DGRAPHIC_DrawRectangle(20 , 280 , 120, 20, GRAPHIC_COLOR_WHITE);
	TM_ILI9341_Puts( 24  , 287 , s , &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);

}

void DataMatching(int a)
{
	if((IS_BIT_SET(DataInt[a],1) >> 1 ) == 0)
		RESET_BIT(Data[a],0);
	else
		SET_BIT(Data[a],0);

	if((IS_BIT_SET(DataInt[a],3) >> 3 ) == 0)
		RESET_BIT(Data[a],1);
	else
		SET_BIT(Data[a],1);

	if((IS_BIT_SET(DataInt[a],4) >> 4 ) == 0)
		RESET_BIT(Data[a],2);
	else
		SET_BIT(Data[a],2);

	if((IS_BIT_SET(DataInt[a],5) >> 5) == 0)
		RESET_BIT(Data[a],3);
	else
		SET_BIT(Data[a],3);

	if((IS_BIT_SET(DataInt[a],8) >>8 ) == 0)
		RESET_BIT(Data[a],4);
	else
		SET_BIT(Data[a],4);

	if((IS_BIT_SET(DataInt[a],11) >> 11 ) == 0)
		RESET_BIT(Data[a],5);
	else
		SET_BIT(Data[a],5);

	if((IS_BIT_SET(DataInt[a],12) >> 12 ) == 0)
		RESET_BIT(Data[a],6);
	else
		SET_BIT(Data[a],6);

	if((IS_BIT_SET(DataInt[a],13) >> 13 ) == 0)
		RESET_BIT(Data[a],7);
	else
		SET_BIT(Data[a],7);
}

void infoPanel5(void)
{
	char s[15];
	char s1[10];
	
	strcpy(s,"+");
	itoa((shift*5*Division1),s1,10);
	strcpy(s,strcat(s,s1));
	strcpy(s,strcat(s, DivisionDiscriminant));
	resetInfoPanel5();
	TM_ILI9341_Puts( 110,305, s, &TM_Font_7x10, ILI9341_COLOR_ORANGE, ILI9341_COLOR_BLACK);
}

void resetInfoPanel5(void)
{
	TM_DMA2DGRAPHIC_DrawFilledRectangle(100, 304 , 100, 10, GRAPHIC_COLOR_BLACK);
}
