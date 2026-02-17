// ==============================================================================
// test_systems.cpp - اختبارات الأنظمة الجديدة / New Systems Tests
// ==============================================================================
// اختبارات: TextureAtlas, Camera2D, ParticleSystem, Sprite, Tween, Timer, Scene
// Tests: TextureAtlas, Camera2D, ParticleSystem, Sprite, Tween, Timer, Scene
// ==============================================================================
#include "catch.hpp"
#include "core/types.h"
#include "texture/texture_atlas.h"
#include "rendering/camera2d.h"
#include "rendering/sprite.h"
#include "effects/particle_system.h"
#include "core/tween.h"
#include "core/timer.h"
#include "core/scene.h"
#include <cmath>

using namespace sad::graphics;

// ============================================================================
// اختبارات أطلس النسيج / Texture Atlas Tests
// ============================================================================

TEST_CASE("TextureAtlas region management", "[atlas]") {
    TextureAtlas atlas;
    
    atlas.AddRegion("player", 0, 0, 32, 32);
    atlas.AddRegion("enemy", 32, 0, 32, 32);
    
    REQUIRE(atlas.GetRegionCount() == 2);
    REQUIRE(atlas.HasRegion("player"));
    REQUIRE(atlas.HasRegion("enemy"));
    REQUIRE_FALSE(atlas.HasRegion("item"));
    
    const AtlasRegion* r = atlas.GetRegion("player");
    REQUIRE(r != nullptr);
    REQUIRE(r->x == Approx(0.0f));
    REQUIRE(r->width == Approx(32.0f));
    REQUIRE(r->pivotX == Approx(0.5f));
}

TEST_CASE("TextureAtlas remove region", "[atlas]") {
    TextureAtlas atlas;
    atlas.AddRegion("a", 0, 0, 16, 16);
    atlas.AddRegion("b", 16, 0, 16, 16);
    
    REQUIRE(atlas.GetRegionCount() == 2);
    atlas.RemoveRegion("a");
    REQUIRE(atlas.GetRegionCount() == 1);
    REQUIRE_FALSE(atlas.HasRegion("a"));
    REQUIRE(atlas.HasRegion("b"));
}

TEST_CASE("TextureAtlas region names", "[atlas]") {
    TextureAtlas atlas;
    atlas.AddRegion("alpha", 0, 0, 10, 10);
    atlas.AddRegion("beta", 10, 0, 10, 10);
    atlas.AddRegion("gamma", 20, 0, 10, 10);
    
    auto names = atlas.GetRegionNames();
    REQUIRE(names.size() == 3);
}

TEST_CASE("TextureAtlas custom pivot", "[atlas]") {
    TextureAtlas atlas;
    atlas.AddRegion("foot", 0, 0, 32, 64, 0.5f, 1.0f);
    
    const AtlasRegion* r = atlas.GetRegion("foot");
    REQUIRE(r != nullptr);
    REQUIRE(r->pivotX == Approx(0.5f));
    REQUIRE(r->pivotY == Approx(1.0f));
}

TEST_CASE("TextureAtlas animation definition", "[atlas][animation]") {
    TextureAtlas atlas;
    for (int i = 0; i < 8; ++i) {
        std::string name = "walk_" + std::string(i < 10 ? "0" : "") + std::to_string(i);
        atlas.AddRegion(name, static_cast<Float32>(i * 32), 0, 32, 32);
    }
    
    atlas.DefineAnimation("walk", "walk_", 8, 0.1f, true);
    
    const AnimationDef* anim = atlas.GetAnimation("walk");
    REQUIRE(anim != nullptr);
    REQUIRE(anim->name == "walk");
    REQUIRE(anim->frames.size() == 8);
    REQUIRE(anim->loop == true);
    REQUIRE(anim->frames[0].duration == Approx(0.1f));
}

// ============================================================================
// اختبارات الكاميرا / Camera2D Tests
// ============================================================================

TEST_CASE("Camera2D construction", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    
    REQUIRE(cam.GetPosition().x == Approx(0.0f));
    REQUIRE(cam.GetPosition().y == Approx(0.0f));
    REQUIRE(cam.GetZoom() == Approx(1.0f));
    REQUIRE(cam.GetRotation() == Approx(0.0f));
    REQUIRE(cam.GetViewportSize().x == Approx(800.0f));
    REQUIRE(cam.GetViewportSize().y == Approx(600.0f));
}

