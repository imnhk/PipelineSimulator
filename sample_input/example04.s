	.data
	.text
main:
	addu	$2, $4, $5		/0
	addu	$2, $6, $7		/4
	subu	$9, $3, $2		/8
lab1:
	and	$11, $11, $0		/c
	addiu	$10, $10, 0x1	/10
	or	$6, $6, $0			/14
	jal	lab3				/18
	addu	$0, $0, $0		/1c
lab3:
	sll	$7, $6, 2			/20
	srl	$5, $4, 2			/24
	sltiu	$9, $10, 100	/28
	beq	$9, $0, lab4		/2c
	jr	$31
lab4:
	sltu	$4, $2, $3		/30
	bne	$4, $0, lab5		/34
	j	lab1				/38
lab5:
	ori	$16, $16, 0xf0f0	/3c
