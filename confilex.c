#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include "dirent.h"
#include <Shellapi.h>
#include <WinUser.h>

#define MAX_PATH 260
#define MAX_ENTRIES 1024

// Windows only program
// Made by NoxNode (https://github.com/noxnode/)
// search for "////////////////////////////////////" to see all the different parts of the program

//////////////////////////////////// TODOs ////////////////////////////////////

// TODO: split pNextDirPath by '\\'s and fixPath for each part
// TODO: tab completion when typing or just go to whichever most matches it when they hit enter (like to go into confilex just type c if there's no other directories starting with c)
// TODO: cut, copy, paste, delete
// TODO: file properties
// TODO: make the action keys rebindable with a config.txt file

// TODO: store and echo multiple directories for the current session
// TODO: save stored directories to a file
// TODO: load saved directories from file

// TODO: exit and change to current directory

//////////////////////////////////// general util function headers - for more details, go to the implementations ////////////////////////////////////

void copyStringToClipboard(char* string1, int length);
void setConsoleColor(int color);
int stringsEqual(char* string1, char* string2);
int stringLength(char* string1);
void copyString(char** string1, int* in_out_index1, char** string2, int* in_out_index2, int length);
void removeCharsAfterSecondToLastBackslash(char** pCurDirPath);
void gotoXY(int x, int y);

//void splitString();
//void concatString();

//////////////////////////////////// program specific function headers - for more details, go to the implementations ////////////////////////////////////

int getDirEntries(char* dirPath, char** out_entries, int** out_entry_indices);
void fixPath(char** pNextDirPath, char** pCurDirPath, char** pTempDirPath);
void displayAndUpdateCurDir(char** pNextDirPath, char** pCurDirPath, char** pDirEntries, int** pDirEntryIndices, int* nEntries, int* curEntryIndex, int* curDirPathLength);

//void completePath();

//////////////////////////////////// program entry point ////////////////////////////////////

