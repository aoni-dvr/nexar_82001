#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
// #include <limits.h>
#include "AmbaKAL.h"
#include "LvglWrap.h"
#include "lvgl.h"



#define LVGL_OK      (0x00000000U)   /* OK */
#define LVGL_NG      (0x00000001U)   /* NG */

#define SVC_OSD_BUF_NUM (1U)

#define LVGL_LOG "LVGL"
#define LOG_BUF_SIZE        512U
#define LOG_ARGC            3U


typedef struct{
    UINT32 width;
    UINT32 height;
}Lvgl_size;



static void LvglWrap_SetStyle(void);
static void LvglWrap_disp_flush_VoutA(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static void LvglWrap_disp_flush_VoutB(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static void* LvglWrap_tick(void* EntryArg);
static void* LvglWrap_TimerHandleEntry(void* EntryArg);
static UINT32 LvglWrap_InternalTaskCreate(void);
static void LvglWrap_LogOK(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2);
static void LvglWrap_LogNG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2);
// static void LvglWrap_InternalLog(INT8 LogLevel, const char *pFileName, UINT32 LineNumber, const char *pFuncName, const char *pDescription);
static void LvglWrap_InternalLog(const char *buf);

static lv_style_t rect_style;
static lv_style_t solid_style;
static lv_style_t fontStyle_ExtraSmall;
static lv_style_t fontStyle_Small;
static lv_style_t fontStyle_Big;
static lv_style_t style_line;
static lv_style_t background_object_style;


//display
static lv_disp_t *g_disp[2][SVC_OSD_BUF_NUM];
static UINT32 disp_count[2] = {0};

//line
static lv_point_t line_points[2][SVC_OSD_MAX_LINE_POINTS];
static UINT32 line_count[2] = {0};

static Lvgl_size Vout_Res[2];
static UINT32 isInitial = 1U;
static UINT32 MemShort = 0U;

#define MAX_OBJ_NUM 2000U
lv_obj_t *objList[2U][MAX_OBJ_NUM];
UINT32 ObjCount[2U] = {0U};

static void LvglWrap_MemShortCB(void)
{
    MemShort = 1U;
    LvglWrap_LogNG(LVGL_LOG, "Memory is not enough, consider increase CONFIG_SVC_LVGL_MEM", 0U, 0U);
}

static void LvglWrap_addObjList(UINT32 Chan, lv_obj_t *obj)
{
    if(ObjCount[Chan] < MAX_OBJ_NUM){
        objList[Chan][ObjCount[Chan]++] = obj;
    } else{
        LvglWrap_LogNG(LVGL_LOG, "Please increase MAX_OBJ_NUM", 0U, 0U);
    }
}

UINT32 LvglWrap_Init(UINT32 VoutID, UINT8 **OsdBuf, UINT32 OsdWidth, UINT32 OsdHeight)
{

    UINT32 RetVal=LVGL_OK, i;
    static UINT32 lv_initialized = 0U;

    static lv_disp_draw_buf_t  disp_buf[2][SVC_OSD_BUF_NUM];
    static lv_disp_drv_t disp_drv[2][SVC_OSD_BUF_NUM]; 			  /*Descriptor of a display driver*/
    lv_mem_monitor_t monitor;

    Vout_Res[VoutID].width = OsdWidth;
    Vout_Res[VoutID].height = OsdHeight;

    if(lv_initialized==0U){
        lv_init();
        lv_mem_monitor(&monitor);
        RetVal = LvglWrap_InternalTaskCreate();

        if(RetVal==LVGL_OK){
            LvglWrap_SetStyle();
            lv_initialized = 1U;
        }

        lv_log_register_print_cb(LvglWrap_InternalLog); /* Register print function for internal logs */
        lv_mem_hookCB(LvglWrap_MemShortCB);
        LvglWrap_LogOK(LVGL_LOG, "Total LVGL memory size:[0x%X], FreeSize:[0x%X]", monitor.total_size, monitor.free_size);
    }

    if(RetVal == LVGL_OK){
        UINT32 addr;
        for(i=0; i<SVC_OSD_BUF_NUM; i++){
            AmbaMisra_TypeCast(&addr, &OsdBuf[i]);
            lv_disp_draw_buf_init(&disp_buf[VoutID][i], OsdBuf[i], NULL, Vout_Res[VoutID].width * Vout_Res[VoutID].height);
            lv_disp_drv_init(&disp_drv[VoutID][i]);		  /*Basic initialization*/

            if(VoutID==0U){
                disp_drv[VoutID][i].flush_cb = LvglWrap_disp_flush_VoutA;	 /*Set your driver function*/
            }
            else if (VoutID==1U){
                disp_drv[VoutID][i].flush_cb = LvglWrap_disp_flush_VoutB;	 /*Set your driver function*/
            }

            disp_drv[VoutID][i].draw_buf = &disp_buf[VoutID][i];		  /*Assign the buffer to the display*/
            disp_drv[VoutID][i].hor_res = Vout_Res[VoutID].width;
            disp_drv[VoutID][i].ver_res = Vout_Res[VoutID].height;
            disp_drv[VoutID][i].screen_transp = 1U;
            g_disp[VoutID][i] = lv_disp_drv_register(&disp_drv[VoutID][i]);	  /*Finally register the driver*/
        }



        isInitial = 1U;

        for(i=0; i<SVC_OSD_BUF_NUM; i++){
            lv_obj_set_style_bg_opa(lv_disp_get_scr_act(g_disp[VoutID][i]),0, LV_PART_MAIN);
        }

        for(i=0; i<SVC_OSD_BUF_NUM; i++){
            lv_refr_now(g_disp[VoutID][i]);
        }
        isInitial=0U;

        LvglWrap_LogOK(LVGL_LOG,"Finish Initial Vout:[%u]",VoutID, 0U);
        LvglWrap_LogOK(LVGL_LOG,"Width:[%u], Height:[%u]",Vout_Res[VoutID].width, Vout_Res[VoutID].height);

    }

    return RetVal;

}

static void LvglWrap_LogOK(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      RetVal, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    RetVal = AmbaUtility_StringPrintStr(LogBuf, LOG_BUF_SIZE, "[%s|OK]: %s", Argc, ArgS);
    if (RetVal < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    }

}

static void LvglWrap_LogNG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{

    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      RetVal, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    RetVal = AmbaUtility_StringPrintStr(LogBuf, LOG_BUF_SIZE, "\033""[0;31m[%s|NG]: %s", Argc, ArgS);
    if (RetVal < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

}

static void LvglWrap_InternalLog(const char *buf)
{
    AmbaPrint_PrintStr5("\033""[0;31m[LVGL_Internal]%s", buf, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
}


static UINT32 LvglWrap_InternalTaskCreate(void)
{

    UINT32 RetVal = LVGL_OK;
    static AMBA_KAL_TASK_t        Lvgl_TickTask;
    static UINT8            LvTaskStack[0x10000U] GNU_SECTION_NOZEROINIT;
    static char LvglTickTaskName[16] = "LvglTick";

    static AMBA_KAL_TASK_t  Lvgl_TimeHandlerTask;
    static UINT8            LvTimeHandlerTaskStack[0x10000U] GNU_SECTION_NOZEROINIT;
    static char LvglTimeHandlerTaskName[16] = "LvglTimeHandler";

    RetVal = AmbaKAL_TaskCreate(&Lvgl_TickTask,
                                LvglTickTaskName,
                                43U,
                                LvglWrap_tick,
                                0U,
                                LvTaskStack,
                                0x10000U,
                                0U);


    if (KAL_ERR_NONE == RetVal) {
        RetVal = AmbaKAL_TaskSetSmpAffinity(&Lvgl_TickTask, 0x02U);
        if (KAL_ERR_NONE == RetVal) {
            RetVal = AmbaKAL_TaskResume(&Lvgl_TickTask);
        }
    }

    RetVal = AmbaKAL_TaskCreate(&Lvgl_TimeHandlerTask,
                                LvglTimeHandlerTaskName,
                                80U,
                                LvglWrap_TimerHandleEntry,
                                0U,
                                LvTimeHandlerTaskStack,
                                0x10000U,
                                0U);

    if (KAL_ERR_NONE == RetVal) {
        RetVal = AmbaKAL_TaskSetSmpAffinity(&Lvgl_TimeHandlerTask, 0x02U);
        if (KAL_ERR_NONE == RetVal) {
            RetVal = AmbaKAL_TaskResume(&Lvgl_TimeHandlerTask);
        }
    }
    return RetVal;

}

static void LvglWrap_DataClean(UINT32 VoutID)
{
    UINT32 i;
    for(i=0U; i<ObjCount[VoutID]; i++){
        lv_obj_del(objList[VoutID][i]);
    }
    ObjCount[VoutID] = 0U;
    line_count[VoutID] = 0U;

}


void LvglWrap_DrawOSD(UINT32 VoutID, UINT32 Level)
{
    (void) Level;
    lv_obj_invalidate(lv_disp_get_scr_act(g_disp[VoutID][disp_count[VoutID]]));
    lv_refr_now(g_disp[VoutID][disp_count[VoutID]]);
    LvglWrap_DataClean(VoutID);
}

void LvglWrap_SetBuffer(UINT32 VoutID, UINT32 BufId, UINT8 *BufAddr)
{


    g_disp[VoutID][0U]->driver->draw_buf->buf1 = BufAddr;
    g_disp[VoutID][0U]->driver->draw_buf->buf_act = BufAddr;
    MemShort = 0U;

   (void)VoutID;
   (void)BufId;
   (void)BufAddr;
}

static void LvglWrap_SetStyle(void)
{

#define LV_COLOR_BLUE ((lv_color_t){{0x00, 0x00, 0xFF, 0xFF}})

    //Hollow Rectangle Style
    lv_style_init(&rect_style);
    lv_style_set_bg_opa(&rect_style,0);
    lv_style_set_radius(&rect_style,0);
    lv_style_set_border_color(&rect_style,LV_COLOR_BLUE);
    lv_style_set_border_width(&rect_style,6);

    //Small font px=10
    lv_style_init(&fontStyle_ExtraSmall);
    lv_style_set_text_font(&fontStyle_ExtraSmall, &lv_font_sans_10_Amba);

    //Small font px=14
    lv_style_init(&fontStyle_Small);
    lv_style_set_text_font(&fontStyle_Small, &lv_font_sans_14_Amba);

    //Big font px=30
    lv_style_init(&fontStyle_Big);
    lv_style_set_text_font(&fontStyle_Big, &lv_font_sans_30_Amba);

    //Soild Rectangle Style
    lv_style_init(&solid_style);
    lv_style_set_radius(&solid_style, 0);
    lv_style_set_border_opa(&solid_style, 0);

    //Line Style
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_rounded(&style_line, true);

    //Parent style of each dispaly
    lv_style_init(&background_object_style);
    lv_style_set_border_opa(&background_object_style, 0);
    lv_style_set_bg_opa(&background_object_style,0);

}


static void LvglWrap_disp_flush_VoutA(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{

    UINT32 RetVal = LVGL_OK;
    static UINT32 Debug = 0U;
    static UINT32 Update_Count = 0U;

    (void) disp_drv;
    (void) color_p;
    (void) area;

    if(isInitial==1U){
        lv_disp_flush_ready(disp_drv);
        RetVal=LVGL_NG;
    }

    if(RetVal==LVGL_OK){
        if(!lv_disp_flush_is_last(disp_drv)){
            Debug = 1U;
            Update_Count++;
            LvglWrap_LogNG(LVGL_LOG, "Vout:[%u] Update Times:[%u]", 0U, Update_Count);
            LvglWrap_LogNG(LVGL_LOG, "x1:[%u], y1:[%u]", area->x1, area->y1);
            LvglWrap_LogNG(LVGL_LOG, "x2:[%u], y2:[%u]", area->x2, area->y2);
            lv_disp_flush_ready(disp_drv);
            RetVal=LVGL_NG;
        }

        if(RetVal==LVGL_OK){

            // lv_obj_clean(lv_disp_get_scr_act(g_disp[0][disp_count[0]]));
            lv_disp_flush_ready(disp_drv);

            if(Debug == 1U){
                LvglWrap_LogNG(LVGL_LOG, "Vout:[%u] last time flush", 0U, 0U);
                LvglWrap_LogNG(LVGL_LOG, "x1:[%u], y1:[%u]", area->x1, area->y1);
                LvglWrap_LogNG(LVGL_LOG, "x2:[%u], y2:[%u]", area->x2, area->y2);
                Debug = 0U;
                Update_Count = 0U;
            }
        }
    }

}

static void LvglWrap_disp_flush_VoutB(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{


    UINT32 RetVal = LVGL_OK;
    static UINT32 Debug = 0U;
    static UINT32 Update_Count = 0U;

    (void) disp_drv;
    (void) color_p;
    (void) area;

    if(isInitial==1U){
        lv_disp_flush_ready(disp_drv);
        RetVal = LVGL_NG;
    }


    if(RetVal == LVGL_OK){
        if(!lv_disp_flush_is_last(disp_drv)){
            Debug = 1U;
            Update_Count++;
            LvglWrap_LogNG(LVGL_LOG, "Vout:[%u] Update Times:[%u]", 0U, Update_Count);
            LvglWrap_LogNG(LVGL_LOG, "x1:[%u], y1:[%u]", area->x1, area->y1);
            LvglWrap_LogNG(LVGL_LOG, "x2:[%u], y2:[%u]", area->x2, area->y2);
            lv_disp_flush_ready(disp_drv);

            RetVal = LVGL_NG;
        }

        if(RetVal == LVGL_OK){

            // lv_obj_clean(lv_disp_get_scr_act(g_disp[1][disp_count[1]]));
            lv_disp_flush_ready(disp_drv);

            if(Debug == 1U){
                LvglWrap_LogNG(LVGL_LOG, "Vout:[%u] last time flush", 1U, 0U);
                LvglWrap_LogNG(LVGL_LOG, "x1:[%u], y1:[%u]", area->x1, area->y1);
                LvglWrap_LogNG(LVGL_LOG, "x2:[%u], y2:[%u]", area->x2, area->y2);
                Debug = 0U;
                Update_Count = 0U;
            }
        }

    }

}


void LvglWrap_DrawBmp(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, const void *pInfo)
{
    lv_img_dsc_t *pBmpInfo;
    lv_obj_t *icon;
    AmbaMisra_TypeCast(&pBmpInfo, &pInfo);
    if(MemShort == 0U){
        icon = lv_img_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, icon);
        lv_img_set_src(icon,pBmpInfo);
        lv_obj_set_pos(icon,(lv_coord_t)Xstart,(lv_coord_t)Ystart);
    }

}

void LvglWrap_DrawRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color, UINT32 Thickness)
{

    lv_obj_t *rect;
    lv_color_t rect_color;
    UINT32 Width, Height;

    Width = Xend-Xstart;
    Height = Yend-Ystart;

    rect_color.full = Color;
    if(MemShort == 0U){
        rect = lv_obj_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, rect);
        lv_obj_set_pos(rect,(lv_coord_t)Xstart,(lv_coord_t)Ystart);
        lv_obj_set_size(rect, (lv_coord_t)Width, (lv_coord_t)Height);
        lv_obj_add_style(rect,&rect_style, LV_PART_MAIN);
        lv_obj_set_style_border_width(rect,(lv_coord_t)Thickness, LV_PART_MAIN);
        lv_obj_set_style_border_color(rect, rect_color, LV_PART_MAIN);
        lv_obj_set_style_border_opa(rect, Color>>24U, LV_PART_MAIN);
    }


}

void LvglWrap_DrawSolidRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color)
{


    lv_color_t rect_color;
    lv_obj_t *rect_solid;

    UINT32 Width,Height;

    Width = Xend-Xstart;
    Height = Yend-Ystart;

    rect_color.full = Color;
    if(MemShort == 0U){
        rect_solid = lv_obj_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, rect_solid);
        lv_obj_set_pos(rect_solid,(lv_coord_t) Xstart, (lv_coord_t)Ystart);
        lv_obj_set_size(rect_solid, (lv_coord_t)Width, (lv_coord_t)Height);
        lv_obj_add_style(rect_solid, &solid_style, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(rect_solid, Color>>24U, LV_PART_MAIN);
        lv_obj_set_style_bg_color(rect_solid, rect_color, LV_PART_MAIN);
    }


}

void LvglWrap_DrawString(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 FontSize, UINT32 Color, const char *pString)
{


    lv_obj_t * label;

    lv_color_t label_color;

    label_color.full = Color;

    if(MemShort == 0U){
        label = lv_label_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, label);
        if(FontSize == 1U){
            lv_obj_add_style(label,&fontStyle_ExtraSmall, LV_PART_MAIN);
        }else if (FontSize == 2U){
            lv_obj_add_style(label,&fontStyle_Small, LV_PART_MAIN);
        }
        else{
            lv_obj_add_style(label,&fontStyle_Big, LV_PART_MAIN);
        }

        lv_label_set_text(label,pString);
        lv_obj_set_pos(label,(lv_coord_t)Xstart,(lv_coord_t)Ystart);
        lv_obj_set_style_text_color(label,label_color, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(label, 0U, LV_PART_MAIN);
        lv_obj_set_style_bg_color(label, lv_color_make(255,255,255), LV_PART_MAIN);
    }


}

void LvglWrap_DrawCircle(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Radius, UINT32 LineWidth, UINT32 Color)
{


    lv_color_t circle_color;
    lv_obj_t * circle;
    INT32 circle_Xstart = (INT32)Xstart - (INT32)Radius;
    INT32 circle_Ystart = (INT32)Ystart - (INT32)Radius;
    UINT32 Width;


    Width = Radius << 1;

    circle_color.full = Color;

    if(MemShort == 0U){
        circle = lv_obj_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, circle);
        lv_obj_set_pos(circle,(lv_coord_t)circle_Xstart,(lv_coord_t)circle_Ystart);
        lv_obj_set_size(circle, (lv_coord_t)Width,(lv_coord_t)Width);
        lv_obj_set_style_radius(circle,LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(circle,0, LV_PART_MAIN);
        lv_obj_set_style_border_width(circle, (lv_coord_t)LineWidth, LV_PART_MAIN);
        lv_obj_set_style_border_color(circle,circle_color,LV_PART_MAIN);
        lv_obj_set_style_border_opa(circle, Color>>24U, LV_PART_MAIN);

    }


}


void LvglWrap_DrawLine(UINT32 Chan, UINT32 X1, UINT32 Y1, UINT32 X2, UINT32 Y2, UINT32 LineWidth, UINT32 Color, UINT32 *LineTotal)
{

    lv_obj_t * line;
    lv_color_t line_color;
    lv_point_t _point[2] = {{(lv_coord_t)X1,(lv_coord_t)Y1},{(lv_coord_t)X2,(lv_coord_t)Y2}};


    line_points[Chan][line_count[Chan]] = _point[0];
    line_points[Chan][line_count[Chan]+1U] = _point[1];

    line_color.full = Color;

    if(MemShort == 0U){
        line = lv_line_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, line);
        lv_obj_set_pos(line,0,0);
        lv_obj_add_style(line, &style_line, LV_PART_MAIN);
        lv_line_set_points(line,&line_points[Chan][line_count[Chan]],2U);
        lv_obj_set_style_line_color(line,line_color, LV_PART_MAIN);
        lv_obj_set_style_line_width(line,(lv_coord_t)LineWidth, LV_PART_MAIN);
        lv_obj_set_style_line_opa(line,Color>>24U, LV_PART_MAIN);
        line_count[Chan]+=2U;
        *LineTotal = line_count[Chan];
    }

}

