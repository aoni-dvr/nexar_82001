
import numpy as np
import argparse
import struct
import cv2


# image: last image
# optical_flow_diff_image: vector from last to current
# optical_flow_diff_image[y][x][0] = x vector
# optical_flow_diff_image[y][x][1] = y vector
def put_optical_flow_arrows_on_image(image, optical_flow_diff_image, threshold=2.0, skip_amount=2, arrow_scale=1):

    image = image.copy()
    
    # Turn grayscale to rgb if needed
    if len(image.shape) == 2:
        image = np.stack((image,)*3, axis=2)
    
    # A = np.meshgrid(range(3), range(5)) -> produce 2 2D index map
    # A[0] = array([[0, 1, 2],
    #       [0, 1, 2],
    #       [0, 1, 2],
    #       [0, 1, 2],
    #       [0, 1, 2]])
    # A[1] = array([[0, 0, 0],
    #       [1, 1, 1],
    #       [2, 2, 2],
    #       [3, 3, 3],
    #       [4, 4, 4]])

    # Get start and end coordinates of the optical flow
    # produce index map, 
    # flow_start[3][1] = array([1, 3])
    flow_start = np.stack(np.meshgrid(range(optical_flow_diff_image.shape[1]), range(optical_flow_diff_image.shape[0])), 2)
    flow_end = (optical_flow_diff_image[flow_start[:,:,1],flow_start[:,:,0],:2]*arrow_scale + flow_start).astype(np.int32)
    
    # Threshold values
    # norm = sqrt( x^2 +  y^2 )
    norm = np.linalg.norm(flow_end - flow_start, axis=2)
    norm[norm < threshold] = 0
    
    # Draw all the nonzero values
    nz = np.nonzero(norm)
    if 0:        
        for i in range(0, len(nz[0]), skip_amount):
            y, x = nz[0][i], nz[1][i]
            cv2.arrowedLine(image,
                            pt1=tuple(flow_start[y,x]), 
                            pt2=tuple(flow_end[y,x]),
                            color=(0, 255, 0), 
                            thickness=1, 
                            tipLength=.2)
    else:
        for i in range(0, len(nz[0])):            
            y, x = nz[0][i], nz[1][i]            
            if (y % skip_amount == 0) and (x % skip_amount == 0):
                #print("pos[{},{}], diff({}, {})".format(x, y, optical_flow_diff_image[y][x][0], optical_flow_diff_image[y][x][1]))
                cv2.arrowedLine(image,
                                pt1=tuple(flow_start[y,x]), 
                                pt2=tuple(flow_end[y,x]),
                                color=(0, 255, 0), 
                                thickness=1, 
                                tipLength=.2)

        
    return image

#Optical flow sets to 16 bpp, qm=5, qf=3 (each component is 5.3, x+y component per pixel)
def optical_flow_hw_fmt_2_sw_fmt(uint16_2D_array, roi_start_x, roi_start_y, of_width, of_height, image_width, image_height):
    uint16_2D_array = uint16_2D_array.copy()    
    uint16_2D_array = np.reshape(uint16_2D_array, [of_height, of_width])

    invalid_x_mask = (uint16_2D_array & 0x00FF) == 0x00FF
    invalid_y_mask = (uint16_2D_array & 0xFF00) == 0xFF00
    invalid_mask = invalid_x_mask | invalid_y_mask    

    print("num of invalid_x = {}".format(sum(sum(invalid_x_mask))))
    print("num of invalid_y = {}".format(sum(sum(invalid_y_mask))))
    print("num of union invalid = {}".format(sum(sum(invalid_mask))))

    '''
    unique, counts = np.unique(uint16_2D_array, return_counts=True)
    #print(dict(zip(unique, counts)))
    stat = dict(zip(unique, counts))
    for key in stat:
        if stat[key] == 31149:
            print(key)
    '''        

    _y = uint16_2D_array & 0xFF00    
    _y >>= 8
    _y = _y.astype(np.uint8)
    _y = _y.view(dtype=np.int8)

    _x = uint16_2D_array & 0x00FF
    _x = _x.astype(np.uint8)
    _x = _x.view(dtype=np.int8)

    _x = _x.astype(np.float64)/8
    _y = _y.astype(np.float64)/8

    _x[invalid_mask] = 0
    _y[invalid_mask] = 0

    x = np.zeros([image_height, image_width])
    y = np.zeros([image_height, image_width])

    x[roi_start_y:(roi_start_y+of_height), roi_start_x:(roi_start_x+of_width)] = _x
    y[roi_start_y:(roi_start_y+of_height), roi_start_x:(roi_start_x+of_width)] = _y    

    # stack along depth axis
    optical_flow_diff_image = np.dstack([x, y])

    return optical_flow_diff_image


