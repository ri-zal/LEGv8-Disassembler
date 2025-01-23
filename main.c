/*
 * Filename: LEGv8Disassembler.c
 * Date: 05/21/2024
 * CS205 Sp '24 Wiese
 * Programmers: Abdurrahman Alyajouri, Jin P., Nelson Nguyen
 *
 * Test Cases 1:
 *  10010001000000000001111111100100
 *  10010001000000000000100010000101
 *  10001011000001000000000010100110
 *  10010001000011111010001111100010
 *  11111111100000000000000001000110
 *
 * Test Cases 2:
 *  10010001000100000000001111100010
 *  10010001000000000100011111100011
 *  11111000000000001000000001000011
 *  11010001000000000001010001100011
 *  11111000010000000000000001000100
 *  10001011000000110000000010000100
 *
 * Test Cases 3:
 *  1001000100 000000000001 11111 00111
 *  1001000100 00000000101 11111 01000
 *  11001011000 01000 000000 00111 00111
 *  10001011000 00111 000000 01000 01000
 */

// LIBRARIES
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// global variable
const int CHAR_SIZE = 1000;

typedef struct OperationInfo {
    char *opcode;
    char *assembly;
    char format;
} OperationInfo;

//Look-up array of opcodes and corresponding information
const OperationInfo LEGV8_OPERATIONS[] = {
        {"10001011000", "ADD", 'R'},
        {"11001011000", "SUB", 'R'},
        {"1001000100", "ADDI", 'I'},
        {"1101000100", "SUBI", 'I'},
        {"11111000010", "LDUR", 'D'},
        {"11111000000", "STUR", 'D'}
};

const int R_FORMAT_CHUNK_SIZES[] = {11, 5, 6, 5, 5};
const int I_FORMAT_CHUNK_SIZES[] = {10, 12, 5, 5};
const int D_FORMAT_CHUNK_SIZES[] = {11, 9, 2, 5, 5};

char MACHINE_CODE_FILE[] = "data.txt";

// PROTOTYPES

//Allows user to enter a machine code instruction.
//If not valid, will be handled by future functions.
void EnterInstruction(char file_name[]);                                                // Done by Nelson

//Reads a line and checks if it is 32-bit.
void Read32BitLine(char target[], FILE* ptr, const int line_number);                    // Done by Abdurrahman

//Deduces the operation being performed by the input machine instruction, returns index.
int DeduceOperationFromInstruction(char instruction[], const int line_number);          // Done by Abdurrahman

//Convert binary char string into an integer representing its decimal expansion.
int BinaryToDecimal(const char* bit_string);                                            // Done by Abdurrahman

//Probably wasn't needed, though I couldn't get
//the other file reading functions to work without a bug arising.
int CheckEOF(FILE* ptr);                                                                // Done by Abdurrahman

//Decodes an instruction based on the proposed format.
void DecodeFormat_R(const char *instruction, int line_number, int op_idx);               // Done by Jin
void DecodeFormat_I(const char *instruction, int line_number, int op_idx);               // Done by Jin
void DecodeFormat_D(const char *instruction, int line_number, int op_idx);               // Done by Jin

int main() {

    // Let user input instructions until STOP
    EnterInstruction(MACHINE_CODE_FILE);
    printf("\n");

    // Read machine code instructions line by line from file.
    FILE *read_ptr;
    read_ptr = fopen(MACHINE_CODE_FILE, "r");
    if (read_ptr == NULL) {
        printf("ERROR: Could not open file: %s\n", MACHINE_CODE_FILE);
        exit(0);
    }

    // 33 chars for 32 bits + null terminator '\0';
    char current_instruction[33];

    // Current line of our machine program.
    int current_line = 1;
    int c;
    while(CheckEOF(read_ptr)) {
        Read32BitLine(current_instruction, read_ptr, current_line);
        int operation_idx = DeduceOperationFromInstruction(current_instruction, current_line);

        switch(LEGV8_OPERATIONS[operation_idx].format) {
            case 'R':
                // Decode based on R format.
                DecodeFormat_R(current_instruction, current_line, operation_idx);
                break;
            case 'I':
                // Decode based on I format.
                DecodeFormat_I(current_instruction, current_line, operation_idx);
                break;
            case 'D':
                // Decode based on D format.
                DecodeFormat_D(current_instruction, current_line, operation_idx);
                break;
        }

        // Increment current line to signify moving onto the next line.
        ++current_line;
    }

    // Close file as we are done reading.
    fclose(read_ptr);

    return 0;
} // END main()

