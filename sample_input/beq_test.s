	.data
	.text
main:
        addiu   $8, $8, 0
        addiu   $9, $9, 0

loop:
        addu    $9, $9, $8
        addiu   $8, $8, 1
        sltiu   $7, $8, 15
        bne     $7, $0, loop

        addiu   $10, $10, 0
        addiu   $11, $11, 0
        addiu   $12, $12, 0
        addiu   $13, $13, 0
