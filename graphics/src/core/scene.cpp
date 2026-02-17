// ==============================================================================
// scene.cpp - تنفيذ نظام المشاهد والطبقات
// Scene & Layer System Implementation
// ==============================================================================

#include "../../include/core/scene.h"
#include <algorithm>

namespace sad {
namespace graphics {

// ==============================================================================
// Layer - تنفيذ الطبقة / Layer Implementation
// ==============================================================================

Layer::Layer(const std::string& name, Int32 zOrder)
    : m_name(name)
    , m_zOrder(zOrder)
    , m_visible(true)
    , m_opacity(1.0f)
    , m_parallax(1.0f, 1.0f)
    , m_nextEntityId(1)
    , m_needsSort(false)
{
}

UInt32 Layer::AddEntity(const Entity& entity) {
    Entity e = entity;
    e.id = m_nextEntityId++;
    m_entities.push_back(e);
    m_needsSort = true;
    return e.id;
}

UInt32 Layer::AddEntity(const std::string& name, Float32 x, Float32 y,
                         Float32 w, Float32 h) {
    Entity e;
    e.name = name;
    e.position.x = x;
    e.position.y = y;
    e.size.x = w;
    e.size.y = h;
    return AddEntity(e);
}

void Layer::RemoveEntity(UInt32 id) {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [id](const Entity& e) { return e.id == id; }),
        m_entities.end()
    );
}

Entity* Layer::GetEntity(UInt32 id) {
    for (auto& e : m_entities) {
        if (e.id == id) return &e;
    }
    return nullptr;
}

const Entity* Layer::GetEntity(UInt32 id) const {
    for (const auto& e : m_entities) {
        if (e.id == id) return &e;
    }
    return nullptr;
}

Entity* Layer::FindByName(const std::string& name) {
    for (auto& e : m_entities) {
        if (e.name == name) return &e;
    }
    return nullptr;
}

std::vector<Entity*> Layer::FindByTag(const std::string& tag) {
    std::vector<Entity*> result;
    for (auto& e : m_entities) {
        if (e.tag == tag) result.push_back(&e);
    }
    return result;
}

void Layer::SetParallax(Float32 x, Float32 y) {
    m_parallax.x = x;
    m_parallax.y = y;
}

void Layer::Clear() {
    m_entities.clear();
}

void Layer::Update(Float32 deltaTime) {
    for (auto& e : m_entities) {
        if (!e.active) continue;
        if (e.onUpdate) {
            e.onUpdate(e, deltaTime);
        }
    }
}

void Layer::Draw() const {
    if (!m_visible) return;
    
    for (const auto& e : m_entities) {
        if (!e.visible) continue;
        if (e.onDraw) {
            e.onDraw(e);
        }
    }
}

void Layer::SortEntities() {
    if (!m_needsSort) return;
    std::sort(m_entities.begin(), m_entities.end());
    m_needsSort = false;
}

// ==============================================================================
// Scene - تنفيذ المشهد / Scene Implementation
// ==============================================================================

Scene::Scene(const std::string& name)
    : m_name(name)
    , m_active(false)
    , m_camera(nullptr)
{
}

Layer* Scene::AddLayer(const std::string& name, Int32 zOrder) {
    auto layer = std::make_shared<Layer>(name, zOrder);
    m_layers.push_back(layer);
    SortLayers();
    return layer.get();
}

void Scene::RemoveLayer(const std::string& name) {
    m_layers.erase(
        std::remove_if(m_layers.begin(), m_layers.end(),
            [&name](const std::shared_ptr<Layer>& l) { return l->GetName() == name; }),
        m_layers.end()
    );
}

Layer* Scene::GetLayer(const std::string& name) {
    for (auto& l : m_layers) {
        if (l->GetName() == name) return l.get();
    }
    return nullptr;
}

const Layer* Scene::GetLayer(const std::string& name) const {
    for (const auto& l : m_layers) {
        if (l->GetName() == name) return l.get();
    }
    return nullptr;
}

void Scene::SetCamera(const Camera2DRef& camera) {
    m_camera = camera;
}

void Scene::OnEnter() {
    m_active = true;
    if (m_onEnter) m_onEnter();
}

void Scene::OnExit() {
    m_active = false;
    if (m_onExit) m_onExit();
}

void Scene::Update(Float32 deltaTime) {
    if (!m_active) return;
    
    if (m_camera) {
        m_camera->Update(deltaTime);
    }
    
    if (m_onUpdate) {
        m_onUpdate(deltaTime);
    }
    
    for (auto& layer : m_layers) {
        layer->SortEntities();
        layer->Update(deltaTime);
    }
}

void Scene::Draw() const {
    if (!m_active) return;
    
    if (m_onDraw) {
        m_onDraw();
    }
    
    for (const auto& layer : m_layers) {
        layer->Draw();
    }
}

void Scene::Clear() {
    m_layers.clear();
}

void Scene::SortLayers() {
    std::sort(m_layers.begin(), m_layers.end(),
        [](const std::shared_ptr<Layer>& a, const std::shared_ptr<Layer>& b) {
            return a->GetZOrder() < b->GetZOrder();
        });
}

// ==============================================================================
// SceneManager - مدير المشاهد / Scene Manager
// ==============================================================================

SceneManager::SceneManager() : m_currentScene(nullptr) {}

SceneManager& SceneManager::GetInstance() {
    static SceneManager instance;
    return instance;
}

Scene* SceneManager::AddScene(const std::string& name) {
    auto scene = std::make_shared<Scene>(name);
    m_scenes[name] = scene;
    return scene.get();
}

void SceneManager::RemoveScene(const std::string& name) {
    if (m_currentScene && m_currentScene->GetName() == name) {
        m_currentScene->OnExit();
        m_currentScene = nullptr;
    }
    m_scenes.erase(name);
}

Scene* SceneManager::GetScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) return it->second.get();
    return nullptr;
}

void SceneManager::SwitchTo(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it == m_scenes.end()) return;
    
    if (m_currentScene) {
        m_currentScene->OnExit();
    }
    
    m_currentScene = it->second.get();
    m_currentScene->OnEnter();
}

void SceneManager::Update(Float32 deltaTime) {
    if (m_currentScene) {
        m_currentScene->Update(deltaTime);
    }
}

void SceneManager::Draw() const {
    if (m_currentScene) {
        m_currentScene->Draw();
    }
}

} // namespace graphics
} // namespace sad