TEST_CASE("Camera2D position and zoom", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    
    cam.SetPosition(100.0f, 200.0f);
    REQUIRE(cam.GetPosition().x == Approx(100.0f));
    REQUIRE(cam.GetPosition().y == Approx(200.0f));
    
    cam.SetZoom(2.0f);
    REQUIRE(cam.GetZoom() == Approx(2.0f));
    
    cam.SetZoom(0.001f); // حد أدنى / Minimum clamped
    REQUIRE(cam.GetZoom() >= 0.01f);
}

TEST_CASE("Camera2D bounds", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    
    REQUIRE_FALSE(cam.HasBounds());
    cam.SetBounds(0.0f, 0.0f, 1000.0f, 800.0f);
    REQUIRE(cam.HasBounds());
    
    cam.ClearBounds();
    REQUIRE_FALSE(cam.HasBounds());
}

TEST_CASE("Camera2D smooth follow", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    
    cam.SetFollowTarget(Vec2(500.0f, 300.0f));
    REQUIRE(cam.IsFollowing());
    
    cam.SetFollowSpeed(0.5f);
    
    // محاكاة عدة إطارات / Simulate several frames
    for (int i = 0; i < 100; ++i) {
        cam.Update(0.016f); // ~60fps
    }
    
    // يجب أن تقترب من الهدف / Should approach target
    REQUIRE(cam.GetPosition().x == Approx(500.0f).margin(5.0f));
    REQUIRE(cam.GetPosition().y == Approx(300.0f).margin(5.0f));
    
    cam.StopFollowing();
    REQUIRE_FALSE(cam.IsFollowing());
}

TEST_CASE("Camera2D shake", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    
    REQUIRE_FALSE(cam.IsShaking());
    cam.Shake(10.0f, 0.5f);
    REQUIRE(cam.IsShaking());
    
    // محاكاة حتى ينتهي الاهتزاز / Simulate until shake ends
    for (int i = 0; i < 40; ++i) {
        cam.Update(0.016f);
    }
    
    REQUIRE_FALSE(cam.IsShaking());
}

TEST_CASE("Camera2D visible rect", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    cam.SetPosition(400.0f, 300.0f);
    
    Rect visible = cam.GetVisibleRect();
    
    REQUIRE(visible.x == Approx(0.0f));
    REQUIRE(visible.y == Approx(0.0f));
    REQUIRE(visible.width == Approx(800.0f));
    REQUIRE(visible.height == Approx(600.0f));
    
    REQUIRE(cam.IsPointVisible(400.0f, 300.0f));
    REQUIRE_FALSE(cam.IsPointVisible(-100.0f, -100.0f));
}

TEST_CASE("Camera2D world-screen conversion roundtrip", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    cam.SetPosition(100.0f, 50.0f);
    cam.SetZoom(1.5f);
    
    Vec2 worldPt(200.0f, 150.0f);
    Vec2 screenPt = cam.WorldToScreen(worldPt);
    Vec2 backToWorld = cam.ScreenToWorld(screenPt);
    
    REQUIRE(backToWorld.x == Approx(worldPt.x).margin(1.0f));
    REQUIRE(backToWorld.y == Approx(worldPt.y).margin(1.0f));
}

TEST_CASE("Camera2D view matrix", "[camera]") {
    Camera2D cam(800.0f, 600.0f);
    Mat4 view = cam.GetViewMatrix();
    
    // بدون حركة أو تكبير، يجب أن تكون قريبة من الهوية
    // No movement or zoom: should be near identity
    REQUIRE(view.m[0] == Approx(1.0f));
    REQUIRE(view.m[5] == Approx(1.0f));
}

// ============================================================================
// اختبارات نظام الجسيمات / Particle System Tests
// ============================================================================

TEST_CASE("ParticleSystem construction", "[particles]") {
    ParticleSystem ps;
    REQUIRE(ps.GetAliveCount() == 0);
    REQUIRE_FALSE(ps.IsActive());
}

