#### QNX

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_qrules.mk.html
#
# SRCVPATH will default to all the directories from the current working directory up to and including the project root directory.
#          You'd almost never want to set this; use EXTRA_SRCVPATH to add paths instead.
# Use EXTRA_SRCVPATH to add path of source code to serach.

include $(objtree)/include/config/auto.conf

LVGL_ROOT = $(srctree)/vendors/foss/lvgl

LVGL_SRC                   = $(LVGL_ROOT)/src/lvgl/src/
LVGL_CORE_SRC              = $(LVGL_ROOT)/src/lvgl/src/core
LVGL_DRAW_SRC              = $(LVGL_ROOT)/src/lvgl/src/draw
LVGL_DRAW_SW_SRC           = $(LVGL_ROOT)/src/lvgl/src/draw/sw
LVGL_FONT_SRC              = $(LVGL_ROOT)/src/lvgl/src/font
LVGL_HAL_SRC               = $(LVGL_ROOT)/src/lvgl/src/hal
LVGL_MISC_SRC              = $(LVGL_ROOT)/src/lvgl/src/misc
LVGL_WIDGETS_SRC           = $(LVGL_ROOT)/src/lvgl/src/widgets
LVGL_EXTRA_SRC             = $(LVGL_ROOT)/src/lvgl/src/extra
LVGL_EXTRA_DEFAULT_SRC     = $(LVGL_ROOT)/src/lvgl/src/extra/themes/default
LVGL_EXTRA_BASIC_SRC       = $(LVGL_ROOT)/src/lvgl/src/extra/themes/basic
LVGL_EXTRA_MONO_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/themes/mono
LVGL_EXTRA_FLEX_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/layouts/flex
LVGL_EXTRA_GRID_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/layouts/grid
LVGL_EXTRA_ANIMING_SRC     = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/animimg
LVGL_EXTRA_CALENDAR_SRC    = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/calendar
LVGL_EXTRA_CHAR_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/chart
LVGL_EXTRA_COLORWHEEL_SRC  = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/colorwheel
LVGL_EXTRA_IMGBTN_SRC      = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/imgbtn
LVGL_EXTRA_KEYBOARD_SRC    = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/keyboard
LVGL_EXTRA_LED_SRC         = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/led
LVGL_EXTRA_LIST_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/list
LVGL_EXTRA_METER_SRC       = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/meter
LVGL_EXTRA_MSGBOX_SRC      = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/msgbox
LVGL_EXTRA_SPAN_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/span
LVGL_EXTRA_SPINBOX_SRC     = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/spinbox
LVGL_EXTRA_SPINNER_SRC     = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/spinner
LVGL_EXTRA_TABVIEW_SRC     = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/tabview
LVGL_EXTRA_TOLEVIEW_SRC    = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/tileview
LVGL_EXTRA_WIN_SRC         = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/win
LVGL_EXTRA_MENU_SRC        = $(LVGL_ROOT)/src/lvgl/src/extra/widgets/menu
LVGL_EXTRA_MONKEY_SRC      = $(LVGL_ROOT)/src/lvgl/src/extra/others/monkey
LVGL_EXTRA_GRINDNAV_SRC    = $(LVGL_ROOT)/src/lvgl/src/extra/others/gridnav
LVGL_EXTRA_SNAPSHOT_SRC    = $(LVGL_ROOT)/src/lvgl/src/extra/others/snapshot
LVGL_EXTRA_FRAGMENT_SRC    = $(LVGL_ROOT)/src/lvgl/src/extra/others/fragment
LVGL_EXTRA_IME_SRC    	   = $(LVGL_ROOT)/src/lvgl/src/extra/others/ime
LVGL_EXTRA_IMGFONT_SRC     = $(LVGL_ROOT)/src/lvgl/src/extra/others/imgfont
LVGL_EXTRA_MSG_SRC         = $(LVGL_ROOT)/src/lvgl/src/extra/others/msg


EXTRA_SRCVPATH += $(LVGL_SRC)
EXTRA_SRCVPATH += $(LVGL_CORE_SRC)
EXTRA_SRCVPATH += $(LVGL_DRAW_SRC)
EXTRA_SRCVPATH += $(LVGL_DRAW_SW_SRC)
EXTRA_SRCVPATH += $(LVGL_FONT_SRC)
EXTRA_SRCVPATH += $(LVGL_HAL_SRC)
EXTRA_SRCVPATH += $(LVGL_MISC_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_SRC)
EXTRA_SRCVPATH += $(LVGL_WIDGETS_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_DEFAULT_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_BASIC_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_MONO_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_FLEX_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_GRID_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_ANIMING_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_CALENDAR_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_CHAR_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_COLORWHEEL_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_IMGBTN_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_KEYBOARD_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_LED_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_LIST_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_METER_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_MSGBOX_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_SPAN_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_SPINBOX_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_SPINNER_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_TABVIEW_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_TOLEVIEW_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_WIN_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_MENU_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_MONKEY_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_GRINDNAV_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_SNAPSHOT_SRC) 
EXTRA_SRCVPATH += $(LVGL_EXTRA_FRAGMENT_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_IME_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_IMGFONT_SRC)
EXTRA_SRCVPATH += $(LVGL_EXTRA_MSG_SRC)
# A space-separated list of source files to be compiled.
# Defaults to all *.s, *.S, *.c, and *.cc files in SRCVPATH.
SRCS =
#lv_core

