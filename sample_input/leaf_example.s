	.data
data1:	.word	1
data2:	.word	2
data3:	.word	3
data4:	.word	4
	.text
main:
	la	$4, data1	# 00 lui $0 $4 0x1000 /data1
	lw	$5, 4($4)	# 04	/2
	lw	$6, 8($4)	# 08	/3
	lw	$7, 12($4)	# 0c	/4
	lw	$4, 0($4)	# 10	/1

	addu	$8, $4, $5	# 14	/1+2 = 3
	addu	$9, $6, $7	# 18	/3+4 = 7
	subu	$16, $9, $8	# 1c	/7-3 = 4
	addu	$2, $16, $0	# 20	/4+0 = 4