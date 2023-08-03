Initialization of mechanism of DSP interrupt event (VIN/VOUT):
0. AmbaDspInt_Init(UINT32 Priority, UINT32 CoreInclusion)

Initialization of iq:
1. AmbaImgMain_IqMemSizeQuery(UINT32 MaxAlgoNum)
2. AmbaImgMain_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte)

Initialization of Algo:
3. AmbaImgMain_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)
4. AmbaImgMain_AlgoMemInit(void *pMem, UINT32 MemSizeInByte)
5. AmbaImgMain_AlgoInit(UINT32 MaxAlgoNum, UINT32 MaxExposureNum)

initialization of img main (DSP space memory):
6. AmbaImgMain_MemSizeQuery(void)
7. AmbaImgMain_MemInit(void *pMem, UINT32 MemSizeInByte)

Register the test command of imgproc of Algo:
8. extern void AmpUT_ImgProcTest(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

Register the test command of imgfrw:
9. extern void AmbaShell_CommandAmbaImgFrw(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);

Initialization of image framework (IF):
10. AmbaImgMain_SysInit(void)

Configuration of APP of IF and Algo: (example code in the test command of AmbaImgFrwCmdFlow.c)
11. AmbaImgMain_Config(UINT32 VinId, UINT64 Param)
