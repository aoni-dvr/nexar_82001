ld out/vas_output/main.vdg

lb cfg_f32 input/gen_esti_road_in_0.raw

run

sb esti_road_disparity_out output/esti_road_disparity_out.bin
sb virtual_obj_top_v_out output/virtual_obj_top_v_out.bin
sb road_disp_coef_out output/road_disp_coef_out.bin
sb deleta_disparity_table_out output/deleta_d_table_out.bin

#printall

