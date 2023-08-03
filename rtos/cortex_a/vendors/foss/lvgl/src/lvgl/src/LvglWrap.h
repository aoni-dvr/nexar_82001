


#ifndef LVGL_WRAP_H
#define LVGL_WRAP_H


#define SVC_OSD_MAX_LINE_POINTS 1300U
void LvglWrap_DrawRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color, UINT32 Thickness);
void LvglWrap_DrawSolidRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Color);
void LvglWrap_DrawString(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 FontSize, UINT32 Color, const char *pString);
void LvglWrap_DrawCircle(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Radius, UINT32 LineWidth, UINT32 Color);
void LvglWrap_DrawLine(UINT32 Chan, UINT32 X1, UINT32 Y1, UINT32 X2, UINT32 Y2, UINT32 LineWidth, UINT32 Color, UINT32 *LineTotal);
void LvglWrap_DrawMultiplePointsLine(UINT32 Chan, void *Points, UINT32 PointNum,UINT32 LineWidth, UINT32 Color, UINT32 *LineTotal);
void LvglWrap_DrawBmp(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, const void *pInfo);
UINT32 LvglWrap_Init(UINT32 VoutID, UINT8 **OsdBuf, UINT32 OsdWidth, UINT32 OsdHeight);
void LvglWrap_DrawOSD(UINT32 VoutID, UINT32 Level);
void LvglWrap_SetBuffer(UINT32 VoutID, UINT32 BufId, UINT8 *BufAddr);
void LvglWrap_DrawDiagBoxRect(UINT32 Chan, UINT32 Xstart, UINT32 Ystart, UINT32 Xend, UINT32 Yend, UINT32 Thickness, UINT32 Bg_Color, UINT32 Border_Color);

#endif

