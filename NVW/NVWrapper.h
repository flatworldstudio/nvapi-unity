#define NVW_CheckDisplays_API __declspec(dllexport) 




extern "C" {
	NVW_CheckDisplays_API int GetConnectedDisplays(char* outStr);
	NVW_CheckDisplays_API int GetGridSetup(char* outStr);
	NVW_CheckDisplays_API	int SetGridSetup(char* inStr, char* outStr);
}
