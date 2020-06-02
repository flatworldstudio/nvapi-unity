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

// Structs to hold info on a display topology. These can be mirrored on the unity side. Transfer via serialisation.

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
	NvU8 applyBlend;
	NvDisplay* displays;

};
struct NvTopo {
	NvGrid* grids;
	NvU32 gridCount;
	NvU8 ApplyGrid;
	//NvU8 ApplyBlend;

	bool Matches(NvTopo match)
	{
		if (gridCount != match.gridCount) return false;

		for (int g = 0;g < gridCount;g++) {
			if (grids[g].displayCount != match.grids[g].displayCount) return false;
			if (grids[g].columns != match.grids[g].columns) return false;
			if (grids[g].rows != match.grids[g].rows) return false;
			if (grids[g].width != match.grids[g].width) return false;
			if (grids[g].height != match.grids[g].height) return false;
			if (grids[g].freq != match.grids[g].freq) return false;
			for (int d = 0;d < grids[g].displayCount;d++) {
				if (grids[g].displays[d].displayId != match.grids[g].displays[d].displayId) return false;
				if (grids[g].displays[d].overlapx != match.grids[g].displays[d].overlapx) return false;
				if (grids[g].displays[d].overlapy != match.grids[g].displays[d].overlapy) return false;
			}
		}


		return true;

	}

};

NvU8 ReadStreamNvU8(char* charArray, int &point);
void WriteStreamNvU8(char* charArray, int &point, NvU8 theChar);
NvU32 ReadStreamNvU32(char* charArray, int &point);
void WriteStreamNvU32(char* charArray, int &point, NvU32 value);


NvTopo DeserialiseTopo(char* input);
void SerialiseTopo(NvTopo topo, char* input);


const int CHARSIZE = 256; // Limit for char arrays to transfer (serialised) data.

