#include "component.hpp"
#include "rigidbody/rigidbody_component.hpp"
#include "texture/texture_component.hpp"
#include "transform/transform_component.hpp"
#include "script/script_component.hpp"
using namespace rttr;

RTTR_REGISTRATION
{
registration::class_<GearX::Component>("Component")
.method("getSharedPtr", &GearX::Component::getSharedPtr) // ע���Ա����
.method("isDirty", &GearX::Component::isDirty) // ע���Ա����
.method("getTypeName", &GearX::Component::getTypeName) // ע���Ա����
.method("setParentObject", &GearX::Component::setParentObject); // ע���Ա����
}
std::map<std::string, std::string> GearX::CCMap({
	{ rttr::type::get<GearX::Component>().get_name().c_str(),u8"���"},
	{ rttr::type::get<GearX::TransformComponent>().get_name().c_str(),u8"�任" },
	{ rttr::type::get<GearX::TextureComponent>().get_name().c_str(),u8"����" },
	{ rttr::type::get<GearX::RigidBodyComponent>().get_name().c_str(),u8"����"},
	{ rttr::type::get<GearX::ScriptComponent>().get_name().c_str(),u8"�ű�"}
	});