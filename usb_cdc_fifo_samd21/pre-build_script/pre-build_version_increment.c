/**
 * @file pre-build_version_increment
 * @author John Petrilli
 * @date 11.Sep.2024
 * @brief This file increments the build version in a header file
 * 
 * Instructions to use this file:
 * 1.) construct a header file that defines a fw major, minor, bux, build, and complete version w/ pre-processor directive
 * 2.) place this file in a scripts folder
 * 3.) place the version header file the include folder with the rest of the header files. 
 * 4.) To find the version header file, this file will go back a directory, then look for
 *     a folder called INC_DIRECTORY. Within that folder it will look for a file called
 *     FILENAME. It will open the file if its found, otherwise it will print an error
 * 5.) Within the FILENAME, this file will scan line by line to look for a string
 *     BUILD_VER_NAME. If it finds this string it will increment the number next to that string. 
 *     For example if it finds #define FW_VERSION_BUILD 200, it will overwrite the file to be
 *     FW_VERSION_BUILD 201
 * 6.) Make sure you update the FILENAME, MAJOR_VER_NAME, MINOR_VER_NAME, BUGFIX_VER_NAME, BUILD_VER_NAME, & INC_DIRECTORY 
 *     fields to match the names in your project
 * 7.) The parsing delimiter is a space (" ") by default. So either put a space after 
 *     BUILD_VER_NAME in your header file, or change the "const char delim[]" to match what
 *     you need
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILENAME            "version.h"             ///< Name of the file which holds the version
#define MAJOR_VER_NAME      "FW_VERSION_MAJOR"          ///< Name of the #define pre-processor directive which holds major version
#define MINOR_VER_NAME      "FW_VERSION_MINOR"          ///< Name of the #define pre-processor directive which holds minor version
#define BUGFIX_VER_NAME     "FW_VERSION_BUGFIX"         ///< Name of the #define pre-processor directive which holds bugfix version
#define BUILD_VER_NAME      "FW_VERSION_BUILD"          ///< Name of the #define pre-processor directive which holds build version
#define COMPLETE_VER_NAME   "FW_VERSION"                ///< Name of the #define pre-processor directive which holds the concatenated firmware version
#define INC_DIRECTORY       "inc"                       ///< Directory/folder name where the version header file is located
#define DECIMAL_BASE_10     10                          ///< Number base for strtol. 10 = base 10 decimal
#define MAX_CHAR_PER_LINE   100                         ///< Max number of characters to hold from each line. 
#define MAX_LEN_FILENAME    128                         ///< Max number of characters in a file name

int get_version_number(char* str, char* ver, char* delim);

int main() {
    // Variable Declarations
    char directory[MAX_LEN_FILENAME];
    FILE *file_ptr;                     // pointer to the header file           
    char str[MAX_CHAR_PER_LINE];        // string which holds lines of code from the header file
    const char delim[] = " ";           // string delimiter for strtok
    int line_count = 0;                 // number of lines in the header file
    int major_num;                      // major revision number
    int minor_num;                      // minor revision number
    int bug_num;                        // bug fix number 
    int build_num;                      // extracted build number from the header file
    int build_position = 0;             // EOL position of the line of code containing the string BUILD_VER_NAME
    int fw_version_position = 0;        // EOL position of the line of code containing the string COMPLETE_VER_NAME
    int build_len = 0;                  // length of the line of code which matches the string in BUILD_VER_NAME
    int fw_ver_len = 0;                 // length of the line of code which matches the string in COMPLETE_VER_NAME

    // Change working directory from the scripts folder to the include folder
    getcwd(directory, MAX_LEN_FILENAME);
    chdir("..");
    chdir(INC_DIRECTORY);

    // Open file in R/W mode
    file_ptr = fopen(FILENAME, "r+");
    
    // Check file_ptr to ensure file is valid
    if (NULL == file_ptr) {
        perror("Error opening file");
        return 1;
    }
    
    printf("Successfully opened file %s\r\n", FILENAME);        //if we reach this point, we have successfully opened the file

    // Lets look for the string held in BUILD_VER_NAME by scanning the header file line by line
    while (fgets(str, MAX_CHAR_PER_LINE, file_ptr) != NULL) {
        line_count++;                                                       // increment the header file line counter
        
        if(strstr(str, BUILD_VER_NAME) != NULL){                            // look for the string in BUILD_VER_NAME
            build_position = ftell(file_ptr);                               // save file position so we can overwrite the build number later
            build_len = strlen(str);                                        // string length of the build version line
            build_num = get_version_number(str, (char*)BUILD_VER_NAME, (char*)delim);   // version number from build version line
            printf("Current build number is %d \r\n",build_num);            // Print out the current build number
            printf("New build number is %d\r\n", ++build_num);              // Print out the next build number
        }
        else if (strstr(str, MAJOR_VER_NAME) != NULL){                      
            major_num = get_version_number(str, (char*)MAJOR_VER_NAME, (char*)delim);
            printf("major rev num: %d\r\n", major_num);
        }
        else if (strstr(str, MINOR_VER_NAME) != NULL){
            minor_num = get_version_number(str, (char*)MINOR_VER_NAME, (char*)delim);
            printf("minor rev num: %d\r\n", minor_num);
        }
        else if (strstr(str, BUGFIX_VER_NAME) != NULL){
            bug_num = get_version_number(str, (char*)BUGFIX_VER_NAME, (char*)delim);
            printf("bugfix rev num: %d\r\n", bug_num);
        }
        else if (strstr(str, COMPLETE_VER_NAME) != NULL){
            fw_ver_len = strlen(str);
            fw_version_position = ftell(file_ptr);
            printf("Firmware Version on line %d position %d in file\r\n", line_count, fw_version_position);
        }
        
    }

    printf("Number of lines in %s: %d\r\n", FILENAME, line_count);          // print the number of lines in the header file
    printf("Overwriting the build number in %s...\r\n", FILENAME);          // notify that we are about to over-write the header file
    fseek(file_ptr, build_position - build_len - 1, SEEK_SET);              // go to the beginning of the line where BUILD_VER_NAME was found
    fprintf(file_ptr, "#define %s %d\n", BUILD_VER_NAME, build_num);        // print this string to the file to overwrite build version
    printf("Build version over-write is complete.\r\n");                    // Notify we are done w/ file and closing it
     printf("Overwriting the fw version in %s...\r\n", FILENAME);           // notify that we are about to over-write the header file
    fseek(file_ptr, fw_version_position - fw_ver_len - 1, SEEK_SET);        // go to the beginning of the line where BUILD_VER_NAME was found
    fprintf(file_ptr, "#define %s \"%d.%d.%d.%d\"\n", COMPLETE_VER_NAME, major_num, minor_num, bug_num, build_num);        // print this string to the file to overwrite build version
    printf("Build version over-write is complete. Closing %s...\r\n", FILENAME);    // Notify we are done w/ file and closing it
    fclose(file_ptr);                                                       // close the file
    return 0;
}

/// @brief Takes in a line of code (str), a substring to compare against (ver), and a string delimiter (delim)
/// The str is tokenized by the delimiters. Once the Substring token (ver) is identified, then we know the next
/// token will be the version number, so we perform a strtol on the version number string
/// the str will look something like this #define FW_VERSION_BUILD 53
/// @param char* str    - string of the entire line of code containing the #define statment
/// @param char* ver    - string containing the firmware version to parse
/// @param char* delim  - string delimiter. Default is space (" ")
/// @return int         - returns the version number line of code containing the #define statament
int get_version_number(char* str, char* ver, char* delim){
    int ver_num = 0;
    char* token;

    token = strtok(str, delim);                                     // tokenize the strings, by the delimiter, on this line
    while(token != NULL){                                           // keep tokenizing as long as string isn't NULL       
        if(strstr(token, ver) != NULL){                  // compare the token against the string in BUILD_VER_NAME
            token = strtok(NULL, delim);                            // If we found BUILD_VER_NAME, then we know the build number will be the next string token
            ver_num = strtol(token, NULL, DECIMAL_BASE_10);       // Get the build number from the header file and assign it to build_num
            break;                                                  // Were done now, so let's leave
        }
        else{
            token = strtok(NULL, delim);                            // find the next string token
        }
    }

    return ver_num;
}