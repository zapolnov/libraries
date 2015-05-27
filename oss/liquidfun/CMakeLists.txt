
cmake_minimum_required(VERSION 3.2)
project(liquidfun)

if(MSVC)
    add_definitions(/wd4456 /wd4457 /wd4458)
endif()

file(GLOB shapes_files Box2D/Box2D/Collision/Shapes/*.cpp Box2D/Box2D/Collision/Shapes/*.h)
file(GLOB collision_files Box2D/Box2D/Collision/*.cpp Box2D/Box2D/Collision/*.h)
file(GLOB common_files Box2D/Box2D/Common/*.cpp Box2D/Box2D/Common/*.h)
file(GLOB contacts_files Box2D/Box2D/Dynamics/Contacts/*.cpp Box2D/Box2D/Dynamics/Contacts/*.h)
file(GLOB joints_files Box2D/Box2D/Dynamics/Joints/*.cpp Box2D/Box2D/Dynamics/Joints/*.h)
file(GLOB dynamics_files Box2D/Box2D/Dynamics/*.cpp Box2D/Box2D/Dynamics/*.h)
file(GLOB particle_files Box2D/Box2D/Particle/*.cpp Box2D/Box2D/Particle/*.h)
file(GLOB rope_files Box2D/Box2D/Rope/*.cpp Box2D/Box2D/Rope/*.h)
file(GLOB root_files Box2D/Box2D/Box2D.h)

add_definitions(-DLIQUIDFUN_EXTERNAL_LANGUAGE_API=1)
#add_definitions(-DLIQUIDFUN_SIMD_NEON=1)

add_library(liquidfun STATIC ${EXCLUDE_FROM_ALL}
    ${shapes_files}
    ${collision_files}
    ${common_files}
    ${contacts_files}
    ${joints_files}
    ${dynamics_files}
    ${particle_files}
    ${rope_files}
    ${root_files}
)

source_group("Source Files\\Collision\\Shapes" FILES ${shapes_files})
source_group("Source Files\\Collision" FILES ${collision_files})
source_group("Source Files\\Common" FILES ${common_files})
source_group("Source Files\\Dynamics\\Contacts" FILES ${contacts_files})
source_group("Source Files\\Dynamics\\Joints" FILES ${joints_files})
source_group("Source Files\\Dynamics" FILES ${dynamics_files})
source_group("Source Files\\Particle" FILES ${particle_files})
source_group("Source Files\\Rope" FILES ${rope_files})
source_group("Source Files" FILES ${root_files})

target_include_directories(liquidfun BEFORE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Box2D")