// FUNCTIONS WRITTEN BY NELSON
void EnterInstruction(char file_name[]) {

    // variables
    char bit_instruction[CHAR_SIZE]; // bit string with character limit of 1000
    int choice;

    // declaring file
    FILE* fptr;
    fptr = fopen(file_name , "w"); // uses "w" to write

    // file is opened and no errors are reached
    // grabs user instruction until user stops
    if(fptr != NULL) {
        // do while loop to take in instructions
        do {
            // user_input
            printf("Please enter a 32-bit instruction: ");
            fgets(bit_instruction, sizeof(bit_instruction), stdin);
            fputs(bit_instruction, fptr);

            // choice validation
            printf("Would you like to enter another bit instruction?(0 for no, 1 for yes): ");
            scanf("%d", &choice);
            while(getchar() != '\n');
            while( choice != 0 && choice != 1) { // while loop until conditions are met
                printf("That is not a valid choice.\n");
                printf("Would you like to enter another bit instruction?(0 for no, 1 for yes): ");
                scanf("%d", &choice);
                while(getchar() != '\n');
            }

        } while(choice != 0); // breaks out of loop if user input is 0
    }

    // file message if not opened succesfully
    if (fptr == NULL) {
        printf("File did not open successfully");
    }

    // close file
    fclose(fptr);

} // END EnterInstruction()

// FUNCTIONS WRITTEN BY ABDURRAHMAN
int CheckEOF(FILE* ptr) {
    int c;
    if((c = fgetc(ptr)) == EOF) {
        return 0;
    }
    ungetc(c, ptr);
    return 1;
}

int BinaryToDecimal(const char* bit_string) {
    int length = strlen(bit_string);
    int decimal_expansion = 0;

    //Ex: 1011 = 1 * 2^3 + 0 * 2^2 + 1 * 2^1 + 1 * 2^0 = 11
    for(int i = 0, p = length - 1; i < length && p >= 0; ++i, --p) {
        decimal_expansion += (int)(bit_string[i] - '0') * (1 << p); //digit * 2^x;
    }
    return decimal_expansion;
} //END BinaryToDecimal(char* bit_string)

void Read32BitLine(char target[], FILE* ptr, const int line_number) {
    int c; //Stores read file characters.
    int char_count = 0; //Current amount of characters read in a given line.

    //While the currently fetched character is not equal to the EOF character...
    while((c = fgetc(ptr)) != EOF) {
        //Ignore spaces;
        if(c == ' ')
            continue;

        //Check if we have reached the end of a line.
        if(c == '\n') {
            //If so, check if we counted 32 characters. If 32 chars weren't counted,
            //then it is not a valid instruction in our context. So print error and exit program.
            if(char_count != 32) {
                printf("LINE %d - ERROR: Non-32 bit instruction detected!", line_number);
                exit(0);
            }
            target[char_count] = '\0'; //Attach null character to prevent garbage values from printing.
            return;
        } else if(c != '0' && c != '1' && c != '\n') {
            printf("LINE %d, CHAR %d - ERROR: Non-binary format detected!", line_number, char_count + 1);
            exit(0);
        }

        //Finally, no errors stopped the program, so add our character to our instruction array.
        target[char_count] = c;
        ++char_count; //Increment char counter to keep track of whether or not the instruction is 32-bit.
    }
    return;
} //END Read32BitLine()

int DeduceOperationFromInstruction(char instruction[], const int line_number) {
    int found;
    //Perform linear search for every defined opcode at the start of the instruction
    for(int i = 0; i < sizeof(LEGV8_OPERATIONS) / sizeof(LEGV8_OPERATIONS[0]); ++i) {
        found = 1;
        //Find first matching opcode from our lookup table. This works due to the opcodes not being substrings of each other.
        for(int j = 0; j < strlen(LEGV8_OPERATIONS[i].opcode); ++j) {
            if(instruction[j] != LEGV8_OPERATIONS[i].opcode[j]) {
                found = 0;
                break;
            }
        }
        if(found)
            return i; //first match will return index to the opcode that matched in LEGv8_Operations.
    }
    //At this point, opcode not found, so not a valid instruction in our context.
    printf("LINE %d - ERROR: Did not find valid operation code!", line_number);
    exit(0);
} //END DeduceOperationFromInstruction

