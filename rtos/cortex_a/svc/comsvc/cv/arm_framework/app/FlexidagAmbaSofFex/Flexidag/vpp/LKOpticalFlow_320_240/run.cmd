ld out/vas_output/optlk.vdg
lb img0 input/old_320x240.y
lb img1 input/cur_320x240.y
run

sb th_dx_dy_reshape_out output/th_dx_dy_out.bin
sb invalid_count_out output/invalid_count_out.bin

#printall

