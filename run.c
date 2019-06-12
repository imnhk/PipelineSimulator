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




// Branch prediction Flush
int bpFlag = 0;
void bpFlush(int flag) {
	if (flag == 0)
		return;
	if (flag == 1) {
		CURRENT_STATE.PIPE[IF_STAGE] = 0;
		CURRENT_STATE.IF_ID_NPC = 0;
		bpFlag -= 1;
		return;
	}
	if (flag == 2) {
		CURRENT_STATE.IF_ID_NPC = 0;
		CURRENT_STATE.ID_EX_NPC = 0;
		bpFlag -= 1;
		return;
	}
}


// Stop IF, ID for 1 cycle for LW forwarding
int lwFlag = FALSE;
int lwMemFlag = FALSE;
void LWstall(int flag) {
	if (!flag) return;

	printf("@@@: LW stalling \n");

	CURRENT_STATE.REGS_LOCK[IF_STAGE] = TRUE;
	CURRENT_STATE.REGS_LOCK[ID_STAGE] = TRUE;

	// Create bubble at EX_MEM buffer

	lwFlag = FALSE;
	//lwMemFlag = TRUE;
	return;
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

	bpFlush(bpFlag);
	LWstall(lwFlag);

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
	if (CURRENT_STATE.REGS_LOCK[IF_STAGE]) {
		// IF stage is locked
		CURRENT_STATE.REGS_LOCK[IF_STAGE] = FALSE; // Stall for 1 cycle
		CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PIPE[IF_STAGE];
		return;
	}


	//printf("IF OPCODE: %d\n", get_inst_info(CURRENT_STATE.PC)->opcode);
	if (get_inst_info(CURRENT_STATE.PC)->opcode < 0) {
		// 명령어 끝 확인(임시) 가상환경에서는 안 될 수도 있음
		FETCH_BIT = FALSE;
	}
	if (!FETCH_BIT && CURRENT_STATE.PIPE[MEM_STAGE] == 0) {
		// Pipeline to be empty
		RUN_BIT = FALSE;
	}

	if (FETCH_BIT) {
		CURRENT_STATE.PIPE[IF_STAGE] = CURRENT_STATE.PC;
		CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PC;
		CURRENT_STATE.IF_ID_INST = get_inst_info(CURRENT_STATE.PC);

		CURRENT_STATE.PC += 4;
	}
	else {
		// No more instruction to fetch
		CURRENT_STATE.PIPE[IF_STAGE] = 0;
		CURRENT_STATE.IF_ID_NPC = CURRENT_STATE.PIPE[IF_STAGE];
	}
}

// Instruction decode & register read 