int main(int argc, char **argv) {
	//////////////////////////////////// variable declarations ////////////////////////////////////
	char nextDirPath[MAX_PATH];
	char tempDirPath[MAX_PATH];
	char curDirPath[MAX_PATH];
	char dirEntries[MAX_ENTRIES * MAX_PATH];
	int dirEntryIndices[MAX_ENTRIES];
	char* pNextDirPath = &nextDirPath[0];
	char* pTempDirPath = &tempDirPath[0];
	char* pCurDirPath = &curDirPath[0];
	char* pDirEntries = &dirEntries[0];
	int* pDirEntryIndices = &dirEntryIndices[0];

	int nEntries = 0;
	int curEntryIndex = 0;
	int curDirPathLength = 0;

	int copyStringIndex1;
	int copyStringIndex2;

	//////////////////////////////////// initialize stuff ////////////////////////////////////

	// get current working directory
	_getcwd(curDirPath, sizeof(curDirPath));
	// if user specifies an initial path, go to that path
	if(argc == 2) {
		copyStringIndex1 = 0;
		copyStringIndex2 = 0;
		copyString(&pNextDirPath, &copyStringIndex1, &(argv[1]), &copyStringIndex2, -1);

		fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);
	}
	else {
		// otherwise, use the current working directory
		copyStringIndex1 = 0;
		copyStringIndex2 = 0;
		copyString(&pNextDirPath, &copyStringIndex1, &pCurDirPath, &copyStringIndex2, -1);
	}
	
	displayAndUpdateCurDir(&pNextDirPath, &pCurDirPath, &pDirEntries,
		&pDirEntryIndices, &nEntries, &curEntryIndex, &curDirPathLength);

	//////////////////////////////////// loop until exit button pressed ////////////////////////////////////
	while(1) {
nextAction:
		{
			char action = getch();
			switch (action)
			{
			case 'w':
			case 'W': { // go up an entry
				curEntryIndex--;
				if (curEntryIndex < 0) {
					curEntryIndex = 0;
				}
				gotoXY(0, curEntryIndex);
				goto nextAction;
				break;
			}
			case 's':
			case 'S': { // go down an entry
				curEntryIndex++;
				if (curEntryIndex >= nEntries) {
					curEntryIndex = nEntries - 1;
				}
				gotoXY(0, curEntryIndex);
				goto nextAction;
				break;
			}
			case 'i':
			case 'I': { // go up 5 entries
				curEntryIndex -= 5;
				if (curEntryIndex < 0) {
					curEntryIndex = 0;
				}
				gotoXY(0, curEntryIndex);
				goto nextAction;
				break;
			}
			case 'k':
			case 'K': { // go down 5 entries
				curEntryIndex += 5;
				if (curEntryIndex >= nEntries) {
					curEntryIndex = nEntries - 1;
				}
				gotoXY(0, curEntryIndex);
				goto nextAction;
				break;
			}
			case 'g':
			case 'G': { // go to n entrie
				// go to last line so they don't have to write over an entry
				curEntryIndex = nEntries;
				gotoXY(0, curEntryIndex);
				
				// get entry index from user
				scanf("%i", &curEntryIndex);
				
				// make sure its in range
				if (curEntryIndex < 0) {
					curEntryIndex = 0;
				}
				if (curEntryIndex >= nEntries) {
					curEntryIndex = nEntries - 1;
				}
				
				// go to that index
				gotoXY(0, curEntryIndex);
				goto nextAction;
				break;
			}
			case 'j':
			case 'J':
			case 'a':
			case 'A': { // go to parent directory
				pNextDirPath[0] = '.';
				pNextDirPath[1] = '.';
				pNextDirPath[2] = '\0';
				break;
			}
			case 'l':
			case 'L':
			case 'd':
			case 'D': { // go into directory
				// copy dirEntries at the curEntryIndex until '\n' into pNextDirPath
				copyStringIndex1 = 0;
				copyStringIndex2 = pDirEntryIndices[curEntryIndex];
				copyString(&pNextDirPath, &copyStringIndex1, &pDirEntries, &copyStringIndex2, -2);
				break;
			}
			case 'o':
			case 'O':
			case 'f':
			case 'F': { // open selected file
				// copy dirEntries at the curEntryIndex until '\n' into pNextDirPath
				copyStringIndex1 = 0;
				copyStringIndex2 = pDirEntryIndices[curEntryIndex];
				copyString(&pNextDirPath, &copyStringIndex1, &pDirEntries, &copyStringIndex2, -2);
				
				// fix relative path to file
				fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);
				
				// TODO: put quotes around path

				// open file in default way
				// for some reason when I compiled with VC ShellExecute doesn't work
				// it works fine when compiling with MinGW tho
				ShellExecute(0, 0, &(pNextDirPath[0]), 0, 0, SW_SHOW);

				//can use system(), but it doesn't open console applications in a new window
//				system(&(pNextDirPath[0]));
				goto nextAction;
				break;
			}
			case 'u':
			case 'U':
			case 'q':
			case 'Q': { // let user type out full path
				// go to last line so they don't have to write over an entry
				curEntryIndex = nEntries;
				gotoXY(0, curEntryIndex);
				
				// get user input
				scanf("%s", pNextDirPath);
				break;
			}
			case 'z':
			case 'Z':
			case 'm':
			case 'M': { // let user type out full path starting with current
				// go to last line so they don't have to write over an entry
				curEntryIndex = nEntries;
				gotoXY(0, curEntryIndex);
				
				// print current directory
				printf("%s", pCurDirPath);
				gotoXY(curDirPathLength, curEntryIndex);
				
				// get user input
				scanf("%s", pNextDirPath);
				break;
			}
			case 'c':
			case 'C': { // copy current directory to clipboard
				copyStringToClipboard(pCurDirPath, curDirPathLength);
				break;
			}
			case 'p':
			case 'P': {
				// concatString("cd ", pCurDirPath, out_string);
				// system(out_string);
				goto nextAction;
			}
			case 'e':
			case 'E':
				goto exit;
			default: {
				goto nextAction;
				break;
			}
			}
		}
		
		// split string(pNextDirPath)

		// start loop through each nextDir

		fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);

		// TODO: add '\\' in fixPath instead of in displayAndUpdateCurDir so in completePath I can display C:\ instead of getting error that C: isn't a directory
		// complete path

		// end loop through each nextDir

		displayAndUpdateCurDir(&pNextDirPath, &pCurDirPath, &pDirEntries,
			&pDirEntryIndices, &nEntries, &curEntryIndex, &curDirPathLength);
	}

