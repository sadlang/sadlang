// ============================================================================
// renderer3d.h - †״¸״§… ״§„״±״³… ״«„״§״« ״§„״£״¨״¹״§״¯
// 3D Renderer System
// ============================================================================
// ״§„ˆ״µ: †״¸״§… ״§„״±״³… ״§„״±״¦״³ „„״¹†״§״µ״± ״«„״§״«״© ״§„״£״¨״¹״§״¯
// Description: Main rendering system for 3D elements
// ============================================================================

#ifndef SAD_GRAPHICS_RENDERER3D_H
#define SAD_GRAPHICS_RENDERER3D_H

#include "../core/types.h"
#include "../core/math3d.h"
#include "mesh.h"
#include "camera3d.h"
#include "lighting.h"
#include "material.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>
#include <algorithm>

namespace sad {
namespace graphics {

// ============================================================================
// ״£…״± ״§„״±״³… / Draw Command
// ============================================================================

/// ״£…״± ״±״³… ˆ״§״­״¯ / Single draw command
struct DrawCommand {
    std::shared_ptr<Mesh> mesh;          ///< ״§„†…ˆ״°״¬ / Mesh
    std::shared_ptr<Material> material;  ///< ״§„…״§״¯״© / Material
    Transform3D transform;                ///< ״§„״×״­ˆ„ / Transform
    AABB bounds;                          ///< ״­״¯ˆ״¯ ״§„״±״³… / Bounds
    Float32 sortKey;                      ///< …״×״§״­ ״§„״×״±״×״¨ / Sort key
    Int32 layer;                          ///< ״§„״·״¨‚״© / Layer
    bool castShadows;                     ///< ״±… ״¸„״§„״§‹ / Casts shadows
    bool receiveShadows;                  ///< ״³״×‚״¨„ ״¸„״§„״§‹ / Receives shadows
    
    DrawCommand()
        : sortKey(0.0f)
        , layer(0)
        , castShadows(true)
        , receiveShadows(true)
    {}
};

/// ״·״§״¨ˆ״± ״§„״±״³… / Render queue
enum class RenderQueue {
    Background = 1000,      ///< ״§„״®„״© (״§„״³…״§״¡) / Background (sky)
    Geometry = 2000,        ///< ״§„‡†״¯״³״© ״§„…״¹״×…״© / Opaque geometry
    AlphaTest = 2450,       ///< ״§״®״×״¨״§״± ״£„״§ / Alpha test
    Transparent = 3000,     ///< ״§„״´״§ / Transparent
    Overlay = 4000          ///< ״§„״×״±״§ƒ״¨ (UI) / Overlay (UI)
};

// ============================================================================
// ״¥״­״µ״§״¦״§״× ״§„״±״³… / Render Statistics
// ============================================================================

/// ״¥״­״µ״§״¦״§״× ״§„״¥״·״§״± / Frame statistics
struct RenderStats {
    UInt32 drawCalls;           ///< ״¹״¯״¯ ״·„״¨״§״× ״§„״±״³… / Draw call count
    UInt32 triangles;           ///< ״¹״¯״¯ ״§„…״«„״«״§״× / Triangle count
    UInt32 vertices;            ///< ״¹״¯״¯ ״§„״±״₪ˆ״³ / Vertex count
    UInt32 meshesRendered;      ///< ״¹״¯״¯ ״§„†…״§״°״¬ ״§„…״±״³ˆ…״© / Meshes rendered
    UInt32 meshesCulled;        ///< ״¹״¯״¯ ״§„†…״§״°״¬ ״§„…‚״·ˆ״¹״© / Meshes culled
    UInt32 lightsActive;        ///< ״¹״¯״¯ ״§„״£״¶ˆ״§״¡ ״§„†״´״·״© / Active lights
    UInt32 shadowPasses;        ///< ……״±״§״× ״§„״¸„״§„ / Shadow passes
    Float32 frameTimeMs;        ///< ˆ‚״× ״§„״¥״·״§״± ״¨״§„…„ / Frame time in ms
    
    RenderStats() { Reset(); }
    
