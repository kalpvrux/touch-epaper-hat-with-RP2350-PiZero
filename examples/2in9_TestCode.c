#include "Test.h"
#include "EPD_2in9_V2.h"
#include "ICNT86X.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern ICNT86_Dev ICNT86_Dev_Now, ICNT86_Dev_Old;
extern int IIC_Address;

UBYTE *BlackImage;

int TestCode_2in9(void)
{
	IIC_Address = 0x48;
	
	UDOUBLE i=0, j=0, k=0;
	UBYTE Page=0;
	UBYTE ReFlag=0, SelfFlag=0;
    
	DEV_ModuleInit();

    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();
	
	ICNT_Init();
	DEV_Delay_ms(100);
    
    //Create a new image cache
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0)? (EPD_2IN9_V2_WIDTH / 8 ): (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

	EPD_2IN9_V2_Display_Base(BlackImage);
    
	while(1) {
        // Poll Touch interrupt pin
        if(DEV_Digital_Read(INT) == 0) {
            ICNT86_Dev_Now.Touch = 1;
        } else {
            ICNT86_Dev_Now.Touch = 0;
        }

		if(i > 30 || ReFlag == 1) {
			if(Page == 1 && SelfFlag != 1) {
				EPD_2IN9_V2_Display_Partial(BlackImage);
				i = 0;
				k = 0;
				j++;
				ReFlag = 0;
				printf("*** Draw Refresh ***\r\n");
			}
			else {
				EPD_2IN9_V2_Display_Partial_Wait(BlackImage);
				i = 0;
				k = 0;
				j++;
				ReFlag = 0;
				printf("*** Touch Refresh ***\r\n");
			}
		}else if(k++>3000000 && i>0 && Page == 1) { // Adjusted delay for RP2350
			EPD_2IN9_V2_Display_Partial(BlackImage);
			i = 0;
			k = 0;
			j++;
			printf("*** Overtime Refresh ***\r\n");
		}else if(j > 100 || SelfFlag) {
			SelfFlag = 0;
			j = 0;
			EPD_2IN9_V2_Init();
			EPD_2IN9_V2_Display_Base(BlackImage);
			printf("--- Self Refresh ---\r\n");
		}
		
		if(ICNT_Scan()==1 || (ICNT86_Dev_Now.X[0] == ICNT86_Dev_Old.X[0] && ICNT86_Dev_Now.Y[0] == ICNT86_Dev_Old.Y[0])) { // No new touch
			continue;
		}

		if(ICNT86_Dev_Now.TouchCount) {
			i++;
			if(Page == 0  && ReFlag == 0) {	//main menu
				if(ICNT86_Dev_Now.X[0] > 39 && ICNT86_Dev_Now.X[0] < 80 && ICNT86_Dev_Now.Y[0] > 31 && ICNT86_Dev_Now.Y[0] < 96) {
					printf("Draw ...\r\n");
					Page = 1;
					ReFlag = 1;
				}
			}

			if(Page == 1 && ReFlag == 0) {	//white board
				Paint_DrawPoint(ICNT86_Dev_Now.X[0], ICNT86_Dev_Now.Y[0], BLACK, 3, DOT_STYLE_DFT);
				
				if(ICNT86_Dev_Now.X[0] > 136 && ICNT86_Dev_Now.X[0] < 159 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Home ...\r\n");
					Page = 0;
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 266 && ICNT86_Dev_Now.X[0] < 289 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Clear ...\r\n");
					Page = 1;
					ReFlag = 1;
				}
				else if(ICNT86_Dev_Now.X[0] > 5 && ICNT86_Dev_Now.X[0] < 27 && ICNT86_Dev_Now.Y[0] > 101 && ICNT86_Dev_Now.Y[0] < 124) {
					printf("Refresh ...\r\n");
					SelfFlag = 1;
					ReFlag = 1;
				}
			}

		}
	}
	return 0;
}

