#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include "dirent.h"

// Windows only program

void setConsoleColor(int color) {
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void listDirEntries(char* dirPath) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(dirPath)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			printf("%s\n", ent->d_name);
		}
		closedir(dir);
	}
	else {
		printf("could not open directory");
	}
}

int getDirEntries(char* dirPath, char** out_entries, int** out_entry_indices) {
	DIR *dir;
	struct dirent *ent;
	int nEntries = 0;
	if ((dir = opendir(dirPath)) != NULL) {
		/* print all the files and directories within directory */
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

// returns 0 if not equal, 1 if equal up until string1's length, 2 if equal up until string2's length, 3 if exactly equal
int stringsAreEqual(char* string1, char* string2) {
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

// TODO: make relative path completion more robust - account for other drive names
void fixPath(char** pNextDirPath, char** pCurDirPath, char** pTempDirPath) {
	if ((*pNextDirPath)[0] == '.') {
		if ((*pNextDirPath)[1] == '.') {
			if (!((*pCurDirPath)[1] == ':' && (*pCurDirPath)[2] == '\\' && (*pCurDirPath)[3] == '\0')) {
				// TODO: go up a dir
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

				index = 0;
				while ((*pCurDirPath)[index] != '\0') {
					(*pNextDirPath)[index] = (*pCurDirPath)[index];
					index++;
				}
				(*pNextDirPath)[index] = '\0';
			}
			else {
				int index = 0;
				while ((*pCurDirPath)[index] != '\0') {
					(*pNextDirPath)[index] = (*pCurDirPath)[index];
					index++;
				}
				(*pNextDirPath)[index] = '\0';
			}
		}
	}
	else if (!((*pNextDirPath)[1] == ':' && (*pNextDirPath)[2] == '\\')) {
		int index = 0;
		while ((*pCurDirPath)[index] != '\0') {
			(*pTempDirPath)[index] = (*pCurDirPath)[index];
			index++;
		}
		int index2 = 0;
		while ((*pNextDirPath)[index2] != '\0') {
			(*pTempDirPath)[index] = (*pNextDirPath)[index2];
			index++;
			index2++;
		}
		(*pTempDirPath)[index] = '\0';
		char* temp = (*pNextDirPath);
		(*pNextDirPath) = (*pTempDirPath);
		(*pTempDirPath) = temp;
	}
}

void gotoXY(int x, int y)
{
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(hStdOut, coord);
}

// TODO: clean up code into smaller functions

int main(int argc, char **argv)
{
	char nextDirPath[512];
	char tempDirPath[512];
	char curDirPath[512];
	char dirEntries[2048];
	int dirEntryIndices[256];
	char* pNextDirPath = &nextDirPath[0];
	char* pTempDirPath = &tempDirPath[0];
	char* pCurDirPath = &curDirPath[0];
	char* pDirEntries = &dirEntries[0];
	int* pDirEntryIndices = &dirEntryIndices[0];

	int nEntries = 0;
	int curEntryIndex = 0;

	// TODO: print out help text

	// get current working directory
	_getcwd(curDirPath, sizeof(curDirPath));
	// if user specifies an initial path, go to that path
	if(argc == 2) {
		int index = 0;
		while(argv[1][index] != '\0') {
			pNextDirPath[index] = argv[1][index];
			index++;
		}
		pNextDirPath[index] = '\0';
		fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);
	}
	else {
		// otherwise, use the current working directory
		int index = 0;
		while(pCurDirPath[index] != '\0') {
			pNextDirPath[index] = pCurDirPath[index];
			index++;
		}
		pNextDirPath[index] = '\0';
	}

	system("cls");
	int lastNEntries = nEntries;
	nEntries = getDirEntries(pNextDirPath, &pDirEntries, &pDirEntryIndices);
	printf(pDirEntries);

	if(nEntries == -1) {
		nEntries = lastNEntries;
		gotoXY(0, nEntries + 1);
		printf("Couldn't open direcotry");
		int index = 0;
		while(pCurDirPath[index] != '\0') {
			pNextDirPath[index] = pCurDirPath[index];
			index++;
		}
		pNextDirPath[index] = '\0';
	}

	if(pDirEntries[0] == '.') {
		curEntryIndex = 2;
		gotoXY(0, curEntryIndex);
	}
	else {
		curEntryIndex = 0;
		gotoXY(0, curEntryIndex);
	}

	// fix current directory trailing backslash
	int index4 = 0;
	while(pNextDirPath[index4] != '\0') {
		pCurDirPath[index4] = pNextDirPath[index4];
		index4++;
	}
	if (pCurDirPath[index4 - 1] == '/') {
		pCurDirPath[index4 - 1] = '\\';
	}
	if (pCurDirPath[index4 - 1] != '\\') {
		pCurDirPath[index4] = '\\';
		pCurDirPath[index4 + 1] = '\0';
	}

	while(1) {
nextAction:
		{
			char action = getch();
			switch (action)
			{
			case 'i':
			case 'I':
			case 'w':
			case 'W': { // go up an entry
				if (curEntryIndex > 0) {
					curEntryIndex--;
					gotoXY(0, curEntryIndex);
				}
				goto nextAction;
				break;
			}
			case 'j':
			case 'J':
			case 'a':
			case 'A': {
				pNextDirPath[0] = '.';
				pNextDirPath[1] = '.';
				pNextDirPath[2] = '\0';
				break;
			}
			case 'k':
			case 'K':
			case 's':
			case 'S': { // go down an entry
				if (curEntryIndex < nEntries - 1) {
					curEntryIndex++;
					gotoXY(0, curEntryIndex);
				}
				goto nextAction;
				break;
			}
			case 'l':
			case 'L':
			case 'd':
			case 'D': { // go into directory
				int index = pDirEntryIndices[curEntryIndex];
				int index2 = 0;
				while (pDirEntries[index] != '\n') {
					pNextDirPath[index2] = pDirEntries[index];
					index++;
					index2++;
				}
				pNextDirPath[index2] = '\0';
				if(pNextDirPath[0] == '.' && pNextDirPath[1] == '\0') {
					goto nextAction;
				}
				break;
			}
			case 'o':
			case 'O':
			case 'f':
			case 'F': { // open file
				int index = pDirEntryIndices[curEntryIndex];
				int index2 = 0;
				while (pDirEntries[index] != '\n') {
					pNextDirPath[index2] = pDirEntries[index];
					index++;
					index2++;
				}
				pNextDirPath[index2] = '\0';
				fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);
				ShellExecute(0, 0, nextDirPath, 0, 0, SW_SHOW);
				goto nextAction;
				break;
			}
			case 'u':
			case 'U':
			case 'q':
			case 'Q': { // let user type out full path
				curEntryIndex = nEntries;
				gotoXY(0, curEntryIndex);
				scanf("%s", pNextDirPath);
				break;
			}
			case 'p':
			case 'P':
			case 'e':
			case 'E':
				goto exit;
			default: {
				goto nextAction;
				break;
			}
			}
		}
		fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);

		system("cls");
		lastNEntries = nEntries;
		nEntries = getDirEntries(pNextDirPath, &pDirEntries, &pDirEntryIndices);
		printf(pDirEntries);

		if(nEntries == -1) {
			nEntries = lastNEntries;
			gotoXY(0, nEntries + 1);
			printf("Couldn't open directory");
			int index = 0;
			while(pCurDirPath[index] != '\0') {
				pNextDirPath[index] = pCurDirPath[index];
				index++;
			}
			pNextDirPath[index] = '\0';
		}

		if(pDirEntries[0] == '.') {
			curEntryIndex = 2;
			gotoXY(0, curEntryIndex);
		}
		else {
			curEntryIndex = 0;
			gotoXY(0, curEntryIndex);
		}

		int index3 = 0;
		while(pNextDirPath[index3] != '\0') {
			pCurDirPath[index3] = pNextDirPath[index3];
			index3++;
		}
		if (pCurDirPath[index3 - 1] == '/') {
			pCurDirPath[index3 - 1] = '\\';
		}
		if (pCurDirPath[index3 - 1] != '\\') {
			pCurDirPath[index3] = '\\';
			pCurDirPath[index3 + 1] = '\0';
		}
	}

exit:
	system("cls");
	return 0;
}
