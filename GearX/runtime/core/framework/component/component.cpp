#include "component.hpp"
#include "rigidbody/rigidbody_component.hpp"
#include "texture/texture_component.hpp"
#include "transform/transform_component.hpp"
#include "script/script_component.hpp"
using namespace rttr;

RTTR_REGISTRATION
{
registration::class_<GearX::Component>("Component")
.method("getSharedPtr", &GearX::Component::getSharedPtr) // 注册成员函数
.method("isDirty", &GearX::Component::isDirty) // 注册成员函数
.method("getTypeName", &GearX::Component::getTypeName) // 注册成员函数
.method("setParentObject", &GearX::Component::setParentObject); // 注册成员函数
}
std::map<std::string, std::string> GearX::CCMap({
	{ rttr::type::get<GearX::Component>().get_name().c_str(),u8"组件"},
	{ rttr::type::get<GearX::TransformComponent>().get_name().c_str(),u8"变换" },
	{ rttr::type::get<GearX::TextureComponent>().get_name().c_str(),u8"纹理" },
	{ rttr::type::get<GearX::RigidBodyComponent>().get_name().c_str(),u8"刚体"},
	{ rttr::type::get<GearX::ScriptComponent>().get_name().c_str(),u8"脚本"}
	});