TEST_CASE("ParticleSystem start and emit", "[particles]") {
    auto config = ParticleSystem::PresetFire();
    config.maxParticles = 100;
    config.emissionRate = 50.0f;
    
    ParticleSystem ps(config);
    ps.SetPosition(400.0f, 300.0f);
    ps.Start();
    
    REQUIRE(ps.IsActive());
    
    // محاكاة إطار / Simulate one frame
    ps.Update(0.1f);
    
    REQUIRE(ps.GetAliveCount() > 0);
}

TEST_CASE("ParticleSystem burst", "[particles]") {
    ParticleEmitterConfig config;
    config.maxParticles = 200;
    config.emissionRate = 0.0f;
    config.lifetime = {1.0f, 1.0f};
    config.speed = {0.0f, 0.0f};
    
    ParticleSystem ps(config);
    ps.Burst(50);
    
    REQUIRE(ps.GetAliveCount() == 50);
}

TEST_CASE("ParticleSystem particles die", "[particles]") {
    ParticleEmitterConfig config;
    config.maxParticles = 100;
    config.emissionRate = 0.0f;
    config.lifetime = {0.1f, 0.1f};
    config.speed = {0.0f, 0.0f};
    
    ParticleSystem ps(config);
    ps.Burst(30);
    REQUIRE(ps.GetAliveCount() == 30);
    
    // تقدم الوقت بما يكفي لكي تموت / Advance enough for particles to die
    ps.Update(0.2f);
    REQUIRE(ps.GetAliveCount() == 0);
}

TEST_CASE("ParticleSystem presets", "[particles]") {
    // تأكد من أن كل الإعدادات المسبقة تعمل / Verify all presets work
    auto fire = ParticleSystem::PresetFire();
    REQUIRE(fire.emissionRate > 0.0f);
    
    auto smoke = ParticleSystem::PresetSmoke();
    REQUIRE(smoke.emissionRate > 0.0f);
    
    auto sparks = ParticleSystem::PresetSparks();
    REQUIRE(sparks.burstCount > 0);
    
    auto explosion = ParticleSystem::PresetExplosion();
    REQUIRE(explosion.burstCount > 0);
    
    auto snow = ParticleSystem::PresetSnow();
    REQUIRE(snow.emissionRate > 0.0f);
    
    auto rain = ParticleSystem::PresetRain();
    REQUIRE(rain.emissionRate > 0.0f);
}

TEST_CASE("ParticleSystem pause and resume", "[particles]") {
    ParticleEmitterConfig config;
    config.maxParticles = 100;
    config.emissionRate = 100.0f;
    config.lifetime = {1.0f, 1.0f};
    
    ParticleSystem ps(config);
    ps.Start();
    ps.Update(0.1f);
    Int32 count1 = ps.GetAliveCount();
    
    ps.Pause();
    ps.Update(0.1f);
    Int32 count2 = ps.GetAliveCount();
    
    REQUIRE(count1 == count2); // لا تغيير أثناء الإيقاف / No change while paused
    
    ps.Resume();
    ps.Update(0.1f);
    REQUIRE(ps.GetAliveCount() >= count2); // الجسيمات الجديدة / New particles spawned
}

// ============================================================================
// اختبارات الرسوم المتحركة / Sprite Tests  
// ============================================================================

TEST_CASE("Sprite construction", "[sprite]") {
    Sprite sprite;
    
    REQUIRE(sprite.GetPosition().x == Approx(0.0f));
    REQUIRE(sprite.GetPosition().y == Approx(0.0f));
    REQUIRE(sprite.GetScale().x == Approx(1.0f));
    REQUIRE(sprite.GetRotation() == Approx(0.0f));
    REQUIRE(sprite.IsVisible());
    REQUIRE_FALSE(sprite.IsFlippedX());
    REQUIRE_FALSE(sprite.IsFlippedY());
}

TEST_CASE("Sprite transform", "[sprite]") {
    Sprite sprite;
    
    sprite.SetPosition(100.0f, 200.0f);
    REQUIRE(sprite.GetPosition().x == Approx(100.0f));
    
    sprite.SetSize(64.0f, 64.0f);
    REQUIRE(sprite.GetSize().x == Approx(64.0f));
    
    sprite.SetScale(2.0f);
    REQUIRE(sprite.GetScale().x == Approx(2.0f));
    REQUIRE(sprite.GetScale().y == Approx(2.0f));
    
    sprite.SetRotation(1.57f);
    REQUIRE(sprite.GetRotation() == Approx(1.57f));
    
    sprite.SetOrigin(0.0f, 0.0f);
    REQUIRE(sprite.GetOrigin().x == Approx(0.0f));
}