exit:
	system("cls");
	return 0;
}

//////////////////////////////////// general util function implementations ////////////////////////////////////

void copyStringToClipboard(char* string1, int length) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, length);
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), string1, length);
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

// sets color of the text to be written, doesn't change color of text already on screen
void setConsoleColor(int color) {
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

// returns 0 if not equal, 1 if equal up until string1's length, 2 if equal up until string2's length, 3 if exactly equal
int stringsEqual(char* string1, char* string2) {
	int index = 0;
	while(string1[index] != '\0') {
		if(string2[index] == '\0') {
			return 2;
		}
		else if(string1[index] != string2[index]) {
			return 0;
		}
		index++;
	}
	if(string2[index] != '\0') {
		return 1;
	}
	return 3;
}

int stringLength(char* string1) {
	int index = 0;
	while(string1[index] != '\0') {
		index++;
	}
	return index;
}

// copies string2 into string1
// put -1 for length to go until '\0'
// put -2 for length to go until '\n'
void copyString(char** string1, int* in_out_index1, char** string2, int* in_out_index2, int length) {
	int index1 = *in_out_index1;
	int index2 = *in_out_index2;
	if (length == -1) {
		while ((*string2)[index2] != '\0') {
			(*string1)[index1] = (*string2)[index2];
			index1++;
			index2++;
		}
		(*string1)[index1] = '\0';
	}
	else if (length == -2) {
		while ((*string2)[index2] != '\n') {
			(*string1)[index1] = (*string2)[index2];
			index1++;
			index2++;
		}
		(*string1)[index1] = '\0';
	}
	else {
		int nLoops = index1 + length;
		while (index1 < nLoops) {
			(*string1)[index1] = (*string2)[index2];
			index1++;
			index2++;
		}
	}
	*in_out_index1 = index1;
	*in_out_index2 = index2;
}

void removeCharsAfterSecondToLastBackslash(char** pCurDirPath) {
	int secondToLastSlashIndex = 0;
	int lastSlashIndex = 0;
	int index = 0;
	while ((*pCurDirPath)[index] != '\0') {
		secondToLastSlashIndex = lastSlashIndex;
		if ((*pCurDirPath)[index] == '\\') {
			lastSlashIndex = index;
		}
		index++;
	}
	(*pCurDirPath)[secondToLastSlashIndex + 1] = '\0';
}

// puts console cursor at position (x, y)
void gotoXY(int x, int y) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(hStdOut, coord);
}

//////////////////////////////////// program specific function implementations ////////////////////////////////////

// get a list of directory entries from dirPath and stores it in out_entries (separated by new line chars (\n))
// stores the indexes of the first char of each entry in out_entry_indices
// returns number of entries
int getDirEntries(char* dirPath, char** out_entries, int** out_entry_indices) {
	DIR *dir;
	struct dirent *ent;
	int nEntries = 0;
	if ((dir = opendir(dirPath)) != NULL) {
		int index = 0;
		while ((ent = readdir(dir)) != NULL) {
			(*out_entry_indices)[nEntries] = index;
			int index2 = 0;
			while (ent->d_name[index2] != '\0') {
				(*out_entries)[index] = ent->d_name[index2];
				index++;
				index2++;
			}
			(*out_entries)[index] = '\n';
			nEntries++;
			index++;
		}
		(*out_entries)[index] = '\0';
		closedir(dir);
	}
	else {
		return -1;
	}
	return nEntries;
}

