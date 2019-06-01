	.data
	.text
main:
        addiu   $8, $8, 0x10        #00
        jal     target              #04
        addu    $0, $0, $0          #08
        addiu   $8, $8, 0x20        #0c
        j       end                 #10

target:
        addiu   $9, $9, 0x30        #14
        addiu   $10, $10, 0x40      #18
        jr      $31                 #1c
end:
        addiu   $15, $15, 0x11      #20
