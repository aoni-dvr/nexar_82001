obj-y += liblvgl.a

LVGL_SRC = ../../src/lvgl/src

#lv_core
liblvgl-objs := $(LVGL_SRC)/core/lv_group.o \
                $(LVGL_SRC)/core/lv_indev.o \
                $(LVGL_SRC)/core/lv_disp.o \
                $(LVGL_SRC)/core/lv_obj.o \
                $(LVGL_SRC)/core/lv_event.o \
                $(LVGL_SRC)/core/lv_indev_scroll.o \
                $(LVGL_SRC)/core/lv_obj_class.o \
                $(LVGL_SRC)/core/lv_obj_draw.o \
                $(LVGL_SRC)/core/lv_obj_pos.o \
                $(LVGL_SRC)/core/lv_obj_scroll.o \
                $(LVGL_SRC)/core/lv_obj_style.o \
                $(LVGL_SRC)/core/lv_obj_style_gen.o \
                $(LVGL_SRC)/core/lv_obj_tree.o \
                $(LVGL_SRC)/core/lv_refr.o \
                $(LVGL_SRC)/core/lv_theme.o

#lv_draw
liblvgl-objs += $(LVGL_SRC)/draw/lv_draw_arc.o \
                $(LVGL_SRC)/draw/lv_draw_img.o \
                $(LVGL_SRC)/draw/lv_draw_label.o \
                $(LVGL_SRC)/draw/lv_draw_line.o \
                $(LVGL_SRC)/draw/lv_draw_mask.o \
                $(LVGL_SRC)/draw/lv_draw_rect.o \
                $(LVGL_SRC)/draw/lv_draw_triangle.o \
                $(LVGL_SRC)/draw/lv_img_buf.o \
                $(LVGL_SRC)/draw/lv_img_cache.o \
                $(LVGL_SRC)/draw/lv_img_decoder.o \
				$(LVGL_SRC)/draw/lv_draw.o \
				$(LVGL_SRC)/draw/lv_draw_layer.o \
				$(LVGL_SRC)/draw/lv_draw_transform.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_blend.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_gradient.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_letter.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_polygon.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_arc.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_dither.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_img.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_line.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_rect.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_layer.o \
				$(LVGL_SRC)/draw/sw/lv_draw_sw_transform.o

# #lv_font
liblvgl-objs += $(LVGL_SRC)/font/lv_font.o \
                $(LVGL_SRC)/font/lv_font_fmt_txt.o \
                $(LVGL_SRC)/font/lv_font_loader.o \
                $(LVGL_SRC)/font/lv_font_montserrat_14.o \
                $(LVGL_SRC)/font/lv_font_sans_14_Amba.o \
                $(LVGL_SRC)/font/lv_font_sans_10_Amba.o \
                $(LVGL_SRC)/font/lv_font_sans_30_Amba.o

#lv_hal
liblvgl-objs += $(LVGL_SRC)/hal/lv_hal_disp.o \
                $(LVGL_SRC)/hal/lv_hal_indev.o \
                $(LVGL_SRC)/hal/lv_hal_tick.o

#lv_misc
liblvgl-objs += $(LVGL_SRC)/misc/lv_anim.o \
                $(LVGL_SRC)/misc/lv_anim_timeline.o \
                $(LVGL_SRC)/misc/lv_area.o \
                $(LVGL_SRC)/misc/lv_async.o \
                $(LVGL_SRC)/misc/lv_bidi.o \
                $(LVGL_SRC)/misc/lv_color.o \
                $(LVGL_SRC)/misc/lv_fs.o \
                $(LVGL_SRC)/misc/lv_gc.o \
                $(LVGL_SRC)/misc/lv_ll.o \
                $(LVGL_SRC)/misc/lv_log.o \
				$(LVGL_SRC)/misc/lv_lru.o \
                $(LVGL_SRC)/misc/lv_math.o \
                $(LVGL_SRC)/misc/lv_mem.o \
                $(LVGL_SRC)/misc/lv_printf.o \
                $(LVGL_SRC)/misc/lv_style.o \
                $(LVGL_SRC)/misc/lv_style_gen.o \
                $(LVGL_SRC)/misc/lv_timer.o \
                $(LVGL_SRC)/misc/lv_tlsf.o \
                $(LVGL_SRC)/misc/lv_txt.o \
                $(LVGL_SRC)/misc/lv_txt_ap.o \
                $(LVGL_SRC)/misc/lv_utils.o \
				$(LVGL_SRC)/misc/lv_templ.o