//FUNCTIONS WRITTEN BY JIN
void DecodeFormat_R(const char *instruction, int line_number, int op_idx) {
    int opcode_size = R_FORMAT_CHUNK_SIZES[0];
    int rm_size = R_FORMAT_CHUNK_SIZES[1];
    int shamt_size = R_FORMAT_CHUNK_SIZES[2];
    int rn_size = R_FORMAT_CHUNK_SIZES[3];
    int rd_size = R_FORMAT_CHUNK_SIZES[4];
    int offset = 0;

    // Skip opcode field
    offset += opcode_size;

    // Get Rm field
    char rm_bin[rm_size + 1];
    strncpy(rm_bin, instruction + offset, rm_size);
    rm_bin[rm_size] = '\0';
    offset += rm_size;

    // Skip shamt field
    offset += shamt_size;

    // Get Rn field
    char rn_bin[rn_size + 1];
    strncpy(rn_bin, instruction + offset, rn_size);
    rn_bin[rn_size] = '\0';
    offset += rn_size;

    // Get Rd field
    char rd_bin[rd_size + 1];
    strncpy(rd_bin, instruction + offset, rd_size);
    rd_bin[rd_size] = '\0';

    // Convert the Rd, Rn, Rd fields to its decimal form
    int rm_int = BinaryToDecimal(rm_bin);
    int rn_int = BinaryToDecimal(rn_bin);
    int rd_int = BinaryToDecimal(rd_bin);

    // Print decoded instruction
    printf("%s X%d, X%d, X%d\n", LEGV8_OPERATIONS[op_idx].assembly, rd_int, rn_int, rm_int);
}

// Written by: Jin Pereyras
// Decode and print I-format instructions
void DecodeFormat_I(const char *instruction, int line_number, int op_idx) {
    int opcode_size = I_FORMAT_CHUNK_SIZES[0];
    int imm_size = I_FORMAT_CHUNK_SIZES[1];
    int rn_size = I_FORMAT_CHUNK_SIZES[2];
    int rd_size = I_FORMAT_CHUNK_SIZES[3];
    int offset = 0;

    // Skip opcode field
    offset += opcode_size;   // offset = 10

    // Get immediate value
    char imm_bin[imm_size + 1];
    strncpy(imm_bin, instruction + offset, imm_size);
    imm_bin[imm_size] = '\0';
    offset += imm_size;     // offset = 22

    // Get Rn field
    char rn_bin[rn_size + 1];
    strncpy(rn_bin, instruction + offset, rn_size);
    rn_bin[rn_size] = '\0';
    offset += rn_size;     // offset = 27

    // Get Rd field
    char rd_bin[rd_size + 1];
    strncpy(rd_bin, instruction + offset, rd_size);
    rd_bin[rd_size] = '\0';

    // Convert the fields we got to its decimal form
    int imm_int = BinaryToDecimal(imm_bin);
    int rn_int = BinaryToDecimal(rn_bin);
    int rd_int = BinaryToDecimal(rd_bin);

    // Print the decoded instruction
    printf("%s X%d, X%d, #%d\n", LEGV8_OPERATIONS[op_idx].assembly, rd_int, rn_int, imm_int);
}

// Written by: Jin Pereyras
// Decode and print D-format instructions
void DecodeFormat_D(const char *instruction, int line_number, int op_idx) {
    int opcode_size = D_FORMAT_CHUNK_SIZES[0];
    int addr_size = D_FORMAT_CHUNK_SIZES[1];
    int op2_size = D_FORMAT_CHUNK_SIZES[2];
    int rn_size = D_FORMAT_CHUNK_SIZES[3];
    int rt_size = D_FORMAT_CHUNK_SIZES[4];
    int offset = 0;

    // Skip opcode field
    offset += opcode_size;

    // Get address field
    char addr_bin[addr_size + 1];
    strncpy(addr_bin, instruction + offset, addr_size);
    addr_bin[addr_size] = '\0';
    offset += addr_size;

    // Skip op2 field
    offset += op2_size;

    // Get Rn field
    char rn_bin[rn_size + 1];
    strncpy(rn_bin, instruction + offset, rn_size);
    rn_bin[rn_size] = '\0';
    offset += rn_size;

    // Get Rt field
    char rt_bin[rt_size + 1];
    strncpy(rt_bin, instruction + offset, rt_size);
    rt_bin[rt_size] = '\0';

    // Convert the fields we got to its decimal form
    int addr_int = BinaryToDecimal(addr_bin);
    int rn_int = BinaryToDecimal(rn_bin);
    int rt_int = BinaryToDecimal(rt_bin);

    // Print decoded instruction
    printf("%s X%d, [X%d, #%d]\n", LEGV8_OPERATIONS[op_idx].assembly, rt_int, rn_int, addr_int);
}