    void Reset() {
        drawCalls = 0;
        triangles = 0;
        vertices = 0;
        meshesRendered = 0;
        meshesCulled = 0;
        lightsActive = 0;
        shadowPasses = 0;
        frameTimeMs = 0.0f;
    }
};

// ============================================================================
// ״¥״¹״¯״§״¯״§״× ״§„״±״³… / Render Settings
// ============================================================================

/// ״¬ˆ״¯״© ״§„״¸„״§„ / Shadow quality
enum class ShadowQuality {
    Off,        ///< ״¨״¯ˆ† ״¸„״§„ / No shadows
    Low,        ///< …†״®״¶״© (512) / Low (512)
    Medium,     ///< …״×ˆ״³״·״© (1024) / Medium (1024)
    High,       ///< ״¹״§„״© (2048) / High (2048)
    Ultra       ///< ״§״¦‚״© (4096) / Ultra (4096)
};

/// ״¬ˆ״¯״© …״¶״§״¯״© ״§„״×״´ˆ״´ / Anti-aliasing quality
enum class AntiAliasingMode {
    None,       ///< ״¨״¯ˆ† / None
    FXAA,       ///< FXAA
    SMAA,       ///< SMAA
    TAA,        ///< TAA (Temporal)
    MSAA2x,     ///< MSAA 2x
    MSAA4x,     ///< MSAA 4x
    MSAA8x      ///< MSAA 8x
};

/// ״¥״¹״¯״§״¯״§״× ״§„״±״³… / Render settings
struct RenderSettings {
    // ״§„״¸„״§„ / Shadows
    ShadowQuality shadowQuality;
    UInt32 shadowCascades;
    Float32 shadowDistance;
    Float32 shadowBias;
    
    // …״¶״§״¯״© ״§„״×״´ˆ״´ / Anti-aliasing
    AntiAliasingMode antiAliasing;
    
    // …״§ ״¨״¹״¯ ״§„…״¹״§„״¬״© / Post-processing
    bool enableHDR;
    bool enableBloom;
    Float32 bloomThreshold;
    Float32 bloomIntensity;
    bool enableTonemap;
    Float32 exposure;
    Float32 gamma;
    
    // ״§„״¶״¨״§״¨ / Fog
    bool enableFog;
    
    // ״§„״£״¯״§״¡ / Performance
    bool enableFrustumCulling;
    bool enableOcclusionCulling;
    bool enableInstancing;
    bool enableLOD;
    
    // ״×״µ״­״­ ״§„״£״®״·״§״¡ / Debugging
    bool showWireframe;
    bool showBounds;
    bool showNormals;
    bool showLightVolumes;
    
    RenderSettings()
        : shadowQuality(ShadowQuality::Medium)
        , shadowCascades(3)
        , shadowDistance(100.0f)
        , shadowBias(0.005f)
        , antiAliasing(AntiAliasingMode::FXAA)
        , enableHDR(true)
        , enableBloom(true)
        , bloomThreshold(1.0f)
        , bloomIntensity(0.5f)
        , enableTonemap(true)
        , exposure(1.0f)
        , gamma(2.2f)
        , enableFog(false)
        , enableFrustumCulling(true)
        , enableOcclusionCulling(false)
        , enableInstancing(true)
        , enableLOD(true)
        , showWireframe(false)
        , showBounds(false)
        , showNormals(false)
        , showLightVolumes(false)
    {}
};

// ============================================================================
// ƒ״§״¦† ‚״§״¨„ „„״±״³… / Renderable Object
// ============================================================================

/// ƒ״§״¦† ‚״§״¨„ „„״±״³… / Renderable object
class Renderable {
public:
    Renderable() 
        : m_visible(true)
        , m_layer(0)
        , m_castShadows(true)
        , m_receiveShadows(true)
    {}
    
    virtual ~Renderable() = default;
    
    // ============================================
    // ״§„†…ˆ״°״¬ ˆ״§„…״§״¯״© / Mesh and Material
    // ============================================
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„†…ˆ״°״¬ / Get mesh
    std::shared_ptr<Mesh> GetMesh() const { return m_mesh; }
    void SetMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; UpdateBounds(); }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„…״§״¯״© / Get material
    std::shared_ptr<Material> GetMaterial() const { return m_material; }
    void SetMaterial(const std::shared_ptr<Material>& material) { m_material = material; }
    
    // ============================================
    // ״§„״×״­ˆ„ / Transform
    // ============================================
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„״×״­ˆ„ / Get transform
    const Transform3D& GetTransform() const { return m_transform; }
    Transform3D& GetTransform() { return m_transform; }
    void SetTransform(const Transform3D& transform) { m_transform = transform; UpdateBounds(); }
    
