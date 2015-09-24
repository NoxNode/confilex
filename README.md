# confilex
A console file explorer all in C. Open confilex.c for TODOs.

Usage:

- W to go up an entry
- I to go up 5 entries
- S to go down an entry
- K to go down 5 entries
- G to go to type the entry index to go to
- A or J to go to parent directory
- D or L to go into selected directory (doesn't work for files)
- F or O to open selected file (opens default explorer if directory selected)
- Q or U to type the directory you want to go to next
- Z or M to type the directory you want to go to next, but it starts at your current directory (can't backspace on the current directory, just use .. to go up a directory)
- X to store current directory into specified slot (0-31)
- N to copy stored directory at specified slot (0-31) to clipboard
- C or B to copy current directory to clipboard
- P or E to exit
