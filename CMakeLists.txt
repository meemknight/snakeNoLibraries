cmake_minimum_required(VERSION 3.13)

project("snake")
add_executable("snake")

#"-machine:x64"
#"-O2"
#"-arch:AVX"

#"-GL" whole program optimization not supported

target_link_options("snake" PRIVATE "-nodefaultlib" "-incremental:no" "-debug" "-subsystem:windows" "-opt:ref"  "-manifest:no" "user32.lib" "kernel32.lib")
target_compile_options("snake" PRIVATE "-Zi" "-GF" "-GS-" "-GR-" "-EHsca-" "-nologo" "-arch:IA32" "-fp:fast" "-fp:except-" "-Oi" "-Ob3")

#https://stackoverflow.com/questions/8587764/remove-runtime-checks-compiler-flag-per-project-in-cmake

foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    STRING (REGEX REPLACE "[/|-]RTC(su|[1suc])" "" ${flag_var} "${${flag_var}}")
endforeach(flag_var) #remiove basic runtime error checks
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    STRING (REGEX REPLACE "/GR" "" ${flag_var} "${${flag_var}}")
endforeach(flag_var)#remove rtti
#set_property(TARGET "flags" APPEND_STRING PROPERTY COMPILE_FLAGS)



#target_compile_options("flags" PRIVATE "-nodefaultlib -incremental:no -opt:ref -machine:x64 -STACK:0x100000,0x100000 -manifest:no -subsystem:console user32.lib kernel32.lib")
#target_link_options("flags" PRIVATE "-Zi -d2Zi+ -Gy -GF -Gs9999999 -GS- -GR- -EHs- -EHc- -EHa- -WX -W4 -nologo -FC -Gm- -diagnostics:column -fp:except- -fp:fast")

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp")

target_include_directories("snake" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
set_property(TARGET "snake" PROPERTY CXX_STANDARD 17)
target_sources("snake" PRIVATE ${MY_SOURCES}  )