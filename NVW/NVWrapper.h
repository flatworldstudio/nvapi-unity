#define NVW_CheckDisplays_API __declspec(dllexport) 


extern "C" {
	NVW_CheckDisplays_API int GetHardware(char* outStr);
	NVW_CheckDisplays_API int GetGrids(char* outStr);
	NVW_CheckDisplays_API	int SetGrids(char* inStr, char* outStr);
}
