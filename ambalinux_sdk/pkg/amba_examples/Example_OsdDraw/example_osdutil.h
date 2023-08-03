#ifndef EXAMPLE_OSD_UTILITY_H_
#define EXAMPLE_OSD_UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

unsigned int OSD_Init(int **Hndlr, int Width, int Height);
unsigned int OSD_DrawLine(int *Hndlr, int X_ST, int Y_ST, int X_END, int Y_END, int Thickness, int Color);
unsigned int OSD_DrawRect(int *Hndlr, int X_TL, int Y_TL, int X_BR, int Y_BR, int Thickness, int Color);
unsigned int OSD_DrawCircle(int *Hndlr, int X_C, int Y_C, int Radius, int Thickness, int Color);
unsigned int OSD_DrawString(int *Hndlr, int X_BL, int Y_BL, char *Text, int Thickness, int Color);
unsigned int OSD_GetBuf(int *Hndlr, unsigned char **Buf, unsigned int *Size);
unsigned int OSD_Release(int *Hndlr);

#ifdef __cplusplus
}
#endif

#endif /* EXAMPLE_OSD_UTILITY_H_ */

