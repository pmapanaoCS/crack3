#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "md5.h"

const int PASS_LEN=50;        // Maximum any password can be
const int HASH_LEN=33;        // Length of MD5 hash strings

// Additional Function Prototypes
struct entry *read_dictionary(char *filename, int *size);
void checkFiles(int n);
void checkOpen(FILE *f, char *fName);
int fileLength(char * fName);

//My new functions: to check all files for opening and get length of file 
// Checks number of files
void checkFiles(int n)
{
    if (n < 3){
        printf("You need to supply two filenames\n");
        exit(1);
    }
} 

// Checks if file can be opened 
void checkOpen(FILE *f, char *fName)
{
    if(!f){
        printf("Cannot open %s for reading \n", fName);
        printf("Program will now close. . . \n");
        exit(1); 
    }
}

// Using stat, this function gets file length 
int fileLength(char * fName)
{
    struct stat info;
    if (stat(fName, &info) == -1) return -1; 
    else return info.st_size; 
}


// Stucture to hold both a plaintext password and a hash.
struct entry 
{
    // TODO: FILL THIS IN
    char * guess; 
    char * hash; 
};

int dComp(const void *a, const void *b)
{
    struct entry *ca = (struct entry *)a;
    struct entry *cb = (struct entry *)b;
    return strcmp(ca->hash, cb->hash);
}

int dbComp(const void *target, const void *elem)
{
    char *target_str = (char *)target;
    struct entry *celem = (struct entry *)elem;
    return strcmp(target_str, celem->hash);
}

// TODO
// Read in the dictionary file and return an array of structs.
// Each entry should contain both the hash and the dictionary
// word.

struct entry *read_dictionary(char *filename, int *size)
{
    /*  Part 1: Checking File, and grabbing length of file
     *  Steps - 
     *  1 - Open Passwords File 
     *  2 - Check if file can be opened
     *  3 - Call stat to read file length
     */
    FILE *p = fopen(filename, "r");
    checkOpen(p , "Passwords File");
    int fLength = fileLength(filename);
    
    /*  Part 2: Reading file into memory
     *  Steps - 
     *  1 - Allocate memory for entire file to be read
     *  2 - Read file into memor
     *  3 - Close Password file, b/c we don't need it anymore
     *  4 - Format file just read into memory, keep track of # of lines
     *  5 - [DEBUG] - Print # of lines
     */
    char * contents = malloc(fLength); 
    fread(contents, 1, fLength, p);
    fclose(p); 
    int lines = 0; 
    for (int i = 0; i < fLength; i++)
    {
        if (contents[i] == '\n'){
            contents[i] = '\0';
            lines++;
        }
    }
    printf("The number of lines is: %d\n", lines); 
    
    /*  Part 3: Create an array of structs, to point to contents in memory 
     *  Steps - 
     *  1 - Allocate memory for the struct of pointers to contents 
     *  2 - Load the dictionary with pointers to contents 
     */
     struct entry *d = malloc(lines * sizeof(struct entry));
     d[0].guess = contents;
     d[0].hash = md5(contents, strlen(contents));
     
     //DEBUG PRINT STATEMENT
     //printf("Count: [0], Guess: [%s], Hash: [%s] \n", d[0].guess, d[0].hash);
     
     int count = 1; 
     
     for (int i = 0; i < fLength-1; i++)
     {
         if (contents[i] == '\0')
         {
             d[count].guess = &contents[i] + 1;
             d[count].hash = md5(d[count].guess, strlen(d[count].guess));
             
             //DEBUG PRINT STATEMENT 
             //printf("Count: [%d], Guess: [%s], Hash: [%s] \n", count, d[count].guess, d[count].hash);
             count++; 

         }
     }

    // Return # of lines which is size, and the dictionary
    *size = lines; 
    return d;
    
}


int main(int argc, char *argv[])
{
    checkFiles(argc);

    // TODO: Read the dictionary file into an array of entry structures
    int dlen;
    struct entry *dict = read_dictionary(argv[2], &dlen);
    
    // TODO: Sort the hashed dictionary using qsort.
    // You will need to provide a comparison function.
    qsort(dict, dlen, sizeof(struct entry), dComp); 

    // TODO
    // Open the hash file for reading.
    FILE *hashFile = fopen(argv[1], "r");
    checkOpen(hashFile, "HASH FILE");
    char line[34];
    
    int correctHashes = 0; 

    // TODO
    // For each hash, search for it in the dictionary using
    // binary search.
    // If you find it, get the corresponding plaintext dictionary word.
    // Print out both the hash and word.
    // Need only one loop. (Yay!)
    int wordNum = 1; 
    while(fgets(line, 34, hashFile) != NULL)
    {
        //Formatting Line to match string
        line[32] = '\0';
        
        //DEBUG
        printf("Checking hash: [%d]:[%s]\n ", wordNum, line);
        
        struct entry *found = bsearch(line, dict, dlen, sizeof(struct entry), dbComp);
        wordNum++;
        
        //Checks if bsearch found a match 
        if(found != NULL)
        {
            printf("FOUND MATCH: H:[%s] P:[%s]\n", line, found->guess);
            correctHashes++; 
        }
        
    }
    
    printf("The number of cracked passwords is: %d\n",correctHashes);
    
    
    // Free allocated mem
    for (int i = 0; i < dlen; i++) free(dict[i].hash);
    //free(dict[0].guess);
    free(dict);
    
    //Close Hash File
    fclose(hashFile);
    
    // Ensure proper pop of main stack
    return 0; 
    
}