    // ״§„…ˆ‚״¹ / Position
    Vec3 GetPosition() const { return m_transform.position; }
    void SetPosition(const Vec3& pos) { m_transform.position = pos; UpdateBounds(); }
    void SetPosition(Float32 x, Float32 y, Float32 z) { SetPosition(Vec3(x, y, z)); }
    
    // ״§„״¯ˆ״±״§† / Rotation
    Quat GetRotation() const { return m_transform.rotation; }
    void SetRotation(const Quat& rot) { m_transform.rotation = rot; UpdateBounds(); }
    
    // ״§„…‚״§״³ / Scale
    Vec3 GetScale() const { return m_transform.scale; }
    void SetScale(const Vec3& scale) { m_transform.scale = scale; UpdateBounds(); }
    void SetScale(Float32 uniform) { SetScale(Vec3(uniform, uniform, uniform)); }
    
    // ============================================
    // ״§„״­״¯ˆ״¯ / Bounds
    // ============================================
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„״­״¯ˆ״¯ / Get bounds
    const AABB& GetBounds() const { return m_worldBounds; }
    
    /// ״×״­״¯״« ״§„״­״¯ˆ״¯ / Update bounds
    void UpdateBounds() {
        if (m_mesh) {
            // ״×״­ˆ„ ״­״¯ˆ״¯ ״§„†…ˆ״°״¬ ״¥„‰ ״¥״­״¯״§״«״§״× ״§„״¹״§„…
            // Transform mesh bounds to world coordinates
            m_worldBounds = m_mesh->GetBoundingBox().Transformed(m_transform.ToMatrix());
        }
    }
    
    // ============================================
    // ״§„״®״µ״§״¦״µ / Properties
    // ============================================
    
    /// ״§„״±״₪״© / Visibility
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    /// ״§„״·״¨‚״© / Layer
    Int32 GetLayer() const { return m_layer; }
    void SetLayer(Int32 layer) { m_layer = layer; }
    
    /// ״§„״¸„״§„ / Shadows
    bool CastsShadows() const { return m_castShadows; }
    void SetCastShadows(bool cast) { m_castShadows = cast; }
    
    bool ReceivesShadows() const { return m_receiveShadows; }
    void SetReceiveShadows(bool receive) { m_receiveShadows = receive; }
    
    // ============================================
    // ״¥†״´״§״¡ ״£…״± ״§„״±״³… / Create Draw Command
    // ============================================
    
    /// ״¥†״´״§״¡ ״£…״± ״§„״±״³… / Create draw command
    DrawCommand ToDrawCommand() const {
        DrawCommand cmd;
        cmd.mesh = m_mesh;
        cmd.material = m_material;
        cmd.transform = m_transform;
        cmd.bounds = m_worldBounds;
        cmd.layer = m_layer;
        cmd.castShadows = m_castShadows;
        cmd.receiveShadows = m_receiveShadows;
        return cmd;
    }
    
protected:
    std::shared_ptr<Mesh> m_mesh;         ///< ״§„†…ˆ״°״¬ / Mesh
    std::shared_ptr<Material> m_material; ///< ״§„…״§״¯״© / Material
    Transform3D m_transform;               ///< ״§„״×״­ˆ„ / Transform
    AABB m_worldBounds;                    ///< ״§„״­״¯ˆ״¯ ״§„״¹״§„…״© / World bounds
    bool m_visible;                        ///< …״±״¦ / Visible
    Int32 m_layer;                         ///< ״§„״·״¨‚״© / Layer
    bool m_castShadows;                    ///< ״±… ״¸„״§„ / Cast shadows
    bool m_receiveShadows;                 ///< ״³״×‚״¨„ ״¸„״§„ / Receive shadows
};

using RenderablePtr = std::shared_ptr<Renderable>;

// ============================================================================
// ״·״§״¨ˆ״± ״§„״±״³… / Render Queue Manager
// ============================================================================

/// …״¯״± ״·״§״¨ˆ״± ״§„״±״³… / Render queue manager
class RenderQueueManager {
public:
    /// …״³״­ ״§„״·ˆ״§״¨״± / Clear queues
    void Clear() {
        m_opaqueQueue.clear();
        m_transparentQueue.clear();
        m_backgroundQueue.clear();
        m_overlayQueue.clear();
    }
    
    /// ״¥״¶״§״© ״£…״± ״±״³… / Add draw command
    void Add(const DrawCommand& cmd) {
        // ״×״­״¯״¯ ״§„״·״§״¨ˆ״± ״¨†״§״¡‹ ״¹„‰ ״§„…״§״¯״© / Determine queue based on material
        if (cmd.material) {
            const auto& blend = cmd.material->GetRenderState().blend;
            if (blend.enabled) {
                m_transparentQueue.push_back(cmd);
            } else {
                m_opaqueQueue.push_back(cmd);
            }
        } else {
            m_opaqueQueue.push_back(cmd);
        }
    }
    
