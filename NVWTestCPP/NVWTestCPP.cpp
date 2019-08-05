// NVWTestCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "NVWrapper.h"
#include <bitset>

using namespace std;


typedef unsigned long NvU32;
typedef unsigned char NvU8;
typedef signed int NvS32;
struct NvDisplay {
	NvU32 displayId;
	NvS32 overlapx;
	NvS32 overlapy;

};
struct NvGrid {
	NvU8 displayCount;
	NvU8 columns;
	NvU8 rows;
	NvU32 width;
	NvU32 height;
	NvU32 freq;
	NvDisplay* displays;

};
struct NvTopo {
	NvGrid* grids;
	NvU32 gridCount;
};

NvU8 ReadStreamNvU8(char* charArray, int &point);
void WriteStreamNvU8(char* charArray, int &point, NvU8 theChar);
NvU32 ReadStreamNvU32(char* charArray, int &point);
void WriteStreamNvU32(char* charArray, int &point, NvU32 value);
NvS32 ReadStreamNvS32(char* charArray, int &point);
void WriteStreamNvS32(char* charArray, int &point, NvS32 value);
NvTopo DeserialiseTopo(char* input);





const int CHARSIZE = 256;

int main()
{
	std::cout << "Testing NVAPI wrapper\n\n";

	char output[CHARSIZE];
	int result;

	result = GetConnectedDisplays(output);

	if (result == 0) {
		cout << "Connected Displays Failed\n";
		cout << output;
		return 0;
	}

	int sp = 0;
	char connected = ReadStreamNvU8(output, sp);
	char active = ReadStreamNvU8(output, sp);
	char intensity = ReadStreamNvU8(output, sp);


	std::cout << "connected: " << std::bitset<8>(connected) << '\n';
	std::cout << "active: " << std::bitset<8>(active) << '\n';
	std::cout << "intensity: " << std::bitset<8>(intensity) << '\n';

	NvU32* DisplayIds = new NvU32[8];

	for (int d = 0;d < 8;d++)
	{
		DisplayIds[d] = ReadStreamNvU32(output, sp);
		cout << "Display ID " << d << ": " << (unsigned int)DisplayIds[d] << "\n";
	}

	//	int sp;
		//int r2;
	NvU8 gridCount;
	NvTopo theTopo = {};
	NvU8 SetGrid;
	NvU8 SetBlend;

	switch (connected) {

	case 0b00001111:

		cout << "Dual desktop plus wall\n";

		result = GetGridSetup(output);

		if (result == 0) {
			cout << "Get grid setup failed\n";
			cout << output;
			return 0;
		}

		theTopo = DeserialiseTopo(output);
		SetGrid = 1;
		SetBlend = 1;

		if (theTopo.gridCount == 1 && theTopo.grids[0].columns == 2) {
			cout << "Topology is already set correctly. \n";
			SetGrid = 0;
		}

		char input[CHARSIZE];
		sp = 0;
		// Gridcount
		WriteStreamNvU8(input, sp, 1);

		// Grids

		// Displays, columns, rows

		WriteStreamNvU8(input, sp, 2);
		WriteStreamNvU8(input, sp, 2);
		WriteStreamNvU8(input, sp, 1);

		// Width, height, freq

		WriteStreamNvU32(input, sp, 1920);
		WriteStreamNvU32(input, sp, 1080);
		WriteStreamNvU32(input, sp, 60);

		// Displays

		// ID, overlapx, overlap y

		WriteStreamNvU32(input, sp, DisplayIds[2]);
		WriteStreamNvU32(input, sp, 0);
		WriteStreamNvU32(input, sp, 0);

		// ID, overlapx, overlap y

		WriteStreamNvU32(input, sp, DisplayIds[3]);
		WriteStreamNvU32(input, sp, 120);
		WriteStreamNvU32(input, sp, 0);

		// Apply grid?

		WriteStreamNvU8(input, sp, SetGrid);

		// Apply blend?

		WriteStreamNvU8(input, sp, SetBlend);

		//result = 0;

		result = SetGridSetup(input, output);

		if (result == 0) {
			cout << "Failed\n";
			cout << output;
			return 0;
		}

		cout << "Success\n";
		cout << "\n";

		break;


	default:
		break;
	}


}