TEST_CASE("Sprite color and alpha", "[sprite]") {
    Sprite sprite;
    
    sprite.SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
    REQUIRE(sprite.GetColor().r == Approx(1.0f));
    REQUIRE(sprite.GetColor().g == Approx(0.0f));
    
    sprite.SetAlpha(0.5f);
    REQUIRE(sprite.GetAlpha() == Approx(0.5f));
}

TEST_CASE("Sprite bounds", "[sprite]") {
    Sprite sprite;
    sprite.SetPosition(100.0f, 100.0f);
    sprite.SetSize(50.0f, 50.0f);
    sprite.SetOrigin(0.5f, 0.5f);
    
    Rect bounds = sprite.GetBounds();
    REQUIRE(bounds.x == Approx(75.0f));
    REQUIRE(bounds.y == Approx(75.0f));
    REQUIRE(bounds.width == Approx(50.0f));
    REQUIRE(bounds.height == Approx(50.0f));
    
    REQUIRE(sprite.ContainsPoint(100.0f, 100.0f));
    REQUIRE_FALSE(sprite.ContainsPoint(0.0f, 0.0f));
}

TEST_CASE("Sprite flip UV", "[sprite]") {
    Sprite sprite;
    sprite.SetTextureRect(0.0f, 0.0f, 1.0f, 1.0f);
    
    Float32 u0, v0, u1, v1;
    
    sprite.GetCurrentUV(u0, v0, u1, v1);
    REQUIRE(u0 == Approx(0.0f));
    REQUIRE(u1 == Approx(1.0f));
    
    sprite.SetFlipX(true);
    sprite.GetCurrentUV(u0, v0, u1, v1);
    REQUIRE(u0 == Approx(1.0f));
    REQUIRE(u1 == Approx(0.0f));
}

TEST_CASE("Sprite animation state", "[sprite][animation]") {
    Sprite sprite;
    REQUIRE(sprite.GetAnimationState() == AnimationState::Stopped);
    
    // إنشاء رسم متحرك يدوياً / Create animation manually
    SpriteAnimation anim;
    anim.name = "idle";
    anim.loop = true;
    anim.frames.push_back({0.0f, 0.0f, 0.25f, 1.0f, 32, 32, 0.1f, 0.5f, 0.5f});
    anim.frames.push_back({0.25f, 0.0f, 0.5f, 1.0f, 32, 32, 0.1f, 0.5f, 0.5f});
    anim.frames.push_back({0.5f, 0.0f, 0.75f, 1.0f, 32, 32, 0.1f, 0.5f, 0.5f});
    
    sprite.AddAnimation("idle", anim);
    sprite.Play("idle");
    
    REQUIRE(sprite.GetAnimationState() == AnimationState::Playing);
    REQUIRE(sprite.GetCurrentAnimation() == "idle");
    REQUIRE(sprite.GetCurrentFrame() == 0);
    
    // تقدم الوقت / Advance time
    sprite.Update(0.15f); // past first frame
    REQUIRE(sprite.GetCurrentFrame() == 1);
    
    sprite.Pause();
    REQUIRE(sprite.GetAnimationState() == AnimationState::Paused);
    
    sprite.Resume();
    REQUIRE(sprite.GetAnimationState() == AnimationState::Playing);
    
    sprite.Stop();
    REQUIRE(sprite.GetAnimationState() == AnimationState::Stopped);
    REQUIRE(sprite.GetCurrentFrame() == 0);
}

// ============================================================================
// اختبارات دوال التسهيل / Easing Function Tests
// ============================================================================

