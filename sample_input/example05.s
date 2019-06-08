	.data
data1:	.word	3
	.word	4096
	.word	0xf4c2a
data2:	.word	2147483646
data3:	.word	2147483647
	.text
main:
	la	$15, data2			/0
							/4
	addiu	$12, $12, -100	/8
	addiu	$14, $14, -1	/c
	addiu	$13, $13, 0xffff/10
	addu	$2, $4, $5		/14
	addu	$2, $6, $7		/18
	subu	$9, $3, $2		/1c
cs311:
	and	$11, $11, $0		/20
	addiu	$10, $10, 0x1	/24
	or	$6, $6, $0			/28
	jal	cs312				/2c
	addu	$0, $0, $0		/30
cs312:						
	lw	$6, 4($15)			/34
	sll	$7, $6, 2			/38
	sw	$7, 8($15)			/3c
	srl	$5, $4, 2			/40
	nor	$5, $5, $0			/44
	sltiu	$9, $10, -4		/48
	beq	$9, $0, cs313		/4c
	jr	$31					/50
cs313:						
	lui	$17, 0x1000			/54
	lw	$18, 0($17)			/58
	sltu	$4, $2, $3		/5c
	bne	$4, $0, cs314		/60
	j	cs311				/64
cs314:
	ori	$16, $16, 0xf0f0
