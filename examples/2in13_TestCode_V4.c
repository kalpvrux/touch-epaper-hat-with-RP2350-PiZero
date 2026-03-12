#include "EPD_2in13_V4.h"
#include "GT1151.h"
#include "GUI_Paint.h"
#include "DEV_Config.h"
#include "Test.h"
#include <stdlib.h>
#include <stdio.h>


extern GT1151_Dev Dev_Now, Dev_Old;
extern int IIC_Address;

int TestCode_2in13_V4(void) {
  IIC_Address = 0x14;

  UDOUBLE i = 0, j = 0, k = 0;
  UBYTE Page = 0, Photo_L = 0, Photo_S = 0;
  UBYTE ReFlag = 0, SelfFlag = 0;
  DEV_ModuleInit();

  EPD_2in13_V4_Init(EPD_2IN13_V4_FULL);

  EPD_2in13_V4_Clear();

  GT_Init();
  DEV_Delay_ms(100);
  // Create a new image cache
  UBYTE *BlackImage;
  // Allocation size remains the same (physical pixels)
  UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0) ? (EPD_2in13_V4_WIDTH / 8) : (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    return -1;
  }
  printf("Paint_NewImage\r\n");
  // Use ROTATE_270 for a complete Landscape view
  Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, ROTATE_270, WHITE);
  Paint_SelectImage(BlackImage);
  Paint_SetMirroring(MIRROR_NONE); // Disable mirror in landscape usually to prevent coordinate flipping
  Paint_Clear(WHITE);
  EPD_2in13_V4_Display(BlackImage);
  
  EPD_2in13_V4_Init(EPD_2IN13_V4_PART);
  DEV_Delay_ms(100);

  int current_scene = 0;
  int num_scenes = 3;
  UBYTE force_redraw = 1;

  printf("Gallery Ready!\r\n");

  while (1) {
    if (DEV_Digital_Read(INT) == 0) {
      Dev_Now.Touch = 1;
    } else {
      Dev_Now.Touch = 0;
    }

    if (force_redraw) {
       Paint_Clear(WHITE);
       
       // Canvas is now 250(Width) x 122(Height)
       if (current_scene == 0) {
           // House
           Paint_DrawRectangle(80, 50, 170, 110, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY); // Body
           Paint_DrawLine(80, 50, 125, 20, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID); // Roof
           Paint_DrawLine(170, 50, 125, 20, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
           Paint_DrawRectangle(110, 80, 140, 110, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Door
       } else if (current_scene == 1) {
           // Robot
           Paint_DrawRectangle(70, 20, 180, 100, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY); // Head
           Paint_DrawRectangle(100, 40, 120, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Eye 1
           Paint_DrawRectangle(130, 40, 150, 60, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Eye 2
           Paint_DrawLine(100, 80, 150, 80, BLACK, DOT_PIXEL_3X3, LINE_STYLE_SOLID); // Mouth
       } else if (current_scene == 2) {
           // Car
           Paint_DrawRectangle(80, 60, 180, 100, BLACK, DOT_PIXEL_3X3, DRAW_FILL_EMPTY); // Body
           Paint_DrawRectangle(100, 30, 160, 60, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY); // Top
           Paint_DrawCircle(100, 100, 15, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Wheel
           Paint_DrawCircle(160, 100, 15, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL); // Wheel
       }

       // Draw UI Frame (Landscape bounds: 250 wide by 122 height)
       // Left UI Side <
       Paint_DrawLine(40, 0, 40, 122, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
       Paint_DrawString_EN(10, 50, "<", &Font24, WHITE, BLACK);
       
       // Right UI Side >
       Paint_DrawLine(210, 0, 210, 122, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
       Paint_DrawString_EN(220, 50, ">", &Font24, WHITE, BLACK);

       // Draw Page Indicator Top Center
       char page_text[10];
       sprintf(page_text, "%d/%d", current_scene + 1, num_scenes);
       Paint_DrawString_EN(110, 5, page_text, &Font16, WHITE, BLACK);

       // Use FULL refresh for perfect clarity instead of partial, to avoid ghosting
       EPD_2in13_V4_Init(EPD_2IN13_V4_FULL);
       EPD_2in13_V4_Display(BlackImage);
       
       force_redraw = 0;
       
       // Flush touch buffer to prevent "double tap" registering from the finger release 
       // that happened while the display was stuck updating for 2+ seconds.
       while (DEV_Digital_Read(INT) == 0) {
           Dev_Now.Touch = 1;
           GT_Scan();
           DEV_Delay_ms(10);
       }
       Dev_Now.TouchpointFlag = 0;
       DEV_Delay_ms(50);
    }

    if (GT_Scan() == 1) { 
      DEV_Delay_ms(10);
      continue;
    }

    if (Dev_Now.TouchpointFlag) {
      Dev_Now.TouchpointFlag = 0;
      // Raw coordinates from the touch pad.
      // Physical orientation: 0,0 is at top-left of portrait orientation.
      // In ROTATE_270 landscape: X (width=250) roughly corresponds to raw Y (0=top right/bottom depending).
      // Y (height=122) roughly corresponds to raw X.
      // When screen is rotated 270 (counter-clockwise 90 from portrait), 
      // visual Landscape Left is physical Bottom.
      // visual Landscape Right is physical Top.
      
      int raw_x = Dev_Now.X[0];
      int raw_y = Dev_Now.Y[0];
      
      // Tap targets!
      // In portrait, raw_y near 250 is bottom physically.
      // After ROTATE_270, physical Bottom turns out to be the Right side!
      // Physical Top (near 0) turns out to be the Left side!
      
      if (raw_y > 170) {
          // physical Bottom = touching Right side (>)
          printf("Next Image\r\n");
          current_scene = (current_scene + 1) % num_scenes;
          force_redraw = 1;
      } else if (raw_y < 80) {
          // physical Top = touching Left side (<)
          printf("Prev Image\r\n");
          current_scene = (current_scene - 1 + num_scenes) % num_scenes;
          force_redraw = 1;
      }
    }
  }

  return 0;
}