TEST_CASE("Ease functions boundary values", "[tween][ease]") {
    // جميع الدوال يجب أن تعيد 0 عند t=0 و 1 عند t=1
    // All functions should return 0 at t=0 and 1 at t=1
    
    REQUIRE(Ease::Linear(0.0f) == Approx(0.0f));
    REQUIRE(Ease::Linear(1.0f) == Approx(1.0f));
    REQUIRE(Ease::Linear(0.5f) == Approx(0.5f));
    
    REQUIRE(Ease::InQuad(0.0f) == Approx(0.0f));
    REQUIRE(Ease::InQuad(1.0f) == Approx(1.0f));
    
    REQUIRE(Ease::OutQuad(0.0f) == Approx(0.0f));
    REQUIRE(Ease::OutQuad(1.0f) == Approx(1.0f));
    
    REQUIRE(Ease::InOutCubic(0.0f) == Approx(0.0f));
    REQUIRE(Ease::InOutCubic(1.0f) == Approx(1.0f));
    
    REQUIRE(Ease::InSine(0.0f) == Approx(0.0f).margin(1e-5f));
    REQUIRE(Ease::InSine(1.0f) == Approx(1.0f).margin(1e-5f));
    
    REQUIRE(Ease::OutBounce(0.0f) == Approx(0.0f));
    REQUIRE(Ease::OutBounce(1.0f) == Approx(1.0f));
    
    REQUIRE(Ease::InElastic(0.0f) == Approx(0.0f));
    REQUIRE(Ease::InElastic(1.0f) == Approx(1.0f));
    
    REQUIRE(Ease::OutElastic(0.0f) == Approx(0.0f));
    REQUIRE(Ease::OutElastic(1.0f) == Approx(1.0f));
}

TEST_CASE("Ease::Evaluate dispatches correctly", "[tween][ease]") {
    REQUIRE(Ease::Evaluate(EaseType::Linear, 0.5f) == Approx(0.5f));
    REQUIRE(Ease::Evaluate(EaseType::EaseInQuad, 0.5f) == Approx(0.25f));
    REQUIRE(Ease::Evaluate(EaseType::EaseOutQuad, 0.5f) == Approx(0.75f));
}

TEST_CASE("Ease InOutQuad symmetry", "[tween][ease]") {
    // EaseInOut يجب أن تكون 0.5 عند t=0.5
    // EaseInOut should be 0.5 at t=0.5
    REQUIRE(Ease::InOutQuad(0.5f) == Approx(0.5f));
    REQUIRE(Ease::InOutCubic(0.5f) == Approx(0.5f));
    REQUIRE(Ease::InOutSine(0.5f) == Approx(0.5f).margin(1e-5f));
}

// ============================================================================
// اختبارات Tween Manager / Tween Manager Tests
// ============================================================================

TEST_CASE("TweenManager basic tween", "[tween]") {
    TweenManager manager;
    Float32 value = 0.0f;
    
    manager.To(&value, 100.0f, 1.0f, EaseType::Linear);
    
    REQUIRE(manager.GetActiveCount() == 1);
    
    manager.Update(0.5f);
    REQUIRE(value == Approx(50.0f).margin(1.0f));
    
    manager.Update(0.5f);
    REQUIRE(value == Approx(100.0f).margin(1.0f));
    
    // بعد الانتهاء / After completion
    REQUIRE(manager.GetActiveCount() == 0);
}

TEST_CASE("TweenManager cancel", "[tween]") {
    TweenManager manager;
    Float32 value = 0.0f;
    
    UInt32 id = manager.To(&value, 100.0f, 1.0f);
    REQUIRE(manager.GetActiveCount() == 1);
    
    manager.Cancel(id);
    manager.Update(0.016f);
    REQUIRE(manager.GetActiveCount() == 0);
}

TEST_CASE("TweenManager delay", "[tween]") {
    TweenManager manager;
    Float32 value = 0.0f;
    
    manager.To(&value, 100.0f, 1.0f, 0.5f, EaseType::Linear);
    
    manager.Update(0.3f); // لا تزال في التأخير / Still in delay
    REQUIRE(value == Approx(0.0f));
    
    manager.Update(0.3f); // بدأت / Started (0.1s into tween)
    REQUIRE(value > 0.0f);
}

TEST_CASE("TweenManager on complete callback", "[tween]") {
    TweenManager manager;
    Float32 value = 0.0f;
    bool completed = false;
    
    UInt32 id = manager.To(&value, 50.0f, 0.5f, EaseType::Linear);
    manager.OnComplete(id, [&completed]() { completed = true; });
    
    manager.Update(0.5f);
    REQUIRE(completed);
}

