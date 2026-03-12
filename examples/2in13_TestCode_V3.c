#include "Test.h"
#include "EPD_2in13_V3.h"
#include "GT1151.h"
#include <stdio.h>
#include <stdlib.h>

extern GT1151_Dev Dev_Now, Dev_Old;
extern int IIC_Address;

int TestCode_2in13_V3(void)
{
	IIC_Address = 0x14;
	
	UDOUBLE i=0, j=0, k=0;
	UBYTE Page=0;
	UBYTE ReFlag=0, SelfFlag=0;
    
	DEV_ModuleInit();
	
    EPD_2in13_V3_Init(EPD_2IN13_V3_FULL);
    EPD_2in13_V3_Clear();
	
	GT_Init();
	DEV_Delay_ms(100);
    
    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 0, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_SetMirroring(MIRROR_ORIGIN);
    Paint_Clear(WHITE);
	EPD_2in13_V3_Display(BlackImage);
	EPD_2in13_V3_Init(EPD_2IN13_V3_PART);
	EPD_2in13_V3_Display_Partial_Wait(BlackImage);

	while(1) {
        // Poll Touch interrupt pin
        if(DEV_Digital_Read(INT) == 0) {
            Dev_Now.Touch = 1;
        } else {
            Dev_Now.Touch = 0;
        }

		if(i > 12 || ReFlag == 1) {
			if(Page == 1 && SelfFlag != 1)
				EPD_2in13_V3_Display_Partial(BlackImage);
			else 
				EPD_2in13_V3_Display_Partial_Wait(BlackImage);
			i = 0;
			k = 0;
			j++;
			ReFlag = 0;
			printf("*** Draw Refresh ***\r\n");
		}else if(k++>3000000 && i>0 && Page == 1) { // Adjusted delay for RP2350
			EPD_2in13_V3_Display_Partial(BlackImage);
			i = 0;
			k = 0;
			j++;
			printf("*** Overtime Refresh ***\r\n");
		}else if(j > 100 || SelfFlag) {
			SelfFlag = 0;
			j = 0;
			EPD_2in13_V3_Init(EPD_2IN13_V3_FULL);
			EPD_2in13_V3_Display_Base(BlackImage);
			EPD_2in13_V3_Init(EPD_2IN13_V3_PART);
			printf("--- Self Refresh ---\r\n");
		}
		
		if(GT_Scan()==1 || (Dev_Now.X[0] == Dev_Old.X[0] && Dev_Now.Y[0] == Dev_Old.Y[0])) { // No new touch
			continue;
		}

		if(Dev_Now.TouchpointFlag) {
			i++;
			Dev_Now.TouchpointFlag = 0;

			if(Page == 0  && ReFlag == 0) {	//main menu
				if(Dev_Now.X[0] > 29 && Dev_Now.X[0] < 92 && Dev_Now.Y[0] > 153 && Dev_Now.Y[0] < 193) {
					printf("Draw ...\r\n");
					Page = 1;
					ReFlag = 1;
				}
			}

			if(Page == 1 && ReFlag == 0) {	//white board
				Paint_DrawPoint(Dev_Now.X[0], Dev_Now.Y[0], BLACK, Dev_Now.S[0]/8+1, DOT_STYLE_DFT);
				
				if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 118 && Dev_Now.Y[0] > 6 && Dev_Now.Y[0] < 30) {
					printf("Home ...\r\n");
					Page = 1;
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 118 && Dev_Now.Y[0] > 113 && Dev_Now.Y[0] < 136) {
					printf("Clear ...\r\n");
					Page = 0;
					ReFlag = 1;
				}
				else if(Dev_Now.X[0] > 96 && Dev_Now.X[0] < 118 && Dev_Now.Y[0] > 220 && Dev_Now.Y[0] < 242) {
					printf("Refresh ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
			}

		}
	}
	return 0;
}

