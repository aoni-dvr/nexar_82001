ld out/vas_output/main.vdg

lb in_disparity input/free_space_in_0.raw
lb in_esti_ground_disparity input/free_space_in_1.raw
lb in_obj_top_x_index input/free_space_in_2.raw
lb in_ground_disp_coef input/free_space_in_3.raw

run

sb min_node_score_pos_out output/min_node_score_pos_out.bin
sb confidence_map_out output/confidence_map_out.bin

#printall