// ============================================================================
// اختبارات المؤقت / Timer Tests
// ============================================================================

TEST_CASE("Timer basic", "[timer]") {
    Timer timer;
    timer.Start();
    
    REQUIRE_FALSE(timer.IsPaused());
    REQUIRE(timer.GetFrameCount() == 0);
    REQUIRE(timer.GetTimeScale() == Approx(1.0f));
    
    timer.Update();
    REQUIRE(timer.GetFrameCount() == 1);
    REQUIRE(timer.GetDeltaTime() >= 0.0f);
}

TEST_CASE("Timer pause and resume", "[timer]") {
    Timer timer;
    timer.Start();
    timer.Update();
    
    timer.Pause();
    REQUIRE(timer.IsPaused());
    
    timer.Update();
    REQUIRE(timer.GetDeltaTime() == Approx(0.0f));
    
    timer.Resume();
    REQUIRE_FALSE(timer.IsPaused());
}

TEST_CASE("Timer time scale", "[timer]") {
    Timer timer;
    timer.Start();
    timer.SetTimeScale(2.0f);
    
    REQUIRE(timer.GetTimeScale() == Approx(2.0f));
    
    timer.Update();
    Float32 dt = timer.GetDeltaTime();
    Float32 sdt = timer.GetScaledDeltaTime();
    
    REQUIRE(sdt == Approx(dt * 2.0f));
}

TEST_CASE("TimerManager set timeout", "[timer]") {
    TimerManager manager;
    bool fired = false;
    
    manager.SetTimeout(0.5f, [&fired]() { fired = true; });
    REQUIRE(manager.GetActiveCount() == 1);
    
    manager.Update(0.3f);
    REQUIRE_FALSE(fired);
    
    manager.Update(0.3f); // 0.6s total - should fire
    REQUIRE(fired);
    REQUIRE(manager.GetActiveCount() == 0); // one-shot removed
}

TEST_CASE("TimerManager set interval", "[timer]") {
    TimerManager manager;
    int count = 0;
    
    manager.SetInterval(0.1f, [&count]() { count++; }, 3);
    
    for (int i = 0; i < 5; ++i) {
        manager.Update(0.1f);
    }
    
    REQUIRE(count == 3);
    REQUIRE(manager.GetActiveCount() == 0);
}

// ============================================================================
// اختبارات المشهد / Scene Tests
// ============================================================================

TEST_CASE("Scene construction", "[scene]") {
    Scene scene("test");
    REQUIRE(scene.GetName() == "test");
    REQUIRE_FALSE(scene.IsActive());
    REQUIRE(scene.GetLayerCount() == 0);
}

TEST_CASE("Scene layer management", "[scene]") {
    Scene scene("game");
    
    Layer* bg = scene.AddLayer("background", -1);
    REQUIRE(bg != nullptr);
    
    Layer* fg = scene.AddLayer("foreground", 1);
    REQUIRE(fg != nullptr);
    
    REQUIRE(scene.GetLayerCount() == 2);
    REQUIRE(scene.GetLayer("background") == bg);
    REQUIRE(scene.GetLayer("foreground") == fg);
    
    // الطبقات مرتبة / Layers are sorted
    REQUIRE(scene.GetLayer("background")->GetZOrder() == -1);
    REQUIRE(scene.GetLayer("foreground")->GetZOrder() == 1);
    
    scene.RemoveLayer("background");
    REQUIRE(scene.GetLayerCount() == 1);
    REQUIRE(scene.GetLayer("background") == nullptr);
}

TEST_CASE("Layer entity management", "[scene][layer]") {
    Layer layer("test", 0);
    
    UInt32 id1 = layer.AddEntity("player", 100.0f, 200.0f);
    UInt32 id2 = layer.AddEntity("enemy", 300.0f, 400.0f);
    
    REQUIRE(layer.GetEntityCount() == 2);
    
    Entity* e = layer.GetEntity(id1);
    REQUIRE(e != nullptr);
    REQUIRE(e->name == "player");
    REQUIRE(e->position.x == Approx(100.0f));
    
    Entity* found = layer.FindByName("enemy");
    REQUIRE(found != nullptr);
    REQUIRE(found->position.x == Approx(300.0f));
    
    layer.RemoveEntity(id1);
    REQUIRE(layer.GetEntityCount() == 1);
    REQUIRE(layer.GetEntity(id1) == nullptr);
}

