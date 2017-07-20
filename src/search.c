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

/** Function prototypes. */
void parseFlags(char ** args, int num_args);
static int dupTag(char * tags, char tag);
void analyzeLine(char * line, char * pattern);
void setFlag(char flag);

/** Flags */
/** Set to one when case should be ignored. */
int Ignore_Case = 0;

void main(int argc, char** argv)
{
    // Stores a line from a file.
    char line[LINE_LENGTH] = {0};

    // File path is stored in last element in commandline arguments.
    char * pattern = argv[argc - PATTERN_OFFSET];
    char * file_path = argv[argc - FILE_OFFSET];

    // Check if a file was named.
    int flag_offset = (pattern[0] == '-') ? 1 : 2;

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
        analyzeLine(line, pattern);
    }

    puts("Done!");
}

/**
    Determines whether the pattern is present within the line that
    is passed in.

    @param line A line read from a file.
*/
void analyzeLine(char * line, char * pattern)
{
    if(0 == Ignore_Case)
    {
        // Case sensitive.
        if(strstr(line, pattern) != NULL)
        {
            printf("%s\n", line);
        }
    }
    else
    {
        if(strcasestr(line, pattern) != NULL)
        {
            printf("%s\n", line);
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
        case 'i':
            Ignore_Case = 1;
            break;
        default:
            break;
    }
}