void ID_Stage() {
	// ID stage is locked
	if (CURRENT_STATE.REGS_LOCK[ID_STAGE]) {
		CURRENT_STATE.REGS_LOCK[ID_STAGE] = FALSE;
		CURRENT_STATE.ID_EX_NPC = CURRENT_STATE.PIPE[ID_STAGE];
		return;
	}
	
	if (!CURRENT_STATE.IF_ID_NPC) {
		// pass NOP to next stage
		CURRENT_STATE.PIPE[ID_STAGE] = 0;
		CURRENT_STATE.ID_EX_NPC = 0;
		return;
	}

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

		// NOP Check. ADDU 0 0 0.
		if (FUNC(instr) == 0x21) {
			if (RS(instr) == 0 && RT(instr) == 0 && RD(instr) == 0) {
				// Its bubble. Clear ID_EX buffer
				CURRENT_STATE.PIPE[ID_STAGE] = 0;
			}
		}


		// Type-R Forwarding. Compare with ex-cycle EX stage.
		//printf("EX- forwarding check EX_MEM RD: %d \n", CURRENT_STATE.EX_MEM_RD);
		//printf("RS: %d RT :%d\n", RS(instr), RT(instr));
		if (CURRENT_STATE.EX_MEM_RD == RS(instr)) {
			CURRENT_STATE.ID_EX_RS = CURRENT_STATE.EX_MEM_ALU_OUT;
			printf("ID: Forwarding, RS is now 0x%x\n", CURRENT_STATE.ID_EX_RS);
		}
		if (CURRENT_STATE.EX_MEM_RD == RT(instr)) {
			CURRENT_STATE.ID_EX_RT = CURRENT_STATE.EX_MEM_ALU_OUT;
			printf("ID: Forwarding, RT is now 0x%x\n", CURRENT_STATE.ID_EX_RT);
		}

		// Compare with ex-ex-cycle MEM stage
		if (CURRENT_STATE.MEM_WB_RD == RS(instr)) {
			CURRENT_STATE.ID_EX_RS = CURRENT_STATE.MEM_WB_ALU_OUT;
			printf("ID: MEM Forwarding, RS is now 0x%x\n", CURRENT_STATE.ID_EX_RS);
		}
		if (CURRENT_STATE.MEM_WB_RD == RT(instr)) {
			CURRENT_STATE.ID_EX_RT = CURRENT_STATE.MEM_WB_ALU_OUT;
			printf("ID: MEM Forwarding, RT is now 0x%x\n", CURRENT_STATE.ID_EX_RT);
		}

		// LW MEM forwarding requires 1 cycle stall
		if (CURRENT_STATE.EX_MEM_OPCODE == 0x23 && !CURRENT_STATE.REGS_LOCK[EX_STAGE]) {
			//printf("LW FW check EX_MEM RD: %d \n", CURRENT_STATE.EX_MEM_RD);
			//printf("RS: %d RT :%d\n", RS(instr), RT(instr));

			// 한 cycle 기다린 뒤 Forward
			if (CURRENT_STATE.EX_MEM_RD == RS(instr)) {
				lwMemFlag = 1;
				lwFlag = TRUE;

			}
			if (CURRENT_STATE.EX_MEM_RD == RT(instr)) {
				lwMemFlag = 2;
				lwFlag = TRUE;
			}
			return;
		}
	}
	else {
		switch (OPCODE(instr)) {
			// TYPE I
		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE
			CURRENT_STATE.ID_EX_RS = CURRENT_STATE.REGS[RS(instr)];
			CURRENT_STATE.ID_EX_RT = CURRENT_STATE.REGS[RT(instr)];
			CURRENT_STATE.ID_EX_IMM = IMM(instr);
			break;

		case 0x9:		//(0x001001)ADDIU
		case 0xc:		//(0x001100)ANDI
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
		case 0xd:		//(0x001101)ORI
		case 0xb:		//(0x001011)SLTIU
		case 0x23:		//(0x100011)LW
		case 0x2b:		//(0x101011)SW
			CURRENT_STATE.ID_EX_RS = CURRENT_STATE.REGS[RS(instr)];
			CURRENT_STATE.ID_EX_RT = RT(instr);
			CURRENT_STATE.ID_EX_IMM = IMM(instr);


			// Type-I Forwarding. Compare with ex-cycle EX stage.
			//printf("ID: Fw check, RD = %d, RS = %d\n", CURRENT_STATE.EX_MEM_RD, RS(instr));
			if (CURRENT_STATE.EX_MEM_RD == RS(instr)) {

				CURRENT_STATE.ID_EX_RS = CURRENT_STATE.EX_MEM_ALU_OUT;
				//printf("ID: Forwarding, RS is now 0x%x\n", CURRENT_STATE.ID_EX_RS);
			}
			if (CURRENT_STATE.MEM_WB_RD == RS(instr)) {

				CURRENT_STATE.ID_EX_RS = CURRENT_STATE.MEM_WB_ALU_OUT;
				//printf("ID: MEM Forwarding, RS is now 0x%x\n", CURRENT_STATE.ID_EX_RS);
			}
			break;

			// LW MEM forwarding requires 1 cycle stall
			if (CURRENT_STATE.EX_MEM_OPCODE == 0x23 && !CURRENT_STATE.REGS_LOCK[EX_STAGE]) {
				printf("LW FW check EX_MEM RD: %d \n", CURRENT_STATE.EX_MEM_RD);
				printf("RS: %d RT :%d\n", RS(instr), RT(instr));

				// 한 cycle 기다린 뒤 Forward
				if (CURRENT_STATE.EX_MEM_RD == RS(instr)) {
					lwMemFlag = 1;
					lwFlag = TRUE;
				}


				return;
			}

			// TYPE J
		case 0x2:		//J
		case 0x3:		//JAL
			CURRENT_STATE.ID_EX_DEST = TARGET(instr);
			break;

		default:
			//FETCH_BIT = FALSE;
			break;
		}
	}
	CURRENT_STATE.ID_EX_NPC = CURRENT_STATE.PIPE[ID_STAGE];

}

