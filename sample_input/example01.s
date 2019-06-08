	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.text
main:
	and	$17, $17, $0	/0
	and	$18, $18, $0	/4
	la	$8, data1		/8
	la	$9, data2		/c
						/10
	and	$10, $10, $0	/14
lab1:
	and	$11, $11, $0	/18
lab2:
	addiu	$17, $17, 0x1	/1c
	addiu	$11, $11, 0x1	/20
	or	$9, $9, $0			/24
	bne	$11, $8, lab2		/28
lab3:
	addiu	$18, $18, 0x2	/2c
	addiu	$11, $11, 1		/30
	sll	$18, $17, 1			/34
	srl	$17, $18, 1			/38
	and	$19, $17, $18		/3c
	bne	$11, $9, lab3		/40
lab4:
	addu	$5, $5, $31		
	nor	$16, $17, $18
	beq	$10, $8, lab5
	j	lab1
lab5:
	ori	$16, $16, 0xf0f0
