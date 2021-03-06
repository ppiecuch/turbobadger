CONFIG += c++11
QT += widgets

TBROOT = ../src
INCLUDEPATH += $$TBROOT/tb

SOURCES += \
	$$TBROOT/tb/animation/tb_animation.cpp \
	$$TBROOT/tb/animation/tb_widget_animation.cpp \
	$$TBROOT/tb/image/tb_image_manager.cpp \
	$$TBROOT/tb/image/tb_image_widget.cpp \
	$$TBROOT/tb/parser/tb_parser.cpp \
	$$TBROOT/tb/renderers/tb_renderer_batcher.cpp \
	$$TBROOT/tb/renderers/tb_renderer_gl.cpp \
	$$TBROOT/tb/tb_addon.cpp \
	$$TBROOT/tb/tb_bitmap_fragment.cpp \
	$$TBROOT/tb/tb_clipboard_dummy.cpp \
	$$TBROOT/tb/tb_clipboard_glfw.cpp \
	$$TBROOT/tb/tb_clipboard_win.cpp \
	$$TBROOT/tb/tb_clipboard_qt.cpp \
	$$TBROOT/tb/tb_color.cpp \
	$$TBROOT/tb/tb_core.cpp \
	$$TBROOT/tb/tb_debug.cpp \
	$$TBROOT/tb/tb_dimension.cpp \
	$$TBROOT/tb/tb_editfield.cpp \
	$$TBROOT/tb/tb_file_posix.cpp \
	$$TBROOT/tb/tb_file_qt.cpp \
	$$TBROOT/tb/tb_font_renderer.cpp \
	$$TBROOT/tb/tb_font_renderer_freetype.cpp \
	$$TBROOT/tb/tb_font_renderer_stb.cpp \
	$$TBROOT/tb/tb_font_renderer_tbbf.cpp \
	$$TBROOT/tb/tb_geometry.cpp \
	$$TBROOT/tb/tb_hash.cpp \
	$$TBROOT/tb/tb_hashtable.cpp \
	$$TBROOT/tb/tb_id.cpp \
	$$TBROOT/tb/tb_image_loader_stb.cpp \
	$$TBROOT/tb/tb_inline_select.cpp \
	$$TBROOT/tb/tb_language.cpp \
	$$TBROOT/tb/tb_layout.cpp \
	$$TBROOT/tb/tb_linklist.cpp \
	$$TBROOT/tb/tb_list.cpp \
	$$TBROOT/tb/tb_menu_window.cpp \
	$$TBROOT/tb/tb_message_window.cpp \
	$$TBROOT/tb/tb_msg.cpp \
	$$TBROOT/tb/tb_node_ref_tree.cpp \
	$$TBROOT/tb/tb_node_tree.cpp \
	$$TBROOT/tb/tb_object.cpp \
	$$TBROOT/tb/tb_popup_window.cpp \
	$$TBROOT/tb/tb_renderer.cpp \
	$$TBROOT/tb/tb_scroll_container.cpp \
	$$TBROOT/tb/tb_scroller.cpp \
	$$TBROOT/tb/tb_select.cpp \
	$$TBROOT/tb/tb_select_item.cpp \
	$$TBROOT/tb/tb_skin.cpp \
	$$TBROOT/tb/tb_skin_util.cpp \
	$$TBROOT/tb/tb_str.cpp \
	$$TBROOT/tb/tb_style_edit.cpp \
	$$TBROOT/tb/tb_style_edit_content.cpp \
	$$TBROOT/tb/tb_system_android.cpp \
	$$TBROOT/tb/tb_system_linux.cpp \
	$$TBROOT/tb/tb_system_win.cpp \
	$$TBROOT/tb/tb_system_qt.cpp \
	$$TBROOT/tb/tb_tab_container.cpp \
	$$TBROOT/tb/tb_tempbuffer.cpp \
	$$TBROOT/tb/tb_toggle_container.cpp \
	$$TBROOT/tb/tb_value.cpp \
	$$TBROOT/tb/tb_widget_skin_condition_context.cpp \
	$$TBROOT/tb/tb_widget_value.cpp \
	$$TBROOT/tb/tb_widgets.cpp \
	$$TBROOT/tb/tb_widgets_common.cpp \
	$$TBROOT/tb/tb_widgets_listener.cpp \
	$$TBROOT/tb/tb_widgets_reader.cpp \
	$$TBROOT/tb/tb_window.cpp \
	$$TBROOT/tb/utf8/utf8.cpp \
	$$TBROOT/tb/tests/tb_test.cpp \
	$$TBROOT/tb/tests/test_tb_color.cpp \
	$$TBROOT/tb/tests/test_tb_dimension.cpp \
	$$TBROOT/tb/tests/test_tb_geometry.cpp \
	$$TBROOT/tb/tests/test_tb_linklist.cpp \
	$$TBROOT/tb/tests/test_tb_node_ref_tree.cpp \
	$$TBROOT/tb/tests/test_tb_object.cpp \
	$$TBROOT/tb/tests/test_tb_parser.cpp \
	$$TBROOT/tb/tests/test_tb_space_allocator.cpp \
	$$TBROOT/tb/tests/test_tb_style_edit.cpp \
	$$TBROOT/tb/tests/test_tb_tempbuffer.cpp \
	$$TBROOT/tb/tests/test_tb_test.cpp \
	$$TBROOT/tb/tests/test_tb_value.cpp \
	$$TBROOT/tb/tests/test_tb_widget_value.cpp