// Execute operation or calculate address 
void EX_Stage() {

	// EX_STAGE is locked
	if (CURRENT_STATE.REGS_LOCK[EX_STAGE]) {
		CURRENT_STATE.REGS_LOCK[EX_STAGE] = FALSE;
		return;
	}

	if (!CURRENT_STATE.ID_EX_NPC) {
		// pass NOP to next stage

		CURRENT_STATE.PIPE[EX_STAGE] = 0;
		CURRENT_STATE.EX_MEM_NPC = 0;
		return;
	}

	// LW Forwarding. create bubble when ID stage is locked
	if (CURRENT_STATE.REGS_LOCK[ID_STAGE]){
		CURRENT_STATE.PIPE[EX_STAGE] = 0;
		CURRENT_STATE.EX_MEM_NPC = 0;
		return;
	}

	// LW stall- Forwarding check.
	if (lwMemFlag) {
		if(lwMemFlag == 1)
			CURRENT_STATE.ID_EX_RS = CURRENT_STATE.MEM_WB_MEM_OUT;
		else if(lwMemFlag == 2)
			CURRENT_STATE.ID_EX_RT = CURRENT_STATE.MEM_WB_MEM_OUT;

		//printf("ID: LW Forwarding done, Flag %d \n", lwMemFlag);

		lwMemFlag = FALSE;
	}

	CURRENT_STATE.PIPE[EX_STAGE] = CURRENT_STATE.ID_EX_NPC;

	CURRENT_STATE.EX_MEM_OPCODE = CURRENT_STATE.ID_EX_OPCODE;
	CURRENT_STATE.EX_MEM_FUNCT = CURRENT_STATE.ID_EX_FUNCT;


	// Execute operation with ID_EX registers

	if (CURRENT_STATE.ID_EX_OPCODE == 0x0) {
		// TYPE R
		CURRENT_STATE.EX_MEM_RD = CURRENT_STATE.ID_EX_RD; // pass RD to WB

		switch (CURRENT_STATE.ID_EX_FUNCT) {
		case 0x21:	// ADD U
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_RT;
			break;
		case 0x24:	// AND
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS & CURRENT_STATE.ID_EX_RT;
			break;
		case 0x27:	// NOR
			CURRENT_STATE.EX_MEM_ALU_OUT = ~(CURRENT_STATE.ID_EX_RS | CURRENT_STATE.ID_EX_RT);
			break;
		case 0x25:	// OR
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS | CURRENT_STATE.ID_EX_RT;
			break;
		case 0x2b:	// SLT U
			CURRENT_STATE.EX_MEM_ALU_OUT = (CURRENT_STATE.ID_EX_RS < CURRENT_STATE.ID_EX_RT) ? 1 : 0;
			break;
		case 0x00:	// SLL
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RT << CURRENT_STATE.ID_EX_SHAMT;
			break;
		case 0x02:	// SRL
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RT >> CURRENT_STATE.ID_EX_SHAMT;
			break;
		case 0x23:	// SUB U
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS - CURRENT_STATE.ID_EX_RT;
			break;
		case 0x08:	//JR
			//printf("JR : PC = 0x%x \n", CURRENT_STATE.REGS[RS(instr)]);
			CURRENT_STATE.JUMP_PC = CURRENT_STATE.ID_EX_RS;
			break;
		default:
			//RUN_BIT = FALSE;
			break;
		}
	}
	else {
		uint32_t PC_addr;
		CURRENT_STATE.EX_MEM_RD = CURRENT_STATE.ID_EX_RT; // pass RT to WB

		switch (CURRENT_STATE.ID_EX_OPCODE) {
			// TYPE I
		case 0x9:		//(0x001001)ADDIU
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_IMM;
			break;
		case 0xc:		//(0x001100)ANDI
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS & CURRENT_STATE.ID_EX_IMM;
			break;
		case 0xd:		//(0x001101)ORI
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS | CURRENT_STATE.ID_EX_IMM;
			break;
		case 0xb:		//(0x001011)SLTIU
			CURRENT_STATE.EX_MEM_ALU_OUT = (CURRENT_STATE.ID_EX_RS < CURRENT_STATE.ID_EX_IMM) ? 1 : 0;
			break;
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
			//printf("LUI :%d = %d \n", RT(instr), IMM(instr));
			//CURRENT_STATE.REGS[RT(instr)] = IMM(instr) * 65536; // 16^4
			// IMM의 앞 비트 부분. Jump에 쓸 메모리 주소
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_IMM * 65536;
			break;
		case 0x23:		//(0x100011)LW
			//CURRENT_STATE.REGS[RT(instr)] = mem_read_32(CURRENT_STATE.REGS[RS(instr)] + IMM(instr));
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_IMM;
			break;
		case 0x2b:		//(0x101011)SW
			//mem_write_32(CURRENT_STATE.REGS[RS(instr)] + IMM(instr), CURRENT_STATE.REGS[RT(instr)]);
			CURRENT_STATE.EX_MEM_ALU_OUT = CURRENT_STATE.ID_EX_RS + CURRENT_STATE.ID_EX_IMM;
			break;
		case 0x4:		//(0x000100)BEQ
			if (CURRENT_STATE.ID_EX_RS == CURRENT_STATE.ID_EX_RT) {
				CURRENT_STATE.EX_MEM_BR_TARGET = CURRENT_STATE.ID_EX_NPC + 4 * CURRENT_STATE.ID_EX_IMM;

				// FLUSH
				bpFlag = 2;
				printf("BEQ FLUSH--- bpflag = %d\n", bpFlag);
			}
			break;
		case 0x5:		//(0x000101)BNE
			printf("EX: BNE rs: %d, rt: %d \n", CURRENT_STATE.ID_EX_RS, CURRENT_STATE.ID_EX_RT);
			if (CURRENT_STATE.ID_EX_RS != CURRENT_STATE.ID_EX_RT) {
				CURRENT_STATE.EX_MEM_BR_TARGET = CURRENT_STATE.ID_EX_NPC + 4 * CURRENT_STATE.ID_EX_IMM;

				// FLUSH
				bpFlag = 2;
				printf("BNE FLUSH--- bpflag = %d\n", bpFlag);
			}
			break;

			// TYPE J
		case 0x2:		//J
			//printf("J :PC = PC[31:28] strcat [0x%x(imm) * 4] \n", TARGET(instr));
			PC_addr = CURRENT_STATE.ID_EX_NPC;
			PC_addr = PC_addr & 0xf0000000; //PC[31:28]
			CURRENT_STATE.JUMP_PC = PC_addr + MEM_TEXT_START + CURRENT_STATE.ID_EX_DEST * 4;

			break;
		case 0x3:		//JAL
			//printf("JAL :R[31]=PC+4, J to 0x%x(imm)*4 \n", TARGET(instr));
			PC_addr = CURRENT_STATE.ID_EX_NPC;
			PC_addr = PC_addr & 0xf0000000; //PC[31:28]
			CURRENT_STATE.JUMP_PC = PC_addr + MEM_TEXT_START + CURRENT_STATE.ID_EX_DEST * 4;
			break;

		default:
			//printf("ERROR: Check process_instruction() TYPE I,, J opcode\n");
			//RUN_BIT = FALSE;
			break;
		}
	}

	// Save result in registers
	CURRENT_STATE.EX_MEM_NPC = CURRENT_STATE.PIPE[EX_STAGE];
}

