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

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
ErrorStatus HSEStartupStatus;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitDef;
TM_STMPE811_TouchData touchData;

TM_DMA2DRAPHIC_Poly_t Coordinates2[] = {
       {25, 40},
       {25, 300},
       {65, 300},
       {65, 40},
       {25, 40}
   };



void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void TIM_Configuration(void);
void DMA2D_Configuration(void);
void TIM2_IRQHandler(void);
void fillChannelsText(void);

//-------------------
void TouchDataConfiguration(void);
void StartMenu(void);
void fillTriggersText(void);
void infoPanel(void);
void infoPanel2(void);
void infoPanel3(void);
void infoPanel4(void);
void resetInfoPanel(void);
void resetGUI(void);
void fillFreqList(void);
void generateDivision(void);

short int SamplingFreq = 1;
//int SamplingFreqHz = 0; //sampling freg w herzach
int SamplingPause = 1;   //w mikrosekundach
char SamplingFreqDiscriminant[2] = "M";

int Division = 20;
int Division1 = 20;		//w mikrosekundach
char DivisionDiscriminant[2] = "u";

char text1[10];
char text2[10];
char text3[15];

char text4[10];
char text5[10];

char Channels[8][4];

char Triggers[2][20];
char Frequencies[12][7];
long int divisions[5];

short int ChannelChosen=0;
short int TriggerChosen=0;

short int MenuLevel=0;
bool ChannelFlag = false;
bool TriggerFlag = false;
bool FreqFlag = false;
bool DivFlag = false;
bool MeasureFlag = false;

int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

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
				}
				else if(touchData.x >= x2 && touchData.x <= x2 + 130 && touchData.y >= y2+30 && touchData.y <= y2+30 + 20)
				{
					TriggerChosen = 2;
					infoPanel();
					TriggerFlag = true;
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
					strcpy(SamplingFreqDiscriminant ,"M");
					FreqFlag = true;
				}
				else if(touchData.x >= x1 + 75 && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 && touchData.y <= y1 + 20)
				{
					SamplingFreq = 500;
					SamplingPause = 2;
					strcpy(SamplingFreqDiscriminant ,"k");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1 + 75*2 && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 && touchData.y <= y1 + 20)
				{
					SamplingFreq = 250;
					SamplingPause = 4;
					strcpy(SamplingFreqDiscriminant ,"k");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1  && touchData.x <= x1 + 50 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
				{
					SamplingFreq = 100;
					SamplingPause = 10;
					strcpy(SamplingFreqDiscriminant ,"k");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1 + 75  && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
				{
					SamplingFreq = 50;
					SamplingPause = 20;
					strcpy(SamplingFreqDiscriminant ,"k");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1 + 75*2  && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 + 40 && touchData.y <= y1 + 40 + 20)
				{
					SamplingFreq = 10;
					SamplingPause = 100;
					strcpy(SamplingFreqDiscriminant ,"k");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1  && touchData.x <= x1 + 50 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
				{
					SamplingFreq = 1;
					SamplingPause = 1000;
					strcpy(SamplingFreqDiscriminant ,"k");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1 + 75  && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
				{
					SamplingFreq = 500;
					SamplingPause = 2000;
					strcpy(SamplingFreqDiscriminant ," ");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1 + 75*2  && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 + 40*2 && touchData.y <= y1 + 40*2 + 20)
				{
					SamplingFreq = 250;
					SamplingPause = 4000;
					strcpy(SamplingFreqDiscriminant ," ");
					FreqFlag = true;
				}
				else if(touchData.x >= x1  && touchData.x <= x1 + 50 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
				{
					SamplingFreq = 100;
					SamplingPause = 10000;
					strcpy(SamplingFreqDiscriminant ," ");
					FreqFlag = true;
				} 
				else if(touchData.x >= x1 + 75  && touchData.x <= x1 + 75 + 50 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
				{
					SamplingFreq = 50;
					SamplingPause = 20000;
					strcpy(SamplingFreqDiscriminant ," ");
					FreqFlag = true;
				}  
				else if(touchData.x >= x1 + 75*2  && touchData.x <= x1 + 75*2 + 50 && touchData.y >= y1 + 40*3 && touchData.y <= y1 + 40*3 + 20)
				{
					SamplingFreq = 10;
					SamplingPause = 100000;
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
			Delayms(500);
			infoPanel2();
		} 
		else if(MenuLevel == 1 && FreqFlag == true)
		{
			MenuLevel = 2;
			FreqFlag = false;
			Delayms(500);
			infoPanel3();

		}
		else if (MenuLevel == 2 && DivFlag == true)
		{
			MenuLevel = 3;
			DivFlag = false;
			Delayms(500);
			infoPanel4();


		}
		else if(MenuLevel == 3 && MeasureFlag == true)
		{
			MenuLevel = 4;
			MeasureFlag = false;
			Delayms(500);
			resetGUI();	
			//uruchomić pomiar
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
}

void GPIO_Configuration(void)
{
	GPIO_InitDef.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOG, &GPIO_InitDef);
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

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
		GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
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

//------------
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

/*
    while (1) {
        if (TM_STMPE811_ReadTouch(&touchData) == TM_STMPE811_State_Pressed) {
            //Touch valid
            sprintf(str, "Pressed    \n\nX: %03d\nY: %03d", touchData.x, touchData.y);
            TM_ILI9341_Puts(20, 80, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_ORANGE);
 
            
            TM_ILI9341_DrawPixel(touchData.x, touchData.y, 0x0000);
        } else {
            sprintf(str, "Not Pressed\n\n       \n      ");
            TM_ILI9341_Puts(20, 80, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_ORANGE);
        }
    */
