set(MY_LIB_NAME CarFramework)

ADD_FILTER(
CarFramework
    CarFramework.cpp
    CarFramework.h
    CarFrameworkPlugin.cpp
    CarFrameworkPlugin.h
    CarEventReceiver.cpp
    CarEventReceiver.h
    Car.h
    Car.cpp
    Tire.h
    Tire.cpp
)

ADD_FILTER(
Utils
    NodeCreationHelper.h
    NodeCreationHelper.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/GOAP/include)
INCLUDE_DIRECTORIES(${MENGINE_CMAKE_DEPENDENCIES_DIR}/GOAP/Config)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MYGAMEFRAMEWORK)

set_target_properties(${MY_LIB_NAME} PROPERTIES
    FOLDER Frameworks
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})