// Access memory operand 
void MEM_Stage() {

	// MEM_STAGE is locked
	if (CURRENT_STATE.REGS_LOCK[MEM_STAGE]) {
		CURRENT_STATE.REGS_LOCK[MEM_STAGE] = FALSE;
		//CURRENT_STATE.PIPE[MEM_STAGE] = 0;
		//CURRENT_STATE.MEM_WB_NPC = 0;
		return;
	}

	if (!CURRENT_STATE.EX_MEM_NPC) {
		// pass NOP to next stage

		CURRENT_STATE.PIPE[MEM_STAGE] = 0;
		CURRENT_STATE.MEM_WB_NPC = 0;
		return;
	}


	CURRENT_STATE.PIPE[MEM_STAGE] = CURRENT_STATE.EX_MEM_NPC;

	CURRENT_STATE.MEM_WB_OPCODE = CURRENT_STATE.EX_MEM_OPCODE;
	CURRENT_STATE.MEM_WB_FUNCT = CURRENT_STATE.EX_MEM_FUNCT;

	
	// Access Memory data

	if (CURRENT_STATE.EX_MEM_OPCODE == 0x0) {
		// TYPE R
		CURRENT_STATE.MEM_WB_RD = CURRENT_STATE.EX_MEM_RD; // pass RD to WB

		switch (CURRENT_STATE.EX_MEM_FUNCT) { 
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
			CURRENT_STATE.PC = CURRENT_STATE.JUMP_PC;
			// BRANCH!!!
			break;

		default:
			//RUN_BIT = FALSE;
			break;
		}
	}
	else {
		CURRENT_STATE.MEM_WB_RD = CURRENT_STATE.EX_MEM_RD; // pass RT to WB

		switch (CURRENT_STATE.EX_MEM_OPCODE) {
			// TYPE I
		case 0x9:		//(0x001001)ADDIU
		case 0xc:		//(0x001100)ANDI
		case 0xd:		//(0x001101)ORI
		case 0xb:		//(0x001011)SLTIU
		case 0xf:		//(0x001111)LUI, Load Upper Imm.
			CURRENT_STATE.MEM_WB_ALU_OUT = CURRENT_STATE.EX_MEM_ALU_OUT;
			break;

		case 0x23:		//(0x100011)LW
			CURRENT_STATE.MEM_WB_MEM_OUT = mem_read_32(CURRENT_STATE.EX_MEM_ALU_OUT);

			break;
		case 0x2b:		//(0x101011)SW
			mem_write_32(CURRENT_STATE.EX_MEM_ALU_OUT, CURRENT_STATE.ID_EX_RT); // 고쳐야 함. 그런가?

			break;

		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE
			CURRENT_STATE.PC = CURRENT_STATE.EX_MEM_BR_TARGET;
			break;

			// TYPE J
		case 0x2:		//J
			CURRENT_STATE.PC = CURRENT_STATE.JUMP_PC;

			break;
		case 0x3:		//JAL
			CURRENT_STATE.REGS[31] = CURRENT_STATE.EX_MEM_NPC + 8;
			CURRENT_STATE.PC = CURRENT_STATE.JUMP_PC;
			//printf("MEM: JAL to 0x%x\n",PC);
			break;

		default:
			//RUN_BIT = FALSE;
			break;
		}
	}
	CURRENT_STATE.MEM_WB_NPC = CURRENT_STATE.PIPE[MEM_STAGE];
}