    /// ״§„״×״±״×״¨ / Sort queues
    void Sort(const Vec3& cameraPos) {
        // ״§„…״¹״×…: …† ״§„״£‚״±״¨ ״¥„‰ ״§„״£״¨״¹״¯ (״£״¶„ „„״£״¯״§״¡) / Opaque: front to back
        std::sort(m_opaqueQueue.begin(), m_opaqueQueue.end(),
            [&cameraPos](const DrawCommand& a, const DrawCommand& b) {
                Float32 distA = (a.bounds.Center() - cameraPos).LengthSq();
                Float32 distB = (b.bounds.Center() - cameraPos).LengthSq();
                return distA < distB;
            });
        
        // ״§„״´״§: …† ״§„״£״¨״¹״¯ ״¥„‰ ״§„״£‚״±״¨ („„…״²״¬ ״§„״µ״­״­) / Transparent: back to front
        std::sort(m_transparentQueue.begin(), m_transparentQueue.end(),
            [&cameraPos](const DrawCommand& a, const DrawCommand& b) {
                Float32 distA = (a.bounds.Center() - cameraPos).LengthSq();
                Float32 distB = (b.bounds.Center() - cameraPos).LengthSq();
                return distA > distB;
            });
    }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„״·ˆ״§״¨״± / Get queues
    const std::vector<DrawCommand>& GetOpaqueQueue() const { return m_opaqueQueue; }
    const std::vector<DrawCommand>& GetTransparentQueue() const { return m_transparentQueue; }
    const std::vector<DrawCommand>& GetBackgroundQueue() const { return m_backgroundQueue; }
    const std::vector<DrawCommand>& GetOverlayQueue() const { return m_overlayQueue; }
    
private:
    std::vector<DrawCommand> m_opaqueQueue;      ///< ״§„…״¹״×… / Opaque
    std::vector<DrawCommand> m_transparentQueue; ///< ״§„״´״§ / Transparent
    std::vector<DrawCommand> m_backgroundQueue;  ///< ״§„״®„״© / Background
    std::vector<DrawCommand> m_overlayQueue;     ///< ״§„״×״±״§ƒ״¨ / Overlay
};

// ============================================================================
// ……״± ״§„״±״³… / Render Pass
// ============================================================================

/// †ˆ״¹ ……״± ״§„״±״³… / Render pass type
enum class RenderPassType {
    ShadowMap,      ///< ״®״±״·״© ״§„״¸„״§„ / Shadow map
    GBuffer,        ///< G-Buffer (deferred)
    Lighting,       ///< ״§„״¥״¶״§״¡״© / Lighting
    Forward,        ///< ״£…״§… / Forward
    Transparent,    ///< ״§„״´״§ / Transparent
    PostProcess,    ///< …״§ ״¨״¹״¯ ״§„…״¹״§„״¬״© / Post-process
    UI              ///< ˆ״§״¬‡״© ״§„…״³״×״®״¯… / UI
};

/// ……״± ״±״³… / Render pass
class RenderPass {
public:
    RenderPass(RenderPassType type, const std::string& name)
        : m_type(type)
        , m_name(name)
        , m_enabled(true)
    {}
    
    virtual ~RenderPass() = default;
    
    /// ״×†״° ״§„……״± / Execute pass
    virtual void Execute() = 0;
    
    /// ״§„†ˆ״¹ / Type
    RenderPassType GetType() const { return m_type; }
    
    /// ״§„״§״³… / Name
    const std::string& GetName() const { return m_name; }
    
    /// …״¹„ / Enabled
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
protected:
    RenderPassType m_type;
    std::string m_name;
    bool m_enabled;
};

using RenderPassPtr = std::shared_ptr<RenderPass>;

// ============================================================================
// ״§„״±״³״§… ״«„״§״« ״§„״£״¨״¹״§״¯ / 3D Renderer
// ============================================================================

/// ״§„״±״³״§… ״«„״§״« ״§„״£״¨״¹״§״¯ ״§„״±״¦״³ / Main 3D renderer
class Renderer3D {
public:
    /// ״§„…†״´״¦ ״§„״§״×״±״§״¶ / Default constructor
    Renderer3D()
        : m_clearColor(0.1f, 0.1f, 0.15f)
    {}
    