HEADERS += \
	$$TBROOT/tb/animation/tb_animation.h \
	$$TBROOT/tb/animation/tb_animation_utils.h \
	$$TBROOT/tb/animation/tb_widget_animation.h \
	$$TBROOT/tb/image/tb_image_manager.h \
	$$TBROOT/tb/image/tb_image_widget.h \
	$$TBROOT/tb/parser/tb_parser.h \
	$$TBROOT/tb/renderers/tb_renderer_batcher.h \
	$$TBROOT/tb/renderers/tb_renderer_gl.h \
	$$TBROOT/tb/tb_addon.h \
	$$TBROOT/tb/tb_bitmap_fragment.h \
	$$TBROOT/tb/tb_color.h \
	$$TBROOT/tb/tb_config.h \
	$$TBROOT/tb/tb_core.h \
	$$TBROOT/tb/tb_debug.h \
	$$TBROOT/tb/tb_dimension.h \
	$$TBROOT/tb/tb_editfield.h \
	$$TBROOT/tb/tb_font_desc.h \
	$$TBROOT/tb/tb_font_renderer.h \
	$$TBROOT/tb/tb_geometry.h \
	$$TBROOT/tb/tb_hash.h \
	$$TBROOT/tb/tb_hashtable.h \
	$$TBROOT/tb/tb_id.h \
	$$TBROOT/tb/tb_inline_select.h \
	$$TBROOT/tb/tb_language.h \
	$$TBROOT/tb/tb_layout.h \
	$$TBROOT/tb/tb_linklist.h \
	$$TBROOT/tb/tb_list.h \
	$$TBROOT/tb/tb_menu_window.h \
	$$TBROOT/tb/tb_message_window.h \
	$$TBROOT/tb/tb_msg.h \
	$$TBROOT/tb/tb_node_ref_tree.h \
	$$TBROOT/tb/tb_node_tree.h \
	$$TBROOT/tb/tb_object.h \
	$$TBROOT/tb/tb_popup_window.h \
	$$TBROOT/tb/tb_renderer.h \
	$$TBROOT/tb/tb_scroll_container.h \
	$$TBROOT/tb/tb_scroller.h \
	$$TBROOT/tb/tb_select.h \
	$$TBROOT/tb/tb_select_item.h \
	$$TBROOT/tb/tb_skin.h \
	$$TBROOT/tb/tb_skin_util.h \
	$$TBROOT/tb/tb_sort.h \
	$$TBROOT/tb/tb_str.h \
	$$TBROOT/tb/tb_style_edit.h \
	$$TBROOT/tb/tb_style_edit_content.h \
	$$TBROOT/tb/tb_system.h \
	$$TBROOT/tb/tb_tab_container.h \
	$$TBROOT/tb/tb_tempbuffer.h \
	$$TBROOT/tb/tb_toggle_container.h \
	$$TBROOT/tb/tb_types.h \
	$$TBROOT/tb/tb_value.h \
	$$TBROOT/tb/tb_widget_skin_condition_context.h \
	$$TBROOT/tb/tb_widget_value.h \
	$$TBROOT/tb/tb_widgets.h \
	$$TBROOT/tb/tb_widgets_common.h \
	$$TBROOT/tb/tb_widgets_listener.h \
	$$TBROOT/tb/tb_widgets_reader.h \
	$$TBROOT/tb/tb_window.h \
	$$TBROOT/tb/thirdparty/stb_image.h \
	$$TBROOT/tb/thirdparty/stb_truetype.h \
	$$TBROOT/tb/utf8/utf8.h \
	$$TBROOT/tb/tests/tb_test.h
