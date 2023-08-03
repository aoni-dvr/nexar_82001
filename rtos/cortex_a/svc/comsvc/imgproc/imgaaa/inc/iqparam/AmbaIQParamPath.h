/**
 * @file AmbaIQParamPath.h
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *  @Description    :: Implementation of IQ param handler
 *
 *
 */
#ifndef AMBA_IQPARAM_PATH_H__
#define AMBA_IQPARAM_PATH_H__

#include "AmbaIQParamHandlerSample.h"

static IQ_TABLE_PATH_s IMX377Path = {
                            .ADJTable = {"adj_table_param_default_IMX377"},
                            .ImgParam = {"img_default_IMX377"},
                            .aaaDefault = {{"aaa_default_00_IMX377"}, {"aaa_default_01_IMX377"}},
                            .video = {{"adj_video_default_00_IMX377"}, {"adj_video_default_01_IMX377"}, {"adj_video_default_02_IMX377"}},
                            .photo = {{"adj_photo_default_00_IMX377"}, {"adj_photo_default_01_IMX377"}},
                            .stillLISO = {{"adj_still_default_00_IMX377"}, {"adj_still_default_01_IMX377"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX377"}},
                            .stillIdxInfo = {"adj_still_idx_IMX377"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX377"}, {"VideoCc1_IMX377"}, {"VideoCc2_IMX377"}, {"VideoCc3_IMX377"}, {"VideoCc4_IMX377"}},
                                             .stillCC = {{"StillCc0_IMX377"}, {"StillCc1_IMX377"}, {"StillCc2_IMX377"}, {"StillCc3_IMX377"}, {"StillCc4_IMX377"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX377"}, {"cc3d_cc_bw_gamma_lin_video_IMX377"}, {"cc3d_cc_bw_gamma_lin_video_IMX377"}, {"cc3d_cc_bw_gamma_lin_video_IMX377"}, {"cc3d_cc_bw_gamma_lin_video_IMX377"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX377"}, {"cc3d_cc_bw_gamma_lin_still_IMX377"}, {"cc3d_cc_bw_gamma_lin_still_IMX377"}, {"cc3d_cc_bw_gamma_lin_still_IMX377"}, {"cc3d_cc_bw_gamma_lin_still_IMX377"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX377"}, {"scene_data_s02_IMX377"}, {"scene_data_s03_IMX377"}, {"scene_data_s04_IMX377"}, {"scene_data_s05_IMX377"}},
                            .DEVideo = {{"de_default_video_IMX377"}},
                            .DEStill = {{"de_default_still_IMX377"}},
                        };

static IQ_TABLE_PATH_s IMX183Path = {
                            .ADJTable = {"adj_table_param_default_IMX183"},
                            .ImgParam = {"img_default_IMX183"},
                            .aaaDefault = {{"aaa_default_00_IMX183"}, {"aaa_default_01_IMX183"}},
                            .video = {{"adj_video_default_00_IMX183"}, {"adj_video_default_01_IMX183"}, {"adj_video_default_02_IMX183"}},
                            .photo = {{"adj_photo_default_00_IMX183"}, {"adj_photo_default_01_IMX183"}},
                            .stillLISO = {{"adj_still_default_00_IMX183"}, {"adj_still_default_01_IMX183"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX183"}},
                            .stillIdxInfo = {"adj_still_idx_IMX183"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX183"}, {"VideoCc1_IMX183"}, {"VideoCc2_IMX183"}, {"VideoCc3_IMX183"}, {"VideoCc4_IMX183"}},
                                             .stillCC = {{"StillCc0_IMX183"}, {"StillCc1_IMX183"}, {"StillCc2_IMX183"}, {"StillCc3_IMX183"}, {"StillCc4_IMX183"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX183"}, {"cc3d_cc_bw_gamma_lin_video_IMX183"}, {"cc3d_cc_bw_gamma_lin_video_IMX183"}, {"cc3d_cc_bw_gamma_lin_video_IMX183"}, {"cc3d_cc_bw_gamma_lin_video_IMX183"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX183"}, {"cc3d_cc_bw_gamma_lin_still_IMX183"}, {"cc3d_cc_bw_gamma_lin_still_IMX183"}, {"cc3d_cc_bw_gamma_lin_still_IMX183"}, {"cc3d_cc_bw_gamma_lin_still_IMX183"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX183"}, {"scene_data_s02_IMX183"}, {"scene_data_s03_IMX183"}, {"scene_data_s04_IMX183"}, {"scene_data_s05_IMX183"}},
                            .DEVideo = {{"de_default_video_IMX183"}},
                            .DEStill = {{"de_default_still_IMX183"}},
                        };

// static IQ_TABLE_PATH_s IMX334Path = {
//                             .ADJTable = {"adj_table_param_default_IMX334"},
//                             .ImgParam = {"img_default_IMX334"},
//                             .aaaDefault = {{"aaa_default_00_IMX334"}, {"aaa_default_01_IMX334"}},
//                             .video = {{"adj_video_default_00_IMX334"}, {"adj_video_default_01_IMX334"}, {"adj_video_default_02_IMX334"}},
//                             .photo = {{"adj_photo_default_00_IMX334"}, {"adj_photo_default_01_IMX334"}},
//                             .stillLISO = {{"adj_still_default_00_IMX334"}, {"adj_still_default_01_IMX334"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_IMX334"}},
//                             .stillIdxInfo = {"adj_still_idx_IMX334"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_IMX334"}, {"VideoCc1_IMX334"}, {"VideoCc2_IMX334"}, {"VideoCc3_IMX334"}, {"VideoCc4_IMX334"}},
//                                              .stillCC = {{"StillCc0_IMX334"}, {"StillCc1_IMX334"}, {"StillCc2_IMX334"}, {"StillCc3_IMX334"}, {"StillCc4_IMX334"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX334"}, {"cc3d_cc_bw_gamma_lin_video_IMX334"}, {"cc3d_cc_bw_gamma_lin_video_IMX334"}, {"cc3d_cc_bw_gamma_lin_video_IMX334"}, {"cc3d_cc_bw_gamma_lin_video_IMX334"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX334"}, {"cc3d_cc_bw_gamma_lin_still_IMX334"}, {"cc3d_cc_bw_gamma_lin_still_IMX334"}, {"cc3d_cc_bw_gamma_lin_still_IMX334"}, {"cc3d_cc_bw_gamma_lin_still_IMX334"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_IMX334"}, {"scene_data_s02_IMX334"}, {"scene_data_s03_IMX334"}, {"scene_data_s04_IMX334"}, {"scene_data_s05_IMX334"}},
//                             .DEVideo = {{"de_default_video_IMX334"}},
//                             .DEStill = {{"de_default_still_IMX334"}},
//                         };

static IQ_TABLE_PATH_s AR0144Path = {
                            .ADJTable = {"adj_table_param_default_AR0144"},
                            .ImgParam = {"img_default_AR0144"},
                            .aaaDefault = {{"aaa_default_00_AR0144"}, {"aaa_default_01_AR0144"}},
                            .video = {{"adj_video_default_00_AR0144"}, {"adj_video_default_01_AR0144"}, {"adj_video_default_02_AR0144"}},
                            .photo = {{"adj_photo_default_00_AR0144"}, {"adj_photo_default_01_AR0144"}},
                            .stillLISO = {{"adj_still_default_00_AR0144"}, {"adj_still_default_01_AR0144"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0144"}},
                            .stillIdxInfo = {"adj_still_idx_AR0144"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0144"}, {"VideoCc1_AR0144"}, {"VideoCc2_AR0144"}, {"VideoCc3_AR0144"}, {"VideoCc4_AR0144"}},
                                             .stillCC = {{"StillCc0_AR0144"}, {"StillCc1_AR0144"}, {"StillCc2_AR0144"}, {"StillCc3_AR0144"}, {"StillCc4_AR0144"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0144"}, {"cc3d_cc_bw_gamma_lin_video_AR0144"}, {"cc3d_cc_bw_gamma_lin_video_AR0144"}, {"cc3d_cc_bw_gamma_lin_video_AR0144"}, {"cc3d_cc_bw_gamma_lin_video_AR0144"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0144"}, {"cc3d_cc_bw_gamma_lin_still_AR0144"}, {"cc3d_cc_bw_gamma_lin_still_AR0144"}, {"cc3d_cc_bw_gamma_lin_still_AR0144"}, {"cc3d_cc_bw_gamma_lin_still_AR0144"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0144"}, {"scene_data_s02_AR0144"}, {"scene_data_s03_AR0144"}, {"scene_data_s04_AR0144"}, {"scene_data_s05_AR0144"}},
                            .DEVideo = {{"de_default_video_AR0144"}},
                            .DEStill = {{"de_default_still_AR0144"}},
                        };

// static IQ_TABLE_PATH_s OV9716Path = {
//                             .ADJTable = {"adj_table_param_default_OV9716"},
//                             .ImgParam = {"img_default_OV9716"},
//                             .aaaDefault = {{"aaa_default_00_OV9716"}, {"aaa_default_01_OV9716"}},
//                             .video = {{"adj_video_default_00_OV9716"}, {"adj_video_default_01_OV9716"}, {"adj_video_default_02_OV9716"}},
//                             .photo = {{"adj_photo_default_00_OV9716"}, {"adj_photo_default_01_OV9716"}},
//                             .stillLISO = {{"adj_still_default_00_OV9716"}, {"adj_still_default_01_OV9716"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_OV9716"}},
//                            .stillIdxInfo = {"adj_still_idx_OV9716"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_OV9716"}, {"VideoCc1_OV9716"}, {"VideoCc2_OV9716"}, {"VideoCc3_OV9716"}, {"VideoCc4_OV9716"}},
//                                              .stillCC = {{"StillCc0_OV9716"}, {"StillCc1_OV9716"}, {"StillCc2_OV9716"}, {"StillCc3_OV9716"}, {"StillCc4_OV9716"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV9716"}, {"cc3d_cc_bw_gamma_lin_video_OV9716"}, {"cc3d_cc_bw_gamma_lin_video_OV9716"}, {"cc3d_cc_bw_gamma_lin_video_OV9716"}, {"cc3d_cc_bw_gamma_lin_video_OV9716"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV9716"}, {"cc3d_cc_bw_gamma_lin_still_OV9716"}, {"cc3d_cc_bw_gamma_lin_still_OV9716"}, {"cc3d_cc_bw_gamma_lin_still_OV9716"}, {"cc3d_cc_bw_gamma_lin_still_OV9716"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_OV9716"}, {"scene_data_s02_OV9716"}, {"scene_data_s03_OV9716"}, {"scene_data_s04_OV9716"}, {"scene_data_s05_OV9716"}},
//                             .DEVideo = {{"de_default_video_OV9716"}},
//                             .DEStill = {{"de_default_still_OV9716"}},
//                         };

static IQ_TABLE_PATH_s IMX290Path = {
                            .ADJTable = {"adj_table_param_default_IMX290"},
                            .ImgParam = {"img_default_IMX290"},
                            .aaaDefault = {{"aaa_default_00_IMX290"}, {"aaa_default_01_IMX290"}},
                            .video = {{"adj_video_default_00_IMX290"}, {"adj_video_default_01_IMX290"}, {"adj_video_default_02_IMX290"}},
                            .photo = {{"adj_photo_default_00_IMX290"}, {"adj_photo_default_01_IMX290"}},
                            .stillLISO = {{"adj_still_default_00_IMX290"}, {"adj_still_default_01_IMX290"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX290"}},
                            .stillIdxInfo = {"adj_still_idx_IMX290"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX290"}, {"VideoCc1_IMX290"}, {"VideoCc2_IMX290"}, {"VideoCc3_IMX290"}, {"VideoCc4_IMX290"}},
                                             .stillCC = {{"StillCc0_IMX290"}, {"StillCc1_IMX290"}, {"StillCc2_IMX290"}, {"StillCc3_IMX290"}, {"StillCc4_IMX290"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX290"}, {"cc3d_cc_bw_gamma_lin_video_IMX290"}, {"cc3d_cc_bw_gamma_lin_video_IMX290"}, {"cc3d_cc_bw_gamma_lin_video_IMX290"}, {"cc3d_cc_bw_gamma_lin_video_IMX290"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX290"}, {"cc3d_cc_bw_gamma_lin_still_IMX290"}, {"cc3d_cc_bw_gamma_lin_still_IMX290"}, {"cc3d_cc_bw_gamma_lin_still_IMX290"}, {"cc3d_cc_bw_gamma_lin_still_IMX290"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX290"}, {"scene_data_s02_IMX290"}, {"scene_data_s03_IMX290"}, {"scene_data_s04_IMX290"}, {"scene_data_s05_IMX290"}},
                            .DEVideo = {{"de_default_video_IMX290"}},
                            .DEStill = {{"de_default_still_IMX290"}},
                        };

static IQ_TABLE_PATH_s IMX577Path = {
                            .ADJTable = {"adj_table_param_default_IMX577"},
                            .ImgParam = {"img_default_IMX577"},
                            .aaaDefault = {{"aaa_default_00_IMX577"}, {"aaa_default_01_IMX577"}},
                            .video = {{"adj_video_default_00_IMX577"}, {"adj_video_default_01_IMX577"}, {"adj_video_default_02_IMX577"}},
                            .photo = {{"adj_photo_default_00_IMX577"}, {"adj_photo_default_01_IMX577"}},
                            .stillLISO = {{"adj_still_default_00_IMX577"}, {"adj_still_default_01_IMX577"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX577"}},
                            .stillIdxInfo = {"adj_still_idx_IMX577"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX577"}, {"VideoCc1_IMX577"}, {"VideoCc2_IMX577"}, {"VideoCc3_IMX577"}, {"VideoCc4_IMX577"}},
                                             .stillCC = {{"StillCc0_IMX577"}, {"StillCc1_IMX577"}, {"StillCc2_IMX577"}, {"StillCc3_IMX577"}, {"StillCc4_IMX577"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX577"}, {"cc3d_cc_bw_gamma_lin_video_IMX577"}, {"cc3d_cc_bw_gamma_lin_video_IMX577"}, {"cc3d_cc_bw_gamma_lin_video_IMX577"}, {"cc3d_cc_bw_gamma_lin_video_IMX577"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX577"}, {"cc3d_cc_bw_gamma_lin_still_IMX577"}, {"cc3d_cc_bw_gamma_lin_still_IMX577"}, {"cc3d_cc_bw_gamma_lin_still_IMX577"}, {"cc3d_cc_bw_gamma_lin_still_IMX577"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX577"}, {"scene_data_s02_IMX577"}, {"scene_data_s03_IMX577"}, {"scene_data_s04_IMX577"}, {"scene_data_s05_IMX577"}},
                            .DEVideo = {{"de_default_video_IMX577"}},
                            .DEStill = {{"de_default_still_IMX577"}},
                        };
static IQ_TABLE_PATH_s IMX586Path = {
                            .ADJTable = {"adj_table_param_default_IMX586"},
                            .ImgParam = {"img_default_IMX586"},
                            .aaaDefault = {{"aaa_default_00_IMX586"}, {"aaa_default_01_IMX586"}},
                            .video = {{"adj_video_default_00_IMX586"}, {"adj_video_default_01_IMX586"}, {"adj_video_default_02_IMX586"}},
                            .photo = {{"adj_photo_default_00_IMX586"}, {"adj_photo_default_01_IMX586"}},
                            .stillLISO = {{"adj_still_default_00_IMX586"}, {"adj_still_default_01_IMX586"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX586"}},
                            .stillIdxInfo = {"adj_still_idx_IMX586"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX586"}, {"VideoCc1_IMX586"}, {"VideoCc2_IMX586"}, {"VideoCc3_IMX586"}, {"VideoCc4_IMX586"}},
                                             .stillCC = {{"StillCc0_IMX586"}, {"StillCc1_IMX586"}, {"StillCc2_IMX586"}, {"StillCc3_IMX586"}, {"StillCc4_IMX586"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX586"}, {"cc3d_cc_bw_gamma_lin_video_IMX586"}, {"cc3d_cc_bw_gamma_lin_video_IMX586"}, {"cc3d_cc_bw_gamma_lin_video_IMX586"}, {"cc3d_cc_bw_gamma_lin_video_IMX586"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX586"}, {"cc3d_cc_bw_gamma_lin_still_IMX586"}, {"cc3d_cc_bw_gamma_lin_still_IMX586"}, {"cc3d_cc_bw_gamma_lin_still_IMX586"}, {"cc3d_cc_bw_gamma_lin_still_IMX586"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX586"}, {"scene_data_s02_IMX586"}, {"scene_data_s03_IMX586"}, {"scene_data_s04_IMX586"}, {"scene_data_s05_IMX586"}},
                            .DEVideo = {{"de_default_video_IMX586"}},
                            .DEStill = {{"de_default_still_IMX586"}},
                        };

static IQ_TABLE_PATH_s IMX455Path = {
                            .ADJTable = {"adj_table_param_default_IMX455"},
                            .ImgParam = {"img_default_IMX455"},
                            .aaaDefault = {{"aaa_default_00_IMX455"}, {"aaa_default_01_IMX455"}},
                            .video = {{"adj_video_default_00_IMX455"}, {"adj_video_default_01_IMX455"}, {"adj_video_default_02_IMX455"}},
                            .photo = {{"adj_photo_default_00_IMX455"}, {"adj_photo_default_01_IMX455"}},
                            .stillLISO = {{"adj_still_default_00_IMX455"}, {"adj_still_default_01_IMX455"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX455"}},
                            .stillIdxInfo = {"adj_still_idx_IMX455"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX455"}, {"VideoCc1_IMX455"}, {"VideoCc2_IMX455"}, {"VideoCc3_IMX455"}, {"VideoCc4_IMX455"}},
                                             .stillCC = {{"StillCc0_IMX455"}, {"StillCc1_IMX455"}, {"StillCc2_IMX455"}, {"StillCc3_IMX455"}, {"StillCc4_IMX455"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX455"}, {"cc3d_cc_bw_gamma_lin_video_IMX455"}, {"cc3d_cc_bw_gamma_lin_video_IMX455"}, {"cc3d_cc_bw_gamma_lin_video_IMX455"}, {"cc3d_cc_bw_gamma_lin_video_IMX455"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX455"}, {"cc3d_cc_bw_gamma_lin_still_IMX455"}, {"cc3d_cc_bw_gamma_lin_still_IMX455"}, {"cc3d_cc_bw_gamma_lin_still_IMX455"}, {"cc3d_cc_bw_gamma_lin_still_IMX455"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX455"}, {"scene_data_s02_IMX455"}, {"scene_data_s03_IMX455"}, {"scene_data_s04_IMX455"}, {"scene_data_s05_IMX455"}},
                            .DEVideo = {{"de_default_video_IMX455"}},
                            .DEStill = {{"de_default_still_IMX455"}},
                        };

static IQ_TABLE_PATH_s IMX290HDR2Path = {
                            .ADJTable = {"adj_table_param_default_IMX290HDR2"},
                            .ImgParam = {"img_default_IMX290HDR2"},
                            .aaaDefault = {{"aaa_default_00_IMX290HDR2"}, {"aaa_default_01_IMX290HDR2"}},
                            .video = {{"adj_video_default_00_IMX290HDR2"}, {"adj_video_default_01_IMX290HDR2"}, {"adj_video_default_02_IMX290HDR2"}},
                            .photo = {{"adj_photo_default_00_IMX290HDR2"}, {"adj_photo_default_01_IMX290HDR2"}},
                            .stillLISO = {{"adj_still_default_00_IMX290HDR2"}, {"adj_still_default_01_IMX290HDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX290HDR2"}},
                            .stillIdxInfo = {"adj_still_idx_IMX290HDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX290HDR2"}, {"VideoCc1_IMX290HDR2"}, {"VideoCc2_IMX290HDR2"}, {"VideoCc3_IMX290HDR2"}, {"VideoCc4_IMX290HDR2"}},
                                             .stillCC = {{"StillCc0_IMX290HDR2"}, {"StillCc1_IMX290HDR2"}, {"StillCc2_IMX290HDR2"}, {"StillCc3_IMX290HDR2"}, {"StillCc4_IMX290HDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX290HDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX290HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX290HDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX290HDR2"}, {"scene_data_s02_IMX290HDR2"}, {"scene_data_s03_IMX290HDR2"}, {"scene_data_s04_IMX290HDR2"}, {"scene_data_s05_IMX290HDR2"}},
                            .DEVideo = {{"de_default_video_IMX290HDR2"}},
                            .DEStill = {{"de_default_still_IMX290HDR2"}},
                        };

static IQ_TABLE_PATH_s IMX390Path = {
                            .ADJTable = {"adj_table_param_default_IMX390"},
                            .ImgParam = {"img_default_IMX390"},
                            .aaaDefault = {{"aaa_default_00_IMX390"}, {"aaa_default_01_IMX390"}},
                            .video = {{"adj_video_default_00_IMX390"}, {"adj_video_default_01_IMX390"}, {"adj_video_default_02_IMX390"}},
                            .photo = {{"adj_photo_default_00_IMX390"}, {"adj_photo_default_01_IMX390"}},
                            .stillLISO = {{"adj_still_default_00_IMX390"}, {"adj_still_default_01_IMX390"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX390"}},
                            .stillIdxInfo = {"adj_still_idx_IMX390"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX390"}, {"VideoCc1_IMX390"}, {"VideoCc2_IMX390"}, {"VideoCc3_IMX390"}, {"VideoCc4_IMX390"}},
                                             .stillCC = {{"StillCc0_IMX390"}, {"StillCc1_IMX390"}, {"StillCc2_IMX390"}, {"StillCc3_IMX390"}, {"StillCc4_IMX390"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX390"}, {"cc3d_cc_bw_gamma_lin_video_IMX390"}, {"cc3d_cc_bw_gamma_lin_video_IMX390"}, {"cc3d_cc_bw_gamma_lin_video_IMX390"}, {"cc3d_cc_bw_gamma_lin_video_IMX390"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX390"}, {"cc3d_cc_bw_gamma_lin_still_IMX390"}, {"cc3d_cc_bw_gamma_lin_still_IMX390"}, {"cc3d_cc_bw_gamma_lin_still_IMX390"}, {"cc3d_cc_bw_gamma_lin_still_IMX390"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX390"}, {"scene_data_s02_IMX390"}, {"scene_data_s03_IMX390"}, {"scene_data_s04_IMX390"}, {"scene_data_s05_IMX390"}},
                            .DEVideo = {{"de_default_video_IMX390"}},
                            .DEStill = {{"de_default_still_IMX390"}},
                        };

static IQ_TABLE_PATH_s IMX390SHDR3Path = {
                            .ADJTable = {"adj_table_param_default_IMX390SHDR3"},
                            .ImgParam = {"img_default_IMX390SHDR3"},
                            .aaaDefault = {{"aaa_default_00_IMX390SHDR3"}, {"aaa_default_01_IMX390SHDR3"}},
                            .video = {{"adj_video_default_00_IMX390SHDR3"}, {"adj_video_default_01_IMX390SHDR3"}, {"adj_video_default_02_IMX390SHDR3"}},
                            .photo = {{"adj_photo_default_00_IMX390SHDR3"}, {"adj_photo_default_01_IMX390SHDR3"}},
                            .stillLISO = {{"adj_still_default_00_IMX390SHDR3"}, {"adj_still_default_01_IMX390SHDR3"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX390SHDR3"}},
                            .stillIdxInfo = {"adj_still_idx_IMX390SHDR3"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX390SHDR3"}, {"VideoCc1_IMX390SHDR3"}, {"VideoCc2_IMX390SHDR3"}, {"VideoCc3_IMX390SHDR3"}, {"VideoCc4_IMX390SHDR3"}},
                                             .stillCC = {{"StillCc0_IMX390SHDR3"}, {"StillCc1_IMX390SHDR3"}, {"StillCc2_IMX390SHDR3"}, {"StillCc3_IMX390SHDR3"}, {"StillCc4_IMX390SHDR3"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX390SHDR3"}, {"scene_data_s02_IMX390SHDR3"}, {"scene_data_s03_IMX390SHDR3"}, {"scene_data_s04_IMX390SHDR3"}, {"scene_data_s05_IMX390SHDR3"}},
                            .DEVideo = {{"de_default_video_IMX390SHDR3"}},
                            .DEStill = {{"de_default_still_IMX390SHDR3"}},
                        };

static IQ_TABLE_PATH_s IMX390SHDR3RCCBPath = {
                            .ADJTable = {"adj_table_param_default_IMX390SHDR3RCCB"},
                            .ImgParam = {"img_default_IMX390SHDR3RCCB"},
                            .aaaDefault = {{"aaa_default_00_IMX390SHDR3RCCB"}, {"aaa_default_01_IMX390SHDR3RCCB"}},
                            .video = {{"adj_video_default_00_IMX390SHDR3RCCB"}, {"adj_video_default_01_IMX390SHDR3RCCB"}, {"adj_video_default_02_IMX390SHDR3RCCB"}},
                            .photo = {{"adj_photo_default_00_IMX390SHDR3RCCB"}, {"adj_photo_default_01_IMX390SHDR3RCCB"}},
                            .stillLISO = {{"adj_still_default_00_IMX390SHDR3RCCB"}, {"adj_still_default_01_IMX390SHDR3RCCB"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX390SHDR3RCCB"}},
                            .stillIdxInfo = {"adj_still_idx_IMX390SHDR3RCCB"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX390SHDR3RCCB"}, {"VideoCc1_IMX390SHDR3RCCB"}, {"VideoCc2_IMX390SHDR3RCCB"}, {"VideoCc3_IMX390SHDR3RCCB"}, {"VideoCc4_IMX390SHDR3RCCB"}},
                                             .stillCC = {{"StillCc0_IMX390SHDR3RCCB"}, {"StillCc1_IMX390SHDR3RCCB"}, {"StillCc2_IMX390SHDR3RCCB"}, {"StillCc3_IMX390SHDR3RCCB"}, {"StillCc4_IMX390SHDR3RCCB"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3RCCB"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3RCCB"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX390SHDR3RCCB"}, {"scene_data_s02_IMX390SHDR3RCCB"}, {"scene_data_s03_IMX390SHDR3RCCB"}, {"scene_data_s04_IMX390SHDR3RCCB"}, {"scene_data_s05_IMX390SHDR3RCCB"}},
                            .DEVideo = {{"de_default_video_IMX390SHDR3RCCB"}},
                            .DEStill = {{"de_default_still_IMX390SHDR3RCCB"}},
                        };

static IQ_TABLE_PATH_s IMX390SHDR3_24_Path = {
                            .ADJTable = {"adj_table_param_default_IMX390SHDR3_24"},
                            .ImgParam = {"img_default_IMX390SHDR3_24"},
                            .aaaDefault = {{"aaa_default_00_IMX390SHDR3_24"}, {"aaa_default_01_IMX390SHDR3_24"}},
                            .video = {{"adj_video_default_00_IMX390SHDR3_24"}, {"adj_video_default_01_IMX390SHDR3_24"}, {"adj_video_default_02_IMX390SHDR3_24"}},
                            .photo = {{"adj_photo_default_00_IMX390SHDR3_24"}, {"adj_photo_default_01_IMX390SHDR3_24"}},
                            .stillLISO = {{"adj_still_default_00_IMX390SHDR3_24"}, {"adj_still_default_01_IMX390SHDR3_24"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX390SHDR3_24"}},
                            .stillIdxInfo = {"adj_still_idx_IMX390SHDR3_24"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX390SHDR3_24"}, {"VideoCc1_IMX390SHDR3_24"}, {"VideoCc2_IMX390SHDR3_24"}, {"VideoCc3_IMX390SHDR3_24"}, {"VideoCc4_IMX390SHDR3_24"}},
                                             .stillCC = {{"StillCc0_IMX390SHDR3_24"}, {"StillCc1_IMX390SHDR3_24"}, {"StillCc2_IMX390SHDR3_24"}, {"StillCc3_IMX390SHDR3_24"}, {"StillCc4_IMX390SHDR3_24"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_video_IMX390SHDR3_24"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3_24"}, {"cc3d_cc_bw_gamma_lin_still_IMX390SHDR3_24"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX390SHDR3_24"}, {"scene_data_s02_IMX390SHDR3_24"}, {"scene_data_s03_IMX390SHDR3_24"}, {"scene_data_s04_IMX390SHDR3_24"}, {"scene_data_s05_IMX390SHDR3_24"}},
                            .DEVideo = {{"de_default_video_IMX390SHDR3_24"}},
                            .DEStill = {{"de_default_still_IMX390SHDR3_24"}},
                        };

 static IQ_TABLE_PATH_s IMX424SHDR3Path = {
                             .ADJTable = {"adj_table_param_default_IMX424SHDR3"},
                             .ImgParam = {"img_default_IMX424SHDR3"},
                             .aaaDefault = {{"aaa_default_00_IMX424SHDR3"}, {"aaa_default_01_IMX424SHDR3"}},
                             .video = {{"adj_video_default_00_IMX424SHDR3"}, {"adj_video_default_01_IMX424SHDR3"}, {"adj_video_default_02_IMX424SHDR3"}},
                             .photo = {{"adj_photo_default_00_IMX424SHDR3"}, {"adj_photo_default_01_IMX424SHDR3"}},
                             .stillLISO = {{"adj_still_default_00_IMX424SHDR3"}, {"adj_still_default_01_IMX424SHDR3"}},
                             .stillHISO = {{"adj_hiso_still_default_00_IMX424SHDR3"}},
                            .stillIdxInfo = {"adj_still_idx_IMX424SHDR3"},
                             .CCSetPaths = {
                                             {.videoCC = {{"VideoCc0_IMX424SHDR3"}, {"VideoCc1_IMX424SHDR3"}, {"VideoCc2_IMX424SHDR3"}, {"VideoCc3_IMX424SHDR3"}, {"VideoCc4_IMX424SHDR3"}},
                                              .stillCC = {{"StillCc0_IMX424SHDR3"}, {"StillCc1_IMX424SHDR3"}, {"StillCc2_IMX424SHDR3"}, {"StillCc3_IMX424SHDR3"}, {"StillCc4_IMX424SHDR3"}},
                                             },
                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX424SHDR3"}},
                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX424SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX424SHDR3"}},
                                             }
                                         },
                             .scene = {{"scene_data_s01_IMX424SHDR3"}, {"scene_data_s02_IMX424SHDR3"}, {"scene_data_s03_IMX424SHDR3"}, {"scene_data_s04_IMX424SHDR3"}, {"scene_data_s05_IMX424SHDR3"}},
                             .DEVideo = {{"de_default_video_IMX424SHDR3"}},
                             .DEStill = {{"de_default_still_IMX424SHDR3"}},
                         };

static IQ_TABLE_PATH_s IMX577HDR2Path = {
                            .ADJTable = {"adj_table_param_default_IMX577HDR2"},
                            .ImgParam = {"img_default_IMX577HDR2"},
                            .aaaDefault = {{"aaa_default_00_IMX577HDR2"}, {"aaa_default_01_IMX577HDR2"}},
                            .video = {{"adj_video_default_00_IMX577HDR2"}, {"adj_video_default_01_IMX577HDR2"}, {"adj_video_default_02_IMX577HDR2"}},
                            .photo = {{"adj_photo_default_00_IMX577HDR2"}, {"adj_photo_default_01_IMX577HDR2"}},
                            .stillLISO = {{"adj_still_default_00_IMX577HDR2"}, {"adj_still_default_01_IMX577HDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX577HDR2"}},
                            .stillIdxInfo = {"adj_still_idx_IMX577HDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX577HDR2"}, {"VideoCc1_IMX577HDR2"}, {"VideoCc2_IMX577HDR2"}, {"VideoCc3_IMX577HDR2"}, {"VideoCc4_IMX577HDR2"}},
                                             .stillCC = {{"StillCc0_IMX577HDR2"}, {"StillCc1_IMX577HDR2"}, {"StillCc2_IMX577HDR2"}, {"StillCc3_IMX577HDR2"}, {"StillCc4_IMX577HDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_video_IMX577HDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX577HDR2"}, {"cc3d_cc_bw_gamma_lin_still_IMX577HDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX577HDR2"}, {"scene_data_s02_IMX577HDR2"}, {"scene_data_s03_IMX577HDR2"}, {"scene_data_s04_IMX577HDR2"}, {"scene_data_s05_IMX577HDR2"}},
                            .DEVideo = {{"de_default_video_IMX577HDR2"}},
                            .DEStill = {{"de_default_still_IMX577HDR2"}},
                        };

static IQ_TABLE_PATH_s AR0239Path = {
                            .ADJTable = {"adj_table_param_default_AR0239"},
                            .ImgParam = {"img_default_AR0239"},
                            .aaaDefault = {{"aaa_default_00_AR0239"}, {"aaa_default_01_AR0239"}},
                            .video = {{"adj_video_default_00_AR0239"}, {"adj_video_default_01_AR0239"}, {"adj_video_default_02_AR0239"}},
                            .photo = {{"adj_photo_default_00_AR0239"}, {"adj_photo_default_01_AR0239"}},
                            .stillLISO = {{"adj_still_default_00_AR0239"}, {"adj_still_default_01_AR0239"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0239"}},
                            .stillIdxInfo = {"adj_still_idx_AR0239"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0239"}, {"VideoCc1_AR0239"}, {"VideoCc2_AR0239"}, {"VideoCc3_AR0239"}, {"VideoCc4_AR0239"}},
                                             .stillCC = {{"StillCc0_AR0239"}, {"StillCc1_AR0239"}, {"StillCc2_AR0239"}, {"StillCc3_AR0239"}, {"StillCc4_AR0239"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0239"}, {"cc3d_cc_bw_gamma_lin_video_AR0239"}, {"cc3d_cc_bw_gamma_lin_video_AR0239"}, {"cc3d_cc_bw_gamma_lin_video_AR0239"}, {"cc3d_cc_bw_gamma_lin_video_AR0239"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0239"}, {"cc3d_cc_bw_gamma_lin_still_AR0239"}, {"cc3d_cc_bw_gamma_lin_still_AR0239"}, {"cc3d_cc_bw_gamma_lin_still_AR0239"}, {"cc3d_cc_bw_gamma_lin_still_AR0239"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0239"}, {"scene_data_s02_AR0239"}, {"scene_data_s03_AR0239"}, {"scene_data_s04_AR0239"}, {"scene_data_s05_AR0239"}},
                            .DEVideo = {{"de_default_video_AR0239"}},
                            .DEStill = {{"de_default_still_AR0239"}},
                        };

// static IQ_TABLE_PATH_s AR0220Path = {
//                             .ADJTable = {"adj_table_param_default_AR0220"},
//                             .ImgParam = {"img_default_AR0220"},
//                             .aaaDefault = {{"aaa_default_00_AR0220"}, {"aaa_default_01_AR0220"}},
//                             .video = {{"adj_video_default_00_AR0220"}, {"adj_video_default_01_AR0220"}, {"adj_video_default_02_AR0220"}},
//                             .photo = {{"adj_photo_default_00_AR0220"}, {"adj_photo_default_01_AR0220"}},
//                             .stillLISO = {{"adj_still_default_00_AR0220"}, {"adj_still_default_01_AR0220"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_AR0220"}},
//                             .stillIdxInfo = {"adj_still_idx_AR0220"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_AR0220"}, {"VideoCc1_AR0220"}, {"VideoCc2_AR0220"}, {"VideoCc3_AR0220"}, {"VideoCc4_AR0220"}},
//                                              .stillCC = {{"StillCc0_AR0220"}, {"StillCc1_AR0220"}, {"StillCc2_AR0220"}, {"StillCc3_AR0220"}, {"StillCc4_AR0220"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0220"}, {"cc3d_cc_bw_gamma_lin_video_AR0220"}, {"cc3d_cc_bw_gamma_lin_video_AR0220"}, {"cc3d_cc_bw_gamma_lin_video_AR0220"}, {"cc3d_cc_bw_gamma_lin_video_AR0220"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0220"}, {"cc3d_cc_bw_gamma_lin_still_AR0220"}, {"cc3d_cc_bw_gamma_lin_still_AR0220"}, {"cc3d_cc_bw_gamma_lin_still_AR0220"}, {"cc3d_cc_bw_gamma_lin_still_AR0220"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_AR0220"}, {"scene_data_s02_AR0220"}, {"scene_data_s03_AR0220"}, {"scene_data_s04_AR0220"}, {"scene_data_s05_AR0220"}},
//                             .DEVideo = {{"de_default_video_AR0220"}},
//                             .DEStill = {{"de_default_still_AR0220"}},
//                         };

static IQ_TABLE_PATH_s AR0239HDR2Path = {
                            .ADJTable = {"adj_table_param_default_AR0239HDR2"},
                            .ImgParam = {"img_default_AR0239HDR2"},
                            .aaaDefault = {{"aaa_default_00_AR0239HDR2"}, {"aaa_default_01_AR0239HDR2"}},
                            .video = {{"adj_video_default_00_AR0239HDR2"}, {"adj_video_default_01_AR0239HDR2"}, {"adj_video_default_02_AR0239HDR2"}},
                            .photo = {{"adj_photo_default_00_AR0239HDR2"}, {"adj_photo_default_01_AR0239HDR2"}},
                            .stillLISO = {{"adj_still_default_00_AR0239HDR2"}, {"adj_still_default_01_AR0239HDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0239HDR2"}},
                            .stillIdxInfo = {"adj_still_idx_AR0239HDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0239HDR2"}, {"VideoCc1_AR0239HDR2"}, {"VideoCc2_AR0239HDR2"}, {"VideoCc3_AR0239HDR2"}, {"VideoCc4_AR0239HDR2"}},
                                             .stillCC = {{"StillCc0_AR0239HDR2"}, {"StillCc1_AR0239HDR2"}, {"StillCc2_AR0239HDR2"}, {"StillCc3_AR0239HDR2"}, {"StillCc4_AR0239HDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0239HDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0239HDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0239HDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0239HDR2"}, {"scene_data_s02_AR0239HDR2"}, {"scene_data_s03_AR0239HDR2"}, {"scene_data_s04_AR0239HDR2"}, {"scene_data_s05_AR0239HDR2"}},
                            .DEVideo = {{"de_default_video_AR0239HDR2"}},
                            .DEStill = {{"de_default_still_AR0239HDR2"}},
                        };
// static IQ_TABLE_PATH_s AR0147SHDR3Path = {
//                             .ADJTable = {"adj_table_param_default_AR0147SHDR3"},
//                             .ImgParam = {"img_default_AR0147SHDR3"},
//                             .aaaDefault = {{"aaa_default_00_AR0147SHDR3"}, {"aaa_default_01_AR0147SHDR3"}},
//                             .video = {{"adj_video_default_00_AR0147SHDR3"}, {"adj_video_default_01_AR0147SHDR3"}, {"adj_video_default_02_AR0147SHDR3"}},
//                             .photo = {{"adj_photo_default_00_AR0147SHDR3"}, {"adj_photo_default_01_IMX390SHDR3_24"}},
//                             .stillLISO = {{"adj_still_default_00_AR0147SHDR3"}, {"adj_still_default_01_AR0147SHDR3"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_AR0147SHDR3"}},
//                             .stillIdxInfo = {"adj_still_idx_AR0147SHDR3"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_AR0147SHDR3"}, {"VideoCc1_AR0147SHDR3"}, {"VideoCc2_AR0147SHDR3"}, {"VideoCc3_AR0147SHDR3"}, {"VideoCc4_AR0147SHDR3"}},
//                                              .stillCC = {{"StillCc0_AR0147SHDR3"}, {"StillCc1_AR0147SHDR3"}, {"StillCc2_AR0147SHDR3"}, {"StillCc3_AR0147SHDR3"}, {"StillCc4_AR0147SHDR3"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR3"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR3"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_AR0147SHDR3"}, {"scene_data_s02_AR0147SHDR3"}, {"scene_data_s03_AR0147SHDR3"}, {"scene_data_s04_AR0147SHDR3"}, {"scene_data_s05_AR0147SHDR3"}},
//                             .DEVideo = {{"de_default_video_AR0147SHDR3"}},
//                             .DEStill = {{"de_default_still_AR0147SHDR3"}},
//                         };
// static IQ_TABLE_PATH_s AR0147SHDR2Path = {
//                             .ADJTable = {"adj_table_param_default_AR0147SHDR2"},
//                             .ImgParam = {"img_default_AR0147SHDR2"},
//                             .aaaDefault = {{"aaa_default_00_AR0147SHDR2"}, {"aaa_default_01_AR0147SHDR2"}},
//                             .video = {{"adj_video_default_00_AR0147SHDR2"}, {"adj_video_default_01_AR0147SHDR2"}, {"adj_video_default_02_AR0147SHDR2"}},
//                             .photo = {{"adj_photo_default_00_AR0147SHDR2"}, {"adj_photo_default_01_AR0147SHDR2"}},
//                             .stillLISO = {{"adj_still_default_00_AR0147SHDR2"}, {"adj_still_default_01_AR0147SHDR2"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_AR0147SHDR2"}},
//                             .stillIdxInfo = {"adj_still_idx_AR0147SHDR2"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_AR0147SHDR2"}, {"VideoCc1_AR0147SHDR2"}, {"VideoCc2_AR0147SHDR2"}, {"VideoCc3_AR0147SHDR2"}, {"VideoCc4_AR0147SHDR2"}},
//                                              .stillCC = {{"StillCc0_AR0147SHDR2"}, {"StillCc1_AR0147SHDR2"}, {"StillCc2_AR0147SHDR2"}, {"StillCc3_AR0147SHDR2"}, {"StillCc4_AR0147SHDR2"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0147SHDR2"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0147SHDR2"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_AR0147SHDR2"}, {"scene_data_s02_AR0147SHDR2"}, {"scene_data_s03_AR0147SHDR2"}, {"scene_data_s04_AR0147SHDR2"}, {"scene_data_s05_AR0147SHDR2"}},
//                             .DEVideo = {{"de_default_video_AR0147SHDR2"}},
//                             .DEStill = {{"de_default_still_AR0147SHDR2"}},
//                        };

static IQ_TABLE_PATH_s IMX224Path = {
                            .ADJTable = {"adj_table_param_default_IMX224"},
                            .ImgParam = {"img_default_IMX224"},
                            .aaaDefault = {{"aaa_default_00_IMX224"}, {"aaa_default_01_IMX224"}},
                            .video = {{"adj_video_default_00_IMX224"}, {"adj_video_default_01_IMX224"}, {"adj_video_default_02_IMX224"}},
                            .photo = {{"adj_photo_default_00_IMX224"}, {"adj_photo_default_01_IMX224"}},
                            .stillLISO = {{"adj_still_default_00_IMX224"}, {"adj_still_default_01_IMX224"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX224"}},
                            .stillIdxInfo = {"adj_still_idx_IMX224"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX224"}, {"VideoCc1_IMX224"}, {"VideoCc2_IMX224"}, {"VideoCc3_IMX224"}, {"VideoCc4_IMX224"}},
                                             .stillCC = {{"StillCc0_IMX224"}, {"StillCc1_IMX224"}, {"StillCc2_IMX224"}, {"StillCc3_IMX224"}, {"StillCc4_IMX224"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX224"}, {"cc3d_cc_bw_gamma_lin_video_IMX224"}, {"cc3d_cc_bw_gamma_lin_video_IMX224"}, {"cc3d_cc_bw_gamma_lin_video_IMX224"}, {"cc3d_cc_bw_gamma_lin_video_IMX224"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX224"}, {"cc3d_cc_bw_gamma_lin_still_IMX224"}, {"cc3d_cc_bw_gamma_lin_still_IMX224"}, {"cc3d_cc_bw_gamma_lin_still_IMX224"}, {"cc3d_cc_bw_gamma_lin_still_IMX224"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX224"}, {"scene_data_s02_IMX224"}, {"scene_data_s03_IMX224"}, {"scene_data_s04_IMX224"}, {"scene_data_s05_IMX224"}},
                            .DEVideo = {{"de_default_video_IMX224"}},
                            .DEStill = {{"de_default_still_IMX224"}},
                        };

static IQ_TABLE_PATH_s IMX490Path = {
                            .ADJTable = {"adj_table_param_default_IMX490"},
                            .ImgParam = {"img_default_IMX490"},
                            .aaaDefault = {{"aaa_default_00_IMX490"}, {"aaa_default_01_IMX490"}},
                            .video = {{"adj_video_default_00_IMX490"}, {"adj_video_default_01_IMX490"}, {"adj_video_default_02_IMX490"}},
                            .photo = {{"adj_photo_default_00_IMX490"}, {"adj_photo_default_01_IMX490"}},
                            .stillLISO = {{"adj_still_default_00_IMX490"}, {"adj_still_default_01_IMX490"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX490"}},
                            .stillIdxInfo = {"adj_still_idx_IMX490"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX490"}, {"VideoCc1_IMX490"}, {"VideoCc2_IMX490"}, {"VideoCc3_IMX490"}, {"VideoCc4_IMX490"}},
                                             .stillCC = {{"StillCc0_IMX490"}, {"StillCc1_IMX490"}, {"StillCc2_IMX490"}, {"StillCc3_IMX490"}, {"StillCc4_IMX490"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX490"}, {"cc3d_cc_bw_gamma_lin_video_IMX490"}, {"cc3d_cc_bw_gamma_lin_video_IMX490"}, {"cc3d_cc_bw_gamma_lin_video_IMX490"}, {"cc3d_cc_bw_gamma_lin_video_IMX490"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX490"}, {"cc3d_cc_bw_gamma_lin_still_IMX490"}, {"cc3d_cc_bw_gamma_lin_still_IMX490"}, {"cc3d_cc_bw_gamma_lin_still_IMX490"}, {"cc3d_cc_bw_gamma_lin_still_IMX490"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX490"}, {"scene_data_s02_IMX490"}, {"scene_data_s03_IMX490"}, {"scene_data_s04_IMX490"}, {"scene_data_s05_IMX490"}},
                            .DEVideo = {{"de_default_video_IMX490"}},
                            .DEStill = {{"de_default_still_IMX490"}},
                        };

static IQ_TABLE_PATH_s IMX490SHDR4Path = {
                            .ADJTable = {"adj_table_param_default_IMX490SHDR4"},
                            .ImgParam = {"img_default_IMX490SHDR4"},
                            .aaaDefault = {{"aaa_default_00_IMX490SHDR4"}, {"aaa_default_01_IMX490SHDR4"}},
                            .video = {{"adj_video_default_00_IMX490SHDR4"}, {"adj_video_default_01_IMX490SHDR4"}, {"adj_video_default_02_IMX490SHDR4"}},
                            .photo = {{"adj_photo_default_00_IMX490SHDR4"}, {"adj_photo_default_01_IMX490SHDR4"}},
                            .stillLISO = {{"adj_still_default_00_IMX490SHDR4"}, {"adj_still_default_01_IMX490SHDR4"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX490SHDR4"}},
                            .stillIdxInfo = {"adj_still_idx_IMX490SHDR4"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX490SHDR4"}, {"VideoCc1_IMX490SHDR4"}, {"VideoCc2_IMX490SHDR4"}, {"VideoCc3_IMX490SHDR4"}, {"VideoCc4_IMX490SHDR4"}},
                                             .stillCC = {{"StillCc0_IMX490SHDR4"}, {"StillCc1_IMX490SHDR4"}, {"StillCc2_IMX490SHDR4"}, {"StillCc3_IMX490SHDR4"}, {"StillCc4_IMX490SHDR4"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX490SHDR4"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX490SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX490SHDR4"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX490SHDR4"}, {"scene_data_s02_IMX490SHDR4"}, {"scene_data_s03_IMX490SHDR4"}, {"scene_data_s04_IMX490SHDR4"}, {"scene_data_s05_IMX490SHDR4"}},
                            .DEVideo = {{"de_default_video_IMX490SHDR4"}},
                            .DEStill = {{"de_default_still_IMX490SHDR4"}},
                        };

static IQ_TABLE_PATH_s IMX728SHDR4Path = {
                            .ADJTable = {"adj_table_param_default_IMX728SHDR4"},
                            .ImgParam = {"img_default_IMX728SHDR4"},
                            .aaaDefault = {{"aaa_default_00_IMX728SHDR4"}, {"aaa_default_01_IMX728SHDR4"}},
                            .video = {{"adj_video_default_00_IMX728SHDR4"}, {"adj_video_default_01_IMX728SHDR4"}, {"adj_video_default_02_IMX728SHDR4"}},
                            .photo = {{"adj_photo_default_00_IMX728SHDR4"}, {"adj_photo_default_01_IMX728SHDR4"}},
                            .stillLISO = {{"adj_still_default_00_IMX728SHDR4"}, {"adj_still_default_01_IMX728SHDR4"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX728SHDR4"}},
                            .stillIdxInfo = {"adj_still_idx_IMX728SHDR4"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX728SHDR4"}, {"VideoCc1_IMX728SHDR4"}, {"VideoCc2_IMX728SHDR4"}, {"VideoCc3_IMX728SHDR4"}, {"VideoCc4_IMX728SHDR4"}},
                                             .stillCC = {{"StillCc0_IMX728SHDR4"}, {"StillCc1_IMX728SHDR4"}, {"StillCc2_IMX728SHDR4"}, {"StillCc3_IMX728SHDR4"}, {"StillCc4_IMX728SHDR4"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_IMX728SHDR4"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX728SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_IMX728SHDR4"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX728SHDR4"}, {"scene_data_s02_IMX728SHDR4"}, {"scene_data_s03_IMX728SHDR4"}, {"scene_data_s04_IMX728SHDR4"}, {"scene_data_s05_IMX728SHDR4"}},
                            .DEVideo = {{"de_default_video_IMX728SHDR4"}},
                            .DEStill = {{"de_default_still_IMX728SHDR4"}},
                        };

static IQ_TABLE_PATH_s AR0233Path = {
                            .ADJTable = {"adj_table_param_default_AR0233"},
                            .ImgParam = {"img_default_AR0233"},
                            .aaaDefault = {{"aaa_default_00_AR0233"}, {"aaa_default_01_AR0233"}},
                            .video = {{"adj_video_default_00_AR0233"}, {"adj_video_default_01_AR0233"}, {"adj_video_default_02_AR0233"}},
                            .photo = {{"adj_photo_default_00_AR0233"}, {"adj_photo_default_01_AR0233"}},
                            .stillLISO = {{"adj_still_default_00_AR0233"}, {"adj_still_default_01_AR0233"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0233"}},
                            .stillIdxInfo = {"adj_still_idx_AR0233"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0233"}, {"VideoCc1_AR0233"}, {"VideoCc2_AR0233"}, {"VideoCc3_AR0233"}, {"VideoCc4_AR0233"}},
                                             .stillCC = {{"StillCc0_AR0233"}, {"StillCc1_AR0233"}, {"StillCc2_AR0233"}, {"StillCc3_AR0233"}, {"StillCc4_AR0233"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0233"}, {"cc3d_cc_bw_gamma_lin_video_AR0233"}, {"cc3d_cc_bw_gamma_lin_video_AR0233"}, {"cc3d_cc_bw_gamma_lin_video_AR0233"}, {"cc3d_cc_bw_gamma_lin_video_AR0233"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0233"}, {"cc3d_cc_bw_gamma_lin_still_AR0233"}, {"cc3d_cc_bw_gamma_lin_still_AR0233"}, {"cc3d_cc_bw_gamma_lin_still_AR0233"}, {"cc3d_cc_bw_gamma_lin_still_AR0233"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0233"}, {"scene_data_s02_AR0233"}, {"scene_data_s03_AR0233"}, {"scene_data_s04_AR0233"}, {"scene_data_s05_AR0233"}},
                            .DEVideo = {{"de_default_video_AR0233"}},
                            .DEStill = {{"de_default_still_AR0233"}},
                        };
static IQ_TABLE_PATH_s AR0233SHDR2Path = {
                            .ADJTable = {"adj_table_param_default_AR0233SHDR2"},
                            .ImgParam = {"img_default_AR0233SHDR2"},
                            .aaaDefault = {{"aaa_default_00_AR0233SHDR2"}, {"aaa_default_01_AR0233SHDR2"}},
                            .video = {{"adj_video_default_00_AR0233SHDR2"}, {"adj_video_default_01_AR0233SHDR2"}, {"adj_video_default_02_AR0233SHDR2"}},
                            .photo = {{"adj_photo_default_00_AR0233SHDR2"}, {"adj_photo_default_01_AR0233SHDR2"}},
                            .stillLISO = {{"adj_still_default_00_AR0233SHDR2"}, {"adj_still_default_01_AR0233SHDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0233SHDR2"}},
                            .stillIdxInfo = {"adj_still_idx_AR0233SHDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0233SHDR2"}, {"VideoCc1_AR0233SHDR2"}, {"VideoCc2_AR0233SHDR2"}, {"VideoCc3_AR0233SHDR2"}, {"VideoCc4_AR0233SHDR2"}},
                                             .stillCC = {{"StillCc0_AR0233SHDR2"}, {"StillCc1_AR0233SHDR2"}, {"StillCc2_AR0233SHDR2"}, {"StillCc3_AR0233SHDR2"}, {"StillCc4_AR0233SHDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_AR0233SHDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0233SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_AR0233SHDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0233SHDR2"}, {"scene_data_s02_AR0233SHDR2"}, {"scene_data_s03_AR0233SHDR2"}, {"scene_data_s04_AR0233SHDR2"}, {"scene_data_s05_AR0233SHDR2"}},
                            .DEVideo = {{"de_default_video_AR0233SHDR2"}},
                            .DEStill = {{"de_default_still_AR0233SHDR2"}},
                        };

// static IQ_TABLE_PATH_s AR0220SHDR4Path = {
//                             .ADJTable = {"adj_table_param_default_AR0220SHDR4"},
//                             .ImgParam = {"img_default_AR0220SHDR4"},
//                             .aaaDefault = {{"aaa_default_00_AR0220SHDR4"}, {"aaa_default_01_AR0220SHDR4"}},
//                             .video = {{"adj_video_default_00_AR0220SHDR4"}, {"adj_video_default_01_AR0220SHDR4"}, {"adj_video_default_02_AR0220SHDR4"}},
//                             .photo = {{"adj_photo_default_00_AR0220SHDR4"}, {"adj_photo_default_01_AR0220SHDR4"}},
//                             .stillLISO = {{"adj_still_default_00_AR0220SHDR4"}, {"adj_still_default_01_AR0220SHDR4"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_AR0220SHDR4"}},
//                             .stillIdxInfo = {"adj_still_idx_AR0233"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_AR0220SHDR4"}, {"VideoCc1_AR0220SHDR4"}, {"VideoCc2_AR0220SHDR4"}, {"VideoCc3_AR0220SHDR4"}, {"VideoCc4_AR0220SHDR4"}},
//                                              .stillCC = {{"StillCc0_AR0220SHDR4"}, {"StillCc1_AR0220SHDR4"}, {"StillCc2_AR0220SHDR4"}, {"StillCc3_AR0220SHDR4"}, {"StillCc4_AR0220SHDR4"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_AR0220SHDR4"}, {"scene_data_s02_AR0220SHDR4"}, {"scene_data_s03_AR0220SHDR4"}, {"scene_data_s04_AR0220SHDR4"}, {"scene_data_s05_AR0220SHDR4"}},
//                             .DEVideo = {{"de_default_video_AR0220SHDR4"}},
//                             .DEStill = {{"de_default_still_AR0220SHDR4"}},
//                         };
// static IQ_TABLE_PATH_s AR0231Path = {
//                             .ADJTable = {"adj_table_param_default_AR0231"},
//                             .ImgParam = {"img_default_AR0231"},
//                             .aaaDefault = {{"aaa_default_00_AR0231"}, {"aaa_default_01_AR0231"}},
//                             .video = {{"adj_video_default_00_AR0231"}, {"adj_video_default_01_AR0231"}, {"adj_video_default_02_AR0231"}},
//                             .photo = {{"adj_photo_default_00_AR0231"}, {"adj_photo_default_01_AR0231"}},
//                             .stillLISO = {{"adj_still_default_00_AR0231"}, {"adj_still_default_01_AR0231"}},
//                             .stillHISO = {{"adj_hiso_still_default_00_AR0231"}},
//                             .stillIdxInfo = {"adj_still_idx_AR0231"},
//                             .CCSetPaths = {
//                                             {.videoCC = {{"VideoCc0_AR0231"}, {"VideoCc1_AR0231"}, {"VideoCc2_AR0231"}, {"VideoCc3_AR0231"}, {"VideoCc4_AR0231"}},
//                                              .stillCC = {{"StillCc0_AR0231"}, {"StillCc1_AR0231"}, {"StillCc2_AR0231"}, {"StillCc3_AR0231"}, {"StillCc4_AR0231"}},
//                                             },
//                                             {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0231"}, {"cc3d_cc_bw_gamma_lin_video_AR0231"}, {"cc3d_cc_bw_gamma_lin_video_AR0231"}, {"cc3d_cc_bw_gamma_lin_video_AR0231"}, {"cc3d_cc_bw_gamma_lin_video_AR0231"}},
//                                              .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0231"}, {"cc3d_cc_bw_gamma_lin_still_AR0231"}, {"cc3d_cc_bw_gamma_lin_still_AR0231"}, {"cc3d_cc_bw_gamma_lin_still_AR0231"}, {"cc3d_cc_bw_gamma_lin_still_AR0231"}},
//                                             }
//                                         },
//                             .scene = {{"scene_data_s01_AR0231"}, {"scene_data_s02_AR0231"}, {"scene_data_s03_AR0231"}, {"scene_data_s04_AR0231"}, {"scene_data_s05_AR0231"}},
//                             .DEVideo = {{"de_default_video_AR0231"}},
//                             .DEStill = {{"de_default_still_AR0231"}},
//                         };


static IQ_TABLE_PATH_s AR0220SHDR3RCCBPath = {
                            .ADJTable = {"adj_table_param_default_AR0220SHDR3RCCB"},
                            .ImgParam = {"img_default_AR0220SHDR3RCCB"},
                            .aaaDefault = {{"aaa_default_00_AR0220SHDR3RCCB"}, {"aaa_default_01_AR0220SHDR3RCCB"}},
                            .video = {{"adj_video_default_00_AR0220SHDR3RCCB"}, {"adj_video_default_01_AR0220SHDR3RCCB"}, {"adj_video_default_02_AR0220SHDR3RCCB"}},
                            .photo = {{"adj_photo_default_00_AR0220SHDR3RCCB"}, {"adj_photo_default_01_AR0220SHDR3RCCB"}},
                            .stillLISO = {{"adj_still_default_00_AR0220SHDR3RCCB"}, {"adj_still_default_01_AR0220SHDR3RCCB"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0220SHDR3RCCB"}},
                            .stillIdxInfo = {"adj_still_idx_AR0220SHDR3RCCB"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0220SHDR3RCCB"}, {"VideoCc1_AR0220SHDR3RCCB"}, {"VideoCc2_AR0220SHDR3RCCB"}, {"VideoCc3_AR0220SHDR3RCCB"}, {"VideoCc4_AR0220SHDR3RCCB"}},
                                             .stillCC = {{"StillCc0_AR0220SHDR3RCCB"}, {"StillCc1_AR0220SHDR3RCCB"}, {"StillCc2_AR0220SHDR3RCCB"}, {"StillCc3_AR0220SHDR3RCCB"}, {"StillCc4_AR0220SHDR3RCCB"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR3RCCB"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR3RCCB"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0220SHDR3RCCB"}, {"scene_data_s02_AR0220SHDR3RCCB"}, {"scene_data_s03_AR0220SHDR3RCCB"}, {"scene_data_s04_AR0220SHDR3RCCB"}, {"scene_data_s05_AR0220SHDR3RCCB"}},
                            .DEVideo = {{"de_default_video_AR0220SHDR3RCCB"}},
                            .DEStill = {{"de_default_still_AR0220SHDR3RCCB"}},
                        };

static IQ_TABLE_PATH_s AR0220SHDR4RCCBPath = {
                            .ADJTable = {"adj_table_param_default_AR0220SHDR4RCCB"},
                            .ImgParam = {"img_default_AR0220SHDR4RCCB"},
                            .aaaDefault = {{"aaa_default_00_AR0220SHDR4RCCB"}, {"aaa_default_01_AR0220SHDR4RCCB"}},
                            .video = {{"adj_video_default_00_AR0220SHDR4RCCB"}, {"adj_video_default_01_AR0220SHDR4RCCB"}, {"adj_video_default_02_AR0220SHDR4RCCB"}},
                            .photo = {{"adj_photo_default_00_AR0220SHDR4RCCB"}, {"adj_photo_default_01_AR0220SHDR4RCCB"}},
                            .stillLISO = {{"adj_still_default_00_AR0220SHDR4RCCB"}, {"adj_still_default_01_AR0220SHDR4RCCB"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0220SHDR4RCCB"}},
                            .stillIdxInfo = {"adj_still_idx_AR0220SHDR4RCCB"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0220SHDR4RCCB"}, {"VideoCc1_AR0220SHDR4RCCB"}, {"VideoCc2_AR0220SHDR4RCCB"}, {"VideoCc3_AR0220SHDR4RCCB"}, {"VideoCc4_AR0220SHDR4RCCB"}},
                                             .stillCC = {{"StillCc0_AR0220SHDR4RCCB"}, {"StillCc1_AR0220SHDR4RCCB"}, {"StillCc2_AR0220SHDR4RCCB"}, {"StillCc3_AR0220SHDR4RCCB"}, {"StillCc4_AR0220SHDR4RCCB"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0220SHDR4RCCB"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0220SHDR4RCCB"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0220SHDR4RCCB"}, {"scene_data_s02_AR0220SHDR4RCCB"}, {"scene_data_s03_AR0220SHDR4RCCB"}, {"scene_data_s04_AR0220SHDR4RCCB"}, {"scene_data_s05_AR0220SHDR4RCCB"}},
                            .DEVideo = {{"de_default_video_AR0220SHDR4RCCB"}},
                            .DEStill = {{"de_default_still_AR0220SHDR4RCCB"}},
                        };

static IQ_TABLE_PATH_s AR0820RCCBPath = {
                            .ADJTable = {"adj_table_param_default_AR0820RCCB"},
                            .ImgParam = {"img_default_AR0820RCCB"},
                            .aaaDefault = {{"aaa_default_00_AR0820RCCB"}, {"aaa_default_01_AR0820RCCB"}},
                            .video = {{"adj_video_default_00_AR0820RCCB"}, {"adj_video_default_01_AR0820RCCB"}, {"adj_video_default_02_AR0820RCCB"}},
                            .photo = {{"adj_photo_default_00_AR0820RCCB"}, {"adj_photo_default_01_AR0820RCCB"}},
                            .stillLISO = {{"adj_still_default_00_AR0820RCCB"}, {"adj_still_default_01_AR0820RCCB"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0820RCCB"}},
                            .stillIdxInfo = {"adj_still_idx_AR0820RCCB"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0820RCCB"}, {"VideoCc1_AR0820RCCB"}, {"VideoCc2_AR0820RCCB"}, {"VideoCc3_AR0820RCCB"}, {"VideoCc4_AR0820RCCB"}},
                                             .stillCC = {{"StillCc0_AR0820RCCB"}, {"StillCc1_AR0820RCCB"}, {"StillCc2_AR0820RCCB"}, {"StillCc3_AR0820RCCB"}, {"StillCc4_AR0820RCCB"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820RCCB"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820RCCB"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0820RCCB"}, {"scene_data_s02_AR0820RCCB"}, {"scene_data_s03_AR0820RCCB"}, {"scene_data_s04_AR0820RCCB"}, {"scene_data_s05_AR0820RCCB"}},
                            .DEVideo = {{"de_default_video_AR0820RCCB"}},
                            .DEStill = {{"de_default_still_AR0820RCCB"}},
                        };

static IQ_TABLE_PATH_s AR0820SHDR3RCCBPath = {
                            .ADJTable = {"adj_table_param_default_AR0820SHDR3RCCB"},
                            .ImgParam = {"img_default_AR0820SHDR3RCCB"},
                            .aaaDefault = {{"aaa_default_00_AR0820SHDR3RCCB"}, {"aaa_default_01_AR0820SHDR3RCCB"}},
                            .video = {{"adj_video_default_00_AR0820SHDR3RCCB"}, {"adj_video_default_01_AR0820SHDR3RCCB"}, {"adj_video_default_02_AR0820SHDR3RCCB"}},
                            .photo = {{"adj_photo_default_00_AR0820SHDR3RCCB"}, {"adj_photo_default_01_AR0820SHDR3RCCB"}},
                            .stillLISO = {{"adj_still_default_00_AR0820SHDR3RCCB"}, {"adj_still_default_01_AR0820SHDR3RCCB"}},
                            .stillHISO = {{"adj_hiso_still_default_00_AR0820SHDR3RCCB"}},
                            .stillIdxInfo = {"adj_still_idx_AR0820SHDR3RCCB"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_AR0820SHDR3RCCB"}, {"VideoCc1_AR0820SHDR3RCCB"}, {"VideoCc2_AR0820SHDR3RCCB"}, {"VideoCc3_AR0820SHDR3RCCB"}, {"VideoCc4_AR0820SHDR3RCCB"}},
                                             .stillCC = {{"StillCc0_AR0820SHDR3RCCB"}, {"StillCc1_AR0820SHDR3RCCB"}, {"StillCc2_AR0820SHDR3RCCB"}, {"StillCc3_AR0820SHDR3RCCB"}, {"StillCc4_AR0820SHDR3RCCB"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_video_AR0820SHDR3RCCB"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820SHDR3RCCB"}, {"cc3d_cc_bw_gamma_lin_still_AR0820SHDR3RCCB"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_AR0820SHDR3RCCB"}, {"scene_data_s02_AR0820SHDR3RCCB"}, {"scene_data_s03_AR0820SHDR3RCCB"}, {"scene_data_s04_AR0820SHDR3RCCB"}, {"scene_data_s05_AR0820SHDR3RCCB"}},
                            .DEVideo = {{"de_default_video_AR0820SHDR3RCCB"}},
                            .DEStill = {{"de_default_still_AR0820SHDR3RCCB"}},
                        };

static IQ_TABLE_PATH_s OV2312SIMULRGBIRPath = {
                            .ADJTable = {"adj_table_param_default_OV2312SIMULRGBIR"},
                            .ImgParam = {"img_default_OV2312SIMULRGBIR"},
                            .aaaDefault = {{"aaa_default_00_OV2312SIMULRGBIR"}, {"aaa_default_01_OV2312SIMULRGBIR"}},
                            .video = {{"adj_video_default_00_OV2312SIMULRGBIR"}, {"adj_video_default_01_OV2312SIMULRGBIR"}, {"adj_video_default_02_OV2312SIMULRGBIR"}},
                            .photo = {{"adj_photo_default_00_OV2312SIMULRGBIR"}, {"adj_photo_default_01_OV2312SIMULRGBIR"}},
                            .stillLISO = {{"adj_still_default_00_OV2312SIMULRGBIR"}, {"adj_still_default_01_OV2312SIMULRGBIR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV2312SIMULRGBIR"}},
                            .stillIdxInfo = {"adj_still_idx_OV2312SIMULRGBIR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV2312SIMULRGBIR"}, {"VideoCc1_OV2312SIMULRGBIR"}, {"VideoCc2_OV2312SIMULRGBIR"}, {"VideoCc3_OV2312SIMULRGBIR"}, {"VideoCc4_OV2312SIMULRGBIR"}},
                                             .stillCC = {{"StillCc0_OV2312SIMULRGBIR"}, {"StillCc1_OV2312SIMULRGBIR"}, {"StillCc2_OV2312SIMULRGBIR"}, {"StillCc3_OV2312SIMULRGBIR"}, {"StillCc4_OV2312SIMULRGBIR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULRGBIR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULRGBIR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV2312SIMULRGBIR"}, {"scene_data_s02_OV2312SIMULRGBIR"}, {"scene_data_s03_OV2312SIMULRGBIR"}, {"scene_data_s04_OV2312SIMULRGBIR"}, {"scene_data_s05_OV2312SIMULRGBIR"}},
                            .DEVideo = {{"de_default_video_OV2312SIMULRGBIR"}},
                            .DEStill = {{"de_default_still_OV2312SIMULRGBIR"}},
                        };

static IQ_TABLE_PATH_s OV2312SIMULIRPath = {
                            .ADJTable = {"adj_table_param_default_OV2312SIMULIR"},
                            .ImgParam = {"img_default_OV2312SIMULIR"},
                            .aaaDefault = {{"aaa_default_00_OV2312SIMULIR"}, {"aaa_default_01_OV2312SIMULIR"}},
                            .video = {{"adj_video_default_00_OV2312SIMULIR"}, {"adj_video_default_01_OV2312SIMULIR"}, {"adj_video_default_02_OV2312SIMULIR"}},
                            .photo = {{"adj_photo_default_00_OV2312SIMULIR"}, {"adj_photo_default_01_OV2312SIMULIR"}},
                            .stillLISO = {{"adj_still_default_00_OV2312SIMULIR"}, {"adj_still_default_01_OV2312SIMULIR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV2312SIMULIR"}},
                            .stillIdxInfo = {"adj_still_idx_OV2312SIMULIR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV2312SIMULIR"}, {"VideoCc1_OV2312SIMULIR"}, {"VideoCc2_OV2312SIMULIR"}, {"VideoCc3_OV2312SIMULIR"}, {"VideoCc4_OV2312SIMULIR"}},
                                             .stillCC = {{"StillCc0_OV2312SIMULIR"}, {"StillCc1_OV2312SIMULIR"}, {"StillCc2_OV2312SIMULIR"}, {"StillCc3_OV2312SIMULIR"}, {"StillCc4_OV2312SIMULIR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312SIMULIR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312SIMULIR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV2312SIMULIR"}, {"scene_data_s02_OV2312SIMULIR"}, {"scene_data_s03_OV2312SIMULIR"}, {"scene_data_s04_OV2312SIMULIR"}, {"scene_data_s05_OV2312SIMULIR"}},
                            .DEVideo = {{"de_default_video_OV2312SIMULIR"}},
                            .DEStill = {{"de_default_still_OV2312SIMULIR"}},
                        };

static IQ_TABLE_PATH_s OV2312TDRGBIRPath = {
                            .ADJTable = {"adj_table_param_default_OV2312TDRGBIR"},
                            .ImgParam = {"img_default_OV2312TDRGBIR"},
                            .aaaDefault = {{"aaa_default_00_OV2312TDRGBIR"}, {"aaa_default_01_OV2312TDRGBIR"}},
                            .video = {{"adj_video_default_00_OV2312TDRGBIR"}, {"adj_video_default_01_OV2312TDRGBIR"}, {"adj_video_default_02_OV2312TDRGBIR"}},
                            .photo = {{"adj_photo_default_00_OV2312TDRGBIR"}, {"adj_photo_default_01_OV2312TDRGBIR"}},
                            .stillLISO = {{"adj_still_default_00_OV2312TDRGBIR"}, {"adj_still_default_01_OV2312TDRGBIR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV2312TDRGBIR"}},
                            .stillIdxInfo = {"adj_still_idx_OV2312TDRGBIR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV2312TDRGBIR"}, {"VideoCc1_OV2312TDRGBIR"}, {"VideoCc2_OV2312TDRGBIR"}, {"VideoCc3_OV2312TDRGBIR"}, {"VideoCc4_OV2312TDRGBIR"}},
                                             .stillCC = {{"StillCc0_OV2312TDRGBIR"}, {"StillCc1_OV2312TDRGBIR"}, {"StillCc2_OV2312TDRGBIR"}, {"StillCc3_OV2312TDRGBIR"}, {"StillCc4_OV2312TDRGBIR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDRGBIR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDRGBIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDRGBIR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV2312TDRGBIR"}, {"scene_data_s02_OV2312TDRGBIR"}, {"scene_data_s03_OV2312TDRGBIR"}, {"scene_data_s04_OV2312TDRGBIR"}, {"scene_data_s05_OV2312TDRGBIR"}},
                            .DEVideo = {{"de_default_video_OV2312TDRGBIR"}},
                            .DEStill = {{"de_default_still_OV2312TDRGBIR"}},
                        };

static IQ_TABLE_PATH_s OV2312TDIRPath = {
                            .ADJTable = {"adj_table_param_default_OV2312TDIR"},
                            .ImgParam = {"img_default_OV2312TDIR"},
                            .aaaDefault = {{"aaa_default_00_OV2312TDIR"}, {"aaa_default_01_OV2312TDIR"}},
                            .video = {{"adj_video_default_00_OV2312TDIR"}, {"adj_video_default_01_OV2312TDIR"}, {"adj_video_default_02_OV2312TDIR"}},
                            .photo = {{"adj_photo_default_00_OV2312TDIR"}, {"adj_photo_default_01_OV2312TDIR"}},
                            .stillLISO = {{"adj_still_default_00_OV2312TDIR"}, {"adj_still_default_01_OV2312TDIR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV2312TDIR"}},
                            .stillIdxInfo = {"adj_still_idx_OV2312TDIR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV2312TDIR"}, {"VideoCc1_OV2312TDIR"}, {"VideoCc2_OV2312TDIR"}, {"VideoCc3_OV2312TDIR"}, {"VideoCc4_OV2312TDIR"}},
                                             .stillCC = {{"StillCc0_OV2312TDIR"}, {"StillCc1_OV2312TDIR"}, {"StillCc2_OV2312TDIR"}, {"StillCc3_OV2312TDIR"}, {"StillCc4_OV2312TDIR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_video_OV2312TDIR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDIR"}, {"cc3d_cc_bw_gamma_lin_still_OV2312TDIR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV2312TDIR"}, {"scene_data_s02_OV2312TDIR"}, {"scene_data_s03_OV2312TDIR"}, {"scene_data_s04_OV2312TDIR"}, {"scene_data_s05_OV2312TDIR"}},
                            .DEVideo = {{"de_default_video_OV2312TDIR"}},
                            .DEStill = {{"de_default_still_OV2312TDIR"}},
                        };

static IQ_TABLE_PATH_s OV10635SHDR2Path = {
                            .ADJTable = {"adj_table_param_default_OV10635SHDR2"},
                            .ImgParam = {"img_default_OV10635SHDR2"},
                            .aaaDefault = {{"aaa_default_00_OV10635SHDR2"}, {"aaa_default_01_OV10635SHDR2"}},
                            .video = {{"adj_video_default_00_OV10635SHDR2"}, {"adj_video_default_01_OV10635SHDR2"}, {"adj_video_default_02_OV10635SHDR2"}},
                            .photo = {{"adj_photo_default_00_OV10635SHDR2"}, {"adj_photo_default_01_OV10635SHDR2"}},
                            .stillLISO = {{"adj_still_default_00_OV10635SHDR2"}, {"adj_still_default_01_OV10635SHDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV10635SHDR2"}},
                            .stillIdxInfo = {"adj_still_idx_OV10635SHDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV10635SHDR2"}, {"VideoCc1_OV10635SHDR2"}, {"VideoCc2_OV10635SHDR2"}, {"VideoCc3_OV10635SHDR2"}, {"VideoCc4_OV10635SHDR2"}},
                                             .stillCC = {{"StillCc0_OV10635SHDR2"}, {"StillCc1_OV10635SHDR2"}, {"StillCc2_OV10635SHDR2"}, {"StillCc3_OV10635SHDR2"}, {"StillCc4_OV10635SHDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV10635SHDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV10635SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV10635SHDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV10635SHDR2"}, {"scene_data_s02_OV10635SHDR2"}, {"scene_data_s03_OV10635SHDR2"}, {"scene_data_s04_OV10635SHDR2"}, {"scene_data_s05_OV10635SHDR2"}},
                            .DEVideo = {{"de_default_video_OV10635SHDR2"}},
                            .DEStill = {{"de_default_still_OV10635SHDR2"}},
                            };

static IQ_TABLE_PATH_s OV2778Path = {
                            .ADJTable = {"adj_table_param_default_OV2778"},
                            .ImgParam = {"img_default_OV2778"},
                            .aaaDefault = {{"aaa_default_00_OV2778"}, {"aaa_default_01_OV2778"}},
                            .video = {{"adj_video_default_00_OV2778"}, {"adj_video_default_01_OV2778"}, {"adj_video_default_02_OV2778"}},
                            .photo = {{"adj_photo_default_00_OV2778"}, {"adj_photo_default_01_OV2778"}},
                            .stillLISO = {{"adj_still_default_00_OV2778"}, {"adj_still_default_01_OV2778"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV2778"}},
                            .stillIdxInfo = {"adj_still_idx_OV2778"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV2778"}, {"VideoCc1_OV2778"}, {"VideoCc2_OV2778"}, {"VideoCc3_OV2778"}, {"VideoCc4_OV2778"}},
                                             .stillCC = {{"StillCc0_OV2778"}, {"StillCc1_OV2778"}, {"StillCc2_OV2778"}, {"StillCc3_OV2778"}, {"StillCc4_OV2778"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV2778"}, {"cc3d_cc_bw_gamma_lin_video_OV2778"}, {"cc3d_cc_bw_gamma_lin_video_OV2778"}, {"cc3d_cc_bw_gamma_lin_video_OV2778"}, {"cc3d_cc_bw_gamma_lin_video_OV2778"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV2778"}, {"cc3d_cc_bw_gamma_lin_still_OV2778"}, {"cc3d_cc_bw_gamma_lin_still_OV2778"}, {"cc3d_cc_bw_gamma_lin_still_OV2778"}, {"cc3d_cc_bw_gamma_lin_still_OV2778"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV2778"}, {"scene_data_s02_OV2778"}, {"scene_data_s03_OV2778"}, {"scene_data_s04_OV2778"}, {"scene_data_s05_OV2778"}},
                            .DEVideo = {{"de_default_video_OV2778"}},
                            .DEStill = {{"de_default_still_OV2778"}},
                        };

static IQ_TABLE_PATH_s OV2778SHDR2Path = {
                            .ADJTable = {"adj_table_param_default_OV2778SHDR2"},
                            .ImgParam = {"img_default_OV2778SHDR2"},
                            .aaaDefault = {{"aaa_default_00_OV2778SHDR2"}, {"aaa_default_01_OV2778SHDR2"}},
                            .video = {{"adj_video_default_00_OV2778SHDR2"}, {"adj_video_default_01_OV2778SHDR2"}, {"adj_video_default_02_OV2778SHDR2"}},
                            .photo = {{"adj_photo_default_00_OV2778SHDR2"}, {"adj_photo_default_01_OV2778SHDR2"}},
                            .stillLISO = {{"adj_still_default_00_OV2778SHDR2"}, {"adj_still_default_01_OV2778SHDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV2778SHDR2"}},
                            .stillIdxInfo = {"adj_still_idx_OV2778SHDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV2778SHDR2"}, {"VideoCc1_OV2778SHDR2"}, {"VideoCc2_OV2778SHDR2"}, {"VideoCc3_OV2778SHDR2"}, {"VideoCc4_OV2778SHDR2"}},
                                             .stillCC = {{"StillCc0_OV2778SHDR2"}, {"StillCc1_OV2778SHDR2"}, {"StillCc2_OV2778SHDR2"}, {"StillCc3_OV2778SHDR2"}, {"StillCc4_OV2778SHDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV2778SHDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV2778SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV2778SHDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV2778SHDR2"}, {"scene_data_s02_OV2778SHDR2"}, {"scene_data_s03_OV2778SHDR2"}, {"scene_data_s04_OV2778SHDR2"}, {"scene_data_s05_OV2778SHDR2"}},
                            .DEVideo = {{"de_default_video_OV2778SHDR2"}},
                            .DEStill = {{"de_default_still_OV2778SHDR2"}},
                        };
static IQ_TABLE_PATH_s VG1762SHDR2Path = {
                            .ADJTable = {"adj_table_param_default_VG1762SHDR2"},
                            .ImgParam = {"img_default_VG1762SHDR2"},
                            .aaaDefault = {{"aaa_default_00_VG1762SHDR2"}, {"aaa_default_01_VG1762SHDR2"}},
                            .video = {{"adj_video_default_00_VG1762SHDR2"}, {"adj_video_default_01_VG1762SHDR2"}, {"adj_video_default_02_VG1762SHDR2"}},
                            .photo = {{"adj_photo_default_00_VG1762SHDR2"}, {"adj_photo_default_01_VG1762SHDR2"}},
                            .stillLISO = {{"adj_still_default_00_VG1762SHDR2"}, {"adj_still_default_01_VG1762SHDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_VG1762SHDR2"}},
                            .stillIdxInfo = {"adj_still_idx_VG1762SHDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_VG1762SHDR2"}, {"VideoCc1_VG1762SHDR2"}, {"VideoCc2_VG1762SHDR2"}, {"VideoCc3_VG1762SHDR2"}, {"VideoCc4_VG1762SHDR2"}},
                                             .stillCC = {{"StillCc0_VG1762SHDR2"}, {"StillCc1_VG1762SHDR2"}, {"StillCc2_VG1762SHDR2"}, {"StillCc3_VG1762SHDR2"}, {"StillCc4_VG1762SHDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762SHDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762SHDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_VG1762SHDR2"}, {"scene_data_s02_VG1762SHDR2"}, {"scene_data_s03_VG1762SHDR2"}, {"scene_data_s04_VG1762SHDR2"}, {"scene_data_s05_VG1762SHDR2"}},
                            .DEVideo = {{"de_default_video_VG1762SHDR2"}},
                            .DEStill = {{"de_default_still_VG1762SHDR2"}},
                        };
static IQ_TABLE_PATH_s VG1762DOUTPUTSHDR2Path = {
                            .ADJTable = {"adj_table_param_default_VG1762DOUTPUTSHDR2"},
                            .ImgParam = {"img_default_VG1762DOUTPUTSHDR2"},
                            .aaaDefault = {{"aaa_default_00_VG1762DOUTPUTSHDR2"}, {"aaa_default_01_VG1762DOUTPUTSHDR2"}},
                            .video = {{"adj_video_default_00_VG1762DOUTPUTSHDR2"}, {"adj_video_default_01_VG1762DOUTPUTSHDR2"}, {"adj_video_default_02_VG1762DOUTPUTSHDR2"}},
                            .photo = {{"adj_photo_default_00_VG1762DOUTPUTSHDR2"}, {"adj_photo_default_01_VG1762DOUTPUTSHDR2"}},
                            .stillLISO = {{"adj_still_default_00_VG1762DOUTPUTSHDR2"}, {"adj_still_default_01_VG1762DOUTPUTSHDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_VG1762DOUTPUTSHDR2"}},
                            .stillIdxInfo = {"adj_still_idx_VG1762DOUTPUTSHDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_VG1762DOUTPUTSHDR2"}, {"VideoCc1_VG1762DOUTPUTSHDR2"}, {"VideoCc2_VG1762DOUTPUTSHDR2"}, {"VideoCc3_VG1762DOUTPUTSHDR2"}, {"VideoCc4_VG1762DOUTPUTSHDR2"}},
                                             .stillCC = {{"StillCc0_VG1762DOUTPUTSHDR2"}, {"StillCc1_VG1762DOUTPUTSHDR2"}, {"StillCc2_VG1762DOUTPUTSHDR2"}, {"StillCc3_VG1762DOUTPUTSHDR2"}, {"StillCc4_VG1762DOUTPUTSHDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTSHDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTSHDR2"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTSHDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_VG1762DOUTPUTSHDR2"}, {"scene_data_s02_VG1762DOUTPUTSHDR2"}, {"scene_data_s03_VG1762DOUTPUTSHDR2"}, {"scene_data_s04_VG1762DOUTPUTSHDR2"}, {"scene_data_s05_VG1762DOUTPUTSHDR2"}},
                            .DEVideo = {{"de_default_video_VG1762DOUTPUTSHDR2"}},
                            .DEStill = {{"de_default_still_VG1762DOUTPUTSHDR2"}},
                        };
static IQ_TABLE_PATH_s VG1762DOUTPUTIRPath = {
                            .ADJTable = {"adj_table_param_default_VG1762DOUTPUTIR"},
                            .ImgParam = {"img_default_VG1762DOUTPUTIR"},
                            .aaaDefault = {{"aaa_default_00_VG1762DOUTPUTIR"}, {"aaa_default_01_VG1762DOUTPUTIR"}},
                            .video = {{"adj_video_default_00_VG1762DOUTPUTIR"}, {"adj_video_default_01_VG1762DOUTPUTIR"}, {"adj_video_default_02_VG1762DOUTPUTIR"}},
                            .photo = {{"adj_photo_default_00_VG1762DOUTPUTIR"}, {"adj_photo_default_01_VG1762DOUTPUTIR"}},
                            .stillLISO = {{"adj_still_default_00_VG1762DOUTPUTIR"}, {"adj_still_default_01_VG1762DOUTPUTIR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_VG1762DOUTPUTIR"}},
                            .stillIdxInfo = {"adj_still_idx_VG1762DOUTPUTIR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_VG1762DOUTPUTIR"}, {"VideoCc1_VG1762DOUTPUTIR"}, {"VideoCc2_VG1762DOUTPUTIR"}, {"VideoCc3_VG1762DOUTPUTIR"}, {"VideoCc4_VG1762DOUTPUTIR"}},
                                             .stillCC = {{"StillCc0_VG1762DOUTPUTIR"}, {"StillCc1_VG1762DOUTPUTIR"}, {"StillCc2_VG1762DOUTPUTIR"}, {"StillCc3_VG1762DOUTPUTIR"}, {"StillCc4_VG1762DOUTPUTIR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_video_VG1762DOUTPUTIR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTIR"}, {"cc3d_cc_bw_gamma_lin_still_VG1762DOUTPUTIR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_VG1762DOUTPUTIR"}, {"scene_data_s02_VG1762DOUTPUTIR"}, {"scene_data_s03_VG1762DOUTPUTIR"}, {"scene_data_s04_VG1762DOUTPUTIR"}, {"scene_data_s05_VG1762DOUTPUTIR"}},
                            .DEVideo = {{"de_default_video_VG1762DOUTPUTIR"}},
                            .DEStill = {{"de_default_still_VG1762DOUTPUTIR"}},
                        };
static IQ_TABLE_PATH_s IMX686Path = {
                            .ADJTable = {"adj_table_param_default_IMX686"},
                            .ImgParam = {"img_default_IMX686"},
                            .aaaDefault = {{"aaa_default_00_IMX686"}, {"aaa_default_01_IMX686"}},
                            .video = {{"adj_video_default_00_IMX686"}, {"adj_video_default_01_IMX686"}, {"adj_video_default_02_IMX686"}},
                            .photo = {{"adj_photo_default_00_IMX686"}, {"adj_photo_default_01_IMX686"}},
                            .stillLISO = {{"adj_still_default_00_IMX686"}, {"adj_still_default_01_IMX686"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX686"}},
                            .stillIdxInfo = {"adj_still_idx_IMX686"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX686"}, {"VideoCc1_IMX686"}, {"VideoCc2_IMX686"}, {"VideoCc3_IMX686"}, {"VideoCc4_IMX686"}},
                                             .stillCC = {{"StillCc0_IMX686"}, {"StillCc1_IMX686"}, {"StillCc2_IMX686"}, {"StillCc3_IMX686"}, {"StillCc4_IMX686"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX686"}, {"cc3d_cc_bw_gamma_lin_video_IMX686"}, {"cc3d_cc_bw_gamma_lin_video_IMX686"}, {"cc3d_cc_bw_gamma_lin_video_IMX686"}, {"cc3d_cc_bw_gamma_lin_video_IMX686"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX686"}, {"cc3d_cc_bw_gamma_lin_still_IMX686"}, {"cc3d_cc_bw_gamma_lin_still_IMX686"}, {"cc3d_cc_bw_gamma_lin_still_IMX686"}, {"cc3d_cc_bw_gamma_lin_still_IMX686"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX686"}, {"scene_data_s02_IMX686"}, {"scene_data_s03_IMX686"}, {"scene_data_s04_IMX686"}, {"scene_data_s05_IMX686"}},
                            .DEVideo = {{"de_default_video_IMX686"}},
                            .DEStill = {{"de_default_still_IMX686"}},
                        };
static IQ_TABLE_PATH_s IMX686SHDR3Path = {
                            .ADJTable = {"adj_table_param_default_IMX686SHDR3"},
                            .ImgParam = {"img_default_IMX686SHDR3"},
                            .aaaDefault = {{"aaa_default_00_IMX686SHDR3"}, {"aaa_default_01_IMX686SHDR3"}},
                            .video = {{"adj_video_default_00_IMX686SHDR3"}, {"adj_video_default_01_IMX686SHDR3"}, {"adj_video_default_02_IMX686SHDR3"}},
                            .photo = {{"adj_photo_default_00_IMX686SHDR3"}, {"adj_photo_default_01_IMX686SHDR3"}},
                            .stillLISO = {{"adj_still_default_00_IMX686SHDR3"}, {"adj_still_default_01_IMX686SHDR3"}},
                            .stillHISO = {{"adj_hiso_still_default_00_IMX686SHDR3"}},
                            .stillIdxInfo = {"adj_still_idx_IMX686SHDR3"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_IMX686SHDR3"}, {"VideoCc1_IMX686SHDR3"}, {"VideoCc2_IMX686SHDR3"}, {"VideoCc3_IMX686SHDR3"}, {"VideoCc4_IMX686SHDR3"}},
                                             .stillCC = {{"StillCc0_IMX686SHDR3"}, {"StillCc1_IMX686SHDR3"}, {"StillCc2_IMX686SHDR3"}, {"StillCc3_IMX686SHDR3"}, {"StillCc4_IMX686SHDR3"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_video_IMX686SHDR3"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX686SHDR3"}, {"cc3d_cc_bw_gamma_lin_still_IMX686SHDR3"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_IMX686SHDR3"}, {"scene_data_s02_IMX686SHDR3"}, {"scene_data_s03_IMX686SHDR3"}, {"scene_data_s04_IMX686SHDR3"}, {"scene_data_s05_IMX686SHDR3"}},
                            .DEVideo = {{"de_default_video_IMX686SHDR3"}},
                            .DEStill = {{"de_default_still_IMX686SHDR3"}},
                        };
static IQ_TABLE_PATH_s OV48C40Path = {
                            .ADJTable = {"adj_table_param_default_OV48C40"},
                            .ImgParam = {"img_default_OV48C40"},
                            .aaaDefault = {{"aaa_default_00_OV48C40"}, {"aaa_default_01_OV48C40"}},
                            .video = {{"adj_video_default_00_OV48C40"}, {"adj_video_default_01_OV48C40"}, {"adj_video_default_02_OV48C40"}},
                            .photo = {{"adj_photo_default_00_OV48C40"}, {"adj_photo_default_01_OV48C40"}},
                            .stillLISO = {{"adj_still_default_00_OV48C40"}, {"adj_still_default_01_OV48C40"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV48C40"}},
                            .stillIdxInfo = {"adj_still_idx_OV48C40"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV48C40"}, {"VideoCc1_OV48C40"}, {"VideoCc2_OV48C40"}, {"VideoCc3_OV48C40"}, {"VideoCc4_OV48C40"}},
                                             .stillCC = {{"StillCc0_OV48C40"}, {"StillCc1_OV48C40"}, {"StillCc2_OV48C40"}, {"StillCc3_OV48C40"}, {"StillCc4_OV48C40"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV48C40"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV48C40"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV48C40"}, {"scene_data_s02_OV48C40"}, {"scene_data_s03_OV48C40"}, {"scene_data_s04_OV48C40"}, {"scene_data_s05_OV48C40"}},
                            .DEVideo = {{"de_default_video_OV48C40"}},
                            .DEStill = {{"de_default_still_OV48C40"}},
                        };
static IQ_TABLE_PATH_s OV48C40SHDR2Path = {
                            .ADJTable = {"adj_table_param_default_OV48C40SHDR2"},
                            .ImgParam = {"img_default_OV48C40SHDR2"},
                            .aaaDefault = {{"aaa_default_00_OV48C40SHDR2"}, {"aaa_default_01_OV48C40SHDR2"}},
                            .video = {{"adj_video_default_00_OV48C40SHDR2"}, {"adj_video_default_01_OV48C40SHDR2"}, {"adj_video_default_02_OV48C40SHDR2"}},
                            .photo = {{"adj_photo_default_00_OV48C40SHDR2"}, {"adj_photo_default_01_OV48C40SHDR2"}},
                            .stillLISO = {{"adj_still_default_00_OV48C40SHDR2"}, {"adj_still_default_01_OV48C40SHDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV48C40SHDR2"}},
                            .stillIdxInfo = {"adj_still_idx_OV48C40SHDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV48C40SHDR2"}, {"VideoCc1_OV48C40SHDR2"}, {"VideoCc2_OV48C40SHDR2"}, {"VideoCc3_OV48C40SHDR2"}, {"VideoCc4_OV48C40SHDR2"}},
                                             .stillCC = {{"StillCc0_OV48C40SHDR2"}, {"StillCc1_OV48C40SHDR2"}, {"StillCc2_OV48C40SHDR2"}, {"StillCc3_OV48C40SHDR2"}, {"StillCc4_OV48C40SHDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40SHDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40SHDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40SHDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV48C40SHDR2"}, {"scene_data_s02_OV48C40SHDR2"}, {"scene_data_s03_OV48C40SHDR2"}, {"scene_data_s04_OV48C40SHDR2"}, {"scene_data_s05_OV48C40SHDR2"}},
                            .DEVideo = {{"de_default_video_OV48C40SHDR2"}},
                            .DEStill = {{"de_default_still_OV48C40SHDR2"}},
                        };
static IQ_TABLE_PATH_s OV48C40HDR2Path = {
                            .ADJTable = {"adj_table_param_default_OV48C40HDR2"},
                            .ImgParam = {"img_default_OV48C40HDR2"},
                            .aaaDefault = {{"aaa_default_00_OV48C40HDR2"}, {"aaa_default_01_OV48C40HDR2"}},
                            .video = {{"adj_video_default_00_OV48C40HDR2"}, {"adj_video_default_01_OV48C40HDR2"}, {"adj_video_default_02_OV48C40HDR2"}},
                            .photo = {{"adj_photo_default_00_OV48C40HDR2"}, {"adj_photo_default_01_OV48C40HDR2"}},
                            .stillLISO = {{"adj_still_default_00_OV48C40HDR2"}, {"adj_still_default_01_OV48C40HDR2"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OV48C40HDR2"}},
                            .stillIdxInfo = {"adj_still_idx_OV48C40HDR2"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OV48C40HDR2"}, {"VideoCc1_OV48C40HDR2"}, {"VideoCc2_OV48C40HDR2"}, {"VideoCc3_OV48C40HDR2"}, {"VideoCc4_OV48C40HDR2"}},
                                             .stillCC = {{"StillCc0_OV48C40HDR2"}, {"StillCc1_OV48C40HDR2"}, {"StillCc2_OV48C40HDR2"}, {"StillCc3_OV48C40HDR2"}, {"StillCc4_OV48C40HDR2"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_video_OV48C40HDR2"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40HDR2"}, {"cc3d_cc_bw_gamma_lin_still_OV48C40HDR2"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OV48C40HDR2"}, {"scene_data_s02_OV48C40HDR2"}, {"scene_data_s03_OV48C40HDR2"}, {"scene_data_s04_OV48C40HDR2"}, {"scene_data_s05_OV48C40HDR2"}},
                            .DEVideo = {{"de_default_video_OV48C40HDR2"}},
                            .DEStill = {{"de_default_still_OV48C40HDR2"}},
                        };
static IQ_TABLE_PATH_s OX03CSHDR4Path = {
                            .ADJTable = {"adj_table_param_default_OX03CSHDR4"},
                            .ImgParam = {"img_default_OX03CSHDR4"},
                            .aaaDefault = {{"aaa_default_00_OX03CSHDR4"}, {"aaa_default_01_OX03CSHDR4"}},
                            .video = {{"adj_video_default_00_OX03CSHDR4"}, {"adj_video_default_01_OX03CSHDR4"}, {"adj_video_default_02_OX03CSHDR4"}},
                            .photo = {{"adj_photo_default_00_OX03CSHDR4"}, {"adj_photo_default_01_OX03CSHDR4"}},
                            .stillLISO = {{"adj_still_default_00_OX03CSHDR4"}, {"adj_still_default_01_OX03CSHDR4"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX03CSHDR4"}},
                            .stillIdxInfo = {"adj_still_idx_OX03CSHDR4"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX03CSHDR4"}, {"VideoCc1_OX03CSHDR4"}, {"VideoCc2_OX03CSHDR4"}, {"VideoCc3_OX03CSHDR4"}, {"VideoCc4_OX03CSHDR4"}},
                                             .stillCC = {{"StillCc0_OX03CSHDR4"}, {"StillCc1_OX03CSHDR4"}, {"StillCc2_OX03CSHDR4"}, {"StillCc3_OX03CSHDR4"}, {"StillCc4_OX03CSHDR4"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX03CSHDR4"}, {"scene_data_s02_OX03CSHDR4"}, {"scene_data_s03_OX03CSHDR4"}, {"scene_data_s04_OX03CSHDR4"}, {"scene_data_s05_OX03CSHDR4"}},
                            .DEVideo = {{"de_default_video_OX03CSHDR4"}},
                            .DEStill = {{"de_default_still_OX03CSHDR4"}},
                        };
static IQ_TABLE_PATH_s OX03FSHDR4Path = {
                            .ADJTable = {"adj_table_param_default_OX03FSHDR4"},
                            .ImgParam = {"img_default_OX03FSHDR4"},
                            .aaaDefault = {{"aaa_default_00_OX03FSHDR4"}, {"aaa_default_01_OX03FSHDR4"}},
                            .video = {{"adj_video_default_00_OX03FSHDR4"}, {"adj_video_default_01_OX03FSHDR4"}, {"adj_video_default_02_OX03FSHDR4"}},
                            .photo = {{"adj_photo_default_00_OX03FSHDR4"}, {"adj_photo_default_01_OX03FSHDR4"}},
                            .stillLISO = {{"adj_still_default_00_OX03FSHDR4"}, {"adj_still_default_01_OX03FSHDR4"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX03FSHDR4"}},
                            .stillIdxInfo = {"adj_still_idx_OX03FSHDR4"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX03FSHDR4"}, {"VideoCc1_OX03FSHDR4"}, {"VideoCc2_OX03FSHDR4"}, {"VideoCc3_OX03FSHDR4"}, {"VideoCc4_OX03FSHDR4"}},
                                             .stillCC = {{"StillCc0_OX03FSHDR4"}, {"StillCc1_OX03FSHDR4"}, {"StillCc2_OX03FSHDR4"}, {"StillCc3_OX03FSHDR4"}, {"StillCc4_OX03FSHDR4"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX03FSHDR4"}, {"scene_data_s02_OX03FSHDR4"}, {"scene_data_s03_OX03FSHDR4"}, {"scene_data_s04_OX03FSHDR4"}, {"scene_data_s05_OX03FSHDR4"}},
                            .DEVideo = {{"de_default_video_OX03FSHDR4"}},
                            .DEStill = {{"de_default_still_OX03FSHDR4"}},
                        };
static IQ_TABLE_PATH_s OX03CSHDR4_ADASPath = {
                            .ADJTable = {"adj_table_param_default_OX03CSHDR4_ADAS"},
                            .ImgParam = {"img_default_OX03CSHDR4_ADAS"},
                            .aaaDefault = {{"aaa_default_00_OX03CSHDR4_ADAS"}, {"aaa_default_01_OX03CSHDR4_ADAS"}},
                            .video = {{"adj_video_default_00_OX03CSHDR4_ADAS"}, {"adj_video_default_01_OX03CSHDR4_ADAS"}, {"adj_video_default_02_OX03CSHDR4_ADAS"}},
                            .photo = {{"adj_photo_default_00_OX03CSHDR4_ADAS"}, {"adj_photo_default_01_OX03CSHDR4_ADAS"}},
                            .stillLISO = {{"adj_still_default_00_OX03CSHDR4_ADAS"}, {"adj_still_default_01_OX03CSHDR4_ADAS"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX03CSHDR4_ADAS"}},
                            .stillIdxInfo = {"adj_still_idx_OX03CSHDR4_ADAS"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX03CSHDR4_ADAS"}, {"VideoCc1_OX03CSHDR4_ADAS"}, {"VideoCc2_OX03CSHDR4_ADAS"}, {"VideoCc3_OX03CSHDR4_ADAS"}, {"VideoCc4_OX03CSHDR4_ADAS"}},
                                             .stillCC = {{"StillCc0_OX03CSHDR4_ADAS"}, {"StillCc1_OX03CSHDR4_ADAS"}, {"StillCc2_OX03CSHDR4_ADAS"}, {"StillCc3_OX03CSHDR4_ADAS"}, {"StillCc4_OX03CSHDR4_ADAS"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_ADAS"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_ADAS"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX03CSHDR4_ADAS"}, {"scene_data_s02_OX03CSHDR4_ADAS"}, {"scene_data_s03_OX03CSHDR4_ADAS"}, {"scene_data_s04_OX03CSHDR4_ADAS"}, {"scene_data_s05_OX03CSHDR4_ADAS"}},
                            .DEVideo = {{"de_default_video_OX03CSHDR4_ADAS"}},
                            .DEStill = {{"de_default_still_OX03CSHDR4_ADAS"}},
                        };
static IQ_TABLE_PATH_s OX03CSHDR4_EMRPath = {
                            .ADJTable = {"adj_table_param_default_OX03CSHDR4_EMR"},
                            .ImgParam = {"img_default_OX03CSHDR4_EMR"},
                            .aaaDefault = {{"aaa_default_00_OX03CSHDR4_EMR"}, {"aaa_default_01_OX03CSHDR4_EMR"}},
                            .video = {{"adj_video_default_00_OX03CSHDR4_EMR"}, {"adj_video_default_01_OX03CSHDR4_EMR"}, {"adj_video_default_02_OX03CSHDR4_EMR"}},
                            .photo = {{"adj_photo_default_00_OX03CSHDR4_EMR"}, {"adj_photo_default_01_OX03CSHDR4_EMR"}},
                            .stillLISO = {{"adj_still_default_00_OX03CSHDR4_EMR"}, {"adj_still_default_01_OX03CSHDR4_EMR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX03CSHDR4_EMR"}},
                            .stillIdxInfo = {"adj_still_idx_OX03CSHDR4_EMR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX03CSHDR4_EMR"}, {"VideoCc1_OX03CSHDR4_EMR"}, {"VideoCc2_OX03CSHDR4_EMR"}, {"VideoCc3_OX03CSHDR4_EMR"}, {"VideoCc4_OX03CSHDR4_EMR"}},
                                             .stillCC = {{"StillCc0_OX03CSHDR4_EMR"}, {"StillCc1_OX03CSHDR4_EMR"}, {"StillCc2_OX03CSHDR4_EMR"}, {"StillCc3_OX03CSHDR4_EMR"}, {"StillCc4_OX03CSHDR4_EMR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03CSHDR4_EMR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03CSHDR4_EMR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX03CSHDR4_EMR"}, {"scene_data_s02_OX03CSHDR4_EMR"}, {"scene_data_s03_OX03CSHDR4_EMR"}, {"scene_data_s04_OX03CSHDR4_EMR"}, {"scene_data_s05_OX03CSHDR4_EMR"}},
                            .DEVideo = {{"de_default_video_OX03CSHDR4_EMR"}},
                            .DEStill = {{"de_default_still_OX03CSHDR4_EMR"}},
                        };
static IQ_TABLE_PATH_s OX03FSHDR4_ADASPath = {
                            .ADJTable = {"adj_table_param_default_OX03FSHDR4_ADAS"},
                            .ImgParam = {"img_default_OX03FSHDR4_ADAS"},
                            .aaaDefault = {{"aaa_default_00_OX03FSHDR4_ADAS"}, {"aaa_default_01_OX03FSHDR4_ADAS"}},
                            .video = {{"adj_video_default_00_OX03FSHDR4_ADAS"}, {"adj_video_default_01_OX03FSHDR4_ADAS"}, {"adj_video_default_02_OX03FSHDR4_ADAS"}},
                            .photo = {{"adj_photo_default_00_OX03FSHDR4_ADAS"}, {"adj_photo_default_01_OX03FSHDR4_ADAS"}},
                            .stillLISO = {{"adj_still_default_00_OX03FSHDR4_ADAS"}, {"adj_still_default_01_OX03FSHDR4_ADAS"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX03FSHDR4_ADAS"}},
                            .stillIdxInfo = {"adj_still_idx_OX03FSHDR4_ADAS"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX03FSHDR4_ADAS"}, {"VideoCc1_OX03FSHDR4_ADAS"}, {"VideoCc2_OX03FSHDR4_ADAS"}, {"VideoCc3_OX03FSHDR4_ADAS"}, {"VideoCc4_OX03FSHDR4_ADAS"}},
                                             .stillCC = {{"StillCc0_OX03FSHDR4_ADAS"}, {"StillCc1_OX03FSHDR4_ADAS"}, {"StillCc2_OX03FSHDR4_ADAS"}, {"StillCc3_OX03FSHDR4_ADAS"}, {"StillCc4_OX03FSHDR4_ADAS"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_ADAS"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_ADAS"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_ADAS"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX03FSHDR4_ADAS"}, {"scene_data_s02_OX03FSHDR4_ADAS"}, {"scene_data_s03_OX03FSHDR4_ADAS"}, {"scene_data_s04_OX03FSHDR4_ADAS"}, {"scene_data_s05_OX03FSHDR4_ADAS"}},
                            .DEVideo = {{"de_default_video_OX03FSHDR4_ADAS"}},
                            .DEStill = {{"de_default_still_OX03FSHDR4_ADAS"}},
                        };
static IQ_TABLE_PATH_s OX03FSHDR4_EMRPath = {
                            .ADJTable = {"adj_table_param_default_OX03FSHDR4_EMR"},
                            .ImgParam = {"img_default_OX03FSHDR4_EMR"},
                            .aaaDefault = {{"aaa_default_00_OX03FSHDR4_EMR"}, {"aaa_default_01_OX03FSHDR4_EMR"}},
                            .video = {{"adj_video_default_00_OX03FSHDR4_EMR"}, {"adj_video_default_01_OX03FSHDR4_EMR"}, {"adj_video_default_02_OX03FSHDR4_EMR"}},
                            .photo = {{"adj_photo_default_00_OX03FSHDR4_EMR"}, {"adj_photo_default_01_OX03FSHDR4_EMR"}},
                            .stillLISO = {{"adj_still_default_00_OX03FSHDR4_EMR"}, {"adj_still_default_01_OX03FSHDR4_EMR"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX03FSHDR4_EMR"}},
                            .stillIdxInfo = {"adj_still_idx_OX03FSHDR4_EMR"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX03FSHDR4_EMR"}, {"VideoCc1_OX03FSHDR4_EMR"}, {"VideoCc2_OX03FSHDR4_EMR"}, {"VideoCc3_OX03FSHDR4_EMR"}, {"VideoCc4_OX03FSHDR4_EMR"}},
                                             .stillCC = {{"StillCc0_OX03FSHDR4_EMR"}, {"StillCc1_OX03FSHDR4_EMR"}, {"StillCc2_OX03FSHDR4_EMR"}, {"StillCc3_OX03FSHDR4_EMR"}, {"StillCc4_OX03FSHDR4_EMR"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_video_OX03FSHDR4_EMR"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_EMR"}, {"cc3d_cc_bw_gamma_lin_still_OX03FSHDR4_EMR"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX03FSHDR4_EMR"}, {"scene_data_s02_OX03FSHDR4_EMR"}, {"scene_data_s03_OX03FSHDR4_EMR"}, {"scene_data_s04_OX03FSHDR4_EMR"}, {"scene_data_s05_OX03FSHDR4_EMR"}},
                            .DEVideo = {{"de_default_video_OX03FSHDR4_EMR"}},
                            .DEStill = {{"de_default_still_OX03FSHDR4_EMR"}},
                        };
static IQ_TABLE_PATH_s OX05B1SPath = {
                            .ADJTable = {"adj_table_param_default_OX05B1S"},
                            .ImgParam = {"img_default_OX05B1S"},
                            .aaaDefault = {{"aaa_default_00_OX05B1S"}, {"aaa_default_01_OX05B1S"}},
                            .video = {{"adj_video_default_00_OX05B1S"}, {"adj_video_default_01_OX05B1S"}, {"adj_video_default_02_OX05B1S"}},
                            .photo = {{"adj_photo_default_00_OX05B1S"}, {"adj_photo_default_01_OX05B1S"}},
                            .stillLISO = {{"adj_still_default_00_OX05B1S"}, {"adj_still_default_01_OX05B1S"}},
                            .stillHISO = {{"adj_hiso_still_default_00_OX05B1S"}},
                            .stillIdxInfo = {"adj_still_idx_OX05B1S"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_OX05B1S"}, {"VideoCc1_OX05B1S"}, {"VideoCc2_OX05B1S"}, {"VideoCc3_OX05B1S"}, {"VideoCc4_OX05B1S"}},
                                             .stillCC = {{"StillCc0_OX05B1S"}, {"StillCc1_OX05B1S"}, {"StillCc2_OX05B1S"}, {"StillCc3_OX05B1S"}, {"StillCc4_OX05B1S"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_video_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_video_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_video_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_video_OX05B1S"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_still_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_still_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_still_OX05B1S"}, {"cc3d_cc_bw_gamma_lin_still_OX05B1S"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_OX05B1S"}, {"scene_data_s02_OX05B1S"}, {"scene_data_s03_OX05B1S"}, {"scene_data_s04_OX05B1S"}, {"scene_data_s05_OX05B1S"}},
                            .DEVideo = {{"de_default_video_OX05B1S"}},
                            .DEStill = {{"de_default_still_OX05B1S"}},
                        };
static IQ_TABLE_PATH_s K351PPath = {
                            .ADJTable = {"adj_table_param_default_K351P"},
                            .ImgParam = {"img_default_K351P"},
                            .aaaDefault = {{"aaa_default_00_K351P"}, {"aaa_default_01_K351P"}},
                            .video = {{"adj_video_default_00_K351P"}, {"adj_video_default_01_K351P"}, {"adj_video_default_02_K351P"}},
                            .photo = {{"adj_photo_default_00_K351P"}, {"adj_photo_default_01_K351P"}},
                            .stillLISO = {{"adj_still_default_00_K351P"}, {"adj_still_default_01_K351P"}},
                            .stillHISO = {{"adj_hiso_still_default_00_K351P"}},
                            .stillIdxInfo = {"adj_still_idx_K351P"},
                            .CCSetPaths = {
                                            {.videoCC = {{"VideoCc0_K351P"}, {"VideoCc1_K351P"}, {"VideoCc2_K351P"}, {"VideoCc3_K351P"}, {"VideoCc4_K351P"}},
                                             .stillCC = {{"StillCc0_K351P"}, {"StillCc1_K351P"}, {"StillCc2_K351P"}, {"StillCc3_K351P"}, {"StillCc4_K351P"}},
                                            },
                                            {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_K351P"}, {"cc3d_cc_bw_gamma_lin_video_K351P"}, {"cc3d_cc_bw_gamma_lin_video_K351P"}, {"cc3d_cc_bw_gamma_lin_video_K351P"}, {"cc3d_cc_bw_gamma_lin_video_K351P"}},
                                             .stillCC = {{"cc3d_cc_bw_gamma_lin_still_K351P"}, {"cc3d_cc_bw_gamma_lin_still_K351P"}, {"cc3d_cc_bw_gamma_lin_still_K351P"}, {"cc3d_cc_bw_gamma_lin_still_K351P"}, {"cc3d_cc_bw_gamma_lin_still_K351P"}},
                                            }
                                        },
                            .scene = {{"scene_data_s01_K351P"}, {"scene_data_s02_K351P"}, {"scene_data_s03_K351P"}, {"scene_data_s04_K351P"}, {"scene_data_s05_K351P"}},
                            .DEVideo = {{"de_default_video_K351P"}},
                            .DEStill = {{"de_default_still_K351P"}},
                        };                                             
static IQ_TABLE_PATH_s IMX415Path = {
                        .ADJTable = {"adj_table_param_default_IMX415"},
                        .ImgParam = {"img_default_IMX415"},
                        .aaaDefault = {{"aaa_default_00_IMX415"}, {"aaa_default_01_IMX415"}},
                        .video = {{"adj_video_default_00_IMX415"}, {"adj_video_default_01_IMX415"}, {"adj_video_default_02_IMX415"}},
                        .photo = {{"adj_photo_default_00_IMX415"}, {"adj_photo_default_01_IMX415"}},
                        .stillLISO = {{"adj_still_default_00_IMX415"}, {"adj_still_default_01_IMX415"}},
                        .stillHISO = {{"adj_hiso_still_default_00_IMX415"}},
                        .stillIdxInfo = {"adj_still_idx_IMX415"},                            
                        .CCSetPaths = {
                                        {.videoCC = {{"VideoCc0_IMX415"}, {"VideoCc1_IMX415"}, {"VideoCc2_IMX415"}, {"VideoCc3_IMX415"}, {"VideoCc4_IMX415"}},
                                         .stillCC = {{"StillCc0_IMX415"}, {"StillCc1_IMX415"}, {"StillCc2_IMX415"}, {"StillCc3_IMX415"}, {"StillCc4_IMX415"}},
                                        },
                                        {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_IMX415"}, {"cc3d_cc_bw_gamma_lin_video_IMX415"}, {"cc3d_cc_bw_gamma_lin_video_IMX415"}, {"cc3d_cc_bw_gamma_lin_video_IMX415"}, {"cc3d_cc_bw_gamma_lin_video_IMX415"}},
                                         .stillCC = {{"cc3d_cc_bw_gamma_lin_still_IMX415"}, {"cc3d_cc_bw_gamma_lin_still_IMX415"}, {"cc3d_cc_bw_gamma_lin_still_IMX415"}, {"cc3d_cc_bw_gamma_lin_still_IMX415"}, {"cc3d_cc_bw_gamma_lin_still_IMX415"}},
                                        }
                                    },
                        .scene = {{"scene_data_s01_IMX415"}, {"scene_data_s02_IMX415"}, {"scene_data_s03_IMX415"}, {"scene_data_s04_IMX415"}, {"scene_data_s05_IMX415"}},
                        .DEVideo = {{"de_default_video_IMX415"}},
                        .DEStill = {{"de_default_still_IMX415"}},
                    };
static IQ_TABLE_PATH_s GC2053Path = {
                        .ADJTable = {"adj_table_param_default_GC2053"},
                        .ImgParam = {"img_default_GC2053"},
                        .aaaDefault = {{"aaa_default_00_GC2053"}, {"aaa_default_01_GC2053"}},
                        .video = {{"adj_video_default_00_GC2053"}, {"adj_video_default_01_GC2053"}, {"adj_video_default_02_GC2053"}},
                        .photo = {{"adj_photo_default_00_GC2053"}, {"adj_photo_default_01_GC2053"}},
                        .stillLISO = {{"adj_still_default_00_GC2053"}, {"adj_still_default_01_GC2053"}},
                        .stillHISO = {{"adj_hiso_still_default_00_GC2053"}},
                        .stillIdxInfo = {"adj_still_idx_GC2053"},                            
                        .CCSetPaths = {
                                        {.videoCC = {{"VideoCc0_GC2053"}, {"VideoCc1_GC2053"}, {"VideoCc2_GC2053"}, {"VideoCc3_GC2053"}, {"VideoCc4_GC2053"}},
                                         .stillCC = {{"StillCc0_GC2053"}, {"StillCc1_GC2053"}, {"StillCc2_GC2053"}, {"StillCc3_GC2053"}, {"StillCc4_GC2053"}},
                                        },
                                        {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_GC2053"}, {"cc3d_cc_bw_gamma_lin_video_GC2053"}, {"cc3d_cc_bw_gamma_lin_video_GC2053"}, {"cc3d_cc_bw_gamma_lin_video_GC2053"}, {"cc3d_cc_bw_gamma_lin_video_GC2053"}},
                                         .stillCC = {{"cc3d_cc_bw_gamma_lin_still_GC2053"}, {"cc3d_cc_bw_gamma_lin_still_GC2053"}, {"cc3d_cc_bw_gamma_lin_still_GC2053"}, {"cc3d_cc_bw_gamma_lin_still_GC2053"}, {"cc3d_cc_bw_gamma_lin_still_GC2053"}},
                                        }
                                    },
                        .scene = {{"scene_data_s01_GC2053"}, {"scene_data_s02_GC2053"}, {"scene_data_s03_GC2053"}, {"scene_data_s04_GC2053"}, {"scene_data_s05_GC2053"}},
                        .DEVideo = {{"de_default_video_GC2053"}},
                        .DEStill = {{"de_default_still_GC2053"}},
                    };
static IQ_TABLE_PATH_s GC4653Path = {
                        .ADJTable = {"adj_table_param_default_GC4653"},
                        .ImgParam = {"img_default_GC4653"},
                        .aaaDefault = {{"aaa_default_00_GC4653"}, {"aaa_default_01_GC4653"}},
                        .video = {{"adj_video_default_00_GC4653"}, {"adj_video_default_01_GC4653"}, {"adj_video_default_02_GC4653"}},
                        .photo = {{"adj_photo_default_00_GC4653"}, {"adj_photo_default_01_GC4653"}},
                        .stillLISO = {{"adj_still_default_00_GC4653"}, {"adj_still_default_01_GC4653"}},
                        .stillHISO = {{"adj_hiso_still_default_00_GC4653"}},
                        .stillIdxInfo = {"adj_still_idx_GC4653"},                            
                        .CCSetPaths = {
                                        {.videoCC = {{"VideoCc0_GC4653"}, {"VideoCc1_GC4653"}, {"VideoCc2_GC4653"}, {"VideoCc3_GC4653"}, {"VideoCc4_GC4653"}},
                                         .stillCC = {{"StillCc0_GC4653"}, {"StillCc1_GC4653"}, {"StillCc2_GC4653"}, {"StillCc3_GC4653"}, {"StillCc4_GC4653"}},
                                        },
                                        {.videoCC = {{"cc3d_cc_bw_gamma_lin_video_GC4653"}, {"cc3d_cc_bw_gamma_lin_video_GC4653"}, {"cc3d_cc_bw_gamma_lin_video_GC4653"}, {"cc3d_cc_bw_gamma_lin_video_GC4653"}, {"cc3d_cc_bw_gamma_lin_video_GC4653"}},
                                         .stillCC = {{"cc3d_cc_bw_gamma_lin_still_GC4653"}, {"cc3d_cc_bw_gamma_lin_still_GC4653"}, {"cc3d_cc_bw_gamma_lin_still_GC4653"}, {"cc3d_cc_bw_gamma_lin_still_GC4653"}, {"cc3d_cc_bw_gamma_lin_still_GC4653"}},
                                        }
                                    },
                        .scene = {{"scene_data_s01_GC4653"}, {"scene_data_s02_GC4653"}, {"scene_data_s03_GC4653"}, {"scene_data_s04_GC4653"}, {"scene_data_s05_GC4653"}},
                        .DEVideo = {{"de_default_video_GC4653"}},
                        .DEStill = {{"de_default_still_GC4653"}},
                    };
#endif
