######################################################################
# Copyright (C) 2013 Esteban Tovagliari. All Rights Reserved.        #
# Licensed under the terms of the CDDL License.                      #
# See CDDL_LICENSE.txt for a copy of the license.                    #
######################################################################

######################################################
# ramen lib sources

IF( CMAKE_SYSTEM_NAME MATCHES "Linux")
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/ramen/version.hpp.in
                    ${PROJECT_BINARY_DIR}/ramen-${RAMEN_VERSION}/include/ramen/version.hpp)

    INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR}/ramen-${RAMEN_VERSION}/include)
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/ramen/version.hpp.in
                    ${PROJECT_BINARY_DIR}/Ramen-${RAMEN_VERSION}.app/Contents/include/ramen/version.hpp)

    INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR}/Ramen-${RAMEN_VERSION}.app/Contents/include)
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    MESSAGE( FATAL_ERROR "Windows not supported yet")
ELSE()
    MESSAGE( FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported yet")
ENDIF()

FILE(GLOB_RECURSE RAMEN_HEADERS ./*.h*)

SET( RAMEN_MOC_CLASSES	params/animated_param.hpp
                        params/aspect_ratio_param.hpp
                        params/bool_param.hpp
                        params/button_param.hpp
                        params/color_param.hpp
                        params/combo_group_param.hpp
                        params/composite_param.hpp
                        params/curve_param.hpp
                        params/file_param.hpp
                        params/float_param.hpp
                        params/float2_param.hpp
                        params/float3_param.hpp
                        params/group_param.hpp
                        params/image_format_param.hpp
                        params/inspector_param.hpp
                        params/motion_blur_param.hpp
                        params/numeric_param.hpp
                        params/ocio_colorspace_param.hpp
                        params/param.hpp
                        params/popup_param.hpp
                        params/proportional_param.hpp
                        params/separator_param.hpp
                        params/static_param.hpp
                        params/string_param.hpp
                        params/tab_group_param.hpp

                        ui/anim/anim_curves_view.hpp
                        ui/anim/anim_editor.hpp
                        ui/anim/track_model.hpp

                        ui/main_window.hpp
                        ui/time_controls.hpp
                        ui/user_interface.hpp
                        ui/anim/anim_editor_toolbar.hpp
                        ui/compview/composition_view.hpp
                        ui/compview/composition_view_toolbar.hpp
                        ui/dialogs/composition_settings_dialog.hpp
                        ui/dialogs/multiline_alert.hpp
                        ui/dialogs/preferences_dialog.hpp
                        ui/dialogs/render_composition_dialog.hpp
                        ui/dialogs/smooth_anim_curves_dialog.hpp
                        ui/inspector/inspector.hpp
                        ui/inspector/panel.hpp
                        ui/viewer/image_view/image_viewer_strategy.hpp
                        ui/viewer/image_view/image_view_toolbar.hpp
                        ui/viewer/viewer.hpp
                        ui/viewer/viewer_context.hpp
                        ui/viewer/viewer_strategy.hpp
                        ui/widgets/color_button.hpp
                        ui/widgets/color_picker.hpp
                        ui/widgets/color_swatch.hpp
                        ui/widgets/exposure_picker.hpp
                        ui/widgets/hue_picker.hpp
                        ui/widgets/saturation_value_picker.hpp
                        ui/widgets/container_widget.hpp
                        ui/widgets/double_spinbox.hpp
                        ui/widgets/eyedropper_button.hpp
                        ui/widgets/group_box.hpp
                        ui/widgets/label_separator.hpp
                        ui/widgets/line_edit.hpp
                        ui/widgets/image_format_widget.hpp
                        ui/widgets/ocio_colorspace_combo.hpp
                        ui/widgets/ocio_combo.hpp
                        ui/widgets/ocio_display_combo.hpp
                        ui/widgets/ocio_display_widget.hpp
                        ui/widgets/ocio_view_combo.hpp
                        ui/widgets/param_spinbox.hpp
                        ui/widgets/spinbox.hpp
                        ui/widgets/text_edit.hpp
                        ui/widgets/time_slider.hpp
                        )

SET( RAMEN_SOURCES      system/system.cpp

                        anim/any_curve.cpp
                        anim/clipboard.cpp
                        anim/curve_algorithm.cpp
                        anim/curve_to_half_lut.cpp
                        anim/float_curve.cpp
                        anim/float_key.cpp
                        anim/keyframe.cpp
                        anim/track.cpp
                        anim/util.cpp

                        app/application.cpp
                        app/composition.cpp
                        app/document.cpp
                        app/export_selected_nodes.cpp
                        app/import_composition.cpp
                        app/preferences.cpp

                        bezier/algorithm.cpp

                        core/name.cpp
                        core/types.cpp

                        filesystem/path.cpp
                        filesystem/path_sequence.cpp

                        ggems/ggveclib.c
                        ggems/nearestpoint.c
                        ggems/convex_opt.c

                        GL/gl.cpp
                        GL/glu.cpp
                        GL/Imath_gl.cpp
                        GL/lut3d.cpp
                        GL/program.cpp

                        half/halfExpLogTable.cpp

                        hash/generator.cpp

                        image/accumulator.cpp
                        image/agg_renderers.cpp
                        image/box_blur.cpp
                        image/buffer.cpp
                        image/channel_range.cpp
                        image/color.cpp
                        image/color_bars.cpp
                        image/denan.cpp
                        image/dilate.cpp
                        image/filters.cpp
                        image/format.cpp
                        image/gaussian_blur.cpp
                        image/init_image_processing.cpp
                        image/invalid_pixels.cpp
                        image/ocio_transform.cpp
                        image/percentile_filter.cpp
                        image/resize.cpp
                        image/smart_blur.cpp

                        image/processing.cpp

                        image/generic/affine_warp.cpp
                        image/generic/alpha_composite.cpp
                        image/generic/alpha_layer_modes.cpp
                        image/generic/apply_color_matrix.cpp
                        image/generic/apply_gamma_exp_log.cpp
                        image/generic/box_blur_channel.cpp
                        image/generic/composite.cpp
                        image/generic/convolve33.cpp
                        image/generic/gaussian_blur_channel.cpp
                        image/generic/gaussian_blur_rgb.cpp
                        image/generic/layer_modes.cpp
                        image/generic/math.cpp
                        image/generic/projective_warp.cpp
                        image/generic/resize.cpp
                        image/generic/samplers.cpp

                        image/sse2/affine_warp.cpp
                        image/sse2/box_blur.cpp
                        image/sse2/channel_math.cpp
                        image/sse2/composite.cpp
                        image/sse2/gaussian_blur.cpp
                        image/sse2/key_mix.cpp
                        image/sse2/layer_modes.cpp
                        image/sse2/math.cpp
                        image/sse2/projective_warp.cpp
                        image/sse2/resize.cpp
                        image/sse2/samplers.cpp
                        image/util.cpp
                        image/sse2/warp.cpp

                        imageio/exceptions.cpp
                        imageio/factory.cpp
                        imageio/format.cpp
                        imageio/multichannel_reader.cpp
                        imageio/reader.cpp

                        imageio/exr/exr_format.cpp
                        imageio/exr/exr_reader.cpp
                        imageio/exr/exr_writer.cpp

                        imageio/hdr/hdr_format.cpp
                        imageio/hdr/hdr_writer.cpp

                        imageio/jpg/jpg_format.cpp
                        imageio/jpg/jpg_reader.cpp
                        imageio/jpg/jpg_writer.cpp
                        imageio/jpg/jpg_size.cpp

                        imageio/oiio/oiio_reader.cpp
                        imageio/oiio/oiio_writer.cpp

                        imageio/png/png_format.cpp
                        imageio/png/png_writer.cpp

                        imageio/tga/tga_format.cpp
                        imageio/tga/tga_writer.cpp

                        imageio/tiff/tiff_format.cpp
                        imageio/tiff/tiff_writer.cpp

                        ImathExt/ImathBoxAlgo.cpp
                        ImathExt/ImathMatrixColorAlgo.cpp

                        manipulators/draw.cpp
                        manipulators/manipulable.cpp
                        manipulators/manipulator.cpp
                        manipulators/pick.cpp
                        manipulators/util.cpp

                        memory/image_cache.cpp
                        memory/manager.cpp
                        memory/pool.cpp

                        movieio/exceptions.cpp
                        movieio/factory.cpp
                        movieio/format.cpp
                        movieio/image_seq_reader.cpp
                        movieio/image_seq_writer.cpp
                        movieio/reader.cpp
                        movieio/writer.cpp

                        nodes/composite_node.cpp
                        nodes/edge.cpp
                        nodes/graph_algorithm.cpp
                        nodes/image_node.cpp
                        nodes/node.cpp
                        nodes/node_factory.cpp
                        nodes/node_graph.cpp
                        nodes/node_metaclass.cpp
                        nodes/node_plug.cpp
                        nodes/node_output_interface.cpp
                        nodes/node_visitor.cpp
                        nodes/world_node.cpp

                        noise/global_noise.cpp
                        noise/noise_base.cpp
                        noise/perlin.cpp
                        noise/permutation_array.cpp
                        noise/simplex_noise.cpp
                        noise/simplex_noise_generator.cpp

                        ocio/context.cpp
                        ocio/manager.cpp
                        ocio/gl_display_manager.cpp
                        ocio/gl_lut3d.cpp

                        params/animated_param.cpp
                        params/animated_param_command.cpp
                        params/aspect_ratio_param.cpp
                        params/bool_param.cpp
                        params/button_param.cpp
                        params/color_param.cpp
                        params/combo_group_param.cpp
                        params/composite_param.cpp
                        params/composite_parameterised.cpp
                        params/curve_param.cpp
                        params/file_param.cpp
                        params/float_param.cpp
                        params/float2_param.cpp
                        params/float3_param.cpp
                        params/group_param.cpp
                        params/image_format_param.cpp
                        params/inspector_param.cpp
                        params/motion_blur_info.cpp
                        params/motion_blur_param.cpp
                        params/numeric_param.cpp
                        params/ocio_colorspace_param.cpp
                        params/param.cpp
                        params/param_set.cpp
                        params/parameterised.cpp
                        params/popup_param.cpp
                        params/proportional_param.cpp
                        params/separator_param.cpp
                        params/static_param.cpp
                        params/static_param_command.cpp
                        params/string_param.cpp
                        params/tab_group_param.cpp

                        render/context.cpp
                        render/context_guard.cpp
                        render/image_node_renderer.cpp
                        render/render_sequence.cpp
                        render/render_thread.cpp

                        serialization/yaml.cpp
                        serialization/yaml_iarchive.cpp
                        serialization/yaml_node.cpp
                        serialization/yaml_oarchive.cpp

                        ui/add_node_command.cpp
                        ui/edit_commands.cpp
                        ui/events.cpp
                        ui/events_util.cpp
                        ui/graph_layout.cpp
                        ui/main_window.cpp
                        ui/nice_numbers.cpp
                        ui/node_menu.cpp
                        ui/palette.cpp
                        ui/render_composition.cpp
                        ui/time_controls.cpp
                        ui/tool.cpp
                        ui/user_interface.cpp
                        ui/viewport.cpp

                        ui/anim/anim_curves_view.cpp
                        ui/anim/anim_editor.cpp
                        ui/anim/anim_editor_command.cpp
                        ui/anim/anim_editor_select_tool.cpp
                        ui/anim/anim_editor_tool.cpp
                        ui/anim/anim_editor_toolbar.cpp
                        ui/anim/drag_keys_command.cpp
                        ui/anim/draw_curves_visitors.cpp
                        ui/anim/edit_keys_visitors.cpp
                        ui/anim/edit_tangents_visitors.cpp
                        ui/anim/pick_curves_visitors.cpp
                        ui/anim/track_model.cpp

                        ui/compview/composition_view.cpp
                        ui/compview/composition_view_commands.cpp
                        ui/compview/composition_view_layout.cpp
                        ui/compview/composition_view_toolbar.cpp
                        ui/compview/draw_pick_visitors.cpp
                        ui/compview/draw_pick_generic_node.cpp

                        ui/dialogs/composition_settings_dialog.cpp
                        ui/dialogs/multiline_alert.cpp
                        ui/dialogs/preferences_dialog.cpp
                        ui/dialogs/render_composition_dialog.cpp
                        ui/dialogs/smooth_anim_curves_dialog.cpp
                        ui/dialogs/splash_screen.cpp

                        ui/inspector/inspector.cpp
                        ui/inspector/panel.cpp
                        ui/inspector/panel_factory.cpp

                        ui/viewer/image_view/empty_image_strategy.cpp
                        ui/viewer/image_view/image.cpp
                        ui/viewer/image_view/image_strategy.cpp
                        ui/viewer/image_view/image_viewer_strategy.cpp
                        ui/viewer/image_view/image_view_toolbar.cpp
                        ui/viewer/image_view/tiled_image_strategy.cpp

                        ui/viewer/viewer_context.cpp
                        ui/viewer/viewer_strategy.cpp
                        ui/viewer/viewer.cpp

                        ui/widgets/color.cpp
                        ui/widgets/color_button.cpp
                        ui/widgets/color_picker.cpp
                        ui/widgets/color_swatch.cpp
                        ui/widgets/exposure_picker.cpp
                        ui/widgets/hue_picker.cpp
                        ui/widgets/saturation_value_picker.cpp
                        ui/widgets/container_widget.cpp
                        ui/widgets/double_spinbox.cpp
                        ui/widgets/eyedropper_button.cpp
                        ui/widgets/image_format_widget.cpp
                        ui/widgets/group_box.cpp
                        ui/widgets/label_separator.cpp
                        ui/widgets/line_edit.cpp

                        ui/widgets/ocio_colorspace_combo.cpp
                        ui/widgets/ocio_combo.cpp
                        ui/widgets/ocio_display_combo.cpp
                        ui/widgets/ocio_display_widget.cpp
                        ui/widgets/ocio_view_combo.cpp

                        ui/widgets/param_spinbox.cpp
                        ui/widgets/spinbox.cpp
                        ui/widgets/text_edit.cpp
                        ui/widgets/time_slider.cpp

                        undo/command.cpp
                        undo/stack.cpp

                        util/calculator.cpp
                        util/command_line_parser.cpp
                        util/edit_distance.cpp
                        util/error_log.cpp
                        util/string.cpp
                        )

IF( WITH_NODES)
    SET( RAMEN_MOC_CLASSES ${RAMEN_MOC_CLASSES} nodes/image/input/image_channels_param.hpp
                                                nodes/image/input/image_seq_param.hpp

                                                nodes/image/roto/roto_shape_param.hpp
                                                nodes/image/roto/shape_transform_param.hpp
                                                nodes/image/roto/toolbar.hpp

                                                params/transform2_param.hpp
                                                params/transform3_param.hpp
                                                )

    SET( RAMEN_SOURCES ${RAMEN_SOURCES}	imageio/import_multichannel_exr.cpp

                                        manipulators/circle_manipulator.cpp
                                        manipulators/point2_manipulator.cpp
                                        manipulators/quad_manipulator.cpp
                                        manipulators/transform2_manipulator.cpp

                                        nodes/image/areaop_node.cpp
                                        nodes/image/base_blur_node.cpp
                                        nodes/image/base_layer_node.cpp
                                        nodes/image/base_output_node.cpp
                                        nodes/image/base_warp_node.cpp
                                        nodes/image/distort_node.cpp
                                        nodes/image/generator_node.cpp
                                        nodes/image/keyer_node.cpp
                                        nodes/image/pointop_node.cpp
                                        nodes/image/xform_node.cpp
                                        nodes/image/xform2d_node.cpp

                                        nodes/image/channel/copy_channels_node.cpp
                                        nodes/image/channel/invert_node.cpp
                                        nodes/image/channel/premult_node.cpp
                                        nodes/image/channel/set_channels_node.cpp
                                        nodes/image/channel/set_matte_node.cpp

                                        nodes/image/color/adjust_hsv_node.cpp
                                        nodes/image/color/cdl_node.cpp
                                        nodes/image/color/channel_mixer_node.cpp
                                        nodes/image/color/clamp_colors_node.cpp
                                        nodes/image/color/color_correct_node.cpp
                                        nodes/image/color/color_match_node.cpp
                                        nodes/image/color/color_replace_node.cpp
                                        nodes/image/color/colorspace_node.cpp
                                        nodes/image/color/curves_node.cpp
                                        nodes/image/color/exposure_node.cpp
                                        nodes/image/color/gamma_node.cpp
                                        nodes/image/color/hsv_colorize_node.cpp
                                        nodes/image/color/ocio_colorspace_node.cpp
                                        nodes/image/color/ocio_file_transform_node.cpp
                                        nodes/image/color/tint_node.cpp

                                        nodes/image/distort/displace_node.cpp
                                        nodes/image/distort/turbulent_displace_node.cpp
                                        nodes/image/distort/twirl_node.cpp

                                        nodes/image/filter/blur_channels_node.cpp
                                        nodes/image/filter/blur_mask_node.cpp
                                        nodes/image/filter/blur_node.cpp
                                        nodes/image/filter/chroma_blur_node.cpp
                                        nodes/image/filter/gauss_blur_node.cpp
                                        nodes/image/filter/percentile_filter_node.cpp
                                        nodes/image/filter/smart_blur_node.cpp
                                        nodes/image/filter/unsharp_mask_node.cpp

                                        nodes/image/generate/checkerboard_node.cpp
                                        nodes/image/generate/color_bars_node.cpp
                                        nodes/image/generate/color_node.cpp
                                        nodes/image/generate/fractal_noise_node.cpp
                                        nodes/image/generate/gradient_node.cpp
                                        nodes/image/generate/grid_node.cpp
                                        nodes/image/generate/rgradient_node.cpp

                                        nodes/image/input/image_channels_param.cpp
                                        nodes/image/input/image_input_command.cpp
                                        nodes/image/input/image_seq_param.cpp
                                        nodes/image/input/input_node.cpp
                                        nodes/image/input/input_clip.cpp

                                        nodes/image/key/color_diff_keyer_node.cpp
                                        nodes/image/key/lum_key_node.cpp
                                        nodes/image/key/spill_suppressor_node.cpp

                                        nodes/image/layer/add_mix_layer_node.cpp
                                        nodes/image/layer/alpha_layer_node.cpp
                                        nodes/image/layer/key_mix_layer_node.cpp
                                        nodes/image/layer/layer_node.cpp

                                        nodes/image/lighting/glow_node.cpp

                                        nodes/image/matte/alpha_levels_node.cpp
                                        nodes/image/matte/alpha_ops_node.cpp

                                        nodes/image/output/output_node.cpp

                                        nodes/image/roto/convert_tool.cpp
                                        nodes/image/roto/create_tool.cpp
                                        nodes/image/roto/manipulator.cpp
                                        nodes/image/roto/null_tool.cpp
                                        nodes/image/roto/roto_commands.cpp
                                        nodes/image/roto/roto_node.cpp
                                        nodes/image/roto/roto_shape_param.cpp
                                        nodes/image/roto/select_tool.cpp
                                        nodes/image/roto/shape_transform_param.cpp
                                        nodes/image/roto/tool.cpp
                                        nodes/image/roto/rect_tool.cpp
                                        nodes/image/roto/toolbar.cpp
                                        nodes/image/roto/shape.cpp
                                        nodes/image/roto/scene.cpp
                                        nodes/image/roto/scene_renderer.cpp
                                        nodes/image/roto/triple.cpp

                                        nodes/image/tonemap/global_tonemap_node.cpp

                                        nodes/image/transform/camera_shake_node.cpp
                                        nodes/image/transform/crop_node.cpp
                                        nodes/image/transform/expand_node.cpp
                                        nodes/image/transform/flip_node.cpp
                                        nodes/image/transform/move2d_node.cpp
                                        nodes/image/transform/resize_node.cpp
                                        nodes/image/transform/transform_blur_node.cpp

                                        nodes/image/util/nop_node.cpp
                                        nodes/image/util/set_dod_node.cpp

                                        params/transform2_param.cpp
                                        params/transform3_param.cpp
                                        )
ENDIF()

######################################################
# platform specific

# platform sources
IF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # OSX
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows
ELSE()
    MESSAGE( FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported yet")
ENDIF()

######################################################
# ui files

SET( RAMEN_DESIGNER_FILES   ../designer/preferences.ui
                            ../designer/composition_settings.ui
                            ../designer/render_composition.ui
                            ../designer/smooth_anim_curves.ui
                            )

######################################################
# QT

QT4_WRAP_UI( RAMEN_DESIGNER_SOURCES ${RAMEN_DESIGNER_FILES})
QT4_WRAP_CPP( RAMEN_MOC_SOURCES	${RAMEN_MOC_CLASSES} OPTIONS ${MOC_EXTRA_ARGS})
QT4_ADD_RESOURCES( RAMEN_RESOURCES ${PROJECT_SOURCE_DIR}/resources/resources.qrc)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR})

######################################################
# ramen lib

ADD_LIBRARY( ramen SHARED   ${RAMEN_HEADERS}
                            ${RAMEN_SOURCES}
                            ${RAMEN_MOC_SOURCES}
                            ${RAMEN_DESIGNER_SOURCES}
                            ${RAMEN_PLATFORM_SOURCES}
                            ${RAMEN_RESOURCES}
                            )

SET_TARGET_PROPERTIES( ramen PROPERTIES
                             VERSION ${RAMEN_VERSION_MAJOR}.${RAMEN_VERSION_MINOR}.${RAMEN_VERSION_PATCH}
                             SOVERSION ${RAMEN_VERSION_MAJOR})

TARGET_LINK_LIBRARIES( ramen ${RAMEN_ALL_LIBS})

######################################################
# ramen binary

SET( RAMEN_APP_SOURCES	main.cpp)

ADD_EXECUTABLE( ramen.bin ${RAMEN_APP_SOURCES})
TARGET_LINK_LIBRARIES( ramen.bin ramen ${RAMEN_ALL_LIBS})

IF( CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET_TARGET_PROPERTIES( ramen PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                            ${PROJECT_BINARY_DIR}/ramen-${RAMEN_VERSION}/lib/)

    SET_TARGET_PROPERTIES( ramen.bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                            ${PROJECT_BINARY_DIR}/ramen-${RAMEN_VERSION}/bin/)
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    SET_TARGET_PROPERTIES( ramen PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                            ${PROJECT_BINARY_DIR}/Ramen-${RAMEN_VERSION}.app/Contents/lib/)

    SET_TARGET_PROPERTIES( ramen.bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                            ${PROJECT_BINARY_DIR}/Ramen-${RAMEN_VERSION}.app/Contents/MacOS/)

    SET_TARGET_PROPERTIES( ramen.bin PROPERTIES OUTPUT_NAME ramen)
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    MESSAGE( FATAL_ERROR "Windows not supported yet")
ELSE()
    MESSAGE( FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported yet")
ENDIF()
