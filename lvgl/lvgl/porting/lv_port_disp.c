/**
 * @file lv_port_disp.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "main.h"
/*********************
 *      DEFINES
 *********************/
#define LVGL_SCREEN_MODE		0 
// 0  - LCDC (全屏) 双缓冲 SDRAM 
// 1  - LCDC (全屏) 单缓冲 SDRAM

#define LVGL_ROTATE_90 			0 //0-正常		1-旋转90°(即横屏竖屏转换)
/* 注意: 若想旋转屏幕，比如横屏转竖屏应用，需要对调 LCD_HDOT 与 LCD_VDOT 的值，
   同时仍须确保 LCD 输出时序符合原屏幕波形，例：800 * 480 翻转屏幕时需要保持 LCD 初始化结构体与下类似
    LCD_initStruct.HnPixel = 800;// 800 == LV_VER_RES_MAX
    LCD_initStruct.VnPixel = 480;// 480 == LV_HOR_RES_MAX
	翻转后，图片与字体都可呈现翻转效果，即素材源为横屏方式，显示为竖屏(注意长宽大小对调)。
*/
#define ARM_LINKER_AUTO			0 // 0:在链接时手动指定绝对地址; 1:在链接时按sct文件设置分配地址

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
        const lv_area_t * fill_area, lv_color_t color);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if ARM_LINKER_AUTO //是否由链接器自动分配地址空间
static uint32_t lcdbuf_gui[LV_HOR_RES_MAX * LV_VER_RES_MAX / 2] __attribute__((section(".SDRAM")));
static uint32_t lcdbuf_show[LV_HOR_RES_MAX * LV_VER_RES_MAX / 2] __attribute__((section(".SDRAM")));

#else
	static uint32_t lcdbuf_gui[LV_HOR_RES_MAX * LV_VER_RES_MAX / 2] __attribute__((at(SDRAMM_BASE)));
	
	#if (1 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 272 == LCD_VDOT) ) // 480*272
	static uint32_t lcdbuf_show[LV_HOR_RES_MAX * LV_VER_RES_MAX / 2] __attribute__((at(SDRAMM_BASE + 0x3FC00))) = {0};
		
	#elif (2 == __RESOLUTION_TARGET__ || (480 == LCD_HDOT && 480 == LCD_VDOT) ) // 480*480
	static uint32_t lcdbuf_show[LV_HOR_RES_MAX * LV_VER_RES_MAX / 2] __attribute__((at(SDRAMM_BASE + 0x70800))) = {0};

	#elif (3 == __RESOLUTION_TARGET__ || (800 == LCD_HDOT && 480 == LCD_VDOT) ) // 800*480
	static uint32_t lcdbuf_show[LV_HOR_RES_MAX * LV_VER_RES_MAX / 2] __attribute__((at(SDRAMM_BASE + 0xBB800))) = {0};

	#endif

#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LittlevGL requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows:
     *      LittlevGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer with some rows:
     *      LittlevGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LittlevGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Create TWO screen-sized buffer:
     *      Similar to 2) but the buffer have to be screen sized. When LittlevGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

//    /* Example for 1) */
//    static lv_disp_buf_t disp_buf_1;
//    static lv_color_t buf1_1[LV_HOR_RES_MAX * 10];                      /*A buffer for 10 rows*/
//    lv_disp_buf_init(&disp_buf_1, buf1_1, NULL, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

//    /* Example for 2) */
//    static lv_disp_buf_t disp_buf_2;
//    static lv_color_t buf2_1[LV_HOR_RES_MAX * 10];                        /*A buffer for 10 rows*/
//    static lv_color_t buf2_2[LV_HOR_RES_MAX * 10];                        /*An other buffer for 10 rows*/
//    lv_disp_buf_init(&disp_buf_2, buf2_1, buf2_2, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

//    /* Example for 3) */
//    static lv_disp_buf_t disp_buf_3;
//    static lv_color_t buf3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
//    static lv_color_t buf3_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/
//    lv_disp_buf_init(&disp_buf_3, buf3_1, buf3_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/
    
    static lv_disp_buf_t disp_buf;
    static lv_color_t buf1_1[LV_HOR_RES_MAX * 20];                        /*A buffer for 20 rows*/
    lv_disp_buf_init(&disp_buf, buf1_1, NULL, LV_HOR_RES_MAX * 20); /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LittlevGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;      /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    
    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;
    /*Set a display buffer*/
    disp_drv.buffer = &disp_buf;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    /*You code here*/
    lcd_init();
	
	#if (0 == LVGL_SCREEN_MODE)
    LCD->SRCADDR = (uint32_t)lcdbuf_show;
	
	#elif (1 == LVGL_SCREEN_MODE)
	LCD->SRCADDR = (uint32_t)lcdbuf_gui;
	
	#endif //(0 == LVGL_SCREEN_MODE)
	
    LCD_Start(LCD);
}