CV_FEX_MAX_BUCKETS = 64
CV_FEX_MAX_KEYPOINTS = 32
#typedef struct {
#        UINT32    Reserved        :15;
#        UINT32    ScoreExponent   :5;
#        UINT32    ScoreMantissa   :12;
#        UINT16    X;      // 14.2 format
#        UINT16    Y;      // 14.2 format
#} AMBA_CV_FEX_KEYPOINT_SCORE_s;
#    relative_ptr_t      KeypointsCountOffset;  /* UINT8* pointer to list of valid points for every block_num [CV_FEX_MAX_BUCKETS]*/
#    relative_ptr_t      KeypointsOffset;       /* AMBA_CV_FEX_KEYPOINT_SCORE_s* pointer to starting block [CV_FEX_MAX_BUCKETS][CV_FEX_MAX_KEYPOINTS] */
def harris_corner_hw_fmt_2_sw_fmt(uint8_keypoints_count_array, uint32_keypoints_array, of_start_x, of_start_y):
    uint32_keypoints_array = uint32_keypoints_array.copy()
    uint32_keypoints_array = np.reshape(uint32_keypoints_array, [CV_FEX_MAX_BUCKETS, CV_FEX_MAX_KEYPOINTS, 2])

    collect_corners = []
    collect_corners_exp = []
    collect_corners_mantissa = []
    for bucket_idx in range(CV_FEX_MAX_BUCKETS):
        count_in_bucket = uint8_keypoints_count_array[bucket_idx]
        for corner_idx in range(count_in_bucket):
            _x_y = uint32_keypoints_array[bucket_idx][corner_idx][0]        
            _x = _x_y & 0x0000FFFF
            _y = (_x_y & 0xFFFF0000) >> 16

            x = _x.astype(np.float64)/4
            y = _y.astype(np.float64)/4

            x += of_start_x
            y += of_start_y
            #print("x,y = ({}, {})".format(x, y))
            collect_corners.append([x, y])

            resp = uint32_keypoints_array[bucket_idx][corner_idx][1]
            resp_exp = (resp & 0x1F000)
            resp_exp >>= 12
            
            resp_mantissa = (resp & 0xFFF)

            collect_corners_exp.append(resp_exp)
            collect_corners_mantissa.append(resp_mantissa)
            '''
            # 10
            rad = 5            
            test_x = 31
            test_y = 23
            for i in range(-rad, rad+1):
                for j in range(-rad, rad+1):
                    if (int(x)+j) == test_x and (int(y)+i) == test_y:
                        print("({}, {}), resp_exp = {}".format(x, y, resp_exp))

            # 8
            rad = 5
            test_x = 462
            test_y = 224
            for i in range(-rad, rad+1):
                for j in range(-rad, rad+1):
                    if (int(x)+j) == test_x and (int(y)+i) == test_y:
                        print("({}, {}), resp_exp = {}".format(x, y, resp_exp))

            # 0
            rad = 5
            test_x = 346
            test_y = 166
            for i in range(-rad, rad+1):
                for j in range(-rad, rad+1):
                    if (int(x)+j) == test_x and (int(y)+i) == test_y:
                        print("({}, {}), resp_exp = {}".format(x, y, resp_exp))     

            '''

    #collect_corners = np.array([collect_corners])
    
    return collect_corners, collect_corners_exp, collect_corners_mantissa


