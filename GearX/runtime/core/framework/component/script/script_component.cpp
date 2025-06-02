#include "script_component.hpp"
#include "../rigidbody/rigidbody_component.hpp"
#include "../texture/texture_component.hpp"
#include "../transform/transform_component.hpp"
#include "../../../../resources/loader/script_loader.hpp"

namespace GearX {
    RTTR_REGISTRATION{
        rttr::registration::class_<ScriptComponent>("ScriptComponent")
            .constructor<>()
            .method("getScriptUrls",&ScriptComponent::getScriptUrls)
            .method("addComponentTo", &ScriptComponent::addComponentTo)
            .method("removeScript", &ScriptComponent::removeScript)
            .method("addScript", &ScriptComponent::addScript)
            .method("getScript", &ScriptComponent::getScript)
            .method("clear",&ScriptComponent::clear);
    }
}
void GearX::ScriptComponent::addComponentTo(std::shared_ptr<GObject> obj){
    auto component = std::make_shared<ScriptComponent>();
    obj->addComponent(component);
}

void GearX::ScriptComponent::removeScript(std::string& url){
    int index = -1;
    for (int i = 0; i < script_assets.size();i++) {
        if (script_assets[i].asset_url == url) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        script_assets.erase(script_assets.begin() + index);
    }

}

void GearX::ScriptComponent::clear(){
    script_assets.clear();
}

void GearX::ScriptComponent::addScript(std::string& url){
    for (auto& i : script_assets) {
        if (i.asset_url == url) {
            return;
        }
    }
	script_assets.push_back(RuntimeGlobalContext::assetManager.loadAssetScript(url));
}

void GearX::ScriptComponent::removeScriptFromBeginContect(std::string& url){
    int index = -1;
    for (int i = 0; i < script_begin_contact.size(); i++) {
        if (script_begin_contact[i].asset_url == url) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        script_begin_contact.erase(script_begin_contact.begin() + index);
    }
}

void GearX::ScriptComponent::addScriptToBeginContact(std::string& url){
    for (auto& i : script_begin_contact) {
        if (i.asset_url == url) {
            return;
        }
    }
    script_begin_contact.push_back(RuntimeGlobalContext::assetManager.loadAssetScript(url));
}

void GearX::ScriptComponent::removeScriptFromEndContect(std::string& url){
    int index = -1;
    for (int i = 0; i < script_end_contact.size(); i++) {
        if (script_end_contact[i].asset_url == url) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        script_end_contact.erase(script_end_contact.begin() + index);
    }
}

void GearX::ScriptComponent::addScriptToEndContact(std::string& url){
    for (auto& i : script_end_contact) {
        if (i.asset_url == url) {
            return;
        }
    }
    script_end_contact.push_back(RuntimeGlobalContext::assetManager.loadAssetScript(url));
}

void GearX::ScriptComponent::addScriptToDoOnce(std::string& url){
    for (auto& i : script_do_once) {
        if (i.asset_url == url) {
            return;
        }
    }
    script_do_once.push_back(RuntimeGlobalContext::assetManager.loadAssetScript(url));
}

void GearX::ScriptComponent::removeScriptFromDoOnce(std::string& url){
    for (int i = 0; i < script_do_once.size(); i++) {
        if (script_do_once[i].asset_url == url) {
            script_do_once.erase(script_do_once.begin() + i);
            break;
        }
    }
}

void GearX::ScriptComponent::reloadScripts(){
    for (auto i : script_assets) {
        if (!i.asset_url.empty() || i.data) {
            static_cast<ScriptHolder*>(i.data)->reload(i.asset_url);
        }
    }
    for (auto& i : script_begin_contact) {
        if (!i.asset_url.empty() || i.data) {
            static_cast<ScriptHolder*>(i.data)->reload(i.asset_url);
        }
    }
    for (auto& i : script_end_contact) {
        if (!i.asset_url.empty() || i.data) {
            static_cast<ScriptHolder*>(i.data)->reload(i.asset_url);
        }
    }
    for (auto& i : script_do_once) {
        if (!i.asset_url.empty() || i.data) {
            static_cast<ScriptHolder*>(i.data)->reload(i.asset_url);
        }
    }
}

void GearX::ScriptComponent::loadScript(){
    for (auto& script : script_assets) {
        if (script.data == nullptr) {
            script = RuntimeGlobalContext::assetManager.loadAssetScript(script.asset_url);
        }
    }
    for (auto& script : script_begin_contact) {
        if (script.data == nullptr) {
            script = RuntimeGlobalContext::assetManager.loadAssetScript(script.asset_url);
        }
    }
    for (auto& script : script_end_contact) {
        if (script.data == nullptr) {
            script = RuntimeGlobalContext::assetManager.loadAssetScript(script.asset_url);
        }
    }
    for (auto& script : script_do_once) {
        if (script.data == nullptr) {
            script = RuntimeGlobalContext::assetManager.loadAssetScript(script.asset_url);
        }
    }
}
