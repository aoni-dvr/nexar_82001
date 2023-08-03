ld out/vas_output/main.vdg

lb disparity_in input/height_seg_in_0.raw
lb bottom_pos input/height_seg_in_1.raw
lb delta_disp_table input/height_seg_in_2.raw
lb filter_disparity input/height_seg_in_3.raw

run

sb min_height_score_pos_out output/min_height_score_pos_out.bin
sb stixel_avg_disparity_out output/stixel_avg_disparity_out.bin

#printall

