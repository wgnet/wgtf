cmake_minimum_required (VERSION 3.0)
project( plg_graph_editor )

#find_package(NGT REQUIRED)

INCLUDE( WGToolsProject )
INCLUDE( WGQtLibrary )

BW_QT_ADD_QRC(plg_graph_editor resources)
BW_GLOB_RESOURCE_FILES()

#LIST(APPEND TARGET_LIBRARIES core_generic_plugin core_qt_common core_ui_framework)

#include_directories   ( ${CMAKE_CURRENT_BINARY_DIR})
#include_directories   ( "src" )
#include_directories   (${ADDITIONAL_INCLUDES})

add_definitions       (-DNGT_INTEGRATION)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui *.xml)
SET(QT_RESOURCE_FILES
  ${QML_SRCS}
)

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

FILE(GLOB_RECURSE METADATA_SRCS "metadata/*.mpp")
SOURCE_GROUP("Metadata" FILES ${METADATA_SRCS})

SET( ALL_SRCS
	src/action.cpp                             
	src/action.h                               
	src/basemodel.h                            
	src/connection_item.cpp                    
	src/connection_item.h                      
	src/connection_manager.cpp                 
	src/connection_manager.h                   
	src/connection_slot.cpp                    
	src/connection_slot.h                      
	src/connector.cpp                          
	src/connector.h                            
	src/graph_editor.cpp                       
	src/graph_editor.h                         
	src/graph_node.cpp                         
	src/graph_node.h                           
	src/quick_items_manager.cpp                
	src/quick_items_manager.h                  
	src/type_registration.cpp                  
	src/type_registration.h    
	plg_graph_editor.cpp
)                                     
SOURCE_GROUP( "Source Files" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_graph_editor SHARED
	${ALL_SRCS}
	${METADATA_SRCS}
	${QT_RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_graph_editor PRIVATE
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
	Qt5::Core
    Qt5::Gui
    Qt5::QuickWidgets
    Qt5::Quick

)

BW_PROJECT_CATEGORY( plg_graph_editor "Plugins" )



#qt5_add_resources ( RCC_APP_SRCS  ${RCC_LIST} )

#set ( Generated          ${UI_APP_HDRS} ${RCC_APP_SRCS} )

#define_source_folders          ( SRC_ROOT "Classes" "Classes/Metadata" GLOBAL_ERASE_FOLDER )
#generate_source_groups_project ( SRC_ROOT "Classes" "Classes/Metadata" "DataQt" Generated)

#set( ADDED_SRC            ${QML_LIST} ${UI_APP_HDRS} ${RCC_APP_SRCS} ${META_LIST})

#append_deploy_dependency(${PROJECT_NAME})
#setup_main_dynlib()

#BW_GET_EXECUTABLE_DST_DIR(EXE_DIR)
#BW_SET_BINARY_DIR(${PROJECT_NAME} "${EXE_DIR}/plugins")
#if (MACOS)
#  BW_COPY_PLUGIN_TO_BUNDLE(${PROJECT_NAME})
#endif()