SRCS += lv_group.c
SRCS += lv_indev.c
SRCS += lv_disp.c
SRCS += lv_obj.c
SRCS += lv_event.c
SRCS += lv_indev_scroll.c
SRCS += lv_obj_class.c
SRCS += lv_obj_draw.c
SRCS += lv_obj_pos.c
SRCS += lv_obj_scroll.c
SRCS += lv_obj_style.c
SRCS += lv_obj_style_gen.c
SRCS += lv_obj_tree.c
SRCS += lv_refr.c
SRCS += lv_theme.c

#lv_draw

SRCS += lv_draw_arc.c
SRCS += lv_draw_img.c
SRCS += lv_draw_label.c
SRCS += lv_draw_line.c
SRCS += lv_draw_mask.c
SRCS += lv_draw_rect.c
SRCS += lv_draw_triangle.c
SRCS += lv_img_buf.c
SRCS += lv_img_cache.c
SRCS += lv_img_decoder.c
SRCS += lv_draw.c
SRCS += lv_draw_layer.c
SRCS += lv_draw_transform.c
SRCS += lv_draw_sw.c
SRCS += lv_draw_sw_blend.c
SRCS += lv_draw_sw_gradient.c
SRCS += lv_draw_sw_letter.c
SRCS += lv_draw_sw_polygon.c
SRCS += lv_draw_sw_arc.c
SRCS += lv_draw_sw_dither.c
SRCS += lv_draw_sw_img.c
SRCS += lv_draw_sw_line.c
SRCS += lv_draw_sw_rect.c
SRCS += lv_draw_sw_layer.c
SRCS += lv_draw_sw_transform.c


#lv_font

SRCS += lv_font.c
SRCS += lv_font_fmt_txt.c
SRCS += lv_font_loader.c
SRCS += lv_font_montserrat_14.c
SRCS += lv_font_sans_14_Amba.c
SRCS += lv_font_sans_10_Amba.c
SRCS += lv_font_sans_30_Amba.c

#lv_hal

SRCS += lv_hal_disp.c
SRCS += lv_hal_indev.c
SRCS += lv_hal_tick.c

#lv_misc

SRCS += lv_anim.c
SRCS += lv_anim_timeline.c
SRCS += lv_area.c
SRCS += lv_async.c
SRCS += lv_bidi.c
SRCS += lv_color.c
SRCS += lv_fs.c
SRCS += lv_gc.c
SRCS += lv_ll.c
SRCS += lv_log.c
SRCS += lv_lru.c
SRCS += lv_math.c
SRCS += lv_mem.c
SRCS += lv_printf.c
SRCS += lv_style.c
SRCS += lv_style_gen.c
SRCS += lv_timer.c
SRCS += lv_tlsf.c
SRCS += lv_txt.c
SRCS += lv_txt_ap.c
SRCS += lv_utils.c
SRCS += lv_templ.c

#lv_widgets
SRCS += lv_arc.c
SRCS += lv_bar.c
SRCS += lv_btn.c
SRCS += lv_btnmatrix.c
SRCS += lv_canvas.c
SRCS += lv_checkbox.c
SRCS += lv_dropdown.c
SRCS += lv_img.c
SRCS += lv_label.c
SRCS += lv_line.c
SRCS += lv_roller.c
SRCS += lv_slider.c
SRCS += lv_switch.c
SRCS += lv_table.c
SRCS += lv_textarea.c
SRCS += lv_objx_templ.c


#lv_extra

SRCS += lv_extra.c
SRCS += lv_theme_default.c
SRCS += lv_theme_basic.c
SRCS += lv_theme_mono.c
SRCS += lv_flex.c
SRCS += lv_grid.c
SRCS += lv_animimg.c
SRCS += lv_calendar.c
SRCS += lv_calendar_header_arrow.c
SRCS += lv_calendar_header_dropdown.c
SRCS += lv_chart.c
SRCS += lv_colorwheel.c
SRCS += lv_imgbtn.c
SRCS += lv_keyboard.c
SRCS += lv_led.c
SRCS += lv_list.c
SRCS += lv_meter.c
SRCS += lv_msgbox.c
SRCS += lv_span.c
SRCS += lv_spinbox.c
SRCS += lv_spinner.c
SRCS += lv_tabview.c
SRCS += lv_tileview.c
SRCS += lv_win.c
SRCS += lv_menu.c
SRCS += lv_monkey.c
SRCS += lv_gridnav.c
SRCS += lv_snapshot.c
SRCS += lv_fragment.c
SRCS += lv_fragment_manager.c
SRCS += lv_ime_pinyin.c
SRCS += lv_imgfont.c
SRCS += lv_msg.c


# lvgl wrapper
SRCS += LvglWrap.c


EXTRA_INCVPATH +=$(srctree)/vendors/foss/lvgl/

EXTRA_INCVPATH += $(srctree)/vendors/foss/lvgl/src/ambarella/inc
EXTRA_INCVPATH += $(srctree)/vendors/foss/lvgl/src/ambarella/inc/std
EXTRA_INCVPATH += $(srctree)/vendors/ambarella/inc

EXTRA_INCVPATH += $(srctree)/vendors
EXTRA_INCVPATH += $(srctree)/vendors/foss/lvgl/src/
EXTRA_INCVPATH += $(srctree)/vendors/foss/lvgl/src/lvgl/src
EXTRA_INCVPATH += $(srctree)
EXTRA_INCVPATH += $(srctree)/svc/comsvc/misc
EXTRA_INCVPATH += $(srctree)/svc/comsvc/print

# cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_LIST_macro.html
# We use CPU levle here
LIST=CPU
include recurse.mk
