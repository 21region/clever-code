/* Extractor of global variables from .map files.
   To get .map file from .cpp code you need the following:
   1. compile and get .obj file (e.g. with gcc)
   2. run through every .obj file with linker (e.g. ld) and get .map files
   3. fill the file "names.txt" with the names of .map files and run this program
   4. in the output.txt you get the "filename" and the list of global vars in it
*/

#include <list>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

int main()
{
    cout << "Read file names.\n";

    // Read the list of file names that should be parsed.
    list<string> fileNameList;
    ifstream in("names.txt");
    while (!in.eof())
    {
        string name;
        in >>  name;

        fileNameList.push_back(name);
    }
    in.close();

    /* List of files and global variables in it.
     * Example:
       000000.cpp ancestor ansv cost graph M N p qr query u
       000001.cpp
       000002.cpp
       000003.cpp
       000004.cpp
       000005.cpp
       000006.cpp
       000007.cpp edge tree
    */
    ofstream out("ouput.txt");

    cout << "Begin global variable names extrusion.\n";

    // Open each file in fileNameList and get global vars from it.
    list<string>::iterator fileName = fileNameList.begin();
    while (fileName != fileNameList.end())
    {
        ifstream mapFile(*fileName); // open map file.

        // Write out name of the source .cpp file.
        // There will be a list of names after it (maybe).
        size_t extensionPos = fileName->find(".o");
        out << fileName->substr(0, extensionPos)
            << ".cpp";

        /* First we read initialized global variables.
         * They're stored in .data sector.
         * Every map file has 3 lines with ".data" word in it.
         * Example:
           .data           0x00405000      0x200
                           0x00405000                __data_start__ = .
           *(.data)
           .data          0x00405000       0x40 c:\Projects\Temp\runs\003626.o
         * We should skip them and check next lines.
         * If we see "*(.data2)" in a string - it's over.
         * Otherwise, it's a global variable.
        */
        string garbage;
        size_t countDataLines = 0;
        while (countDataLines < 3)     // Read line by line, until ".data"
        {                              // was encountered 3 times.
            getline(mapFile, garbage);
            if (garbage.find(".data") != string::npos)
                countDataLines++;
        }

        // Extract global variable names until we see "(.data2)".
        // Example:
        // 0x00405000                set
        string variableName;
        mapFile >>  variableName;     // Either "0x00405000" or "*(.data2)".
        while (variableName != "*(.data2)")
        {
            mapFile >> variableName;  // If we're here, it's a name.
            if (variableName.find("::") == string::npos)
                out << " " << variableName;

            mapFile >> variableName;  // Again it's "0x00405000" or "*(.data2)".
        }
        // Operator >> leaves symbol "\n" in a stream, so delete it.
        mapFile.ignore();

        /* To this moment, all initialized global variables were read.
         * We need to extract uninitialized data and things're done.
         * Unitialized data are stored in .bss sector.
         * The same thing with 3 occurences of sector name applies to .bss.
         * Read stream until we encounter ".bss" 3 times.
         * After that, there're strings of names or special word: "*(COMMON)"
         * Example:
           .bss            0x00408000        0xc
                           0x00408000                __bss_start__ = .
           *(.bss)
           .bss            0x00408000        0xc c:\Projects\Temp\runs\003626.o
                           0x00408000                wordsCounter
                           0x00408004                timer
           *(COMMON)
        */
        size_t countBssLines = 0;
        while (countBssLines < 3)
        {
            getline(mapFile, garbage);
            if (garbage.find(".bss") != string::npos)
                countBssLines++;
        }

        // Extract global variable names until we see "*(COMMON)".
        mapFile >>  variableName;
        while (variableName != "*(COMMON)")
        {
            mapFile >> variableName;
            if (variableName.find("::") == string::npos)
                out << " " << variableName;
            mapFile >> variableName;
        }

        // We've read all global variables.
        // Go to the next line.
        out << "\n";
        mapFile.close();
        fileName++;
    }

    cout << "Extrusion is ended.\n";
    cout << "Watch file \"output\".txt\n";
}
