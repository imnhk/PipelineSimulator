	.data
	.text
main:
        addiu   $8, $8, 0	/0
        addiu   $9, $9, 0	/4

loop:
        addu    $9, $9, $8	/8
        addiu   $8, $8, 1	/c
        sltiu   $7, $8, 15	/10
        bne     $7, $0, loop/14

        addiu   $10, $10, 0	/18
        addiu   $11, $11, 0	/1c
        addiu   $12, $12, 0	/20
        addiu   $13, $13, 0	/24