def put_corner_on_image(image, corners, score_exp_list, score_mantissa_list, dot_size = 3):
    draw_image = image.copy()
    
    # Turn grayscale to rgb if needed
    if len(image.shape) == 2:
        draw_image = np.stack((draw_image,)*3, axis=2)

    '''
    # for verifying mantissa
    max_score_mantissa = 0
    for (corner, score_exp, score_mantissa) in zip(corners, score_exp_list, score_mantissa_list):
        if score_exp == 0:
            if max_score_mantissa < score_mantissa:
                max_score_mantissa = score_mantissa

    for (corner, score_exp, score_mantissa) in zip(corners, score_exp_list, score_mantissa_list):
        if score_exp == 0:
            if score_mantissa <= max_score_mantissa/3:
                # blue for low
                cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (255,0,0),-1)
            elif score_mantissa >= max_score_mantissa/3 and score_mantissa < max_score_mantissa*2/3:
                # green for med
                cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (0,255,0),-1)
            else:
                # red for large
                cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (0,0,255),-1)

    '''
    
    '''
    # for verifying exp
    for (corner, score_exp, score_mantissa) in zip(corners, score_exp_list, score_mantissa_list):
        if score_exp <= 2:
            # blue for low
            cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (255,0,0),-1)
        elif score_exp >= 3 and score_exp <= 4:
            # green for med
            cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (0,255,0),-1)
        elif score_exp >= 5:
            # red for large
            cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (0,0,255),-1)
        else:
            print("exceptional exp??")
    '''

    for corner in corners:
        cv2.circle(draw_image,(int(corner[0]), int(corner[1])), dot_size, (0,255,0),-1)
    return draw_image    


scale_index = 0
image_width = 640
image_pitch = 640
image_height = 480
of_start_x = 0
of_start_y = 0
of_width = 640
of_height = 480
    

input_dir = "./input"
last_img_name = "{}/old_{}x{}.y".format(input_dir, image_width, image_height)
current_img_name = "{}/cur_{}x{}.y".format(input_dir, image_width, image_height)
of_name = "./output/th_dx_dy_out.bin"
of_invalid_cnt_name = "./output/invalid_count_out.bin"

'''
primary_keypoints_count_name = "{}/PrimaryKeypointsCount_scale{}.bin".format(input_dir, scale_index)
primary_keypoints_name = "{}/PrimaryKeypoints_scale{}.bin".format(input_dir, scale_index)
'''
secondary_keypoints_count_name = "/dump16/hyyu/LinuxTest/HarrisCorner/key_point_count.bin"
secondary_keypoints_name = "/dump16/hyyu/LinuxTest/HarrisCorner/key_point.bin"


last_image = np.fromfile(last_img_name, dtype=np.uint8)
current_image = np.fromfile(current_img_name, dtype=np.uint8)
of = np.fromfile(of_name, dtype=np.uint16)
of_invalid_cnt = np.fromfile(of_invalid_cnt_name, dtype=np.uint16)

'''
primary_keypoints_count = np.fromfile(primary_keypoints_count_name, dtype=np.uint8)
primary_keypoints = np.fromfile(primary_keypoints_name, dtype=np.uint32)
'''
secondary_keypoints_count = np.fromfile(secondary_keypoints_count_name, dtype=np.uint8)
secondary_keypoints = np.fromfile(secondary_keypoints_name, dtype=np.uint32)

last_image_gray = np.reshape(last_image, [image_height, image_pitch])
last_image_RGB = cv2.cvtColor(last_image_gray, cv2.COLOR_GRAY2RGB)
current_image_gray = np.reshape(current_image, [image_height, image_pitch])
current_image_RGB = cv2.cvtColor(current_image_gray, cv2.COLOR_GRAY2RGB)


optical_flow_diff_image = optical_flow_hw_fmt_2_sw_fmt(of, of_start_x, of_start_y, of_width, of_height, image_width, image_height)
opk_image = put_optical_flow_arrows_on_image(last_image_RGB, optical_flow_diff_image, threshold=0.0001, skip_amount=5, arrow_scale=5)
cv2.imwrite("opk_image_scale{}.jpg".format(scale_index), opk_image)
print("invalid count of optical flow from VP:", sum(of_invalid_cnt))

'''
# last, right
corners = harris_corner_hw_fmt_2_sw_fmt(primary_keypoints_count, primary_keypoints, of_start_x, of_start_y)
corner_image = put_corner_on_image(last_image_RGB, corners, dot_size = 1)
cv2.imwrite("corner_image_scale{}_r.jpg".format(scale_index), corner_image)
'''
# current, left
if 1:
    corners, score_exp, score_mantissa = harris_corner_hw_fmt_2_sw_fmt(secondary_keypoints_count, secondary_keypoints, of_start_x, of_start_y)
    corner_image = put_corner_on_image(current_image_RGB, corners, score_exp, score_mantissa, dot_size = 3)
    cv2.imwrite("corner_image_scale{}_l.jpg".format(scale_index), corner_image)