static inline void drawpoint(int16_t x, int16_t y, uint32_t c)
{
	#if LVGL_ROTATE_90 //0-正常		1-旋转90°
    uint32_t temp;
    uint32_t index = ((LV_HOR_RES -1- x)*LV_VER_RES+y)/2;
    uint32_t pos = ((y%2) == 0 ? 0 : 16);
    temp  = lcdbuf_gui[index];
    temp &= ~(0xFFFF << pos);
    temp |=  (c      << pos);
    lcdbuf_gui[index] = temp;
	
	#else
    uint32_t temp;
    uint32_t index = (y*LV_HOR_RES+x) / 2;
    uint32_t pos = ((x%2) == 0 ? 0 : 16);
    temp = lcdbuf_gui[index];
    temp &= ~(0xFFFF << pos);
    temp |=  (c      << pos);
    lcdbuf_gui[index] = temp;
	
	#endif
}

void gui2show(void)
{
	#if (0 == LVGL_SCREEN_MODE)
    memcpy(lcdbuf_show,lcdbuf_gui,sizeof(lcdbuf_show));
	
	#elif (1 == LVGL_SCREEN_MODE)
	
	#endif // (0 == LVGL_SCREEN_MODE)
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    int32_t x;
    int32_t y;
    uint32_t temp;
	#if LVGL_ROTATE_90 //0-正常		1-旋转90°
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            /* Put a pixel to the display. For example: */
            /* put_px(x, y, *color_p)*/
            drawpoint(x, y, color_p->full);
            color_p++;
        }
    }
	#else
	//该画点函数已经过测试验证, 请不要随意修改.(4 字节边界对齐优化)
    if(((area->x1) % 2 == 0) && ((area->x2) % 2 == 0))
    {
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x < area->x2; x+=2) {
                /* Put a pixel to the display. For example: */
                /* put_px(x, y, *color_p)*/
                temp = color_p->full;
                color_p++;
                temp |= (color_p->full) << 16;
                color_p++;
                lcdbuf_gui[(y*LV_HOR_RES+x) / 2] = temp;
            }
            drawpoint(area->x2, y, color_p->full);
            color_p++;
        }
    }
    else if(((area->x1) % 2 == 0) && ((area->x2) % 2 == 1))
    {
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x <= area->x2; x+=2) {
                /* Put a pixel to the display. For example: */
                /* put_px(x, y, *color_p)*/
                temp = color_p->full;
                color_p++;
                temp |= (color_p->full) << 16;
                color_p++;
                lcdbuf_gui[(y*LV_HOR_RES+x) / 2] = temp;
            }
        }
    }
    else if(((area->x1) % 2 == 1) && ((area->x2) % 2 == 0))
    {
        for(y = area->y1; y <= area->y2; y++) {
            drawpoint(area->x1, y, color_p->full);
            color_p++;
            for(x = (area->x1) + 1; x < area->x2; x+=2) {
                /* Put a pixel to the display. For example: */
                /* put_px(x, y, *color_p)*/
                temp = color_p->full;
                color_p++;
                temp |= (color_p->full) << 16;
                color_p++;
                lcdbuf_gui[(y*LV_HOR_RES+x) / 2] = temp;
            }
            drawpoint(area->x2, y, color_p->full);
            color_p++;
        }
    }
    else if(((area->x1) % 2 == 1) && ((area->x2) % 2 == 1))
    {
        for(y = area->y1; y <= area->y2; y++) {
            drawpoint(area->x1, y, color_p->full);
            color_p++;
            for(x = (area->x1)+1; x < area->x2; x+=2) {
                /* Put a pixel to the display. For example: */
                /* put_px(x, y, *color_p)*/
                temp = color_p->full;
                color_p++;
                temp |= (color_p->full) << 16;
                color_p++;
                lcdbuf_gui[(y*LV_HOR_RES+x) / 2] = temp;
            }
        }
    }
	#endif
    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for(i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color)
{
    /*It's an example code which should be done by your GPU*/
    int32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y <= fill_area->y2; y++) {
        for(x = fill_area->x1; x <= fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}

#endif  /*LV_USE_GPU*/

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
