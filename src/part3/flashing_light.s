ldr w1, selected_pin
ldr w4, selected_pin
ldr w3, set_pin_output_base
ldr w6, set_pin_base
ldr w7, clear_pin_base
ldr w10, sleep_count
movz w5, #1
cmp w4, #10
b.lt get_right_value_in_output
get_right_output_reg:
add w3, w3, #4
sub w4, w4, #10
cmp w4, #10
b.ge get_right_output_reg
get_right_value_in_output:
subs w4, w4, #1
b.lt set_out
orr w5, wzr, w5, lsl #3
b get_right_value_in_output
set_out:
str w5, [w3] 
cmp w1, #32
b.lt skip_add_offset
add w6, w6, #4
add w7, w7, #4
sub w1, w1, #32
skip_add_offset:
movz w5, #1 
set_set_and_clear_mask:
subs w1, w1, #1
b.lt main_loop
orr w5, wzr, w5, lsl #1
b set_set_and_clear_mask

main_loop:
str wzr, [w7]
str w5, [w6]
movz w8, #0
sleep_set:
and w0, w0, w0 
add w8, w8, #1
cmp w8, w10
b.lt sleep_set
str wzr, [w6]
str w5, [w7]
movz w8, #0
sleep_clear:
and w0, w0, w0
add w8, w8, #1
cmp w8, w10
b.lt sleep_clear
b main_loop

set_pin_base:
.int 0x3f20001c

clear_pin_base:
.int 0x3f200028

set_pin_output_base:
.int 0x3f200000

selected_pin:
.int 17

sleep_count:
.int 1000000