    /// ״§„…״¯…״± / Destructor
    virtual ~Renderer3D() = default;
    
    // ============================================
    // ״§„״×‡״¦״© / Initialization
    // ============================================
    
    /// ״§„״×‡״¦״© / Initialize
    virtual bool Initialize(UInt32 width, UInt32 height) {
        m_width = width;
        m_height = height;
        return true;
    }
    
    /// ״×״÷״± ״§„״­״¬… / Resize
    virtual void Resize(UInt32 width, UInt32 height) {
        m_width = width;
        m_height = height;
        
        // ״×״­״¯״« ״£״¨״¹״§״¯ ״§„ƒ״§…״±״§ / Update camera aspect
        if (m_camera) {
            m_camera->SetAspectRatio(static_cast<Float32>(width) / height);
        }
    }
    
    /// ״§„״¥״÷„״§‚ / Shutdown
    virtual void Shutdown() {}
    
    // ============================================
    // ״§„ƒ״§…״±״§ / Camera
    // ============================================
    
    /// ״×״¹† ״§„ƒ״§…״±״§ / Set camera
    void SetCamera(const Camera3DPtr& camera) { m_camera = camera; }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„ƒ״§…״±״§ / Get camera
    Camera3DPtr GetCamera() const { return m_camera; }
    
    // ============================================
    // ״§„״¨״¦״© / Environment
    // ============================================
    
    /// ״×״¹† ״§„״¨״¦״© ״§„״¶ˆ״¦״© / Set light environment
    void SetLightEnvironment(const std::shared_ptr<LightEnvironment>& env) { m_lightEnv = env; }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„״¨״¦״© ״§„״¶ˆ״¦״© / Get light environment
    std::shared_ptr<LightEnvironment> GetLightEnvironment() const { return m_lightEnv; }
    
    /// ״×״¹† „ˆ† ״§„…״³״­ / Set clear color
    void SetClearColor(const Color& color) { m_clearColor = color; }
    Color GetClearColor() const { return m_clearColor; }
    
    // ============================================
    // ״§„״¥״¹״¯״§״¯״§״× / Settings
    // ============================================
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„״¥״¹״¯״§״¯״§״× / Get settings
    RenderSettings& GetSettings() { return m_settings; }
    const RenderSettings& GetSettings() const { return m_settings; }
    
    /// ״×״¹† ״§„״¥״¹״¯״§״¯״§״× / Set settings
    void SetSettings(const RenderSettings& settings) { m_settings = settings; }
    
    // ============================================
    // ״§„״¥״­״µ״§״¦״§״× / Statistics
    // ============================================
    
    /// ״§„״­״µˆ„ ״¹„‰ ״¥״­״µ״§״¦״§״× ״§„״¥״·״§״± ״§„״£״®״± / Get last frame stats
    const RenderStats& GetStats() const { return m_stats; }
    
    // ============================================
    // ״§„״±״³… / Rendering
    // ============================================
    
    /// ״¨״¯״¡ ״§„״¥״·״§״± / Begin frame
    virtual void BeginFrame() {
        m_stats.Reset();
        m_queueManager.Clear();
    }
    
    /// ״¥״±״³״§„ „„״±״³… / Submit for rendering
    void Submit(const DrawCommand& cmd) {
        // ‚״·״¹ ״§„״±״₪״© / Frustum culling
        if (m_settings.enableFrustumCulling && m_camera) {
            Frustum frustum = m_camera->GetFrustum();
            if (!frustum.IntersectsAABB(cmd.bounds)) {
                m_stats.meshesCulled++;
                return;
            }
        }
        
        m_queueManager.Add(cmd);
        m_stats.meshesRendered++;
    }
    
    /// ״¥״±״³״§„ ƒ״§״¦† „„״±״³… / Submit renderable
    void Submit(const Renderable& renderable) {
        if (renderable.IsVisible()) {
            Submit(renderable.ToDrawCommand());
        }
    }
    
    /// ״¥†‡״§״¡ ״§„״¥״·״§״± ˆ״§„״±״³… / End frame and render
    virtual void EndFrame() {
        if (m_camera) {
            m_queueManager.Sort(m_camera->GetPosition());
        }
        
        // ״±״³… ״§„״·ˆ״§״¨״± / Render queues
        RenderQueues();
    }
    