void LvglWrap_DrawMultiplePointsLine(UINT32 Chan, void *Points, UINT32 PointNum,UINT32 LineWidth, UINT32 Color, UINT32 *LineTotal)
{


    lv_obj_t * line;
    lv_color_t line_color;
    lv_point_t * _point;

    AmbaMisra_TypeCast(&_point, &Points);
    memcpy(&line_points[Chan][line_count[Chan]], &_point[0], PointNum*sizeof(lv_point_t));

    line_color.full = Color;

    if(MemShort == 0U){
        line = lv_line_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, line);
        lv_obj_set_pos(line,0,0);
        lv_obj_add_style(line, &style_line, LV_PART_MAIN);
        lv_line_set_points(line,&line_points[Chan][line_count[Chan]],(UINT16)PointNum);
        lv_obj_set_style_line_color(line,line_color, LV_PART_MAIN);
        lv_obj_set_style_line_width(line,LineWidth, LV_PART_MAIN);
        lv_obj_set_style_line_opa(line,Color>>24, LV_PART_MAIN);
        line_count[Chan]+=PointNum;

        *LineTotal = line_count[Chan];

    }


}

static void* LvglWrap_TimerHandleEntry(void* EntryArg)
{
    (void) EntryArg;
    while(1) {
        lv_timer_handler();
        AmbaKAL_TaskSleep(5U);
    }

    return NULL;
}