NvTopo DeserialiseTopo(char* input) {

	int sp = 0;

	NvTopo theTopo = NvTopo{};
	theTopo.gridCount = ReadStreamNvU8(input, sp);
	theTopo.grids = new NvGrid[theTopo.gridCount];

	cout << "Gridcount " << (unsigned int)theTopo.gridCount << "\n";

	for (int g = 0;g < theTopo.gridCount;g++) {

		//	theTopo.grids[g] = NvGrid{};

		theTopo.grids[g].displayCount = ReadStreamNvU8(input, sp);
		theTopo.grids[g].columns = ReadStreamNvU8(input, sp);
		theTopo.grids[g].rows = ReadStreamNvU8(input, sp);

		cout << "displayCount " << (unsigned int)theTopo.grids[g].displayCount << "\n";
		cout << "columns " << (unsigned int)theTopo.grids[g].columns << "\n";
		cout << "rows " << (unsigned int)theTopo.grids[g].rows << "\n";

		theTopo.grids[g].width = ReadStreamNvU32(input, sp);
		theTopo.grids[g].height = ReadStreamNvU32(input, sp);
		theTopo.grids[g].freq = ReadStreamNvU32(input, sp);
		cout << "width " << (unsigned int)theTopo.grids[g].width << "\n";
		cout << "height " << (unsigned int)theTopo.grids[g].height << "\n";
		cout << "freq " << (unsigned int)theTopo.grids[g].freq << "\n";

		theTopo.grids[g].displays = new NvDisplay[theTopo.grids[g].displayCount];

		for (int d = 0;d < theTopo.grids[g].displayCount;d++) {

			theTopo.grids[g].displays[d].displayId = ReadStreamNvU32(input, sp);
			theTopo.grids[g].displays[d].overlapx = ReadStreamNvU32(input, sp);
			theTopo.grids[g].displays[d].overlapy = ReadStreamNvU32(input, sp);
			cout << "Display ID " << (unsigned int)theTopo.grids[g].displays[d].displayId << "\n";
			cout << "overlap x " << (signed int)theTopo.grids[g].displays[d].overlapx << "\n";
			cout << "overlap y " << (signed int)theTopo.grids[g].displays[d].overlapy << "\n";

		}

	}
	return theTopo;
}


NvU8 ReadStreamNvU8(char* charArray, int &point) {
	point++;
	return charArray[point - 1];
}

void WriteStreamNvU8(char* charArray, int &point, NvU8 theChar) {
	charArray[point] = theChar;
	point++;

}


NvU32 ReadStreamNvU32(char* charArray, int &point) {


	//	NvU32 value = charArray[point + 0] + charArray[point + 1] << 8 + charArray[point + 2] << 16 + charArray[point + 3] << 24;

	NvU8 value0 = (NvU8)charArray[point + 0];
	NvU8 value1 = (NvU8)charArray[point + 1];
	NvU8 value2 = (NvU8)charArray[point + 2];
	NvU8 value3 = (NvU8)charArray[point + 3];

	NvU32 value = value0 + value1 * (1 << 8) + value2 * (1 << 16) + value3 * (1 << 24);

	//NvU32 value = 128;
	point += 4;
	return value;

}

void WriteStreamNvU32(char* charArray, int &point, NvU32 value) {

	charArray[point + 0] = value & 0x000000ff;
	charArray[point + 1] = (value & 0x0000ff00) >> 8;
	charArray[point + 2] = (value & 0x00ff0000) >> 16;
	charArray[point + 3] = (value & 0xff000000) >> 24;

	point += 4;

}

NvS32 ReadStreamNvS32(char* charArray, int &point) {

	NvS32 value = charArray[point + 0] + charArray[point + 1] << 8 + charArray[point + 2] << 16 + charArray[point + 3] << 24;

	point += 4;
	return value;

}

void WriteStreamNvS32(char* charArray, int &point, NvS32 value) {

	charArray[point + 0] = value & 0x000000ff;
	charArray[point + 1] = (value & 0x0000ff00) >> 8;
	charArray[point + 2] = (value & 0x00ff0000) >> 16;
	charArray[point + 3] = (value & 0xff000000) >> 24;

	point += 4;

}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