    /// ״±״³… ״§„״·ˆ״§״¨״± / Render all queues
    virtual void RenderQueues() {
        // 1. ״±״³… ״§„״®„״© / Render background
        RenderBackground();
        
        // 2. ״±״³… ״§„״¸„״§„ ״¥״°״§ …״¹„״© / Render shadows if enabled
        if (m_settings.shadowQuality != ShadowQuality::Off) {
            RenderShadows();
        }
        
        // 3. ״±״³… ״§„…״¹״×… / Render opaque
        for (const auto& cmd : m_queueManager.GetOpaqueQueue()) {
            RenderDrawCommand(cmd);
        }
        
        // 4. ״±״³… ״§„״´״§ / Render transparent
        for (const auto& cmd : m_queueManager.GetTransparentQueue()) {
            RenderDrawCommand(cmd);
        }
        
        // 5. …״§ ״¨״¹״¯ ״§„…״¹״§„״¬״© / Post-processing
        if (m_settings.enableBloom || m_settings.enableTonemap) {
            ApplyPostProcessing();
        }
    }
    
protected:
    /// ״±״³… ״§„״®„״© / Render background
    virtual void RenderBackground() {
        // †״°  ״§„״¦״§״× ״§„״±״¹״© / Implemented in subclasses
    }
    
    /// ״±״³… ״§„״¸„״§„ / Render shadows
    virtual void RenderShadows() {
        m_stats.shadowPasses++;
        // †״°  ״§„״¦״§״× ״§„״±״¹״© / Implemented in subclasses
    }
    
    /// ״±״³… ״£…״± ˆ״§״­״¯ / Render single command
    virtual void RenderDrawCommand(const DrawCommand& cmd) {
        if (cmd.mesh) {
            m_stats.drawCalls++;
            m_stats.triangles += cmd.mesh->GetTriangleCount();
            m_stats.vertices += cmd.mesh->GetVertexCount();
        }
    }
    
    /// ״×״·״¨‚ …״§ ״¨״¹״¯ ״§„…״¹״§„״¬״© / Apply post-processing
    virtual void ApplyPostProcessing() {
        // †״°  ״§„״¦״§״× ״§„״±״¹״© / Implemented in subclasses
    }
    
protected:
    UInt32 m_width = 0;                              ///< ״§„״¹״±״¶ / Width
    UInt32 m_height = 0;                             ///< ״§„״§״±״×״§״¹ / Height
    Camera3DPtr m_camera;                            ///< ״§„ƒ״§…״±״§ / Camera
    std::shared_ptr<LightEnvironment> m_lightEnv;    ///< ״§„״¨״¦״© ״§„״¶ˆ״¦״© / Light environment
    Color m_clearColor;                              ///< „ˆ† ״§„…״³״­ / Clear color
    RenderSettings m_settings;                       ///< ״§„״¥״¹״¯״§״¯״§״× / Settings
    RenderStats m_stats;                             ///< ״§„״¥״­״µ״§״¦״§״× / Stats
    RenderQueueManager m_queueManager;               ///< …״¯״± ״§„״·ˆ״§״¨״± / Queue manager
    std::vector<RenderPassPtr> m_renderPasses;       ///< ……״±״§״× ״§„״±״³… / Render passes
};

using Renderer3DPtr = std::shared_ptr<Renderer3D>;

// ============================================================================
// …״µ†״¹ ״§„ƒ״§״¦†״§״× / Object Factory
// ============================================================================

/// …״µ†״¹ ״§„ƒ״§״¦†״§״× ״§„‚״§״¨„״© „„״±״³… / Factory for renderables
class RenderableFactory {
public:
    /// ״¥†״´״§״¡ …ƒ״¹״¨ / Create cube
    static RenderablePtr CreateCube(
        Float32 size = 1.0f,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreateCube(size);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        renderable->SetMaterial(material ? material : std::make_shared<PBRMaterial>());
        return renderable;
    }
    
    /// ״¥†״´״§״¡ ƒ״±״© / Create sphere
    static RenderablePtr CreateSphere(
        Float32 radius = 0.5f,
        UInt32 segments = 32,
        UInt32 rings = 16,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreateSphere(radius, segments, rings);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        renderable->SetMaterial(material ? material : std::make_shared<PBRMaterial>());
        return renderable;
    }
    
    /// ״¥†״´״§״¡ ״£״³״·ˆ״§†״© / Create cylinder
    static RenderablePtr CreateCylinder(
        Float32 radius = 0.5f,
        Float32 height = 1.0f,
        UInt32 segments = 32,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreateCylinder(radius, height, segments);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        renderable->SetMaterial(material ? material : std::make_shared<PBRMaterial>());
        return renderable;
    }
    
