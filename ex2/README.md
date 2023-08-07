
## Guidelines for 21ex:

* Name of file to send: `c21.ex`
* Write a program that accepts paths to two files as arguments to main and checks whether the two files are identical/similar/different.
* For the avoidance of doubt, the paths include the filenames.
* If the files are identical (contain exactly the same content), the program will return 1. If the files are similar (explained later), the program will return 3. Otherwise, if the files are different, it will return 2 (note: this is not the case here in a program that prints to the screen).
* Identical files are files in which all characters are equal (for example: "World Hello" and "World Hello").
* Similar files are files that are not identical but contain the same text, with differences in usage of lowercase or uppercase letters, spaces, and/or line breaks.
* Different files are files that are neither identical nor similar.

## Instructions for 22ex:

* Name of file to send: `c22.ex`
* Write a program that accepts a path to a configuration file as an argument to main. It can be assumed that the file exists (and not a folder).
* The configuration file contains 3 lines:
  1. Path to the folder containing subfolders. On the first level (one inside), each folder represents a user and should contain a C file.
  2. Path to the file containing input.
  3. Path to the file containing the correct output for the input file from line 2.
* The configuration file will end with a newline character.
* Your program must enter all subfolders (and ignore other files that are not folders, if they exist in the folder from row 1), search in each of its subfolders (and not in deeper levels) for a C file, and compile it. The created runtime file must be run with the input that appears in the file at the location in line 2. The program you want to run receives input from stdin and prints to stdout, so you must use I/O redirection (`<` and `>`) to capture the output.
* Assume that there will be at most one C file in the folder (there may not be a C file at all, and there can be files and folders of other types, except for files with the extension `.out`).
* It can be assumed that the C file will necessarily have a suitable extension, for example, `c.file`. The name of the file has no meaning.
* The output of the program must be compared with the desired output file, whose location comes from line 3, using the program `out.comp` that you implemented in part A of the exercise. Run the program `out.comp` and pass as arguments to main the location of the correct output found in the configuration file in line 3 and the output of the user's program that you ran.
* Your program should create a file (in the folder from which your program was run) named `csv.results` which contains, for each username (subfolder name), its score (between 0 and 100) depending on the answer that `out.comp` returned, and why. The character `,` must be written between the username and the grade and its reason (without spaces).
* Possible reasons and scores:
  1. `FILE_C_NO` - there is no file in the user's folder with the extension `.c`. The grade to be given: 0.
  2. `ERROR_COMPILATION` – compilation error (file does not compile). The grade to be given: 10.
  3. `TIMEOUT` - The compiled C file runs for more than 5 seconds. The grade to be given: 20.
  4. `WRONG` – the output is different from the desired output. The grade to be given: 50.
  5. `SIMILAR` – the output is different from the desired output but similar. The grade to be given: 75.
  6. `EXCELLENT` – the output is the same as the desired output. The grade to be given: 100.

