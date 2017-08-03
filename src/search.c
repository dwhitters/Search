/**
    Grep simulator. Searches for lines within a file matching a pattern.
    Searches the named inputs files for lines containing a match to the
    given pattern. Prints the matching lines by default.

    @note The pattern is expected to precede the file path(s).

    search[-options] pattern file

    OPTIONS
        -c
            Print only the count of lines that matched.
        -l
            List only the filenames of files containing a match.
        -w
            Select only those lines containing matches that form whole
            words. The matching substring must either be at the beginning
            of the line, or preceded by a non-word constituent character.
            Similarly, it must be either at the end of the line or followed
            by a non-word constituent character. Word-constituent
            characters are letters, digits, and the underscore.
        -x
            Select only those matches that exactly match the whole line.
        -i
            Ignore case.
        -c
            Print only count of the lines that matched.
        -n
            Gives line number of pattern match.

    @author David Whitters
    @date 7/18/17
*/

/** Allows strcasestr to be used as it is a nonstandard extension. */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <dirent.h>

/** The length of each line that is read. */
#define LINE_LENGTH 80
/** What index the pattern is at in the argv array after input parsing. */
#define PATTERN_INDEX 1
/** How far from the end of the input arguments the file name is. */
#define FILE_INDEX 2
/** How long each flag string is expected to be. */
#define FLAG_LENGTH 2
/** The value of a flag when it is set. */
#define SET 1
/** The value of a flag when it is not set. */
#define NOT_SET 0
/** Value that is used to signify a match is found in a line. */
#define MATCH 1
/** The maximum number of files that will be processed. */
#define MAX_NUM_FILES 100

/** Function prototypes. */
void parseInput(char ** args, int num_args);
static int dupTag(char * tags, char tag);
int analyzeLine(char * line, char * pattern, int pattern_size);
void setFlag(char flag);
void printLine(char * line, int line_num);
int findPattern(char * line, char * pattern, int pattern_size);

/** Flags */
/** Set to one when case should be ignored. (-i) */
int Ignore_Case = 0;
/** Set to one when only a count of the matching lines should be printed. (-c) */
int Only_Count = 0;
/**
    Set to one when only filenames of files containing a match
    should be listed. (-l)
*/
int Only_File_Name = 0;
/** Set to one when line number of pattern match should be included. (-n)*/
int Line_Num = 0;
/** Set to one when returning lines NOT containing the pattern. (-v) */
int Rev_Search = 0;
/** Set to one when only whole word pattern matches should be printed. (-w) */
int Whole_Word = 0;
/** Set to one when only whole line pattern matches should be printed. (-x) */
int Whole_Line = 0;

