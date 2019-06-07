/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from Computer Architecture@KAIST                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) {
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{
	/** Implement this function */


	WB_Stage();
	
	MEM_Stage();
	
	EX_Stage();

	ID_Stage();

	IF_Stage();


	CURRENT_STATE.PC += 4;


	/*
	if (OPCODE(instr) == 0x0) {

		// TYPE R
		switch (FUNC(instr)) {
		case 0x21:	// ADD U
			//printf("ADDU :$%d = $%d + $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] + CURRENT_STATE.REGS[RT(instr)];
			break;
		case 0x24:	// AND
			//printf("AND :$%d = $%d & $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] & CURRENT_STATE.REGS[RT(instr)];
			break;
		case 0x27:	// NOR
			//printf("NOR :$%d = ~($%d | $%d) \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.REGS[RD(instr)] = ~(CURRENT_STATE.REGS[RS(instr)] | CURRENT_STATE.REGS[RT(instr)]);
			break;
		case 0x25:	// OR
			//printf("OR :$%d = $%d | $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] | CURRENT_STATE.REGS[RT(instr)];
			break;
		case 0x2b:	// SLT U
			//printf("SLTU :$%d = ($%d < $%d) ? 1:0 \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.REGS[RD(instr)] = (CURRENT_STATE.REGS[RS(instr)] < CURRENT_STATE.REGS[RT(instr)]) ? 1 : 0;
			break;
		case 0x00:	// SLL
			//printf("SLL :$%d = $%d << ($%d)shamt \n", RD(instr), RT(instr), SHAMT(instr));
			CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RT(instr)] << SHAMT(instr);
			break;
		case 0x02:	// SRL
			//printf("SRL :$%d = $%d >> ($%d)shamt \n", RD(instr), RT(instr), SHAMT(instr));
			CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RT(instr)] >> SHAMT(instr);
			break;
		case 0x23:	// SUB U
			//printf("SUBU :$%d = $%d - $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.REGS[RD(instr)] = CURRENT_STATE.REGS[RS(instr)] - CURRENT_STATE.REGS[RT(instr)];
			break;
		case 0x08:	//JR
			//printf("JR : PC = 0x%x \n", CURRENT_STATE.REGS[RS(instr)]);
			CURRENT_STATE.PC = CURRENT_STATE.REGS[RS(instr)];
			break;
		default:
			//printf("ERROR: Check process_instruction() TYPE R func_code\m");
			RUN_BIT = FALSE;
			CURRENT_STATE.PC -= 4;

		}
	}
	else {
		switch (OPCODE(instr)) {

			// TYPE I
		case 0x9:		//(0x001001)ADDIU
			//printf("ADDIU :$%d = $%d + %d \n", RT(instr), RS(instr), IMM(instr));
			CURRENT_STATE.REGS[RT(instr)] = CURRENT_STATE.REGS[RS(instr)] + IMM(instr);
			break;
		case 0xc:		//(0x001100)ANDI
			//printf("ANDI :$%d = $%d & $%d \n", RD(instr), RS(instr), IMM(instr));
			CURRENT_STATE.REGS[RT(instr)] = CURRENT_STATE.REGS[RS(instr)] & IMM(instr);
			break;
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
			//printf("LUI :%d = %d \n", RT(instr), IMM(instr));
			CURRENT_STATE.REGS[RT(instr)] = IMM(instr) * 65536; // 16^4
			break;
		case 0xd:		//(0x001101)ORI
			//printf("ORI :%d = %d or %d \n", RT(instr), RS(instr), IMM(instr));
			CURRENT_STATE.REGS[RT(instr)] = CURRENT_STATE.REGS[RS(instr)] | IMM(instr);
			break;
		case 0xb:		//(0x001011)SLTIU
			//printf("SLTIU :$%d = ($%d < $%d) ? 1:0 \n", RT(instr), RS(instr), IMM(instr));
			CURRENT_STATE.REGS[RT(instr)] = (CURRENT_STATE.REGS[RS(instr)] < IMM(instr)) ? 1 : 0;
			break;
		case 0x23:		//(0x100011)LW
			//printf("LW :$%d = M[0x%8x + %d] \n", RT(instr), CURRENT_STATE.REGS[RS(instr)], IMM(instr));
			CURRENT_STATE.REGS[RT(instr)] = mem_read_32(CURRENT_STATE.REGS[RS(instr)] + IMM(instr));
			break;
		case 0x2b:		//(0x101011)SW
			//printf("SW :M[0x%8x + %d] = $%d \n", CURRENT_STATE.REGS[RS(instr)], IMM(instr), RT(instr));
			mem_write_32(CURRENT_STATE.REGS[RS(instr)] + IMM(instr), CURRENT_STATE.REGS[RT(instr)]);
			break;
		case 0x4:		//(0x000100)BEQ
			//printf("BEQ :if($%d == $%d) goto PC + %d(*4) \n", RS(instr), RT(instr), IMM(instr));
			if (CURRENT_STATE.REGS[RS(instr)] == CURRENT_STATE.REGS[RT(instr)])
				CURRENT_STATE.PC += 4 * IMM(instr);
			break;
		case 0x5:		//(0x000101)BNE
			//printf("BNE :if($%d != $%d) goto PC + %d(*4) \n", RS(instr), RT(instr), IMM(instr));
			if (CURRENT_STATE.REGS[RS(instr)] != CURRENT_STATE.REGS[RT(instr)])
				CURRENT_STATE.PC += 4 * IMM(instr);
			break;

			// TYPE J
		case 0x2:		//J
			//printf("J :PC = PC[31:28] strcat [0x%x(imm) * 4] \n", TARGET(instr));
			PC_addr = CURRENT_STATE.PC;
			PC_addr = PC_addr & 0xf0000000; //PC[31:28]
			CURRENT_STATE.PC = PC_addr + TARGET(instr) * 4;
			break;
		case 0x3:		//JAL
			//printf("JAL :R[31]=PC+4, J to 0x%x(imm)*4 \n", TARGET(instr));
			CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
			PC_addr = CURRENT_STATE.PC;
			PC_addr = PC_addr & 0xf0000000; //PC[31:28]
			CURRENT_STATE.PC = PC_addr + TARGET(instr) * 4;
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE I,, J opcode\n");
			RUN_BIT = FALSE;
			CURRENT_STATE.PC -= 4;

		}
	}
	*/

}

// Function for each pipeline stage

// Instruction fetch from memory 
void IF_Stage() {
	CURRENT_STATE.PIPE[IF_STAGE] = CURRENT_STATE.PC;

	CURRENT_STATE.IF_ID_INST = get_inst_info(CURRENT_STATE.PC);

	CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PIPE[IF_STAGE];
}

// Instruction decode & register read 
void ID_Stage() {
	if (!CURRENT_STATE.IF_ID_NPC) return;

	CURRENT_STATE.PIPE[ID_STAGE] = CURRENT_STATE.IF_ID_NPC;

	// Decode this instruction
	instruction* instr = CURRENT_STATE.IF_ID_INST;

	CURRENT_STATE.ID_EX_OPCODE = OPCODE(instr);

	if (OPCODE(instr) == 0x0) {
		// TYPE R
		CURRENT_STATE.ID_EX_RS = CURRENT_STATE.REGS[RS(instr)];
		CURRENT_STATE.ID_EX_RT = CURRENT_STATE.REGS[RT(instr)];
		CURRENT_STATE.ID_EX_RD = RD(instr);
		CURRENT_STATE.ID_EX_SHAMT = SHAMT(instr);
		CURRENT_STATE.ID_EX_FUNCT = FUNC(instr);
	}
	else {
		switch (OPCODE(instr)) {

			// TYPE I
		case 0x9:		//(0x001001)ADDIU
		case 0xc:		//(0x001100)ANDI
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
		case 0xd:		//(0x001101)ORI
		case 0xb:		//(0x001011)SLTIU
		case 0x23:		//(0x100011)LW
		case 0x2b:		//(0x101011)SW
		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE

			CURRENT_STATE.ID_EX_RS = CURRENT_STATE.REGS[RS(instr)];
			CURRENT_STATE.ID_EX_RT = RT(instr);
			CURRENT_STATE.ID_EX_IMM = IMM(instr);
			break;

			// TYPE J
		case 0x2:		//J
		case 0x3:		//JAL
			CURRENT_STATE.ID_EX_DEST = TARGET(instr) * 4;
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE I,, J opcode\n");
			RUN_BIT = FALSE;
			CURRENT_STATE.PC -= 4;
		}
	}
	
	CURRENT_STATE.ID_EX_NPC = CURRENT_STATE.PIPE[ID_STAGE];
}

// Execute operation or calculate address 
void EX_Stage() {
	if (!CURRENT_STATE.ID_EX_NPC) return;

	CURRENT_STATE.PIPE[EX_STAGE] = CURRENT_STATE.ID_EX_NPC;

	CURRENT_STATE.EX_MEM_OPCODE = CURRENT_STATE.ID_EX_OPCODE;

	// Execute operation with ID_EX registers

	if (CURRENT_STATE.ID_EX_OPCODE == 0x0) {
		// TYPE R
		CURRENT_STATE.EX_MEM_RD = CURRENT_STATE.ID_EX_RD; // pass RD to WB

		switch (CURRENT_STATE.ID_EX_FUNCT) {
		case 0x21:	// ADD U
			//printf("ADDU :$%d = $%d + $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_RT;
			break;
		case 0x24:	// AND
			//printf("AND :$%d = $%d & $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS & CURRENT_STATE.ID_EX_RT;
			break;
		case 0x27:	// NOR
			//printf("NOR :$%d = ~($%d | $%d) \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = ~(CURRENT_STATE.ID_EX_RS | CURRENT_STATE.ID_EX_RT);
			break;
		case 0x25:	// OR
			//printf("OR :$%d = $%d | $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS | CURRENT_STATE.ID_EX_RT;
			break;
		case 0x2b:	// SLT U
			//printf("SLTU :$%d = ($%d < $%d) ? 1:0 \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = (CURRENT_STATE.ID_EX_RS < CURRENT_STATE.ID_EX_RT) ? 1 : 0;
			break;
		case 0x00:	// SLL
			//printf("SLL :$%d = $%d << ($%d)shamt \n", RD(instr), RT(instr), SHAMT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS << CURRENT_STATE.ID_EX_RT;
			break;
		case 0x02:	// SRL
			//printf("SRL :$%d = $%d >> ($%d)shamt \n", RD(instr), RT(instr), SHAMT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS >> CURRENT_STATE.ID_EX_RT;
			break;
		case 0x23:	// SUB U
			//printf("SUBU :$%d = $%d - $%d \n", RD(instr), RS(instr), RT(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS - CURRENT_STATE.ID_EX_RT;
			break;
		case 0x08:	//JR
			//printf("JR : PC = 0x%x \n", CURRENT_STATE.REGS[RS(instr)]);
			CURRENT_STATE.EX_MEM_BR_TARGET = CURRENT_STATE.ID_EX_RS;
			// BRANCH!!!
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE R func_code\m");
			RUN_BIT = FALSE;
			//CURRENT_STATE.PC -= 4;
		}
	}
	else {
		uint32_t PC_addr;
		switch (CURRENT_STATE.ID_EX_OPCODE) {
			CURRENT_STATE.EX_MEM_RD = CURRENT_STATE.ID_EX_RT; // pass RT to WB

			// TYPE I
		case 0x9:		//(0x001001)ADDIU
			//printf("ADDIU :$%d = $%d + %d \n", RT(instr), RS(instr), IMM(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_IMM;
			break;
		case 0xc:		//(0x001100)ANDI
			//printf("ANDI :$%d = $%d & $%d \n", RD(instr), RS(instr), IMM(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS & CURRENT_STATE.ID_EX_IMM;
			break;
		case 0xd:		//(0x001101)ORI
			//printf("ORI :%d = %d or %d \n", RT(instr), RS(instr), IMM(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS | CURRENT_STATE.ID_EX_IMM;
			break;
		case 0xb:		//(0x001011)SLTIU
			//printf("SLTIU :$%d = ($%d < $%d) ? 1:0 \n", RT(instr), RS(instr), IMM(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = (CURRENT_STATE.ID_EX_RS < CURRENT_STATE.ID_EX_IMM) ? 1 : 0;
			break;
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
			//printf("LUI :%d = %d \n", RT(instr), IMM(instr));
			//CURRENT_STATE.REGS[RT(instr)] = IMM(instr) * 65536; // 16^4
			// IMM의 앞 비트 부분. Jump에 쓸 메모리 주소
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_IMM * 65536;
			break;
		case 0x23:		//(0x100011)LW
			//printf("LW :$%d = M[0x%8x + %d] \n", RT(instr), CURRENT_STATE.REGS[RS(instr)], IMM(instr));
			//CURRENT_STATE.REGS[RT(instr)] = mem_read_32(CURRENT_STATE.REGS[RS(instr)] + IMM(instr));
			// 접근할 메모리 주소(읽기)
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_IMM;
			break;
		case 0x2b:		//(0x101011)SW
			//printf("SW :M[0x%8x + %d] = $%d \n", CURRENT_STATE.REGS[RS(instr)], IMM(instr), RT(instr));
			//mem_write_32(CURRENT_STATE.REGS[RS(instr)] + IMM(instr), CURRENT_STATE.REGS[RT(instr)]);
			// 접근할 메모리 주소(쓰기)
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_IMM;
			break;
		case 0x4:		//(0x000100)BEQ
			//printf("BEQ :if($%d == $%d) goto PC + %d(*4) \n", RS(instr), RT(instr), IMM(instr));
			if (CURRENT_STATE.ID_EX_RS == CURRENT_STATE.ID_EX_RT)
				CURRENT_STATE.EX_MEM_BR_TARGET += 4 * CURRENT_STATE.ID_EX_IMM;
			break;
		case 0x5:		//(0x000101)BNE
			//printf("BNE :if($%d != $%d) goto PC + %d(*4) \n", RS(instr), RT(instr), IMM(instr));
			if (CURRENT_STATE.ID_EX_RS != CURRENT_STATE.ID_EX_RT)
				CURRENT_STATE.EX_MEM_BR_TARGET += 4 * CURRENT_STATE.ID_EX_IMM;
			break;

			// TYPE J
		case 0x2:		//J
			//printf("J :PC = PC[31:28] strcat [0x%x(imm) * 4] \n", TARGET(instr));
			PC_addr = CURRENT_STATE.ID_EX_NPC;
			PC_addr = PC_addr & 0xf0000000; //PC[31:28]
			CURRENT_STATE.EX_MEM_BR_TARGET = PC_addr + CURRENT_STATE.ID_EX_DEST;

			break;
		case 0x3:		//JAL
			//printf("JAL :R[31]=PC+4, J to 0x%x(imm)*4 \n", TARGET(instr));
			PC_addr = CURRENT_STATE.ID_EX_NPC;
			PC_addr = PC_addr & 0xf0000000; //PC[31:28]
			CURRENT_STATE.EX_MEM_BR_TARGET = PC_addr + CURRENT_STATE.ID_EX_DEST;
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE I,, J opcode\n");
			RUN_BIT = FALSE;
			//CURRENT_STATE.PC -= 4;

		}
	}

	// Save result in registers
	CURRENT_STATE.EX_MEM_NPC = CURRENT_STATE.PIPE[EX_STAGE];
}

// Access memory operand 
void MEM_Stage() {
	if (!CURRENT_STATE.EX_MEM_NPC) return;

	CURRENT_STATE.PIPE[MEM_STAGE] = CURRENT_STATE.EX_MEM_NPC;

	CURRENT_STATE.MEM_WB_OPCODE = CURRENT_STATE.EX_MEM_OPCODE;

	
	// Access Memory data

	if (CURRENT_STATE.EX_MEM_OPCODE == 0x0) {
		// TYPE R
		CURRENT_STATE.MEM_WB_RD = CURRENT_STATE.EX_MEM_RD; // pass RD to WB

		switch (CURRENT_STATE.ID_EX_FUNCT) { // 고쳐야 함
		case 0x21:	// ADD U
		case 0x24:	// AND
		case 0x27:	// NOR
		case 0x25:	// OR
		case 0x2b:	// SLT U
		case 0x00:	// SLL
		case 0x02:	// SRL
		case 0x23:	// SUB U
			CURRENT_STATE.MEM_WB_ALU_OUT = CURRENT_STATE.EX_MEM_ALU_OUT;
			break;
		case 0x08:	//JR
			CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;
			// BRANCH!!!
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE R func_code\m");
			RUN_BIT = FALSE;
			CURRENT_STATE.PC -= 4;
		}
	}
	else {
		CURRENT_STATE.MEM_WB_RD = CURRENT_STATE.EX_MEM_RD; // pass RD to WB

		switch (CURRENT_STATE.EX_MEM_OPCODE) {
			// TYPE I
		case 0x9:		//(0x001001)ADDIU
		case 0xc:		//(0x001100)ANDI
		case 0xd:		//(0x001101)ORI
		case 0xb:		//(0x001011)SLTIU
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
			// IMM의 앞 비트 부분. Jump에 쓸 메모리 주소
			CURRENT_STATE.MEM_WB_ALU_OUT = CURRENT_STATE.EX_MEM_ALU_OUT;
			break;

		case 0x23:		//(0x100011)LW
			// 메모리를 읽어 그 값을 MEM_OUT에
			CURRENT_STATE.MEM_WB_MEM_OUT = mem_read_32(CURRENT_STATE.EX_MEM_ALU_OUT);

			break;
		case 0x2b:		//(0x101011)SW
			// RT의 값을 메모리에 쓴다
			mem_write_32(CURRENT_STATE.EX_MEM_ALU_OUT, CURRENT_STATE.ID_EX_RT); // 고쳐야 함

			break;

		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE
			CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;

			break;

			// TYPE J
		case 0x2:		//J
			CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;

			break;
		case 0x3:		//JAL
			CURRENT_STATE.REGS[31] = CURRENT_STATE.EX_MEM_NPC + 4;
			CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE I,, J opcode\n");
			RUN_BIT = FALSE;
			CURRENT_STATE.PC -= 4;
		}
	}
	CURRENT_STATE.MEM_WB_NPC = CURRENT_STATE.PIPE[MEM_STAGE];
}

// Write result back to register
void WB_Stage() {
	if (!CURRENT_STATE.MEM_WB_NPC) return;

	CURRENT_STATE.PIPE[WB_STAGE] = CURRENT_STATE.MEM_WB_NPC;

	if (CURRENT_STATE.MEM_WB_OPCODE == 0x0) {
		// TYPE R

		switch (CURRENT_STATE.ID_EX_FUNCT) { // 고쳐야 함
		case 0x21:	// ADD U
		case 0x24:	// AND
		case 0x27:	// NOR
		case 0x25:	// OR
		case 0x2b:	// SLT U
		case 0x00:	// SLL
		case 0x02:	// SRL
		case 0x23:	// SUB U
			CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_RD] = CURRENT_STATE.MEM_WB_ALU_OUT;
			break;
		case 0x08:	//JR
			// empty
			break;

		default:
			break;
		}
	}
	else {

		switch (CURRENT_STATE.MEM_WB_OPCODE) {
			// TYPE I
		case 0x9:		//(0x001001)ADDIU
		case 0xc:		//(0x001100)ANDI
		case 0xd:		//(0x001101)ORI
		case 0xb:		//(0x001011)SLTIU
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
			// IMM의 앞 비트 부분. Jump에 쓸 메모리 주소
			CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_RD] = CURRENT_STATE.MEM_WB_ALU_OUT;
			break;

		case 0x23:		//(0x100011)LW
			CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_RD] = CURRENT_STATE.MEM_WB_MEM_OUT;

			break;
		case 0x2b:		//(0x101011)SW
		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE

		case 0x2:		//J
		case 0x3:		//JAL
			// Nothing to do with registers
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE I,, J opcode\n");
			RUN_BIT = FALSE;
			CURRENT_STATE.PC -= 4;
		}
	}

	INSTRUCTION_COUNT++; // Need to move this
}