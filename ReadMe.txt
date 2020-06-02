

C# wrapper for NvApi.

Solution contains 2 projects. 

The NVW is the main project, creating a few methods that can be called from c# (unity) to interact with NvApi.


The NVWTestCPP is a test project.
This project references the NVW project in a few ways. 
-> the c/c++ general tab has an 'additional include directories' field, this should reference the nvw project folder. Using relative paths now, so should be ok.
-> the linker general tab has an 'additional library directories' field, this should reference the nvw project output folder (bin/release)
-> build events post build has an command xcopy /y /d "$(SolutionDir)\NVW\bin\Release\NVW.dll" "$(OutDir)" which copies the dll from nvw into nvw test output dir


Serialisation for getting and setting grids:

gridcount
grids:	displaycount
		rows
		columns
		displaywidth
		displayheight
		displayfrequency	
		applyblend (note that this only applies when setting a topology
		displays:		display ID
						overlapx
						overlapy


Serialisation for getting hardware info

connected
active
intensitiy
display:	ID
