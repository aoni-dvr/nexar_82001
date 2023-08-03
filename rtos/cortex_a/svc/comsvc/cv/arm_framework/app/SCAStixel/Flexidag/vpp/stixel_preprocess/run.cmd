ld out/vas_output/main.vdg

lb disparity_in input/preprocess_in_0.raw

run

sb transposed_maxpooling_dispairty_out output/transposed_maxpooling_dispairty_out.raw

#printall

