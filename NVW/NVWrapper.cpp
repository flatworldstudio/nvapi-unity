#include <stdio.h>

#include "pch.h"
#include "NVWrapper.h"
//#include <stdlib.h>
#include <math.h> 
#include <windows.h>
#//include <assert.h>
#include "include\nvapi.h"

const int CHARSIZE = 256;


NvU8 ReadStreamNvU8(char* charArray, int &point);

void WriteStreamNvU8(char* charArray, int &point, NvU8 theChar);

NvU32 ReadStreamNvU32(char* charArray, int &point);

void WriteStreamNvU32(char* charArray, int &point, NvU32 value);

NvS32 ReadStreamNvS32(char* charArray, int &point);

void WriteStreamNvS32(char* charArray, int &point, NvS32 value);

NvAPI_Status SetBlend(NV_MOSAIC_GRID_TOPO topo);

static void InitConfigMap() {

}



int NVInit(char* outStr) {

	// Initialize NVAPI
	NvAPI_Status error;
	NvAPI_ShortString result = "";
	error = NvAPI_Initialize();

	if ((error != NVAPI_OK))
	{
		NvAPI_GetErrorMessage(error, result);
		for (int i = 0; i < 64; ++i) outStr[i] = result[i];
		return 0;
	}

	return 1;
}


int GetGrids(char* outStr) {

	// Init nvapi before calling

	NvAPI_Status error;
	NvAPI_ShortString message = "";

	// Get number of grids
	NvU32 gridCount;
	NvAPI_Mosaic_EnumDisplayGrids(NULL, &gridCount);

	NV_MOSAIC_GRID_TOPO* gridTopo = NULL;

	gridTopo = new NV_MOSAIC_GRID_TOPO[gridCount];
	gridTopo->version = NV_MOSAIC_GRID_TOPO_VER;

	error = NvAPI_Mosaic_EnumDisplayGrids(gridTopo, &gridCount);

	if ((error != NVAPI_OK))
	{
		NvAPI_GetErrorMessage(error, message);
		for (int i = 0; i < 64; ++i) outStr[i] = message[i];
		return 0;
	}

	int sp = 0;
	WriteStreamNvU8(outStr, sp, gridCount);

	for (NvU32 iGrid = 0; iGrid < gridCount; iGrid++) {

		NvU32 numDisplays = gridTopo[iGrid].displayCount;// no of displays in grid
		NvU32 numRows = gridTopo[iGrid].rows;// no of rows in grid
		NvU32 numColumns = gridTopo[iGrid].columns;// no of columns in grid
		WriteStreamNvU8(outStr, sp, numDisplays);
		WriteStreamNvU8(outStr, sp, numColumns);
		WriteStreamNvU8(outStr, sp, numRows);


		NV_MOSAIC_DISPLAY_SETTING& ds = gridTopo[iGrid].displaySettings;

		WriteStreamNvU32(outStr, sp, ds.width);
		WriteStreamNvU32(outStr, sp, ds.height);
		WriteStreamNvU32(outStr, sp, ds.freq);

		for (NvU32 iDisplay = 0; iDisplay < gridTopo[iGrid].displayCount; iDisplay++) {

			NV_MOSAIC_GRID_TOPO_DISPLAY& display = gridTopo[iGrid].displays[iDisplay];

			WriteStreamNvU32(outStr, sp, display.displayId);
			WriteStreamNvU32(outStr, sp, display.overlapX);
			WriteStreamNvU32(outStr, sp, display.overlapY);

		}

	}

	return 1;
}