void main(int argc, char** argv)
{
    // Stores a line from a file.
    char line[LINE_LENGTH] = {0};
    char * files[MAX_NUM_FILES] = {0};

    int line_count = 0;
    int file_count = 0;
    int match_file_count = 0;
    int num_match = 0;

    // Used to determine when an EOF has been reached in a file.
    int rollover = 0;

    // Parse the commandline flags.
    parseInput(argv, argc);

    // File path is stored in last element in commandline arguments.
    char * pattern = argv[PATTERN_INDEX];

    FILE *f;
    f = fopen(argv[FILE_INDEX], "r");  // Open the file for reading.

    /*
        Continue looping until EOF has been hit.
        Get a line from a file with a maximum length of 100 chars.
    */
    while(fgets(line, LINE_LENGTH, f) || (rollover = Only_File_Name == SET))
    {
        ++line_count;

        /**
            If only file names should be displayed, there may be multiple
            files to search...
        */
        if(rollover == SET)
        {
            rollover = 0;
            ++file_count;
            if(argv[FILE_INDEX + file_count] != "-")
            {
                f = fopen(argv[FILE_INDEX + file_count], "r");
                continue;
            }
            else
            {
                break;
            }
        }

        // Remove newline from the line.
        line[strcspn(line, "\n")] = 0;
        if((MATCH == analyzeLine(line, pattern, strlen(pattern))))
        {
            ++num_match;

            if(Rev_Search == NOT_SET)
            {
                if(Only_File_Name == NOT_SET)
                {
                    printLine(line, line_count);
                }
                else
                {
                    // Store the names of the files that contain a match.
                    files[match_file_count] = argv[FILE_INDEX + file_count];
                    ++file_count;
                    ++match_file_count;
                    if(argv[FILE_INDEX + file_count] != "-")
                    {
                        f = fopen(argv[FILE_INDEX + file_count], "r");
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if(Rev_Search == SET)
        {
            if(Only_File_Name == NOT_SET)
            {
                printLine(line, line_count);
            }
            else
            {
                // Store the names of the files that contain a match.
                files[match_file_count] = argv[FILE_INDEX + file_count];
                ++file_count;
                ++match_file_count;
                if(argv[FILE_INDEX + file_count] != "-")
                {
                    f = fopen(argv[FILE_INDEX + file_count], "r");
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Print the number of matches.
    if(Only_Count == SET)
    {
        if(Rev_Search == NOT_SET)
        {
            printf("%d\n", num_match);
        }
        else
        {
            // Print the number of non-matching lines.
            printf("%d\n", line_count - num_match);
        }
    }
    else if(Only_File_Name == SET)
    {
        for(int i = 0; i < match_file_count; ++i)
        {
            printf("File: %s\n", files[i]);
        }
    }
}

/**
    Determines whether the pattern is present within the line that
    is passed in.

    @param line A line read from a file.

    @return int Set to 1 when a match is found, otherwise 0.
*/
int analyzeLine(char * line, char * pattern, int pattern_size)
{
    int ret_val = 0;
    if(NOT_SET == Ignore_Case)
    {
        if(Whole_Line == NOT_SET)
        {
            if(Whole_Word == NOT_SET)
            {
                // Case sensitive.
                if(strstr(line, pattern) != NULL)
                {
                    ret_val = 1;
                }
            }
            else
            {
                ret_val = findPattern(line, pattern, pattern_size);
            }
        }
        else
        {
            // Compare the entire line with the pattern.
            if(strcmp(line, pattern) == 0)
            {
                ret_val = 1;
            }
        }
    }
    else
    {
        if(Whole_Line == NOT_SET)
        {
            if(Whole_Word == NOT_SET)
            {
                if(strcasestr(line, pattern) != NULL)
                {
                    ret_val = 1;
                }
            }
            else
            {
                ret_val = findPattern(line, pattern, pattern_size);
            }
        }
        else
        {
            if(strcasecmp(line, pattern) == 0)
            {
                ret_val = 1;
            }
        }
    }

    return ret_val;
}

/**
    Finds a whole word in a character array. Initially searches for
    the word in general then analyzes the characters before and after
    the found word to determine if it is a full word.

    @param line The array to search.
    @param word The word to search for in the array.

    @return int 1 when the word is found. 0 otherwise.
*/
int findPattern(char * line, char * pattern, int pattern_size)
{
    char * pointer;

    // Search for the word in the line regardless of surroundings.
    if(Ignore_Case == NOT_SET)
    {
        pointer = strstr(line, pattern);
    }
    else
    {
        pointer = strcasestr(line, pattern);
    }

    static int first_run = 1;

    if(pointer == NULL)
    {
        return 0;
    }

    int ret_val = 0;

    // Only allow the lack of a preceding space on the first run.
    if(((pointer == line) && (first_run == 1) &&
                ((*(pointer + pattern_size) == ' ') ||
                ((*(pointer + pattern_size) == 0)))) ||
      ((*(pointer - 1) == ' ') &&
       (*(pointer + pattern_size) == ' ') || (*(pointer + pattern_size) == 0)))
    {
        ret_val = 1;
    }
    else
    {
        first_run = 0;
        return findPattern(pointer+1, pattern, pattern_size);
    }

    // Only reset when the loop is finished.
    first_run = 1;
    return ret_val;
}

/**
    Prints the passed in string with possible formatting depending on
    the global flags.

    @param line The string to print.
*/
void printLine(char * line, int line_num)
{
    // Lines shouldn't be printed when the Only_Count flag is set.
    if(Only_Count == NOT_SET)
    {
        if(NOT_SET == Line_Num)
        {
            printf("%s\n", line);
        }
        else
        {
            printf("%d:%s\n", line_num, line);
        }
    }
}

/**
    Parses the commandline flags and calls a function to set the
    corresponding global flags.

    @param args The commandline arguments.
    @param num_args The number of commandline arguments.
*/
void parseInput(char ** args, int num_args)
{
    // Used to place the pattern and file(s) into the args array,
    int j = 0;

    for(int i = 0; i < num_args; ++i)
    {
        if((args[i][0] == '-') && (strlen(args[i]) == FLAG_LENGTH))
        {
            // Duplicate tags are not a concern.
            setFlag(args[i][1]);
        }
        else
        {
            args[j] = args[i];
            ++j;
        }
    }

    // Used to signify the end of the file list.
    args[j] = "-";
}

/**
    Sets the global flags that correspond with the flags that were
    read from stdin.

    @param flag A commandline flag.
*/
void setFlag(char flag)
{
    switch(flag)
    {
        case 'c':
            Only_Count = SET;
            break;
        case 'i':
            Ignore_Case = SET;
            break;
        case 'l':
            Only_File_Name = SET;
            break;
        case 'n':
            Line_Num = SET;
            break;
        case 'v':
            Rev_Search = SET;
            break;
        case 'w':
            Whole_Word = SET;
            break;
        case 'x':
            Whole_Line = SET;
            break;
        default:
            break;
    }
}
