Use these implementations when compiling on Oncourse.
Bit of a hack since ideally this would have been done with #ifdefs in the original source code.
If a new function doesn't get added here then it'll give an error only when it's used in the application (relatively unlikely).
Note that return values of reading button state etc. will not behave correctly.
Overall, this hack will work only for Oncourse programs that use PYNQ output only (e.g. sorting), or at run-time don't use the PYNQ I/O (e.g. tetris) which is all we need.