int SetGrids(char* inStr, char* outStr) {

	NvAPI_Status error;
	NvAPI_ShortString message = "";

	// Get number of grids

	NvU32 gridCount;
	NvAPI_Mosaic_EnumDisplayGrids(NULL, &gridCount);

	NV_MOSAIC_GRID_TOPO* gridTopo = NULL;

	gridTopo = new NV_MOSAIC_GRID_TOPO[gridCount];
	gridTopo->version = NV_MOSAIC_GRID_TOPO_VER;

	error = NvAPI_Mosaic_EnumDisplayGrids(gridTopo, &gridCount);

	if ((error != NVAPI_OK))
	{
		NvAPI_GetErrorMessage(error, message);
		for (int i = 0; i < 64; ++i) outStr[i] = message[i];
		return 0;
	}

	// We take the first grid, and use as template. 
	NV_MOSAIC_GRID_TOPO BaseGrid = NV_MOSAIC_GRID_TOPO(gridTopo[0]);

	// create grid topologies
	delete gridTopo;

	// Retrieve input

	int sp = 0;
	int op = 0;
	gridCount = ReadStreamNvU8(inStr, sp);

	gridTopo = new NV_MOSAIC_GRID_TOPO[gridCount];
	gridTopo->version = NV_MOSAIC_GRID_TOPO_VER;

	for (int g = 0;g < gridCount;g++) {

		NvU8 displayCount = ReadStreamNvU8(inStr, sp);
		NvU8 columns = ReadStreamNvU8(inStr, sp);
		NvU8 rows = ReadStreamNvU8(inStr, sp);

		//	WriteStreamNvU8(outStr, op, displayCount);

		NvU32 width = ReadStreamNvU32(inStr, sp);
		NvU32 height = ReadStreamNvU32(inStr, sp);
		NvU32 freq = ReadStreamNvU32(inStr, sp);

		// Create topo

		NV_MOSAIC_GRID_TOPO Topo = NV_MOSAIC_GRID_TOPO(BaseGrid);
		Topo.displaySettings.height = height;
		Topo.displaySettings.width = width;
		Topo.displaySettings.freq = freq;

		NV_MOSAIC_GRID_TOPO_DISPLAY BaseDisplay = NV_MOSAIC_GRID_TOPO_DISPLAY();
		BaseDisplay.version = NV_MOSAIC_GRID_TOPO_DISPLAY_VER;
		BaseDisplay.logicalDimensions.height = 0;
		BaseDisplay.logicalDimensions.width = 0;
		BaseDisplay.logicalDimensions.positionX = 0;
		BaseDisplay.logicalDimensions.positionY = 0;
		BaseDisplay.logicalDisplayRegion.height = 0;
		BaseDisplay.logicalDisplayRegion.width = 0;
		BaseDisplay.logicalDisplayRegion.positionX = 0;
		BaseDisplay.logicalDisplayRegion.positionY = 0;
		BaseDisplay.physicalDimensions.height = 0;
		BaseDisplay.physicalDimensions.width = 0;
		BaseDisplay.physicalDimensions.positionX = 0;
		BaseDisplay.physicalDimensions.positionY = 0;
		BaseDisplay.physicalDisplayRegion.height = 0;
		BaseDisplay.physicalDisplayRegion.width = 0;
		BaseDisplay.physicalDisplayRegion.positionX = 0;
		BaseDisplay.physicalDisplayRegion.positionY = 0;
		BaseDisplay.cloneGroup = 0;
		BaseDisplay.overlapX = 0;
		BaseDisplay.overlapY = 0;
		BaseDisplay.pixelShiftType = NV_PIXEL_SHIFT_TYPE_NO_PIXEL_SHIFT;
		BaseDisplay.rotation = NV_ROTATE_0;
		BaseDisplay.rotationReserved = 0;

		Topo.displays[displayCount];

		//WriteStreamNvU32(outStr, op, width);

		for (int d = 0;d < displayCount;d++) {
			NvU32 displayId = ReadStreamNvU32(inStr, sp);
			NvS32 overlapx = (signed int)ReadStreamNvU32(inStr, sp);
			NvS32 overlapy = (signed int)ReadStreamNvU32(inStr, sp);

			Topo.displays[d] = NV_MOSAIC_GRID_TOPO_DISPLAY(BaseDisplay);
			Topo.displays[d].displayId = displayId;
			Topo.displays[d].overlapX = overlapx;
			Topo.displays[d].overlapY = overlapy;

		}
		Topo.displayCount = displayCount;
		Topo.columns = columns;
		Topo.rows = rows;
		gridTopo[g] = Topo;
	}

	NvU8 applyGrid = ReadStreamNvU8(inStr, sp);

	if (applyGrid > 0)
	{
		error = NvAPI_Mosaic_SetDisplayGrids(gridTopo, gridCount, NV_MOSAIC_SETDISPLAYTOPO_FLAG_CURRENT_GPU_TOPOLOGY);

		if ((error != NVAPI_OK))
		{
			NvAPI_GetErrorMessage(error, message);
			for (int i = 0; i < 64; ++i) outStr[i] = message[i];
			return 0;
		}
	}

	NvU8 applyBlend = ReadStreamNvU8(inStr, sp);

	if (applyBlend > 0)
	{
		// Implied from grid topo. Any overlap will be blended.

		error = SetBlend(gridTopo[0]);

		if ((error != NVAPI_OK))
		{
			NvAPI_GetErrorMessage(error, message);
			for (int i = 0; i < 64; ++i) outStr[i] = message[i];
			return 0;
		}

	}


	return 1;

}

