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
		printf("could not open directory");
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

// TODO: make relative path completion more robust
void fixPath(char** pNextDirPath, char** pCurDirPath, char** pTempDirPath) {
	if ((*pNextDirPath)[0] == '.') {
		if ((*pNextDirPath)[1] == '.') {
			if (stringsAreEqual((*pCurDirPath), "C:\\\0") != 3) {
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
	else if (stringsAreEqual((*pNextDirPath), "C:\\\0") == 0) {
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
	// TODO: check working directory when called so I can call confilex .
	char* dirPath =  "C:\\\0";
	if(argc == 2) {
		dirPath = argv[1];
	}
	
	int initialCurDirPathIndex = 0;
	while(dirPath[initialCurDirPathIndex] != '\0') {
		pCurDirPath[initialCurDirPathIndex] = dirPath[initialCurDirPathIndex];
		initialCurDirPathIndex++;
	}
	pCurDirPath[initialCurDirPathIndex] = '\0';
	
	system("cls");
	nEntries = getDirEntries(dirPath, &pDirEntries, &pDirEntryIndices);
	printf(pDirEntries);
	
	while(1) {
		//scanf("%s", pNextDirPath);

		// TODO: allow opening of files
		// TODO: let user type out a full path if they hit a certain button
		gotoXY(0, curEntryIndex);
nextAction:
		{
			char action = getch();
			switch (action)
			{
			case 'w':
			case 'W': {
				if (curEntryIndex > 0) {
					curEntryIndex--;
					gotoXY(0, curEntryIndex);
				}
				goto nextAction;
				break;
			}
			case 'a':
			case 'A': {
				// TODO: save previous paths and entry indexes and go to them here (store the previous indices in a linked list)
				pNextDirPath[0] = '.';
				pNextDirPath[1] = '.';
				pNextDirPath[2] = '\0';
				break;
			}
			case 's':
			case 'S': {
				if (curEntryIndex < nEntries - 1) {
					curEntryIndex++;
					gotoXY(0, curEntryIndex);
				}
				goto nextAction;
				break;
			}
			case 'd':
			case 'D': {
				int index = pDirEntryIndices[curEntryIndex];
				int index2 = 0;
				while (pDirEntries[index] != '\n') {
					pNextDirPath[index2] = pDirEntries[index];
					index++;
					index2++;
				}
				pNextDirPath[index2] = '\0';
				break;
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
		curEntryIndex = 0;
		gotoXY(0, curEntryIndex);

		fixPath(&pNextDirPath, &pCurDirPath, &pTempDirPath);
		
		system("cls");
		nEntries = getDirEntries(pNextDirPath, &pDirEntries, &pDirEntryIndices);
		printf(pDirEntries);

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
