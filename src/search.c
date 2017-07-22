/**
    Grep simulator. Searches for lines within a file matching a pattern.
    Searches the named inputs files for lines containing a match to the
    given pattern. Prints the matching lines by default.

    search[-options] pattern file

    @author David Whitters
    @date 7/18/17
*/

/** Allows strcasestr to be used as it is a nonstandard extension. */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>

/** The length of each line that is read. */
#define LINE_LENGTH 80
/** How far from the end of the input arguments the pattern is. */
#define PATTERN_OFFSET 2
/** How far from the end of the input arguments the file name is. */
#define FILE_OFFSET 1
/** How long each flag string is expected to be. */
#define FLAG_LENGTH 2
/** The value of a flag when it is set. */
#define SET 1
/** The value of a flag when it is not set. */
#define NOT_SET 0
/** Value that is used to signify a match is found in a line. */
#define MATCH 1

/** Function prototypes. */
void parseFlags(char ** args, int num_args);
static int dupTag(char * tags, char tag);
int analyzeLine(char * line, char * pattern);
void setFlag(char flag);
void printLine(char * line, int line_num);

/** Flags */
/** Set to one when case should be ignored. (-i) */
int Ignore_Case = 0;
/** Set to one when only a count of the matching lines should be printed. (-c) */
int Only_Count = 0;
/**
    Set to one when only filenames of files containing a match
    should be listed. (-l)
*/
int Only_File_Names = 0;
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

    // File path is stored in last element in commandline arguments.
    char * pattern = argv[argc - PATTERN_OFFSET];
    char * file_path = argv[argc - FILE_OFFSET];

    // Check if a file was named.
    int flag_offset = (pattern[0] == '-') ? 1 : 2;
    int line_count = 0;
    int num_match = 0;

    // Parse the commandline flags.
    parseFlags(argv, argc - flag_offset);

    FILE *f;
    f = fopen(file_path, "r");  // Open the file for reading.

    /*
        Continue looping until EOF has been hit.
        Get a line from a file with a maximum length of 100 chars.
    */
    while(fgets(line, LINE_LENGTH, f))
    {
        ++line_count;
        if((MATCH == analyzeLine(line, pattern)))
        {
            ++num_match;

            if(Rev_Search == NOT_SET)
            {
                printLine(line, line_count);
            }
        }
        else if(Rev_Search == SET)
        {
            printLine(line, line_count);
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
}

/**
    Determines whether the pattern is present within the line that
    is passed in.

    @param line A line read from a file.

    @return int Set to 1 when a match is found, otherwise 0.
*/
int analyzeLine(char * line, char * pattern)
{
    int ret_val = 0;
    if(NOT_SET == Ignore_Case)
    {
        if(Whole_Line == NOT_SET)
        {
            // Case sensitive.
            if(strstr(line, pattern) != NULL)
            {
                ret_val = 1;
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
            if(strcasestr(line, pattern) != NULL)
            {
                ret_val = 1;
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
            printf("%s", line);
        }
        else
        {
            printf("%d:%s", line_num, line);
        }
    }
}

/**
    Parses the commandline flags and calls a function to set the
    corresponding global flags.

    @param args The commandline arguments.
    @param num_args The number of commandline arguments.
*/
void parseFlags(char ** args, int num_args)
{
    for(int i = 0; i < num_args; ++i)
    {
        if((args[i][0] == '-') && (strlen(args[i]) == FLAG_LENGTH))
        {
            // Duplicate tags are not a concern.
            setFlag(args[i][1]);
        }
    }
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
            Only_File_Names = SET;
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