#lv_widgets
liblvgl-objs += $(LVGL_SRC)/widgets/lv_arc.o \
                $(LVGL_SRC)/widgets/lv_bar.o \
                $(LVGL_SRC)/widgets/lv_btn.o \
                $(LVGL_SRC)/widgets/lv_btnmatrix.o \
                $(LVGL_SRC)/widgets/lv_canvas.o \
                $(LVGL_SRC)/widgets/lv_checkbox.o \
                $(LVGL_SRC)/widgets/lv_dropdown.o \
                $(LVGL_SRC)/widgets/lv_img.o \
                $(LVGL_SRC)/widgets/lv_label.o \
                $(LVGL_SRC)/widgets/lv_line.o \
                $(LVGL_SRC)/widgets/lv_roller.o \
                $(LVGL_SRC)/widgets/lv_slider.o \
                $(LVGL_SRC)/widgets/lv_switch.o \
                $(LVGL_SRC)/widgets/lv_table.o \
                $(LVGL_SRC)/widgets/lv_textarea.o \
				$(LVGL_SRC)/widgets/lv_objx_templ.o

liblvgl-objs += $(LVGL_SRC)/extra/lv_extra.o \
                $(LVGL_SRC)/extra/themes/default/lv_theme_default.o \
                $(LVGL_SRC)/extra/layouts/flex/lv_flex.o \
                $(LVGL_SRC)/extra/layouts/grid/lv_grid.o \
                $(LVGL_SRC)/extra/themes/basic/lv_theme_basic.o \
                $(LVGL_SRC)/extra/themes/mono/lv_theme_mono.o \
                $(LVGL_SRC)/extra/widgets/animimg/lv_animimg.o \
                $(LVGL_SRC)/extra/widgets/calendar/lv_calendar.o \
                $(LVGL_SRC)/extra/widgets/calendar/lv_calendar_header_arrow.o \
                $(LVGL_SRC)/extra/widgets/calendar/lv_calendar_header_dropdown.o \
                $(LVGL_SRC)/extra/widgets/chart/lv_chart.o \
                $(LVGL_SRC)/extra/widgets/colorwheel/lv_colorwheel.o \
                $(LVGL_SRC)/extra/widgets/imgbtn/lv_imgbtn.o \
                $(LVGL_SRC)/extra/widgets/keyboard/lv_keyboard.o \
                $(LVGL_SRC)/extra/widgets/led/lv_led.o \
                $(LVGL_SRC)/extra/widgets/list/lv_list.o \
                $(LVGL_SRC)/extra/widgets/meter/lv_meter.o \
                $(LVGL_SRC)/extra/widgets/msgbox/lv_msgbox.o \
                $(LVGL_SRC)/extra/widgets/span/lv_span.o \
                $(LVGL_SRC)/extra/widgets/spinbox/lv_spinbox.o \
                $(LVGL_SRC)/extra/widgets/spinner/lv_spinner.o \
                $(LVGL_SRC)/extra/widgets/tabview/lv_tabview.o \
                $(LVGL_SRC)/extra/widgets/tileview/lv_tileview.o \
                $(LVGL_SRC)/extra/widgets/win/lv_win.o \
				$(LVGL_SRC)/extra/widgets/menu/lv_menu.o \
                $(LVGL_SRC)/extra/others/monkey/lv_monkey.o \
                $(LVGL_SRC)/extra/others/gridnav/lv_gridnav.o \
                $(LVGL_SRC)/extra/others/snapshot/lv_snapshot.o \
				$(LVGL_SRC)/extra/others/fragment/lv_fragment.o \
				$(LVGL_SRC)/extra/others/fragment/lv_fragment_manager.o \
				$(LVGL_SRC)/extra/others/ime/lv_ime_pinyin.o \
				$(LVGL_SRC)/extra/others/imgfont/lv_imgfont.o \
				$(LVGL_SRC)/extra/others/msg/lv_msg.o


liblvgl-objs += $(LVGL_SRC)/LvglWrap.o

ccflags-y += -I$(srctree)/vendors/foss/lvgl/
ccflags-y += -I$(srctree)/vendors/ambarella/inc
ccflags-y += -I$(srctree)/vendors
ccflags-y += -I$(srctree)/vendors/foss/lvgl/src/lvgl/src
ccflags-y += -I$(srctree)
ccflags-y += -I$(srctree)/svc/comsvc/misc
ccflags-y += -I$(srctree)/svc/comsvc/print
ccflags-y += -Wno-pointer-to-int-cast -Wno-declaration-after-statement -Wno-switch-default

install-files := liblvgl.a
