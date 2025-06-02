# Install script for directory: G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/GearX")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/access_levels.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/argument.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/array_mapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/constructor.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/destructor.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/enumeration.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/instance.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/method.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/policy.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/property.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/parameter_info.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/registration")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/registration.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/rttr_cast.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/rttr_enable.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/type")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/type.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/variant.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/variant_array_view.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/wrapper_mapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/array" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/array/array_accessor.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/array" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/array/array_accessor_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/array" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/array/array_mapper_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/array" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/array/array_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/array" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/array/array_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/base" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/base/core_prerequisites.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/base" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/test/Depencies/rttr/src/rttr/detail/base/version.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/base" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/test/Depencies/rttr/src/rttr/detail/base/version.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/constructor" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/constructor/constructor_invoker.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/constructor" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/constructor/constructor_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/constructor" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/constructor/constructor_wrapper_defaults.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/constructor" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/constructor/constructor_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/conversion" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/conversion/std_conversion_functions.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/conversion" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/conversion/number_conversion.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/default_arguments" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/default_arguments/default_arguments.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/default_arguments" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/default_arguments/invoke_with_defaults.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/destructor" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/destructor/destructor_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/destructor" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/destructor/destructor_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/enumeration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/enumeration/enumeration_helper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/enumeration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/enumeration/enumeration_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/enumeration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/enumeration/enumeration_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/enumeration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/enumeration/enum_data.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/impl" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/impl/argument_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/impl" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/impl/instance_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/impl" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/impl/rttr_cast_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/impl" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/impl/wrapper_mapper_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/metadata" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/metadata/metadata.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/metadata" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/metadata/metadata_handler.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/method" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/method/method_accessor.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/method" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/method/method_invoker.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/method" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/method/method_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/method" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/method/method_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/argument_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/argument_extractor.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/compare_equal.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/compare_equal_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/compare_less.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/compare_less_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/data_address_container.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/function_traits.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/misc_type_traits.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/std_type_traits.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/misc" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/misc/utility.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/parameter_info" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/parameter_info/parameter_infos.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/parameter_info" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/parameter_info/parameter_names.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/parameter_info" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/parameter_info/parameter_info_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/parameter_info" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/parameter_info/parameter_info_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/policies" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/policies/prop_policies.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/policies" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/policies/meth_policies.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/policies" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/policies/ctor_policies.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_accessor.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_wrapper.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_wrapper_base.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_wrapper_func.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_wrapper_member_func.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_wrapper_member_object.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/property" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/property/property_wrapper_object.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/registration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/registration/bind_types.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/registration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/registration/bind_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/registration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/registration/registration_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/registration" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/registration/registration_executer.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/accessor_type.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/base_classes.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/get_create_variant_func.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/get_derived_info_func.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/type_converter.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/type_comparator.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/type_register.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/type" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/type/type_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant/variant_compare_less.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant/variant_data.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant/variant_data_converter.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant/variant_data_policy.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant/variant_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant_array_view" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant_array_view/variant_array_view_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant_array_view" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant_array_view/variant_array_view_creator.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant_array_view" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant_array_view/variant_array_view_creator_impl.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/rttr/detail/variant_array_view" TYPE FILE PERMISSIONS OWNER_READ FILES "G:/Android_Project/CppAndroidApplication/GearX_dev/Depencies/rttr/src/rttr/detail/variant_array_view/variant_array_view_traits.h")
endif()