    /// ״¥†״´״§״¡ …״®״±ˆ״· / Create cone
    static RenderablePtr CreateCone(
        Float32 radius = 0.5f,
        Float32 height = 1.0f,
        UInt32 segments = 32,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreateCone(radius, height, segments);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        renderable->SetMaterial(material ? material : std::make_shared<PBRMaterial>());
        return renderable;
    }
    
    /// ״¥†״´״§״¡ ״·ˆ‚ / Create torus
    static RenderablePtr CreateTorus(
        Float32 outerRadius = 0.5f,
        Float32 innerRadius = 0.2f,
        UInt32 outerSegments = 32,
        UInt32 innerSegments = 16,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreateTorus(outerRadius, innerRadius, outerSegments, innerSegments);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        renderable->SetMaterial(material ? material : std::make_shared<PBRMaterial>());
        return renderable;
    }
    
    /// ״¥†״´״§״¡ …״³״×ˆ‰ / Create plane
    static RenderablePtr CreatePlane(
        Float32 width = 1.0f,
        Float32 height = 1.0f,
        UInt32 segmentsX = 1,
        UInt32 segmentsY = 1,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreatePlane(width, height, segmentsX, segmentsY);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        renderable->SetMaterial(material ? material : std::make_shared<PBRMaterial>());
        return renderable;
    }
    
    /// ״¥†״´״§״¡ ״´״¨ƒ״© / Create grid
    static RenderablePtr CreateGrid(
        Float32 size = 10.0f,
        UInt32 divisions = 10,
        const MaterialPtr& material = nullptr
    ) {
        auto renderable = std::make_shared<Renderable>();
        auto meshData = MeshGenerator::CreateGrid(size, divisions);
        auto mesh = std::make_shared<Mesh>();
        mesh->SetData(meshData);
        renderable->SetMesh(mesh);
        
        auto gridMaterial = material;
        if (!gridMaterial) {
            auto unlit = std::make_shared<UnlitMaterial>();
            unlit->SetMainColor(Color(0.5f, 0.5f, 0.5f, 0.5f));
            gridMaterial = unlit;
        }
        renderable->SetMaterial(gridMaterial);
        renderable->SetCastShadows(false);
        return renderable;
    }
};

// ============================================================================
// …״³״§״¹״¯ ״§„…״´‡״¯ / Scene Helper
// ============================================================================

/// …״´‡״¯ ״¨״³״· „„״±״³… / Simple rendering scene
class Scene3D {
public:
    /// ״§„…†״´״¦ ״§„״§״×״±״§״¶ / Default constructor
    Scene3D() {}
    
    /// ״¥״¶״§״© ƒ״§״¦† / Add renderable
    void Add(const RenderablePtr& renderable) {
        m_renderables.push_back(renderable);
    }
    
    /// ״¥״²״§„״© ƒ״§״¦† / Remove renderable
    void Remove(const RenderablePtr& renderable) {
        auto it = std::find(m_renderables.begin(), m_renderables.end(), renderable);
        if (it != m_renderables.end()) {
            m_renderables.erase(it);
        }
    }
    
    /// …״³״­ ״§„ƒ„ / Clear all
    void Clear() {
        m_renderables.clear();
    }
    
    /// ״¥״±״³״§„ „„״±״³… / Submit to renderer
    void Submit(Renderer3D& renderer) {
        for (const auto& r : m_renderables) {
            if (r && r->IsVisible()) {
                renderer.Submit(*r);
            }
        }
    }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„ƒ״§״¦†״§״× / Get renderables
    const std::vector<RenderablePtr>& GetRenderables() const { return m_renderables; }
    
    /// ״§„״¹״¯״¯ / Count
    size_t Count() const { return m_renderables.size(); }
    
private:
    std::vector<RenderablePtr> m_renderables;
};

using Scene3DPtr = std::shared_ptr<Scene3D>;

// ============================================================================
// …״¯״± LOD / LOD Manager
// ============================================================================

/// …״³״×ˆ‰ ״§„״×״§״µ„ / Level of detail
struct LODLevel {
    Float32 distance;              ///< ״§„…״³״§״© / Distance
    std::shared_ptr<Mesh> mesh;    ///< ״§„†…ˆ״°״¬ / Mesh
    
