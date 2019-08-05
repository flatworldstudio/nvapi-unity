#include <stdio.h>

#include "pch.h"
#include "NVWrapper.h"
//#include <stdlib.h>
//#include <math.h> 
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
	// Return grid config

	// gridCount

	// GRIDS 
	// displayCount
	// rowCount
	// columnCount

	// DISPLAYS 
	// ID 4 bytes NvU32
	// overlapx 4 bytes NvS32
	// overlapy 4 bytes NvS32

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
		/*	WriteStreamNvU32(outStr, sp, 1920);
			WriteStreamNvU32(outStr, sp, 125);
			WriteStreamNvU32(outStr, sp, 70);*/
		WriteStreamNvU32(outStr, sp, ds.height);
		WriteStreamNvU32(outStr, sp, ds.freq);

		//ds.width; ds.height; ds.freq; //Width, Heiht and Refresh Rate for all displays
		//printf("Width %d\n", ds.width);
		//printf("Height %d\n", ds.height);
		//printf("Frequency %d\n", ds.freq);


		for (NvU32 iDisplay = 0; iDisplay < gridTopo[iGrid].displayCount; iDisplay++) {

			//	int displayIndex = headerSize + iGrid * gridSize + 4 + iDisplay * displaySize;

			NV_MOSAIC_GRID_TOPO_DISPLAY& display = gridTopo[iGrid].displays[iDisplay];

			//	NvU32 displayId = display.displayId;//unique identifier for this display, that  will be used for all subsequent functions

			WriteStreamNvU32(outStr, sp, display.displayId);
			//	WriteStreamNvU32(outStr, sp, 15);
			WriteStreamNvU32(outStr, sp, display.overlapX);
			WriteStreamNvU32(outStr, sp, display.overlapY);

			/*WriteStreamNvU32(outStr, sp, 1024);
			WriteStreamNvU32(outStr, sp, -1024);*/

			//display.overlapX; //horizontal overlap for this display, explained later
			//display.overlapY; //vertical overlap for this display, explained later




		}



	}

	//for (int i = 0; i < 64; ++i) outStr[i] = message[i];

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

	error = NvAPI_Mosaic_SetDisplayGrids(gridTopo, gridCount, NV_MOSAIC_SETDISPLAYTOPO_FLAG_CURRENT_GPU_TOPOLOGY);

	if ((error != NVAPI_OK))
	{
		NvAPI_GetErrorMessage(error, message);
		for (int i = 0; i < 64; ++i) outStr[i] = message[i];
		return 0;
	}

	return 1;

}

int GetDisplays(char* outStr) {

	// Init nvapi before calling.

	// This populates a char outChar with each bit a connected display. (powered or not)
	// Result or error message is in outStr;



	NvAPI_Status error;
	NvAPI_ShortString message = "";

	NvU8 config;

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

	config = 0;

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

		if (dispIdCount > 0)
		{
			// Create an array to hold refs and get refs

			NV_GPU_DISPLAYIDS* dispIds = NULL;
			dispIds = new NV_GPU_DISPLAYIDS[dispIdCount];
			dispIds->version = NV_GPU_DISPLAYIDS_VER;

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

				if (i > 7) {

					for (int i = 0; i < 64; ++i) outStr[i] = message[i];
					return 0;
				}

				DisplayIdList[i] = dispIds[dispIndex].displayId;

				if ((dispIndex < dispIdCount))

					//	if (DisplayIdList[i] != 0)
				{

					//			DisplayConnected[i] = 1;
					config |= 1 << i;

				}
				else
				{
					//		DisplayConnected[i] = 0;
				}
			}
		}

	}

	int sp = 0;
	WriteStreamNvU8(outStr, sp, config);
	for (int d = 0;d < DisplaysPerGpu*gpuCount;d++) {

		WriteStreamNvU32(outStr, sp, DisplayIdList[d]);

	}

	//outStr[8] = config;
	//	for (int i = 0; i < 64; ++i) outStr[i] = message[i];


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