TEST_CASE("Layer entity tags", "[scene][layer]") {
    Layer layer("test", 0);
    
    Entity e1;
    e1.name = "goblin1";
    e1.tag = "enemy";
    layer.AddEntity(e1);
    
    Entity e2;
    e2.name = "goblin2";
    e2.tag = "enemy";
    layer.AddEntity(e2);
    
    Entity e3;
    e3.name = "player";
    e3.tag = "player";
    layer.AddEntity(e3);
    
    auto enemies = layer.FindByTag("enemy");
    REQUIRE(enemies.size() == 2);
    
    auto players = layer.FindByTag("player");
    REQUIRE(players.size() == 1);
}

TEST_CASE("Scene lifecycle", "[scene]") {
    Scene scene("test");
    bool entered = false;
    bool exited = false;
    
    scene.SetOnEnter([&entered]() { entered = true; });
    scene.SetOnExit([&exited]() { exited = true; });
    
    scene.OnEnter();
    REQUIRE(entered);
    REQUIRE(scene.IsActive());
    
    scene.OnExit();
    REQUIRE(exited);
    REQUIRE_FALSE(scene.IsActive());
}

TEST_CASE("SceneManager switch scenes", "[scene]") {
    SceneManager manager;
    
    Scene* menu = manager.AddScene("menu");
    Scene* game = manager.AddScene("game");
    
    REQUIRE(manager.GetCurrentScene() == nullptr);
    
    manager.SwitchTo("menu");
    REQUIRE(manager.GetCurrentScene() == menu);
    REQUIRE(menu->IsActive());
    
    manager.SwitchTo("game");
    REQUIRE(manager.GetCurrentScene() == game);
    REQUIRE(game->IsActive());
    REQUIRE_FALSE(menu->IsActive());
}

TEST_CASE("Layer parallax", "[scene][layer]") {
    Layer layer("bg", -1);
    layer.SetParallax(0.5f, 0.5f);
    
    REQUIRE(layer.GetParallax().x == Approx(0.5f));
    REQUIRE(layer.GetParallax().y == Approx(0.5f));
}

TEST_CASE("Layer visibility and opacity", "[scene][layer]") {
    Layer layer("test", 0);
    
    REQUIRE(layer.IsVisible());
    REQUIRE(layer.GetOpacity() == Approx(1.0f));
    
    layer.SetVisible(false);
    REQUIRE_FALSE(layer.IsVisible());
    
    layer.SetOpacity(0.5f);
    REQUIRE(layer.GetOpacity() == Approx(0.5f));
}

// ============================================================================
// اختبارات ColorGradient / ColorGradient Tests
// ============================================================================

TEST_CASE("ColorGradient evaluate", "[particles][gradient]") {
    ColorGradient grad;
    grad.Clear();
    grad.AddKey(0.0f, Color(1.0f, 0.0f, 0.0f));
    grad.AddKey(1.0f, Color(0.0f, 0.0f, 1.0f));
    
    Color start = grad.Evaluate(0.0f);
    REQUIRE(start.r == Approx(1.0f));
    REQUIRE(start.b == Approx(0.0f));
    
    Color end = grad.Evaluate(1.0f);
    REQUIRE(end.r == Approx(0.0f));
    REQUIRE(end.b == Approx(1.0f));
    
    Color mid = grad.Evaluate(0.5f);
    REQUIRE(mid.r == Approx(0.5f));
    REQUIRE(mid.b == Approx(0.5f));
}

TEST_CASE("FloatRange random", "[particles][range]") {
    FloatRange r(10.0f, 20.0f);
    
    for (int i = 0; i < 100; ++i) {
        Float32 val = r.Random();
        REQUIRE(val >= 10.0f);
        REQUIRE(val <= 20.0f);
    }
    
    FloatRange fixed(5.0f);
    REQUIRE(fixed.Random() == Approx(5.0f));
}