// fixes relative paths to make full paths
void fixPath(char** pNextDirPath, char** pCurDirPath, char** pTempDirPath) {
	int copyStringIndex1;
	int copyStringIndex2;
	
	if ((*pNextDirPath)[0] == '.' && ((*pNextDirPath)[1] == '\0' || (*pNextDirPath)[2] == '\0')) { // if . or ..
		if ((*pNextDirPath)[1] == '.') { // if ..
			if (!((*pCurDirPath)[1] == ':' && (*pCurDirPath)[2] == '\\' && (*pCurDirPath)[3] == '\0')) { // if not at highest directory
				removeCharsAfterSecondToLastBackslash(pCurDirPath); // go to parent dir
			}
		}
		copyStringIndex1 = 0;
		copyStringIndex2 = 0;
		copyString(pNextDirPath, &copyStringIndex1, pCurDirPath, &copyStringIndex2, -1);
	}
	else if (!((*pNextDirPath)[1] == ':' && (*pNextDirPath)[2] == '\\')) { // if(not full path) { make relative path into a full path }
		// copy current dir into temp
		copyStringIndex1 = 0;
		copyStringIndex2 = 0;
		copyString(pTempDirPath, &copyStringIndex1, pCurDirPath, &copyStringIndex2, -1);

		// copy next relative dir into temp after current dir
		copyStringIndex2 = 0;
		copyString(pTempDirPath, &copyStringIndex1, pNextDirPath, &copyStringIndex2, -1);
		
		// swap temp and next to get full path
		char* temp = (*pNextDirPath);
		(*pNextDirPath) = (*pTempDirPath);
		(*pTempDirPath) = temp;
	}
}

// requires path to be a fixed, full path
void displayAndUpdateCurDir(char** pNextDirPath, char** pCurDirPath, char** pDirEntries, int** pDirEntryIndices, int* nEntries, int* curEntryIndex, int* curDirPathLength) {
	int copyStringIndex1 = 0;
	int copyStringIndex2 = 0;
	
	// display directory entries
	system("cls");
	int lastNEntries = *nEntries;
	*nEntries = getDirEntries((*pNextDirPath), pDirEntries, pDirEntryIndices);
	printf((*pDirEntries));

	// if getDirEntries returned -1, print out error and copy current directory into next directory
	if(*nEntries == -1) {
		*nEntries = lastNEntries;
		gotoXY(0, *nEntries + 1);
		printf("Couldn't open directory");
		
		copyStringIndex1 = 0;
		copyStringIndex2 = 0;
		copyString(pNextDirPath, &copyStringIndex1, pCurDirPath, &copyStringIndex2, -1);
	}

	// set console cursor position to first entry
	if((*pDirEntries)[0] == '.' && (*pDirEntries)[1] == '\n' && *nEntries > 2) { // skip the . and .. entries if they're not the only entries
		*curEntryIndex = 2;
		gotoXY(0, *curEntryIndex);
	}
	else {
		*curEntryIndex = 0;
		gotoXY(0, *curEntryIndex);
	}

	// copy new directory into current directory
	copyStringIndex1 = 0;
	copyStringIndex2 = 0;
	copyString(pCurDirPath, &copyStringIndex1, pNextDirPath, &copyStringIndex2, -1);
	
	// fix current directory trailing backslash and set curDirPathLength
	*curDirPathLength = copyStringIndex1;
	if ((*pCurDirPath)[copyStringIndex1 - 1] == '/') {
		(*pCurDirPath)[copyStringIndex1 - 1] = '\\';
	}
	if ((*pCurDirPath)[copyStringIndex1 - 1] != '\\') {
		(*pCurDirPath)[copyStringIndex1] = '\\';
		(*pCurDirPath)[copyStringIndex1 + 1] = '\0';
		*curDirPathLength = copyStringIndex1 + 1;
	}
}