int GetDisplays(char* outStr) {

	// Init nvapi before calling.
	// Returns a serialised overview of connected displays, their id's and if they have an active intensity mod

	NvAPI_Status error;
	NvAPI_ShortString message = "";

	NvU8 connected;
	NvU8 active;
	NvU8 intensity;

	// Get GPUs
	NvPhysicalGpuHandle nvGPUHandles[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32 gpuCount = 0;
	ZeroMemory(&nvGPUHandles, sizeof(nvGPUHandles));
	error = NvAPI_EnumPhysicalGPUs(nvGPUHandles, &gpuCount);

	if ((error != NVAPI_OK))
	{
		NvAPI_GetErrorMessage(error, message);
		for (int i = 0; i < 64; ++i) outStr[i] = message[i];
		return 0;
	}

	NvU32 gpu;
	NvU32* DisplayIdList;
	NvU32* DisplayConnected;

	NvU32 DisplaysPerGpu = 4;
	DisplayIdList = new NvU32[DisplaysPerGpu*gpuCount];
	//	DisplayConnected = new NvU32[DisplaysPerGpu*gpuCount];

	connected = 0;
	active = 0;
	intensity = 0;

	for (gpu = 0; gpu < gpuCount; gpu++)
	{
		// Query the active physical display connected to each gpu. Get a count first.
		NvU32 dispIdCount = 0;
		error = NvAPI_GPU_GetConnectedDisplayIds(nvGPUHandles[gpu], NULL, &dispIdCount, 0);

		if (error != NVAPI_OK) {
			NvAPI_GetErrorMessage(error, message);
			for (int i = 0; i < 64; ++i) outStr[i] = message[i];
			return 0;
		}


		// Create an array to hold refs and get refs

		NV_GPU_DISPLAYIDS* dispIds = NULL;
		dispIds = new NV_GPU_DISPLAYIDS[dispIdCount];
		dispIds->version = NV_GPU_DISPLAYIDS_VER;

		NV_SCANOUT_INTENSITY_STATE_DATA intensityState = {};
		intensityState.version = NV_SCANOUT_INTENSITY_STATE_VER;

		error = NvAPI_GPU_GetConnectedDisplayIds(nvGPUHandles[gpu], dispIds, &dispIdCount, 0);

		if (error != NVAPI_OK)
		{
			delete[] dispIds;// 
			NvAPI_GetErrorMessage(error, message);
			for (int i = 0; i < 64; ++i) outStr[i] = message[i];
			return 0;
		}

		// Go over displays.

		for (int dispIndex = 0; dispIndex < DisplaysPerGpu; dispIndex++)
		{
			int i = gpu * DisplaysPerGpu + dispIndex;

			if (i > 7)
			{
				NvAPI_ShortString outofrange = "Display index out of range.";
				for (int i = 0; i < 64; ++i) outStr[i] = outofrange[i];
				return 0;
			}

			if ((dispIndex < dispIdCount))
				// It appears displays need to be connected bottom gpu ports first. So no skipping of ports.

			{
				DisplayIdList[i] = dispIds[dispIndex].displayId;
				connected |= 1 << i;

				if (dispIds[dispIndex].isActive) {

					active |= 1 << i;

					error = NvAPI_GPU_GetScanoutIntensityState(dispIds[dispIndex].displayId, &intensityState);

					if (error != NVAPI_OK)
					{
						NvAPI_GetErrorMessage(error, message);
						for (int i = 0; i < 64; ++i) outStr[i] = message[i];
						return 0;
					}

					if (intensityState.bEnabled) intensity |= 1 << i;

				}


				/*
				//	error = NvAPI_GPU_GetScanoutIntensityState(dispIds[dispIndex].displayId, &intensityState);

				//	error = NvAPI_GPU_GetScanoutIntensityState(displayId, &intensityState);


				error = NvAPI_GPU_GetScanoutIntensityState(2147881093, &intensityState);
				//	error = NVAPI_OK;

				if (error != NVAPI_OK)
				{
					NvAPI_GetErrorMessage(error, message);
					for (int i = 0; i < 64; ++i) outStr[i] = message[i];
					return 0;
				}

				if (intensityState.bEnabled) intensityActive |= 1 << i;
				*/
			}
			else
			{
				DisplayIdList[i] = 0;
			}
		}
	}

	int sp = 0;
	WriteStreamNvU8(outStr, sp, connected);
	WriteStreamNvU8(outStr, sp, active);
	WriteStreamNvU8(outStr, sp, intensity);

	for (int d = 0;d < DisplaysPerGpu*gpuCount;d++) {

		WriteStreamNvU32(outStr, sp, DisplayIdList[d]);

	}

	return 1;
}


NvU8 ReadStreamNvU8(char* charArray, int &point) {
	point++;
	point = point % CHARSIZE; // 
	return charArray[point - 1];
}

void WriteStreamNvU8(char* charArray, int &point, NvU8 theChar) {
	charArray[point] = theChar;
	point++;
	point = point % CHARSIZE; // 
}


NvU32 ReadStreamNvU32(char* charArray, int &point) {

	//NvU32 value = charArray[point + 0] + charArray[point + 1] << 8 + charArray[point + 2] << 16 + charArray[point + 3] << 24;

	NvU8 value0 = (NvU8)charArray[point + 0];
	NvU8 value1 = (NvU8)charArray[point + 1];
	NvU8 value2 = (NvU8)charArray[point + 2];
	NvU8 value3 = (NvU8)charArray[point + 3];

	NvU32 value = value0 + value1 * (1 << 8) + value2 * (1 << 16) + value3 * (1 << 24);

	point += 4;
	point = point % CHARSIZE; // 
	return value;

}

void WriteStreamNvU32(char* charArray, int &point, NvU32 value) {

	charArray[point + 0] = value & 0x000000ff;
	charArray[point + 1] = (value & 0x0000ff00) >> 8;
	charArray[point + 2] = (value & 0x00ff0000) >> 16;
	charArray[point + 3] = (value & 0xff000000) >> 24;

	point += 4;
	point = point % CHARSIZE; // 
}

NvS32 ReadStreamNvS32(char* charArray, int &point) {


	NvS32 value = charArray[point + 0] + charArray[point + 1] << 8 + charArray[point + 2] << 16 + charArray[point + 3] << 24;

	point += 4;
	point = point % CHARSIZE; // 
	return value;

}

void WriteStreamNvS32(char* charArray, int &point, NvS32 value) {

	//NvU32 vus = value + 0x7fffffff;
	charArray[point + 0] = value & 0x000000ff;
	charArray[point + 1] = (value & 0x0000ff00) >> 8;
	charArray[point + 2] = (value & 0x00ff0000) >> 16;
	charArray[point + 3] = (value & 0xff000000) >> 24;


	point += 4;
	point = point % CHARSIZE; // 

}


NvAPI_Status SetBlend(NV_MOSAIC_GRID_TOPO topo) {

	//	NV_MOSAIC_GRID_TOPO& Topo = TopoP;

	NvAPI_Status error = NVAPI_OK;
	NvAPI_ShortString estring;

	NV_SCANOUT_INTENSITY_DATA intensityData0, intensityData1;

	// Find overlap.

	if (topo.displayCount == 2 && topo.columns == 2) {
		// horizontal
		NvS32 overlapx = (float)topo.displays[1].overlapX;
		NvU32 width = topo.displaySettings.width;

		const int Steps = 256;
		//float BlendGamma = 0.5f;
		float BlendGamma = 0.454f;
		float intensityTexture0[Steps * 3];
		float intensityTexture1[Steps * 3];

		float treshold0 = (float)(width - overlapx);
		float treshold1 = (float)(overlapx);

		float value;
		float step = width / (float)Steps;

		float offsetTexture[4] = { 0.0f, 0.0f, 0.0f,0.0f };

		for (int i = 0; i < Steps; i++) {

			float x = i * step;

			value = x < treshold0 ? 1.0f : pow(1 - ((x - treshold0) / overlapx), BlendGamma);

		//	value = pow(1 - ((x - treshold0) / overlapx), BlendGamma);

			intensityTexture0[i * 3 + 0] = value;
			intensityTexture0[i * 3 + 1] = value;
			intensityTexture0[i * 3 + 2] = value;

			value = x > treshold1 ? 1.0f : pow( x / overlapx,BlendGamma);

			intensityTexture1[i * 3 + 0] = value;
			intensityTexture1[i * 3 + 1] = value;
			intensityTexture1[i * 3 + 2] = value;
		}

		intensityData0.version = NV_SCANOUT_INTENSITY_DATA_VER;
		intensityData0.width = Steps;
		intensityData0.height = 1;
		intensityData0.blendingTexture = intensityTexture0;
		//	intensityData0.blendingTexture = NULL;
		intensityData0.offsetTexture = NULL;
		intensityData0.offsetTexChannels = 1;

		intensityData1.version = NV_SCANOUT_INTENSITY_DATA_VER;
		intensityData1.width = Steps;
		intensityData1.height = 1;
		intensityData1.blendingTexture = intensityTexture1;
		//	intensityData1.blendingTexture = NULL;
		intensityData1.offsetTexture = NULL;
		intensityData1.offsetTexChannels = 1;

		int sticky;

		// This call does the intensity map

		error = NvAPI_GPU_SetScanoutIntensity(topo.displays[0].displayId, &intensityData0, &sticky);

		if (error != NVAPI_OK)
		{
			return	error;
		}

		error = NvAPI_GPU_SetScanoutIntensity(topo.displays[1].displayId, &intensityData1, &sticky);

		if (error != NVAPI_OK)
		{
			return	error;
		}

	}



	return error;
	/*


	const int Steps = 256;
	float BlendGamma = 0.5f;

	float intensityTexture0[Steps * 3];
	float intensityTexture1[Steps * 3];

	//	int width = Steps*BlendFactor;

	int treshold0 = Steps - BlendSteps;
	int treshold1 = BlendSteps;

	float value;

	for (int i = 0; i < Steps; i++) {

		if (i < treshold0) {
			value = 1.0f;

			intensityTexture0[i * 3 + 0] = value;
			intensityTexture0[i * 3 + 1] = value;
			intensityTexture0[i * 3 + 2] = value;
		}
		else {

			//	value = 1.0f - (pow((i - treshold)*(1.0f / width), 2.0f));
			value = (pow(1 - ((i - treshold0)*(1.0f / BlendSteps)), BlendGamma));

			intensityTexture0[i * 3 + 0] = value;
			intensityTexture0[i * 3 + 1] = value;
			intensityTexture0[i * 3 + 2] = value;
		}

		if (i > treshold1) {
			value = 1.0f;

			intensityTexture1[i * 3 + 0] = value;
			intensityTexture1[i * 3 + 1] = value;
			intensityTexture1[i * 3 + 2] = value;
		}
		else {

			//	value = 1.0f - (pow((i - treshold)*(1.0f / width), 2.0f));
			value = (pow((i)*(1.0f / BlendSteps), BlendGamma));

			intensityTexture1[i * 3 + 0] = value;
			intensityTexture1[i * 3 + 1] = value;
			intensityTexture1[i * 3 + 2] = value;
		}


	}

	float offsetTexture[4] = { 0.0f, 0.0f, 0.0f,0.0f };

	intensityData0.version = NV_SCANOUT_INTENSITY_DATA_VER;
	intensityData0.width = Steps;
	intensityData0.height = 1;
	intensityData0.blendingTexture = intensityTexture0;
	intensityData0.offsetTexture = NULL;
	intensityData0.offsetTexChannels = 1;

	intensityData1.version = NV_SCANOUT_INTENSITY_DATA_VER;
	intensityData1.width = Steps;
	intensityData1.height = 1;
	intensityData1.blendingTexture = intensityTexture1;
	intensityData1.offsetTexture = NULL;
	intensityData1.offsetTexChannels = 1;


	int sticky = 0;

	// This call does the intensity map

	error = NvAPI_GPU_SetScanoutIntensity(Topo.displays[0].displayId, &intensityData0, &sticky);

	if (error != NVAPI_OK)
	{

		return	error;

	}


	error = NvAPI_GPU_SetScanoutIntensity(Topo.displays[1].displayId, &intensityData1, &sticky);

	if (error != NVAPI_OK)
	{

		return	error;

	}

	// result would be nvapi ok
	return error;
	*/
}

//

extern "C" {

	// 
//
//	int CheckConfig(char* outStr) {
//
//		// Init
//
//		if (NVInit(outStr) == 0) {
//			NvAPI_Unload();
//			return 0;
//		}
//
//		// Execute
//
//		if (GetDisplays(outStr) == 0) {
//			NvAPI_Unload();
//			return 0;
//		}
//
//		//outStr[0] = 'a' ;
//
//		/*switch (outStr[0]) {
//		case 0x00011111:
//
//			break;
//		default:
//			break;
//		}
//*/
//// Unload
//
//		NvAPI_Unload();
//
//		return 1;
//
//
//
//	}
//
	// Returns success bool and a string of 0 and 1 for 8 connected displays
	// char[8] is a single byte with the same info encoded in bits

	int GetConnectedDisplays(char* outStr) {

		for (int i = 0;i < CHARSIZE;i++) outStr[i] = 0;
		// Init

		if (NVInit(outStr) == 0) {
			NvAPI_Unload();
			return 0;
		}

		// Execute

		if (GetDisplays(outStr) == 0) {
			NvAPI_Unload();
			return 0;
		}

		NvAPI_Unload();

		return 1;
	}


	int GetGridSetup(char* outStr)
	{
		for (int i = 0;i < CHARSIZE;i++) outStr[i] = 0;

		// Init

		if (NVInit(outStr) == 0) {
			NvAPI_Unload();
			return 0;
		}

		if (GetGrids(outStr) == 0) {
			NvAPI_Unload();
			return 0;
		}

		NvAPI_Unload();

		return 1;

	}

	int SetGridSetup(char* inStr, char* outStr)
	{
		for (int i = 0;i < CHARSIZE;i++) outStr[i] = 0;

		// Init

		if (NVInit(outStr) == 0) {
			NvAPI_Unload();
			return 0;
		}

		if (SetGrids(inStr, outStr) == 0) {
			NvAPI_Unload();
			return 0;
		}

		NvAPI_Unload();

		return 1;

	}





}

