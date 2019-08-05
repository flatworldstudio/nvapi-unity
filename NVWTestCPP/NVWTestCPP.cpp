// NVWTestCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "NVWrapper.h"
using namespace std;


typedef unsigned long NvU32;
typedef unsigned char NvU8;
typedef signed int NvS32;

NvU8 ReadStreamNvU8(char* charArray, int &point);
void WriteStreamNvU8(char* charArray, int &point, NvU8 theChar);
NvU32 ReadStreamNvU32(char* charArray, int &point);
void WriteStreamNvU32(char* charArray, int &point, NvU32 value);
NvS32 ReadStreamNvS32(char* charArray, int &point);
void WriteStreamNvS32(char* charArray, int &point, NvS32 value);


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



int main()
{
	std::cout << "Testing NVAPI wrapper\n";

	char feedback[256];
	char input[256];
	int r, r2;

	r = GetConnectedDisplays(feedback);
	int sp = 0;
	char config = ReadStreamNvU8(feedback, sp);


	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";
	cout << "Display ID " << (unsigned int)ReadStreamNvU32(feedback, sp) << "\n";


	//std::string  FeedbackString = feedback;

	if (r == 0) {
		cout << "Failed\n";
		cout << feedback;
		cout << "\n";
	}
	else {
		cout << "Success\n";
		//	cout << feedback;
		cout << "\n";

		int sp;
		NvU8 gridCount;

		switch (config) {
		case 0b00001100:
			cout << "Dual desktop only\n";

			break;
		case 0b00001111:

			cout << "Dual desktop plus wall\n";

			r = GetGridSetup(feedback);
			sp = 0;
			gridCount = ReadStreamNvU8(feedback, sp);

			cout << "Gridcount " << (unsigned int)gridCount << "\n";

			for (int g = 0;g < gridCount;g++) {
				NvU8 displayCount = ReadStreamNvU8(feedback, sp);
				NvU8 columns = ReadStreamNvU8(feedback, sp);
				NvU8 rows = ReadStreamNvU8(feedback, sp);
				cout << "displayCount " << (unsigned int)displayCount << "\n";
				cout << "columns " << (unsigned int)columns << "\n";
				cout << "rows " << (unsigned int)rows << "\n";

				NvU32 width = ReadStreamNvU32(feedback, sp);
				//	width = 1920;
				NvU32 height = ReadStreamNvU32(feedback, sp);
				NvU32 freq = ReadStreamNvU32(feedback, sp);
				cout << "width " << (unsigned int)width << "\n";
				cout << "height " << (unsigned int)height << "\n";
				cout << "freq " << (unsigned int)freq << "\n";

				for (int d = 0;d < displayCount;d++) {
					NvU32 displayId = ReadStreamNvU32(feedback, sp);
					NvS32 overlapx = ReadStreamNvU32(feedback, sp);
					NvS32 overlapy = ReadStreamNvU32(feedback, sp);
					cout << "Display ID " << (unsigned int)displayId << "\n";
					cout << "overlap x " << (signed int)overlapx << "\n";
					cout << "overlap y " << (signed int)overlapy << "\n";

				}

			}



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

			WriteStreamNvU32(input, sp, 2147881093);
			WriteStreamNvU32(input, sp, 0);
			WriteStreamNvU32(input, sp, 0);

			// ID, overlapx, overlap y

			WriteStreamNvU32(input, sp, 2147881092);
			WriteStreamNvU32(input, sp, 100);
			WriteStreamNvU32(input, sp, 0);


			//sp = 0;
			r2=0;
		//	r2 = SetGridSetup(input, feedback);

			if (r2 == 0) {
				cout << "Failed\n";
				cout << feedback;
				cout << "\n";
			}
			else {
				cout << "Success\n";
				cout << feedback;
				cout << "\n";
			}

			//	cout << "Set Grid " << feedback << "\n";

			/*	cout << "Read  " << (unsigned int)ReadStreamNvU8(feedback,sp) << "\n";
				cout << "Read  " << (unsigned int)ReadStreamNvU8(feedback, sp) << "\n";*/
				/*		cout << "Read  " << (signed int)ReadStreamNvU32(feedback, sp) << "\n";
						cout << "Read  " << (signed int)ReadStreamNvU32(feedback, sp) << "\n";*/

			break;


		default:
			break;
		}


	}



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
