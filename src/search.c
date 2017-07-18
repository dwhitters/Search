/**
    Grep simulator. Searches for lines within a file matching a pattern.
    Searches the named inputs files for lines containing a match to the
    given pattern. Prints the matching lines by default.

    search[-options] pattern file

    @author David Whitters
    @date 7/18/17
*/
#include <stdio.h>
#include <string.h>

#define PATTERN_OFFSET 2
#define FILE_OFFSET 1

void getFlags(char * tags, char ** args, int num_args);
static int dupTag(char * tags, char tag);

void main(int argc, char** argv)
{
    char tags[10] = {0};

    // File path is stored in last element in commandline arguments.
    char * pattern = argv[argc - PATTERN_OFFSET];
    char * file_path = argv[argc - FILE_OFFSET];

    // Check if a file was named.
    int flag_offset = (pattern[0] == '-') ? 1 : 2;
    getFlags(tags, argv, argc - flag_offset);
}

/**
    Parses the commandline flags and stores them in a passed in
    character array.

    @param tags The array that the flags will be held in.
    @param args The commandline arguments.
    @param num_args The number of commandline arguments.
*/
void getFlags(char * tags, char ** args, int num_args)
{
    int num_tags = 0;
    for(int i = 0; i < num_args; ++i)
    {
        if((args[i][0] == '-') && (strlen(args[i]) == 2))
        {
            // Duplicate tags are not a concern.
            tags[num_tags] = args[i][1];
            ++num_tags;
        }
    }
}
