# confilex
A console file explorer all in C. Open confilex.c for TODOs.

Controls:
```
Navigation
	Navigate directories
		W, A, S, D
	Navigate directories, but go up and down 5 entries instead of 1
		I, J, K, L
	Go to n entry index
		GE
	Add n to entry index
		GA
	Go to copied directory
		GD
	Type out directory
		TD
	Type out directory starting with current
		TC
Operations
	Open entry
		O
	Cut / Move copied entry into current directory
		X
	Copy selected entry
		C
	Paste copied entry into current directory (only works for files right now - can't paste directories)
		V
	Rename selected entry
		R
	Delete selected entry
		Z
	Properties of selected entry (edit default program to open with and other stuff) - coming soon
		P
	Run console command
		F
Storage
	Store selected entry at slot n
		QS
	Copy stored entry at slot n to clipboard
		QC
Program
	Exit
		E
```
