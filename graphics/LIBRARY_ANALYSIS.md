# SadGraphics Library — Complete Analysis
## مكتبة الرسومات للغة ص — تحليل شامل

> **Version**: 0.1.0  
> **Language**: C++17  
> **Date of Analysis**: 2026  
> **Purpose**: To plan major expansions  

---

## Table of Contents

1. [Architecture Overview](#1-architecture-overview)
2. [Technology Stack](#2-technology-stack)
3. [Complete File Inventory](#3-complete-file-inventory)
4. [Module-by-Module Deep Dive](#4-module-by-module-deep-dive)
5. [Class/Struct/Enum Registry](#5-classstruct-enum-registry)
6. [Rendering Pipeline](#6-rendering-pipeline)
7. [UI System Architecture](#7-ui-system-architecture)
8. [Mobile UI Framework](#8-mobile-ui-framework)
9. [3D Capabilities](#9-3d-capabilities)
10. [stdlib Integration Layer](#10-stdlib-integration-layer)
11. [Build System](#11-build-system)
12. [Development State & Known Issues](#12-development-state--known-issues)
13. [Expansion Recommendations](#13-expansion-recommendations)

---

## 1. Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                     Sad Language Runtime                         │
│                    (stdlib/graphics/)                            │
├─────────────────────────────────────────────────────────────────┤
│   graphics_module.h/cpp       │  Raylib Backend (alt)           │
│   SDL2+OpenGL implementation  │  graphics_module_raylib.cpp     │
│   ID-based resource mgmt      │  Stub fallback available        │
├────────────┬─────────────┬────┴──────┬──────────────────────────┤
│  Window    │  Rendering  │    UI     │   Mobile UI (Flutter-    │
│  System    │  Pipeline   │  Widgets  │   style components)      │
├────────────┼─────────────┼───────────┼──────────────────────────┤
│  Input     │  Effects    │  Drawing  │   Resources              │
│  Manager   │  System     │  Shapes   │   Manager                │
├────────────┼─────────────┼───────────┼──────────────────────────┤
│  Audio     │  Physics    │  Text &   │   World/Tilemap          │
│  System    │  2D         │  Arabic   │   System                 │
├────────────┴─────────────┴───────────┴──────────────────────────┤
│                        Core Layer                                │
│  types.h │ ecs.h │ event_bus.h │ math3d.h │ scene.h │ timer.h  │
│  profiler.h │ serialization.h │ state_machine.h │ tween.h      │
├─────────────────────────────────────────────────────────────────┤
│                  OS Framebuffer (GPU-free)                       │
│  Direct pixel manipulation, RGB565/888/RGBA8888/BGR/Grayscale   │
├─────────────────────────────────────────────────────────────────┤
│              Third-Party Dependencies                            │
│  SDL2 2.28.5 │ GLAD (OpenGL 3.3) │ stb_image │ stb_truetype   │
│  Catch2 (testing)                                                │
└─────────────────────────────────────────────────────────────────┘
```

**Namespace hierarchy**:
- `sad::graphics` — primary namespace (aliased as `SadGraphics`)
- `sad::graphics::ui` — Flexbox/CSS Grid layout system
- `sad::graphics::ui::mobile` — Flutter-inspired mobile widgets
- `sad::graphics::os` — OS-level framebuffer (no GPU)
- `Graphics::UI` — Older/desktop widget classes (Label, Button, etc.)
- `sad::stdlib::graphics` — Language binding layer

**Design Patterns Used**:
- **Singleton**: EventBus, ResourceManager, SceneManager, Profiler
- **Entity-Component-System (ECS)**: Full `World` with component pools
- **Observer/Event Bus**: Type-safe publish/subscribe with deferred events
- **State Machine**: Generic `StateMachine<StateId>` with transitions
- **Builder/Fluent API**: Path, LayoutStyle, FlexContainer, Element chain methods
- **Factory**: Static `Create*` methods on most classes

---

## 2. Technology Stack

| Component | Technology | Details |
|-----------|-----------|---------|
| **Windowing** | SDL2 2.28.5 | Local copy in `third_party/SDL2/SDL2-2.28.5` |
| **Graphics API** | OpenGL 3.3 Core Profile | Via GLAD function loader |
| **Image Loading** | stb_image.h | Header-only, in `third_party/` |
| **Font Loading** | stb_truetype.h | Header-only, in `third_party/` |
| **Testing** | Catch2 | In `third_party/catch2/` |
| **Build** | CMake 3.15+ | Supports shared/static builds |
| **Compiler** | C++17 required | `CMAKE_CXX_STANDARD 17` |
| **Alt Backend** | Raylib | Alternative via `graphics_module_raylib.cpp` |
| **Bilingual** | Arabic + English | All comments/docs in both languages |

---

## 3. Complete File Inventory

### 3.1 Headers (`include/`)

#### Core (`include/core/`) — 10 files
| File | Lines | Description |
|------|-------|-------------|
| `types.h` | 440 | Type aliases (Int32, Float32, u8-u64, Handle), Color, Vec2/3/4, Mat3/4, Point2D, Size2D, Rect, Result, LogLevel |
| `ecs.h` | 794 | Full ECS: Entity, ComponentPool<T>, ISystem, World with ForEach queries |
| `event_bus.h` | 658 | Type-safe EventBus singleton, 14 built-in events, deferred publishing, thread-safe |
| `math3d.h` | 1596 | Quat, Transform3D, Ray, Plane, AABB, Sphere, Frustum, math utilities |
| `profiler.h` | 626 | HighResTimer, ProfileSample, FrameStats, MemoryTracker, Profiler singleton |
| `scene.h` | ~300 | Entity, Layer (parallax, z-order), Scene (lifecycle callbacks), SceneManager |
| `serialization.h` | 989 | ValueType, Value (variant), JsonSerializer |
| `state_machine.h` | 713 | IState, StateDefinition<StateId>, Transition, StateMachine (bidirectional/global) |
| `timer.h` | ~200 | Timer (DeltaTime, FPS, TimeScale), ScheduledEvent, TimerManager (SetTimeout/SetInterval) |
| `tween.h` | ~300 | 30 EaseType functions, Tween, TweenManager (yoyo, repeat) |

#### Rendering (`include/rendering/`) — 11 files
| File | Lines | Description |
|------|-------|-------------|
| `context.h` | ~200 | RenderContext (OpenGL 3.3, MSAA, VSync, clear, viewport) |
| `shader.h` | ~250 | ShaderProgram (compile, uniforms), factory functions |
| `renderer2d.h` | 459 | Vertex2D, batched 2D renderer (shapes, text, textures, transforms) |
| `framebuffer.h` | ~200 | Framebuffer (FBO: color/depth/stencil attachments, ReadPixel, Blit) |
| `camera2d.h` | ~200 | Camera2D (smooth follow, dead zone, shake, screen↔world) |
| `camera3d.h` | 975 | Camera3D (perspective/ortho, quaternion rotation, orbit) |
| `renderer3d.h` | 882 | RenderQueue, RenderSettings (shadows, AA, HDR, bloom, fog, LOD), RenderQueueManager |
| `lighting.h` | 879 | Directional/Point/Spot/Area lights, cascaded shadows |
| `material.h` | 871 | MaterialProperty, BlendState, DepthState, StencilState, RenderState |
| `mesh.h` | 978 | Vertex3D, SkinnedVertex (4 bones), MeshData (normals, tangents) |
| `sprite.h` | 233 | Sprite with animation system (frames, atlas support) |

#### Texture (`include/texture/`) — 3 files
| File | Lines | Description |
|------|-------|-------------|
| `texture.h` | ~300 | TextureFormat (16 formats inc. float), Texture (create, bind, mipmaps) |
| `image.h` | ~250 | Image (load, flip, rotate, grayscale, pixel access) |
| `texture_atlas.h` | ~250 | AtlasRegion, AnimationDef, TextureAtlas (grid slicing, animations) |

#### Text (`include/text/`) — 2 files
| File | Lines | Description |
|------|-------|-------------|
| `font.h` | ~200 | GlyphInfo, Font (TTF via stb_truetype, atlas generation, kerning, UTF-8) |
| `arabic_text.h` | ~200 | ArabicText (UTF-8 decode/encode, Arabic shaping, BiDi reorder, RTL) |

#### Drawing (`include/drawing/`) — 1 file
| File | Lines | Description |
|------|-------|-------------|
| `shapes.h` | 626 | LineCap/Join, DashPattern, StrokeStyle, Gradient, FillStyle, Arc/Pie/Ring/RoundedRect/Star/RegularPolygon, QuadraticBezier/CubicBezier/Spline, Path class, ShapeRenderer |

#### Effects (`include/effects/`) — 2 files
| File | Lines | Description |
|------|-------|-------------|
| `particle_system.h` | 305 | EmitterShape, ParticleEmitterConfig, Particle, ParticleSystem (6 presets: Fire/Smoke/Sparks/Explosion/Snow/Rain) |
| `post_processing.h` | 448 | 18 PostEffectTypes, parameter structs (Bloom/Blur/ChromaticAberration/Vignette/ColorGrading/Pixelate/CRT/FilmGrain), PostProcessingPipeline, 5 presets |

#### UI (`include/ui/`) — 8 desktop files + 9 mobile files
| File | Lines | Description |
|------|-------|-------------|
| `widget.h` | 211 | Widget base (position, state, mouse/key handlers, parent-child) |
| `button.h` | ~150 | Button (state-based colors, click/hover/press callbacks) |
| `label.h` | ~100 | Label (TextAlignment 12 positions, word wrap, auto-size) |
| `panel.h` | ~300 | Panel (None/Vertical/Horizontal/Grid layout, scrollable) |
| `checkbox.h` | ~100 | Checkbox (toggle, label, color customization) |
| `slider.h` | ~200 | Slider (H/V orientation, thumb, step, format) |
| `textinput.h` | ~200 | TextInput (selection, clipboard, password, cursor blink) |
| `ui_layout.h` | 817 | **Full Flexbox/CSS Grid system**: FlexDirection, JustifyContent, AlignItems, StyleValue (px/pct/em/vh/vw), EdgeInsets, LayoutStyle, Element, FlexContainer, Row, Column, ScrollView, Stack, Label, Button, Image, ProgressBar, Slider, Checkbox, RadioButton, TextInput, LayoutEngine, builder helpers |

#### Mobile UI (`include/ui/mobile/`) — 9 files (Flutter-style)
| File | Lines | Description |
|------|-------|-------------|
| `mobile_core.h` | 1167 | Container, Row, Column, Stack, Padding, Center, Align, SizedBox, Expanded, Flexible, Spacer, AspectRatio, FractionallySizedBox, ConstrainedBox, FittedBox, Wrap, LayoutBuilder |
| `mobile_buttons.h` | 897 | ElevatedButton, TextButton, OutlinedButton, IconButton, FloatingActionButton, ExtendedFAB, ToggleButton, SegmentedButton, DropdownButton, PopupMenuButton |
| `mobile_dialogs.h` | 1146 | AlertDialog, BottomSheet, ModalBottomSheet, Snackbar, Toast, Dialog, SimpleDialog, FullScreenDialog, Banner, AppNotification |
| `mobile_display.h` | 1262 | Card, Chip, Badge, Avatar, Tooltip, ProgressBar, CircularProgress, Skeleton, Divider, ListTile, ExpansionTile, DataTable, Carousel, Timeline, EmptyState |
| `mobile_input.h` | 1527 | TextField, PasswordField, SearchBar, OTPInput, PinInput, DatePicker, TimePicker, DateTimePicker, ColorPicker, FilePicker, ImagePicker, Autocomplete, Dropdown, MultiSelect, RatingInput |
| `mobile_lists.h` | 1166 | ListView, GridView, AnimatedList, ReorderableList, TreeView, VirtualizedList, GroupedList, InfiniteScroll, SwipeableList, NestedScrollView |
| `mobile_media.h` | 1246 | VideoPlayer, ImageViewer, PhotoGallery, CameraView, UploadProgress, AnimatedWidget, AnimatedSwitcher, RippleEffect, PageTransition, GestureDetector |
| `mobile_navigation.h` | 1038 | AppBar, BottomNavigationBar, TabBar, Drawer, NavigationRail, Breadcrumbs, Stepper, PageView, NavigationBar, NestedNavigator |
| `mobile_advanced.h` | 1511 | Chart, PieChart, LineChart, Calendar, RatingBar, Stepper, SegmentedControl, ColorPicker, DateTimePicker, SearchBar, Banner, Chip/ChipGroup, BottomSheet, SpeedDial, TreeView, DataTable, Timeline, KanbanBoard |

#### Other Headers
| File | Lines | Description |
|------|-------|-------------|
| `window/window.h` | 353 | WindowFlags, EventType, Event, Window (SDL2 backend) |
| `input/input_manager.h` | ~300 | KeyCode, MouseButton, InputManager (callbacks) |
| `audio/audio_system.h` | 462 | AudioSystem (channels, groups, spatial audio, streaming) |
| `physics/physics2d.h` | 674 | BodyType, ShapeType, PhysicsMaterial (presets), Collider2D, CollisionLayer |
| `resources/resource_manager.h` | 391 | ResourceManager singleton (textures/fonts/shaders, caching, search paths) |
| `os/os_framebuffer.h` | 804 | GPU-free framebuffer: PixelFormat (7 formats), PixelColor, ClipRect, double-buffering, scaling, rotation |
| `world/tilemap.h` | 497 | Tileset, TilemapLayer, TileCell, MapObject, tilemap system for 2D games |

### 3.2 Source Files (`src/`)

| Directory | Files |
|-----------|-------|
| `core/` | `math.cpp`, `scene.cpp`, `timer.cpp`, `tween.cpp`, `types.cpp` |
| `rendering/` | `camera2d.cpp`, `context.cpp`, `framebuffer.cpp`, `renderer2d.cpp`, `shader.cpp`, `sprite.cpp` |
| `texture/` | `image.cpp`, `texture.cpp`, `texture_atlas.cpp` |
| `text/` | `arabic_text.cpp`, `font.cpp` |
| `drawing/` | `shapes.cpp` |
| `effects/` | `particle_system.cpp`, `post_processing.cpp` |
| `input/` | `input_manager.cpp` |
| `audio/` | `audio_system.cpp` |
| `physics/` | `physics2d.cpp` |
| `resources/` | `resource_manager.cpp` |
| `ui/` | `button.cpp`, `checkbox.cpp`, `label.cpp`, `panel.cpp`, `slider.cpp`, `textinput.cpp`, `ui_layout.cpp`, `widget.cpp` |
| `window/` | `window.cpp` |
| `world/` | `tilemap.cpp` |

### 3.3 Examples (`examples/`)

`resource_demo.cpp`, `simple_demo.cpp`, `sprite_demo.cpp`, `texture_demo.cpp`, `text_demo.cpp`, `ui_demo.cpp`, `ui_demo_complete.cpp`

### 3.4 Tests (`tests/`)

`test_arabic_text.cpp`, `test_color.cpp`, `test_main.cpp`, `test_resource_manager.cpp`, `test_systems.cpp`, `test_types.cpp`, `test_ui.cpp`

### 3.5 stdlib Integration (`stdlib/graphics/`)

| File | Lines | Description |
|------|-------|-------------|
| `graphics_module.h` | 207 | C function API for Sad Language bindings (windows, renderers, widgets) |
| `graphics_module.cpp` | 777 | SDL2+OpenGL implementation with global ID-based resource maps |
| `graphics_module_raylib.cpp` | 260 | Alternative Raylib backend implementation |
| `graphics_module_stub.cpp` | ~70 | No-op stub when no graphics backend available |

---

## 4. Module-by-Module Deep Dive

### 4.1 Core System

**Type Foundation** (`types.h`):
- Fixed-width aliases: `Int8/16/32/64`, `UInt8/16/32/64`, `Float32/64`, `u8`-`u64`, `i8`-`i64`
- Math types: `Vec2`, `Vec3`, `Vec4`, `Mat3`, `Mat4`, `Point2D`, `Size2D`, `Rect`
- `Color` struct with 20+ named presets (Red, Green, Blue, DarkRed, LightBlue, Gold, Pink, Transparent, etc.)
- `Handle` type for opaque resource handles
- `Result` enum: `Success`, `Failure`, `NotFound`, `InvalidParam`, `OutOfMemory`, `NotInitialized`, `AlreadyExists`
- `LogLevel` with `LogCallback` support

**ECS** (`ecs.h`):
- `Entity` = `UInt32`, `ComponentMask` = `bitset<64>`
- `ComponentPool<T>` — sparse set storage with type-erased interface
- `World` — entity creation/destruction, component add/get/remove, variadic `ForEach` queries
- `ISystem` interface with `Update(float dt)`, `Draw()`, `GetPriority()`

**EventBus** (`event_bus.h`):
- Thread-safe (mutex-protected)
- `Subscribe<T>`, `SubscribeOnce<T>`, `Unsubscribe`, `Publish<T>`, `PublishDeferred<T>`
- 14 built-in events: `FrameUpdateEvent`, `GameStartEvent`, `GamePauseEvent`, `GameResumeEvent`, `GameEndEvent`, `SceneChangeEvent`, `WindowResizeEvent`, `KeyPressEvent`, `KeyReleaseEvent`, `MouseClickEvent`, `MouseMoveEvent`, `MouseWheelEvent`, `CollisionEvent`, `ResourceLoadEvent`

**Math3D** (`math3d.h`):
- Constants: PI, HALF_PI, TWO_PI, DEG_TO_RAD, RAD_TO_DEG, EPSILON
- Functions: Radians, Degrees, Clamp, Saturate, Lerp, SmoothStep, SmootherStep, WrapAngle, NextPowerOf2, Sign
- **Quaternion** (`Quat`): FromAxisAngle, FromEuler, FromMatrix, LookRotation, SLERP, LERP, ToMatrix, ToEuler
- **Transform3D**: position + Quat rotation + scale → matrix conversion
- **Geometry**: Ray, Plane (intersection), AABB, Sphere, Frustum

**Profiler** (`profiler.h`):
- `HighResTimer` — high-resolution timing
- `MemoryTracker` — allocation/deallocation tracking with tags
- `Profiler` singleton — BeginSample/EndSample, BeginFrame/EndFrame, cumulative stats

**Scene** (`scene.h`):
- Entity struct (id, name, tag, position, size, rotation, scale, callbacks)
- Layer class (parallax, z-order, opacity)
- Scene class (OnEnter/OnExit lifecycle)
- SceneManager singleton (AddScene, SwitchTo, Update, Draw)

**State Machine** (`state_machine.h`):
- Generic `StateMachine<StateId>` with `StateDefinition` fluent builder
- `Transition<StateId>` with conditions and priorities
- Supports bidirectional/global transitions

**Timer** (`timer.h`):
- DeltaTime, FPS calculation, TimeScale
- `TimerManager`: SetTimeout, SetInterval, CancelEvent

**Tween** (`tween.h`):
- 30 easing functions (Linear → InOutBounce, all standard easing curves)
- `TweenManager`: To, TweenPosition, SetRepeat, yoyo support

### 4.2 Rendering System

**2D Renderer** (`renderer2d.h`):
- Batched OpenGL rendering (max 65536 vertices, 131072 indices per batch)
- Draw primitives: Line, Rect, RectOutline, Circle, Ellipse, Polygon, Triangle, Arc
- Text rendering: DrawText, DrawTextCentered, DrawTextAligned, DrawTextArabic, DrawTextAuto
- Texture rendering: DrawTexture, DrawTextureRotated, DrawTextureRegion, DrawTexturedQuad
- Transform stack: PushTransform, PopTransform, Translate, Rotate, Scale
- Orthographic projection

**3D Renderer** (`renderer3d.h`):
- Render queues: Background, Geometry, AlphaTest, Transparent, Overlay
- Render settings: shadows (cascaded), AA (None/FXAA/SMAA/TAA/MSAA 2x/4x/8x), HDR, bloom, tonemapping, fog, frustum/occlusion culling, instancing, LOD
- Shadow quality levels
- Renderable class with mesh + material + transform + bounds

**Camera 2D** (`camera2d.h`):
- Position, zoom, rotation with bounds
- Smooth follow with dead zone
- Camera shake
- ScreenToWorld / WorldToScreen projection
- Visibility testing

**Camera 3D** (`camera3d.h`):
- Perspective and orthographic projection
- Quaternion-based rotation
- Movement (Forward/Right/Up, local)
- RotateAround, LookAt
- Full view/projection matrix access

**Shader** (`shader.h`):
- Compile vertex/fragment/geometry shaders
- Uniform setters: Int, Float, Float2/3/4, Color, Matrix4, Texture
- Factory: CreateDefaultShader, CreateTextShader, CreateTextureShader

**Framebuffer** (`framebuffer.h`):
- Multi-attachment FBOs (color, depth, stencil)
- ReadPixel, Blit, BlitToScreen
- Resize support

**Lighting** (`lighting.h`):
- DirectionalLight (cascaded shadows)
- PointLight (range, attenuation)
- SpotLight (cone angles)
- AreaLight

**Material** (`material.h`):
- Material properties (variant: Float/Vec2/Vec3/Vec4/Color/Mat4/Texture/int)
- Blend states: Opaque, Alpha, Additive, Premultiplied
- Depth/stencil states

**Mesh** (`mesh.h`):
- Vertex3D (position, normal, texCoord, tangent, color)
- SkinnedVertex (up to 4 bone influences)
- MeshData: CalculateBounds, CalculateNormals, CalculateTangents, FlipNormals, Transform, Merge

### 4.3 Drawing System

**Shapes** (`shapes.h`) — Advanced vector drawing:
- **Stroke**: LineCap (Butt/Round/Square), LineJoin (Miter/Round/Bevel), DashPattern (Solid/Dashed/Dotted/DashDot), StrokeStyle
- **Fill**: Gradient (Linear/Radial/Angular with stops), FillStyle (None/Solid/Gradient)
- **Shapes**: Arc, Pie, Ring, RoundedRect, RegularPolygon, Star
- **Curves**: QuadraticBezier, CubicBezier, Spline
- **Path**: Full vector path API (MoveTo, LineTo, QuadraticTo, CubicTo, ArcTo, Close) with built-in shape helpers and transformations (Translate/Scale/Rotate)
- **ShapeRenderer**: Batched rendering of all shapes with anti-aliasing option
- **Special shapes**: Arrow, DoubleArrow, Grid, X mark, Checkmark, Heart

### 4.4 Effects System

**Particle System** (`particle_system.h`):
- Emitter shapes: Point, Circle, Rectangle, Line, Ring
- Blend modes: Alpha, Additive, Multiply
- Properties: emission rate, burst, lifetime, speed, direction, gravity, radial/tangential acceleration, damping, size over lifetime, rotation, color gradient over lifetime
- 6 presets: Fire, Smoke, Sparks, Explosion, Snow, Rain

**Post-Processing** (`post_processing.h`):
- 18 effect types: None, Bloom, Blur, GaussianBlur, MotionBlur, ChromaticAberration, Vignette, ColorGrading, Grayscale, Sepia, Invert, Pixelate, CRT, FilmGrain, Scanlines, Sharpen, EdgeDetection, Custom
- Full pipeline with ping-pong FBOs
- Effect classes: BloomEffect, BlurEffect, ChromaticAberrationEffect, VignetteEffect, ColorGradingEffect, PixelateEffect, CRTEffect, FilmGrainEffect
- 5 presets: Cinematic, Retro, Dream, Horror, BlackAndWhite

### 4.5 Text & Arabic Support

**Font** (`font.h`):
- TrueType loading via stb_truetype
- Texture atlas generation (basic ASCII + extended Unicode ranges)
- `GenerateArabicAtlas()` — ASCII + all Arabic Unicode ranges
- Glyph metrics, kerning, MeasureText, MeasureTextUTF8
- Line height, baseline, ascent, descent

**Arabic Text** (`arabic_text.h`):
- Full UTF-8 encode/decode
- Arabic character classification (IsArabic, IsArabicJoining, IsRTL, IsDiacritic)
- Arabic shaping: 4 forms (Isolated, Initial, Medial, Final)
- Simple BiDi reordering for mixed LTR/RTL text
- Arabic Unicode range tables

### 4.6 UI System

**Two parallel UI systems exist:**

#### System A: Desktop Widgets (`Graphics::UI` namespace)
- `Widget` base → `Button`, `Label`, `Checkbox`, `Slider`, `TextInput`, `Panel`
- Simple positional layout with optional Panel layouts (Vertical/Horizontal/Grid)
- Direct mouse/keyboard event handling

#### System B: Flexbox/CSS Grid Layout Engine (`sad::graphics::ui` namespace)
- Full **Flexbox** implementation: direction, wrap, justify-content, align-items, align-content, gap
- **CSS-style units**: px, %, em, vh, vw, auto
- **Style system**: EdgeInsets, LayoutStyle, PositionMode (Relative/Absolute)
- **Element** base with fluent API: Width(), Height(), FlexGrow(), Margin(), Padding(), BackgroundColor()
- **Containers**: FlexContainer, Row, Column, ScrollView, Stack
- **Widgets**: Label, Button, Image, ProgressBar, Slider, Checkbox, RadioButton, TextInput
- **LayoutEngine**: recursive Flex layout calculation, event dispatch, focus management
- **Builder helpers**: MakeRow(), MakeColumn(), MakeLabel(), MakeButton(), MakeSlider(), etc.

### 4.7 Audio System

**AudioSystem** (`audio_system.h`):
- Initialize with sample rate, channels, buffer size
- Sound playback: Play, Pause, Resume, Stop
- PlaybackSettings: volume, pitch, loop, fade in/out
- SpatialSettings: 2D positional audio with listener position
- Audio groups for volume control
- Channel management

### 4.8 Physics 2D

**Physics2D** (`physics2d.h`):
- Body types: Static, Kinematic, Dynamic
- Shape types: Circle, Box, Polygon, Edge, Chain
- PhysicsMaterial presets: Default, Bouncy, Icy, Sticky
- Collision detection: ContactPoint, CollisionInfo, RaycastHit
- Collision layers (16 layers, 2 reserved: Default, UI)
- Collider2D: CreateCircle, CreateBox, CreatePolygon, CreateEdge

### 4.9 World / Tilemap

**Tilemap** (`tilemap.h`):
- Tile flags: FlipH/V/Diagonal, Rotate 90/180/270, Collision, Trigger, OneWay, Animated
- TileDefinition with custom properties and animation frames
- Tileset: LoadFromImage with margins/spacing, LoadFromAtlas
- TilemapLayer: set/get/clear tiles, fill, parallax, opacity
- MapObject: Rectangle, Ellipse, Point, Polygon, Polyline shapes

### 4.10 OS Framebuffer

**OS Framebuffer** (`os_framebuffer.h`) — **GPU-free rendering**:
- 7 pixel formats: RGB565, RGB888, RGBA8888, BGR888, BGRA8888, Grayscale8, Indexed8
- PixelColor with blend, pack/unpack, format conversion
- ClipRect intersection
- ColorPalette (256 colors)
- Direct pixel manipulation for embedded/OS-level graphics
- Double buffering, scaling, rotation

---

## 5. Class/Struct/Enum Registry

### Enums (70+)

| Module | Enum | Values |
|--------|------|--------|
| Core | `Result` | Success, Failure, NotFound, InvalidParam, OutOfMemory, NotInitialized, AlreadyExists |
| Core | `LogLevel` | Debug, Info, Warning, Error, Fatal |
| Core | `EaseType` | 30 easing functions (Linear → InOutBounce) |
| Core | `TweenState` | Playing, Paused, Stopped, Completed |
| Rendering | `ShaderType` | Vertex, Fragment, Geometry |
| Rendering | `DrawMode` | Triangles, Lines, Points |
| Rendering | `ProjectionType` | Perspective, Orthographic |
| Rendering | `RenderQueue` | Background, Geometry, AlphaTest, Transparent, Overlay |
| Rendering | `LightType` | Directional, Point, Spot, Area |
| Rendering | `ShadowQuality` | (Multiple levels) |
| Rendering | `AntiAliasingMode` | None, FXAA, SMAA, TAA, MSAA2x, MSAA4x, MSAA8x |
| Rendering | `PrimitiveType` | (Mesh primitive types) |
| Texture | `TextureFormat` | 16 formats: R8 → DEPTH24_STENCIL8 inc. float |
| Texture | `TextureWrap` | REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER |
| Texture | `TextureFilter` | NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, etc. |
| Texture | `ImageFormat` | PNG, JPG, BMP, TGA, HDR, PSD, GIF |
| Text | `TextDirection` | LTR, RTL, Auto |
| Text | `ArabicForm` | Isolated, Initial, Medial, Final |
| Drawing | `LineCap` | Butt, Round, Square |
| Drawing | `LineJoin` | Miter, Round, Bevel |
| Drawing | `GradientType` | Linear, Radial, Angular |
| Drawing | `PathCommandType` | MoveTo, LineTo, QuadraticTo, CubicTo, ArcTo, Close |
| Effects | `EmitterShape` | Point, Circle, Rectangle, Line, Ring |
| Effects | `ParticleBlendMode` | Alpha, Additive, Multiply |
| Effects | `PostEffectType` | 18 types |
| Window | `WindowFlags` | Fullscreen, Resizable, Borderless, Hidden, etc. |
| Window | `EventType` | (Window event types) |
| Input | `KeyCode` | Letters, numbers, arrows, F-keys, modifiers |
| Input | `MouseButton` | Left, Right, Middle, etc. |
| Input | `KeyState` | Pressed, Held, Released |
| Physics | `BodyType` | Static, Kinematic, Dynamic |
| Physics | `ShapeType` | Circle, Box, Polygon, Edge, Chain |
| UI Layout | `FlexDirection` | Row, RowReverse, Column, ColumnReverse |
| UI Layout | `FlexWrap` | NoWrap, Wrap, WrapReverse |
| UI Layout | `JustifyContent` | FlexStart, FlexEnd, Center, SpaceBetween, SpaceAround, SpaceEvenly |
| UI Layout | `AlignItems` | FlexStart, FlexEnd, Center, Stretch, Baseline |
| UI Layout | `AlignSelf` | Auto + all AlignItems values |
| UI Layout | `AlignContent` | FlexStart, FlexEnd, Center, Stretch, SpaceBetween, SpaceAround |
| UI Layout | `PositionMode` | Relative, Absolute |
| UI Layout | `Unit` | Auto, Pixels, Percent, Em, Vh, Vw |
| UI | `TextAlignment` | 12 positions (Left/Center/Right × Top/Middle/Bottom + Left/Center/Right) |
| UI | `SliderOrientation` | Horizontal, Vertical |
| UI | `PanelLayout` | None, Vertical, Horizontal, Grid |
| UI | `WidgetState` | Normal, Hovered, Pressed, Disabled |
| Resource | `ResourceType` | Texture, Font, Shader, Unknown |
| World | `TilemapLayerType` | Tiles, Objects, Image |
| World | `MapObjectShape` | Rectangle, Ellipse, Point, Polygon, Polyline |
| OS | `PixelFormat` | 7 formats |
| Mobile | `MainAxisAlignment` | Start, End, Center, SpaceBetween, SpaceAround, SpaceEvenly |
| Mobile | `CrossAxisAlignment` | Start, End, Center, Stretch, Baseline |
| Mobile | `ButtonShape` | Rectangle, RoundedRect, Stadium, Circle, BeveledRect |
| Mobile | `ButtonSize` | Small, Medium, Large, Custom |
| Mobile | `KeyboardType` | Text, Number, Decimal, Phone, Email, Url, Password, Multiline |
| Mobile | `TextFieldStyle` | Outlined, Filled, Underlined, None |
| Mobile | `CardVariant` | Elevated, Filled, Outlined |
| Mobile | `AppBarStyle` | Standard, Large, Medium, CenterAligned, Transparent, Collapsing |
| Mobile | `ChartType` | Bar, Line, Area, Scatter, Bubble |
| Mobile | `BoxFit` | Fill, Contain, Cover, FitWidth, FitHeight, None, ScaleDown |
| Mobile | `StackFit` | Loose, Expand, Passthrough |

### Major Classes (80+)

**Core**: World, EventBus, SceneManager, StateMachine<T>, Timer, TimerManager, TweenManager, Profiler, MemoryTracker, JsonSerializer, Value

**Rendering**: RenderContext, ShaderProgram, Renderer2D, Renderer3D, Framebuffer, Camera2D, Camera3D, Sprite, Light, DirectionalLight, PointLight, SpotLight, Material, Mesh/MeshData, Renderable, RenderQueueManager

**Texture**: Texture, Image, TextureAtlas

**Text**: Font, ArabicText

**Drawing**: Path, ShapeRenderer

**Effects**: ParticleSystem, PostEffect (+ 7 subclasses), PostProcessingPipeline

**UI Desktop**: Widget, Button, Label, Checkbox, Slider, TextInput, Panel

**UI Layout**: Element, FlexContainer, Row, Column, ScrollView, Stack, Label, Button, Image, ProgressBar, Slider, Checkbox, RadioButton, TextInput, LayoutEngine

**Mobile (~100+ classes)**: Container, Row, Column, Stack, Padding, Center, Align, SizedBox, Expanded, Flexible, Spacer, AspectRatio, ElevatedButton, TextButton, OutlinedButton, IconButton, FloatingActionButton, ToggleButton, SegmentedButton, DropdownButton, PopupMenuButton, AlertDialog, BottomSheet, ModalBottomSheet, Snackbar, Toast, Dialog, SimpleDialog, FullScreenDialog, Banner, AppNotification, Card, Chip, Badge, Avatar, Tooltip, ProgressBar, CircularProgress, Skeleton, Divider, ListTile, ExpansionTile, DataTable, Carousel, Timeline, EmptyState, TextField, PasswordField, SearchBar, OTPInput, PinInput, DatePicker, TimePicker, DateTimePicker, ColorPicker, FilePicker, ImagePicker, Autocomplete, Dropdown, MultiSelect, RatingInput, ListView, GridView, AnimatedList, ReorderableList, TreeView, VirtualizedList, GroupedList, InfiniteScroll, SwipeableList, NestedScrollView, VideoPlayer, ImageViewer, PhotoGallery, CameraView, UploadProgress, AnimatedWidget, AnimatedSwitcher, RippleEffect, PageTransition, GestureDetector, AppBar, BottomNavigationBar, TabBar, Drawer, NavigationRail, Breadcrumbs, Stepper, PageView, NavigationBar, NestedNavigator, Chart, PieChart, LineChart, Calendar, RatingBar, SegmentedControl, SpeedDial, KanbanBoard

**Other**: Window, InputManager, AudioSystem, Collider2D, ResourceManager, Tileset, TilemapLayer, MapObject, Framebuffer (OS)

---

## 6. Rendering Pipeline

```
┌─────────────┐
│  Window      │  SDL2 window creation
│  (SDL2)      │  Event polling
└──────┬───────┘
       │
┌──────▼───────┐
│ RenderContext │  OpenGL 3.3 Core Profile
│  (GLAD)      │  MSAA, VSync, blending, depth
└──────┬───────┘
       │
┌──────▼───────────────────────────────────────────────┐
│                   PostProcessingPipeline              │
│  Begin → renders to internal FBO                     │
│  ┌──────────────────────────────────────────────┐    │
│  │            Renderer2D (Batched)               │    │
│  │  Shapes → Text → Textures → Sprites          │    │
│  │  Transform stack (push/pop)                   │    │
│  │  Orthographic projection                      │    │
│  └──────────────────────────────────────────────┘    │
│  ┌──────────────────────────────────────────────┐    │
│  │            ShapeRenderer                      │    │
│  │  Paths, Beziers, Gradients, Anti-aliasing    │    │
│  └──────────────────────────────────────────────┘    │
│  ┌──────────────────────────────────────────────┐    │
│  │            ParticleSystem                     │    │
│  │  Point/Circle/Rect/Line/Ring emitters        │    │
│  └──────────────────────────────────────────────┘    │
│  End → applies effect chain → screen                 │
│  Bloom → Blur → ChromAb → Vignette → ColorGrade     │
└──────────────────────────────────────────────────────┘
       │
┌──────▼───────┐
│  Swap Buffers │
│  (SDL2)      │
└──────────────┘
```

**3D Pipeline** (header-defined, implementation status unknown):
```
Renderable → RenderQueueManager → sort by queue/distance
  ├─ Background queue
  ├─ Geometry queue (opaque, front-to-back)
  ├─ AlphaTest queue
  ├─ Transparent queue (back-to-front)
  └─ Overlay queue
Lighting: Directional (cascaded shadows) + Point + Spot + Area
Materials: Blend, depth, stencil states
Settings: HDR, bloom, tonemapping, fog, LOD, instancing, occlusion culling
```

---

## 7. UI System Architecture

Three tiers of UI:

### Tier 1: Basic Desktop Widgets (`Graphics::UI`)
- Simple widget tree with position/size
- Panel provides basic Vertical/Horizontal/Grid layout
- Direct event handling (OnMouseDown, OnKeyDown, etc.)
- Best for: simple desktop tool UIs

### Tier 2: Flexbox Layout Engine (`sad::graphics::ui`)
- CSS Flexbox-equivalent layout computation
- 6 CSS-like units (px, %, em, vh, vw, auto)
- Full element tree with event bubbling
- LayoutEngine calculates bounds recursively
- Row/Column/ScrollView/Stack containers
- 8 widget types built-in
- Builder helper functions (MakeRow, MakeButton, etc.)
- Best for: responsive desktop/web-style layouts

### Tier 3: Mobile Flutter-Style Components (`sad::graphics::ui::mobile`)
- **100+ components** across 9 header files (10,000+ lines total)
- Material Design 3 styling (border radius 28px, elevation, etc.)
- Component categories:
  - Core layout (17): Container, Row, Column, Stack, Padding, Center, etc.
  - Buttons (10): ElevatedButton, TextButton, OutlinedButton, IconButton, FAB, etc.
  - Dialogs (10): AlertDialog, BottomSheet, Snackbar, Toast, FullScreenDialog, etc.
  - Display (15): Card, Chip, Badge, Avatar, Tooltip, ProgressBar, Carousel, Timeline, etc.
  - Input (15): TextField, PasswordField, SearchBar, OTPInput, DatePicker, ColorPicker, etc.
  - Lists (10): ListView, GridView, AnimatedList, TreeView, VirtualizedList, InfiniteScroll, etc.
  - Media (10): VideoPlayer, ImageViewer, PhotoGallery, CameraView, GestureDetector, etc.
  - Navigation (10): AppBar, BottomNavigationBar, TabBar, Drawer, NavigationRail, Stepper, etc.
  - Advanced (18): Chart, PieChart, LineChart, Calendar, RatingBar, KanbanBoard, etc.
- Best for: mobile apps and Material Design interfaces

---

## 8. Mobile UI Framework

The mobile framework is an ambitious Flutter-equivalent with 100+ components:

**Component count by file**:
| File | Count | Total Lines |
|------|-------|-------------|
| `mobile_core.h` | 17 | 1,167 |
| `mobile_buttons.h` | 10 | 897 |
| `mobile_dialogs.h` | 10 | 1,146 |
| `mobile_display.h` | 15 | 1,262 |
| `mobile_input.h` | 15 | 1,527 |
| `mobile_lists.h` | 10 | 1,166 |
| `mobile_media.h` | 10 | 1,246 |
| `mobile_navigation.h` | 10 | 1,038 |
| `mobile_advanced.h` | 18 | 1,511 |
| **TOTAL** | **~115** | **~9,960** |

**Key design features**:
- Each widget is a class inheriting from `Widget` or `MobileWidget`
- Properties stored directly as public members
- Callback-driven (`std::function<void()>` for events)
- Animation built into many widgets (animProgress, animSpeed)
- Material Design 3 defaults (colors, border radii, elevations)
- RTL awareness (for Arabic)

---

## 9. 3D Capabilities

**Current 3D support is headers-only (designed but likely not fully implemented)**:

- `Camera3D` — quaternion-based, perspective/ortho, orbit
- `Renderer3D` — render queues, shadow/AA/HDR/bloom settings
- `Lighting` — 4 light types with cascaded shadows
- `Material` — full PBR-ready material system (blend, depth, stencil states)
- `Mesh` — Vertex3D with position/normal/texCoord/tangent/color, skinned vertices (4 bones)
- `math3d.h` — Quaternion, Transform3D, Ray, Plane, AABB, Sphere, Frustum

**Note**: There are no `.cpp` implementation files for renderer3d, camera3d, lighting, material, or mesh in `src/`. These are header-declared interfaces awaiting implementation.

---

## 10. stdlib Integration Layer

The `stdlib/graphics/` directory provides Sad Language bindings:

**Architecture**:
```
Sad Language Script → Interpreter → graphics_module.h functions → Backend
```

**Dual backend design**:
1. **SDL2 + OpenGL** (`graphics_module.cpp`) — uses the graphics library directly
2. **Raylib** (`graphics_module_raylib.cpp`) — alternative lightweight backend
3. **Stub** (`graphics_module_stub.cpp`) — no-op fallback

**Resource management**: Global ID-based maps (`g_windows`, `g_renderers`, `g_widgets`) with auto-incrementing IDs.

**API surface** (from `graphics_module.h`):
- Window: create, show, hide, close, should_close, poll_events, swap_buffers
- Renderer: create, begin/end frame, clear, draw_line, draw_rect, draw_circle, draw_text
- UI: label (create/set/get text), button (create/is_clicked/set text), textinput (create/get/set value), checkbox (create/is_checked/set checked)
- Widget: draw, update, mouse_event

---

## 11. Build System

**CMakeLists.txt** (264 lines):
```cmake
cmake_minimum_required(VERSION 3.15)
project(SadGraphics VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
```

- Builds `sad_graphics` as shared or static library
- Includes all source files from `src/` subdirectories
- Links SDL2 (local copy), GLAD, stb_image, stb_truetype
- Builds examples if `BUILD_EXAMPLES=ON`
- Builds tests if `BUILD_TESTS=ON` (Catch2)
- Exports include directories for consumers

---

## 12. Development State & Known Issues

### Build Status: NOT COMPILING

Per `BUILD_REPORT.md`, the following headers have missing `#include "../core/types.h"`:
- `rendering/context.h` (reportedly fixed)
- `rendering/shader.h`
- `rendering/renderer2d.h`
- `texture/texture.h`
- `texture/image.h`
- `input/input_manager.h`

### Implementation completeness:

| Module | Headers | Source | Status |
|--------|---------|--------|--------|
| Core types | ✅ | ✅ types.cpp | Likely functional |
| Core ECS | ✅ | ❌ No .cpp | Header-only (template) |
| Core EventBus | ✅ | ❌ No .cpp | Likely header-only |
| Core math3d | ✅ | ✅ math.cpp | Partial |
| Core scene | ✅ | ✅ scene.cpp | Partial |
| Core timer/tween | ✅ | ✅ | Likely functional |
| Rendering 2D | ✅ | ✅ | Core pipeline |
| Rendering 3D | ✅ | ❌ No .cpp files | **Headers only, not implemented** |
| Drawing shapes | ✅ | ✅ shapes.cpp | Likely functional |
| Effects | ✅ | ✅ both .cpp | Likely functional |
| Text/Arabic | ✅ | ✅ both .cpp | Likely functional |
| UI desktop | ✅ | ✅ all .cpp | Likely functional |
| UI layout | ✅ | ✅ ui_layout.cpp | Likely functional |
| UI mobile | ✅ (9,960 lines) | ❌ No .cpp files | **Headers only, not implemented** |
| Audio | ✅ | ✅ | Partial (no audio backend?) |
| Physics2D | ✅ | ✅ | Partial |
| Tilemap | ✅ | ✅ | Likely functional |
| OS Framebuffer | ✅ | ❌ No .cpp file | Possibly header-only |
| Resource mgr | ✅ | ✅ | Likely functional |

### Key Gaps:
1. **3D rendering**: All headers but no implementation files
2. **Mobile UI**: 100+ components defined in headers but no `.cpp` implementations
3. **Build errors**: Missing includes preventing compilation
4. **Audio backend**: Header defined but unclear what audio library is actually used
5. **OS Framebuffer**: 804-line header but no source implementation

---

## 13. Expansion Recommendations

### Priority 1: Fix Build
- Add missing `#include "../core/types.h"` to all affected headers
- Verify CMake configuration compiles cleanly
- Run existing test suite

### Priority 2: Complete 3D Pipeline
- Implement `renderer3d.cpp`, `camera3d.cpp`, `lighting.cpp`, `material.cpp`, `mesh.cpp`
- Add 3D model loading (OBJ, glTF)
- Implement shadow mapping

### Priority 3: Mobile UI Implementation
- Implement `.cpp` files for all 9 mobile header files
- Start with core layout, then buttons, then input components
- Consider implementing a mobile rendering backend (touch events, gestures)

### Priority 4: Additional Systems
- **Networking**: Multiplayer/online capabilities
- **Scripting**: Hot-reload shader editing
- **Animation**: Skeletal animation system (vertex skinning infrastructure exists)
- **Navigation mesh**: For 2D/3D pathfinding
- **Editor tools**: Scene editor, particle editor, material editor

### Priority 5: Polish
- Complete audio backend integration
- Add gamepad input support
- Add accessibility features
- Performance profiling and optimization
- Documentation generation from bilingual comments

---

*This analysis covers every file in `c:\s\s_language\graphics\` and `c:\s\s_language\stdlib\graphics\`.*  
*Total header lines analyzed: ~25,000+*  
*Total classes documented: ~200+*  
*Total enums documented: ~70+*