    LODLevel() : distance(0.0f) {}
    LODLevel(Float32 d, const std::shared_ptr<Mesh>& m) : distance(d), mesh(m) {}
};

/// ƒ״§״¦† …״¹ …״³״×ˆ״§״× ״×״§״µ„ / LOD renderable
class LODRenderable : public Renderable {
public:
    /// ״¥״¶״§״© …״³״×ˆ‰ / Add LOD level
    void AddLOD(Float32 distance, const std::shared_ptr<Mesh>& mesh) {
        m_lodLevels.push_back(LODLevel(distance, mesh));
        // ״×״±״×״¨ ״¨״§„…״³״§״© / Sort by distance
        std::sort(m_lodLevels.begin(), m_lodLevels.end(),
            [](const LODLevel& a, const LODLevel& b) {
                return a.distance < b.distance;
            });
    }
    
    /// ״×״­״¯״« LOD ״¨†״§״¡‹ ״¹„‰ ״§„ƒ״§…״±״§ / Update LOD based on camera
    void UpdateLOD(const Vec3& cameraPos) {
        if (m_lodLevels.empty()) return;
        
        Float32 dist = (m_transform.position - cameraPos).Length();
        
        // ״§״®״×״§״± ״§„…״³״×ˆ‰ ״§„…†״§״³״¨ / Choose appropriate level
        m_mesh = m_lodLevels[0].mesh;
        for (const auto& level : m_lodLevels) {
            if (dist > level.distance) {
                m_mesh = level.mesh;
            } else {
                break;
            }
        }
        UpdateBounds();
    }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„…״³״×ˆ״§״× / Get LOD levels
    const std::vector<LODLevel>& GetLODLevels() const { return m_lodLevels; }
    
private:
    std::vector<LODLevel> m_lodLevels;
};

using LODRenderablePtr = std::shared_ptr<LODRenderable>;

// ============================================================================
// ״¨״§״×״´״± Instanced / Instanced Batcher
// ============================================================================

/// ״¨״§†״§״× ״§„…״«„ / Instance data
struct InstanceData {
    Mat4 transform;      ///< …״µˆ״© ״§„״×״­ˆ„ / Transform matrix
    Color color;         ///< ״§„„ˆ† / Color
    
    InstanceData() : color(Color::White) {}
    InstanceData(const Mat4& t, const Color& c = Color::White) : transform(t), color(c) {}
};

/// ״¨״§״×״´״± „„״±״³… ״§„…״×ƒ״±״± / Batcher for instanced rendering
class InstancedBatcher {
public:
    /// ״¨״¯״¡ ״¯״¹״© / Begin batch
    void Begin(const std::shared_ptr<Mesh>& mesh, const MaterialPtr& material) {
        m_mesh = mesh;
        m_material = material;
        m_instances.clear();
    }
    
    /// ״¥״¶״§״© …״«„ / Add instance
    void Add(const Mat4& transform, const Color& color = Color::White) {
        m_instances.push_back(InstanceData(transform, color));
    }
    
    /// ״¥״¶״§״© …† Transform3D / Add from Transform3D
    void Add(const Transform3D& transform, const Color& color = Color::White) {
        m_instances.push_back(InstanceData(transform.ToMatrix(), color));
    }
    
    /// ״¥†‡״§״¡ ˆ״±״³… / End and render
    void End(Renderer3D& renderer) {
        //  ״§„״×†״° ״§„״­‚‚״ ״×… ״§״³״×״®״¯״§… instanced rendering
        // In real implementation, use instanced rendering
        for (const auto& inst : m_instances) {
            DrawCommand cmd;
            cmd.mesh = m_mesh;
            cmd.material = m_material;
            // ״×״­„„ ״§„…״µˆ״© ״¥„‰ Transform3D / Decompose matrix to Transform3D
            // (…״¨״³״· ‡†״§) / (simplified here)
            cmd.transform.position = Vec3(inst.transform.m[12], inst.transform.m[13], inst.transform.m[14]);
            renderer.Submit(cmd);
        }
    }
    
    /// ״¹״¯״¯ ״§„…״«„״§״× / Instance count
    size_t GetInstanceCount() const { return m_instances.size(); }
    
    /// ״§„״­״µˆ„ ״¹„‰ ״§„…״«„״§״× / Get instances
    const std::vector<InstanceData>& GetInstances() const { return m_instances; }
    
private:
    std::shared_ptr<Mesh> m_mesh;
    MaterialPtr m_material;
    std::vector<InstanceData> m_instances;
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_RENDERER3D_H