int main()
{
	std::cout << "Testing NVAPI wrapper\n\n";

	char output[CHARSIZE];
	int result;

	result = GetHardware(output);

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

	NvU8 gridCount;
	NvTopo theTopo = {};
	NvU8 SetGrid;
	NvU8 SetBlend;

	switch (connected) {

	case 0b00000011:

		cout << "Dual desktop \n";

		result = GetGrids(output);

		if (result == 0) {
			cout << "Get grid setup failed\n";
			cout << output;
			return 0;
		}

		theTopo = DeserialiseTopo(output);

		NvTopo newTopo = NvTopo{};
		newTopo.gridCount = 1;

		newTopo.grids = new NvGrid[newTopo.gridCount];
		newTopo.grids[0].displayCount = 2;
		newTopo.grids[0].columns = 2;
		newTopo.grids[0].rows = 1;
		newTopo.grids[0].width = 1920;
		newTopo.grids[0].height = 1080;
		newTopo.grids[0].freq = 60;
		newTopo.grids[0].applyBlend = 1;

		newTopo.grids[0].displays = new NvDisplay[newTopo.grids[0].displayCount];

		newTopo.grids[0].displays[0].displayId = DisplayIds[0];
		newTopo.grids[0].displays[0].overlapx = 0;
		newTopo.grids[0].displays[0].overlapy = 0;

		newTopo.grids[0].displays[1].displayId = DisplayIds[1];
		newTopo.grids[0].displays[1].overlapx = 120;
		newTopo.grids[0].displays[1].overlapy = 0;
		
		/*
		NvTopo newTopo = NvTopo{};
		newTopo.gridCount = 3;

		newTopo.grids = new NvGrid[newTopo.gridCount];
		newTopo.grids[0].displayCount = 2;
		newTopo.grids[0].columns = 2;
		newTopo.grids[0].rows = 1;
		newTopo.grids[0].width = 1920;
		newTopo.grids[0].height = 1200;
		newTopo.grids[0].freq = 60;
		newTopo.grids[0].applyBlend = 0;

		newTopo.grids[0].displays = new NvDisplay[newTopo.grids[0].displayCount];

		newTopo.grids[0].displays[0].displayId = DisplayIds[0];
		newTopo.grids[0].displays[0].overlapx = 0;
		newTopo.grids[0].displays[0].overlapy = 0;
		newTopo.grids[0].displays[1].displayId = DisplayIds[1];
		newTopo.grids[0].displays[1].overlapx = 0;
		newTopo.grids[0].displays[1].overlapy = 0;

		//

		newTopo.grids[1].displayCount = 1;
		newTopo.grids[1].columns = 1;
		newTopo.grids[1].rows = 1;
		newTopo.grids[1].width = 1920;
		newTopo.grids[1].height = 1080;
		newTopo.grids[1].freq = 60;
		newTopo.grids[1].applyBlend = 0;

		newTopo.grids[1].displays = new NvDisplay[newTopo.grids[1].displayCount];

		newTopo.grids[1].displays[0].displayId = DisplayIds[2];
		newTopo.grids[1].displays[0].overlapx = 0;
		newTopo.grids[1].displays[0].overlapy = 0;

		//

		newTopo.grids[2].displayCount = 1;
		newTopo.grids[2].columns = 1;
		newTopo.grids[2].rows = 1;
		newTopo.grids[2].width = 1920;
		newTopo.grids[2].height = 1080;
		newTopo.grids[2].freq = 60;
		newTopo.grids[2].applyBlend = 0;

		newTopo.grids[2].displays = new NvDisplay[newTopo.grids[2].displayCount];

		newTopo.grids[2].displays[0].displayId = DisplayIds[3];
		newTopo.grids[2].displays[0].overlapx = 0;
		newTopo.grids[2].displays[0].overlapy = 0;

		*/
		newTopo.ApplyGrid = 1;

		if (theTopo.Matches(newTopo))
		{
			cout << "Topo already set\n";
	newTopo.ApplyGrid = 0;
		}

		//	newTopo.ApplyBlend = 0;


			/*
					NvTopo dualTopo = NvTopo{};
					dualTopo.gridCount = 2;

					dualTopo.grids = new NvGrid[dualTopo.gridCount];

					dualTopo.grids[0].displayCount = 1;
					dualTopo.grids[0].columns = 1;
					dualTopo.grids[0].rows = 1;
					dualTopo.grids[0].width = 1920;
					dualTopo.grids[0].height = 1080;
					dualTopo.grids[0].freq = 60;

					dualTopo.grids[0].displays = new NvDisplay[dualTopo.grids[0].displayCount];

					dualTopo.grids[0].displays[0].displayId = DisplayIds[2];
					dualTopo.grids[0].displays[0].overlapx = 0;
					dualTopo.grids[0].displays[0].overlapy = 0;

					dualTopo.grids[1].displayCount = 1;
					dualTopo.grids[1].columns = 1;
					dualTopo.grids[1].rows = 1;
					dualTopo.grids[1].width = 1920;
					dualTopo.grids[1].height = 1080;
					dualTopo.grids[1].freq = 60;

					dualTopo.grids[1].displays = new NvDisplay[dualTopo.grids[0].displayCount];

					dualTopo.grids[1].displays[0].displayId = DisplayIds[3];
					dualTopo.grids[1].displays[0].overlapx = 0;
					dualTopo.grids[1].displays[0].overlapy = 0;

					dualTopo.ApplyGrid = 1;
					dualTopo.ApplyBlend = 0;*/


		char input[CHARSIZE];

		SerialiseTopo(newTopo, input);

		result = SetGrids(input, output);

	//	cout << result;

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

	// Deserialises char array into a new topology struct. To be mirrored on unity side.

	int sp = 0;

	NvTopo theTopo = NvTopo{};

	theTopo.gridCount = ReadStreamNvU8(input, sp);
	theTopo.grids = new NvGrid[theTopo.gridCount];

	cout << "Gridcount " << (unsigned int)theTopo.gridCount << "\n";

	for (int g = 0;g < theTopo.gridCount;g++) {

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

		theTopo.grids[g].applyBlend = ReadStreamNvU8(input, sp);// no value when getting from nvapi.
		
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

	// last nvu8 is applygrid in our struct, no point reading it here

	return theTopo;
}

void SerialiseTopo(NvTopo theTopo, char* input) {

	// Serialises  a topology struct into a char array. To be mirrored on unity side.

	int sp = 0;

	// Serialise gridcount.

	NvU8 Gridcount = theTopo.gridCount;
	WriteStreamNvU8(input, sp, Gridcount);// dot notation cannot be applied to pointers.

		// Serialise Grids

	for (int g = 0;g < Gridcount;g++) {

		NvGrid theGrid = theTopo.grids[g];

		// Displays, columns, rows

		WriteStreamNvU8(input, sp, theGrid.displayCount);
		WriteStreamNvU8(input, sp, theGrid.columns);
		WriteStreamNvU8(input, sp, theGrid.rows);

		// Width, height, freq

		WriteStreamNvU32(input, sp, theGrid.width);
		WriteStreamNvU32(input, sp, theGrid.height);
		WriteStreamNvU32(input, sp, theGrid.freq);

		// Apply blend?

		WriteStreamNvU8(input, sp, theGrid.applyBlend);

		// Serialise displays

		for (int d = 0;d < theGrid.displayCount;d++)
		{
			NvDisplay theDisplay = theGrid.displays[d];

			// ID, overlapx, overlap y

			WriteStreamNvU32(input, sp, theDisplay.displayId);
			WriteStreamNvU32(input, sp, theDisplay.overlapx);
			WriteStreamNvU32(input, sp, theDisplay.overlapy);

		}

	}

	// Apply grid?

	WriteStreamNvU8(input, sp, theTopo.ApplyGrid);
	

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

	// Read 32 bit unsigned

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

	// write 32 bit unsigned 

	charArray[point + 0] = value & 0x000000ff;
	charArray[point + 1] = (value & 0x0000ff00) >> 8;
	charArray[point + 2] = (value & 0x00ff0000) >> 16;
	charArray[point + 3] = (value & 0xff000000) >> 24;

	point += 4;

}

//NvS32 ReadStreamNvS32(char* charArray, int &point) {
//
//	NvS32 value = charArray[point + 0] + charArray[point + 1] << 8 + charArray[point + 2] << 16 + charArray[point + 3] << 24;
//
//	point += 4;
//	return value;
//
//}
//
//void WriteStreamNvS32(char* charArray, int &point, NvS32 value) {
//
//	charArray[point + 0] = value & 0x000000ff;
//	charArray[point + 1] = (value & 0x0000ff00) >> 8;
//	charArray[point + 2] = (value & 0x00ff0000) >> 16;
//	charArray[point + 3] = (value & 0xff000000) >> 24;
//
//	point += 4;
//
//}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