// Write result back to register
void WB_Stage() {
	if (!CURRENT_STATE.MEM_WB_NPC) {
		CURRENT_STATE.PIPE[WB_STAGE] = 0;
		return;
	}

	// WB_STAGE is locked
	if (CURRENT_STATE.REGS_LOCK[WB_STAGE]) {
		CURRENT_STATE.REGS_LOCK[WB_STAGE] = FALSE;
		return;
	}

	CURRENT_STATE.PIPE[WB_STAGE] = CURRENT_STATE.MEM_WB_NPC;

	if (CURRENT_STATE.MEM_WB_OPCODE == 0x0) {
		// TYPE R

		switch (CURRENT_STATE.MEM_WB_FUNCT) {
		case 0x21:	// ADD U
		case 0x24:	// AND
		case 0x27:	// NOR
		case 0x25:	// OR
		case 0x2b:	// SLT U
		case 0x00:	// SLL
		case 0x02:	// SRL
		case 0x23:	// SUB U
			CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_RD] = CURRENT_STATE.MEM_WB_ALU_OUT;
			printf("WB Type R: reg[%d] is now 0x%x \n", CURRENT_STATE.MEM_WB_RD, CURRENT_STATE.MEM_WB_ALU_OUT);
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
			CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_RD] = CURRENT_STATE.MEM_WB_ALU_OUT;
			printf("WB Type I: reg[%d] is now 0x%x \n", CURRENT_STATE.MEM_WB_RD, CURRENT_STATE.MEM_WB_ALU_OUT);
			break;

		case 0x23:		//(0x100011)LW
			CURRENT_STATE.REGS[CURRENT_STATE.MEM_WB_RD] = CURRENT_STATE.MEM_WB_MEM_OUT;
			printf("(WB) LW: reg[%d] is now 0x%x \n", CURRENT_STATE.MEM_WB_RD, CURRENT_STATE.MEM_WB_MEM_OUT);

			break;
		case 0x2b:		//(0x101011)SW
		case 0x4:		//(0x000100)BEQ
		case 0x5:		//(0x000101)BNE
		case 0x2:		//J
		case 0x3:		//JAL
			// Nothing to do with registers
			break;
		default:
			//RUN_BIT = FALSE;
			break;
		}
	}

	INSTRUCTION_COUNT++;
}