static void* LvglWrap_tick(void* EntryArg)
{
    (void) EntryArg;
    while(1) {
        lv_tick_inc(1U);
        AmbaKAL_TaskSleep(1U);
    }
    return NULL;
}


void LvglWrap_DrawDiagBoxRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Thickness, UINT32 Bg_Color, UINT32 Border_Color)
{


    lv_obj_t *rect_solid;
    lv_obj_t *rect_hollow;
    lv_color_t bg_color, border_color;
    UINT32 hollow_wdith, hollow_height;
    UINT32 solid_width, solid_height, solid_Xstart, solid_Ystart;


    if(MemShort == 0U){
        rect_solid = lv_obj_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        rect_hollow = lv_obj_create(lv_disp_get_scr_act(g_disp[Chan][disp_count[Chan]]));
        LvglWrap_addObjList(Chan, rect_solid);
        LvglWrap_addObjList(Chan, rect_hollow);
        bg_color.full = Bg_Color;
        border_color.full = Border_Color;

        hollow_wdith = Xend -Xstart;
        hollow_height = Yend - Ystart;

        solid_width = hollow_wdith - 2*Thickness;
        solid_height = hollow_height - 2*Thickness;
        solid_Xstart = Xstart + Thickness;
        solid_Ystart = Ystart + Thickness;

        lv_obj_set_pos(rect_hollow, Xstart, Ystart);
        lv_obj_set_size(rect_hollow, hollow_wdith, hollow_height);
        lv_obj_set_pos(rect_solid, solid_Xstart, solid_Ystart);
        lv_obj_set_size(rect_solid, solid_width, solid_height);

        //For inside solid rectangle
        lv_obj_set_style_bg_opa(rect_solid, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_bg_color(rect_solid, bg_color, LV_PART_MAIN);
        lv_obj_set_style_radius(rect_solid, 0U, LV_PART_MAIN);
        lv_obj_set_style_border_opa(rect_solid, 0U, LV_PART_MAIN);


        //For outside border
        lv_obj_set_style_border_color(rect_hollow, border_color, LV_PART_MAIN);
        lv_obj_set_style_border_width(rect_hollow, Thickness, LV_PART_MAIN);
        lv_obj_set_style_radius(rect_hollow, 5U, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(rect_hollow, 0U, LV_PART_MAIN);

    }

}


