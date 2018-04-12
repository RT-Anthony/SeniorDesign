PROJECT(beaconbutton)

ARC_PATCH_ID(${PROJECT_NAME} 1)
ARC_USER_BUILD(${PROJECT_NAME} 0x4417)

ADD_LD_FLAGS(-Bsymin=${LIBS_DIR}/fota_update/static_functions.sym)

# Set the project directory.
SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

SET(${PROJECT_NAME}_SRCS
    src/beaconbutton.c
    src/buttons.c
    src/leds.c
    src/tmr.c
)

ARC_SYMBOLS(
    ${ROM_SYM}
)

APP_IN_RAM()

ARC_EXECUTABLE(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})

ARC_INCLUDE_DIRECTORIES(${PROJECT_NAME}
    ${PROJECT_DIR}/src
    ${LIBS_DIR}/alpw_ll/Interfaces
    ${LIBS_DIR}/alpw_ll/ExternalInterfaces
    ${LIBS_DIR}/alpw_ll/Source/Includes
    ${LIBS_DIR}/alpw_ll_int
    ${LIBS_DIR}/alpw_ll_int/platform_Integration
    ${LIBS_DIR}/alpw_ll_int/qp_Integration
    ${LIBS_DIR}/alpw_corestack/Interfaces
    ${LIBS_DIR}/alpw_corestack/Interfaces/ATT
    ${LIBS_DIR}/alpw_corestack_int/qp_Integration
    ${LIBS_DIR}/alpw_corestack_int/SingleChip/Aci/AciCommands
    ${LIBS_DIR}/alpw_ble_profiles/BLESW_FindMe/Interfaces
    ${LIBS_DIR}/alpw_ble_profiles/BLESW_EM9304FOTA/Interfaces
    ${LIBS_DIR}/alpw_corestack/Interfaces/Services
    ${LIBS_DIR}/LinkLayerSupport/includes
    ${LIBS_DIR}/security/includes
    ${LIBS_DIR}/alpw_corestack/Source/Includes
    ${LIBS_DIR}/InSystemPatching/includes
    ${LIBS_DIR}/isp_extension/includes
    ${LIBS_DIR}/fota_update/includes
    ${COMMON_INCLUDES}
)

# Include libraries required for the project
#   Required
ARC_LINK_LIBRARIES(${PROJECT_NAME} unitimer unitimer_patch isp_extension)

