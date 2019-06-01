/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   util.c                                                    */
/*   Adapted from Computer Architecture@KAIST                  */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*    DO NOT MODIFY THIS FILE!                                  */
/*    You should only modify the run.c, run.h and util.h file!  */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include "util.h"

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/

/* memory will be dynamically allocated at initialization */
mem_region_t MEM_REGIONS[] = {
    { MEM_TEXT_START, MEM_TEXT_SIZE, NULL },
    { MEM_DATA_START, MEM_DATA_SIZE, NULL },
};

#define MEM_NREGIONS (sizeof(MEM_REGIONS)/sizeof(mem_region_t))

/***************************************************************/
/* CPU State info.                                             */
/***************************************************************/
CPU_State CURRENT_STATE;
int RUN_BIT;		/* run bit */
int FETCH_BIT;		/* instruction fetch bit */
int INSTRUCTION_COUNT;

uint64_t MAX_INSTRUCTION_NUM;
uint64_t CYCLE_COUNT;
int BR_BIT;
int FORWARDING_BIT;

/***************************************************************/
/* CPU State info.                                             */
/***************************************************************/
instruction *INST_INFO;
int NUM_INST;

/***************************************************************/
/*                                                             */
/* Procedure: str_split                                        */
/*                                                             */
/* Purpose: To parse main function argument                    */
/*                                                             */
/***************************************************************/
char** str_split(char *a_str, const char a_delim){
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
     *        knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

/***************************************************************/
/*                                                             */
/* Procedure: fromBinary                                       */
/*                                                             */
/* Purpose: From binary to integer                             */
/*                                                             */
/***************************************************************/
int fromBinary(const char *s){
    return (int) strtol(s, NULL, 2);
}

/***************************************************************/
/*                                                             */
/* Procedure: mem_read_32                                      */
/*                                                             */
/* Purpose: Read a 32-bit word from memory                     */
/*                                                             */
/***************************************************************/
uint32_t mem_read_32(uint32_t address){
    int i;
    int valid_flag = 0;

    for (i = 0; i < MEM_NREGIONS; i++) {
        if (address >= MEM_REGIONS[i].start &&
                address < (MEM_REGIONS[i].start + MEM_REGIONS[i].size)) {
            uint32_t offset = address - MEM_REGIONS[i].start;

            valid_flag = 1;

            return
                (MEM_REGIONS[i].mem[offset+3] << 24) |
                (MEM_REGIONS[i].mem[offset+2] << 16) |
                (MEM_REGIONS[i].mem[offset+1] <<  8) |
                (MEM_REGIONS[i].mem[offset+0] <<  0);
        }
    }

    if (!valid_flag){
        printf("Memory Read Error: Exceed memory boundary 0x%x\n", address);
        exit(1);
    }

    return 0;
}

/***************************************************************/
/*                                                             */
/* Procedure: mem_write_32                                     */
/*                                                             */
/* Purpose: Write a 32-bit word to memory                      */
/*                                                             */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value) {
    int i;
    int valid_flag = 0;

    for (i = 0; i < MEM_NREGIONS; i++) {
        if (address >= MEM_REGIONS[i].start &&
                address < (MEM_REGIONS[i].start + MEM_REGIONS[i].size)) {
            uint32_t offset = address - MEM_REGIONS[i].start;

            valid_flag = 1;

            MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
            MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
            MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
            MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
            return;
        }
    }

    if(!valid_flag){
        printf("Memory Write Error: Exceed memory boundary 0x%x\n", address);
        exit(1);
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    if (INSTRUCTION_COUNT >= MAX_INSTRUCTION_NUM) {
        RUN_BIT = FALSE;
    }

    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate MIPS for n instructions                */
/*                                                             */
/***************************************************************/
void run() {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %lu instructions...\n\n", MAX_INSTRUCTION_NUM);

    while (RUN_BIT) {
	    cycle();
    }

    printf("Simulator halted after %lu cycles\n\n", CYCLE_COUNT);
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate MIPS until HALTed                      */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (RUN_BIT) {
	    cycle();
    }

    printf("Simulator halted after %lu cycles\n\n", CYCLE_COUNT);
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(int start, int stop) {
    int address;

    printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = start; address <= stop; address += 4)
        printf("0x%08x: 0x%08x\n", address, mem_read_32(address));
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump() {
    int k;

    printf("Current register values :\n");
    printf("-------------------------------------\n");
    printf("PC: 0x%08x\n", CURRENT_STATE.PC);
    printf("Registers:\n");
    for (k = 0; k < MIPS_REGS; k++)
        printf("R%d: 0x%08x\n", k, CURRENT_STATE.REGS[k]);
    printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : pdump                                           */
/*                                                             */
/* Purpose   : Dump current pipeline PC state                  */
/*                                                             */
/***************************************************************/
void pdump() {
    int k;

    printf("Current pipeline PC state :\n");
    printf("-------------------------------------\n");
    printf("CYCLE %lu:", CYCLE_COUNT );

    for (k = 0; k < 5; k++) {
        if (CURRENT_STATE.PIPE[k]) {
            printf("0x%08x", CURRENT_STATE.PIPE[k]);
        } else {
            printf("          ");
        }

        if ( k != PIPE_STAGE - 1 ) {
            printf("|");
        }
    }
    printf("\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Allocate and zero memory                        */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;
    for (i = 0; i < MEM_NREGIONS; i++) {
        MEM_REGIONS[i].mem = malloc(MEM_REGIONS[i].size);
        memset(MEM_REGIONS[i].mem, 0, MEM_REGIONS[i].size);
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_inst_info                                  */
/*                                                             */
/* Purpose   : Initialize instruction info                     */
/*                                                             */
/***************************************************************/
void init_inst_info()
{
    int i;

    for(i = 0; i < NUM_INST; i++) {
        INST_INFO[i].value = 0;
        INST_INFO[i].opcode = 0;
        INST_INFO[i].func_code = 0;
        INST_INFO[i].r_t.r_i.rs = 0;
        INST_INFO[i].r_t.r_i.rt = 0;
        INST_INFO[i].r_t.r_i.r_i.r.rd = 0;
        INST_INFO[i].r_t.r_i.r_i.imm = 0;
        INST_INFO[i].r_t.r_i.r_i.r.shamt = 0;
        INST_INFO[i].r_t.target = 0;
    }
}
