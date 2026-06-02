# PrismDraft

PrismDraft is a constrained C11 modeling and rendering prototype for vector-like, faceted hard-surface 3D art.<br/>**PrismDraft 是一个受强约束的 C11 建模与渲染原型，目标是制作类似矢量图形的低面硬表面 3D 美术。**

It aims for spatial poster drafting rather than a general DCC workflow: flat color blocks, hard edges, orthographic or near-isometric composition, clean silhouettes, and export-friendly mesh data.<br/>**它更接近“空间海报草图工具”，而不是通用 DCC：强调扁平色块、硬边、正交或近等轴构图、清晰轮廓，以及适合导出的网格数据。**

## Status

- The core mesh model, storage, validation, cube fixture, face inset, extrude, bevel, and quad loop-cut paths are implemented with indexed C data structures.<br/>**核心网格模型、存储、校验、立方体 fixture、面内缩、挤出、倒角和四边面 loop cut 已用索引式 C 数据结构实现。**
- The Raylib viewport supports hard-step shading, depth/normal edge separation, anti-aliased internal rendering, projected ground shadows, a semi-transparent ground plane, dashed grid, and orthographic/perspective camera switching.<br/>**Raylib 视口已支持硬阶梯光照、深度/法线描边分离、内部抗锯齿渲染、地面投影阴影、半透明地面、虚线网格，以及正交/透视相机切换。**
- The editor surface supports face picking, selected-face highlight, object transform controls, visual tuning, lighting controls, grouped operation panels, and multiple independent cube objects.<br/>**编辑器表层已支持面拾取、选中面高亮、对象变换控制、视觉调参、光照控制、分组操作面板，以及多个独立方块对象。**
- The export and animation smoke paths cover glTF/GLB writing and a minimal object-level PRS timeline demo.<br/>**导出与动画 smoke 路径已覆盖 glTF/GLB 写出，以及一个最小对象级 PRS 时间线演示。**
- Deeper multi-object UX remains open: explicit object list, delete/duplicate actions, object names, and clearer object selection controls.<br/>**更完整的多对象体验仍在待办中：显式对象列表、删除/复制、对象命名，以及更清晰的对象选择控件。**

## Visual Target

The `sample/` directory is the visual target set for the project.<br/>**`sample/` 目录是项目的视觉对标集合。**

- Use `sample/sample1.png` through `sample/sample4.png` to judge whether viewport work still feels graphic, flat, hard-edged, and low-detail.<br/>**使用 `sample/sample1.png` 到 `sample/sample4.png` 判断视口效果是否仍然保持图形化、扁平、硬边和低细节。**
- The local design mirror is in `docs/prismdraft-design.md`, with project framing in `docs/project-brief.md` and visual checkpoints in `docs/visual-reference.md` and `docs/visual-checks.md`.<br/>**本地设计镜像在 `docs/prismdraft-design.md`，项目定位在 `docs/project-brief.md`，视觉检查点在 `docs/visual-reference.md` 和 `docs/visual-checks.md`。**

## Requirements

- Windows development environment with Visual Studio C tools, CMake, Ninja, and a local Raylib source checkout.<br/>**Windows 开发环境，需要 Visual Studio C 工具链、CMake、Ninja，以及本地 Raylib 源码检出。**
- The helper scripts under `tools/` auto-detect common tool locations; `tools/FindRaylib.ps1` also accepts `RAYLIB_SOURCE_DIR` for the Raylib source path.<br/>**`tools/` 下的辅助脚本会自动探测常见工具位置；`tools/FindRaylib.ps1` 也接受 `RAYLIB_SOURCE_DIR` 指定 Raylib 源码路径。**
- The CMake cache requires `PRISMDRAFT_RAYLIB_SOURCE_DIR`; prefer `tools/ConfigureBuild.ps1` instead of calling CMake by hand.<br/>**CMake cache 需要 `PRISMDRAFT_RAYLIB_SOURCE_DIR`；建议优先使用 `tools/ConfigureBuild.ps1`，不要手动拼 CMake 命令。**

## Quick Start

Open the interactive viewport from the repository root:<br/>**在仓库根目录打开交互式视口：**

```powershell
RunPrismDraft.cmd
```

Equivalent direct smoke command:<br/>**等价的直接 smoke 命令：**

```powershell
tools\SmokeViewportInteractive.cmd
```

The window exits when closed. The default automated viewport smoke writes `captures/phase2_cube.png`.<br/>**关闭窗口后进程会退出。默认自动化视口 smoke 会写出 `captures/phase2_cube.png`。**

## Validation

Run the configured validation workflow:<br/>**运行已配置的验证工作流：**

```powershell
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Validate.cmd
```

Run the full smoke workflow:<br/>**运行完整 smoke 工作流：**

```powershell
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Smoke.cmd
```

Useful focused smoke commands:<br/>**常用的聚焦 smoke 命令：**

```powershell
tools\SmokeViewport.cmd
tools\SmokeInteraction.cmd
tools\SmokeExport.cmd
tools\SmokeTimeline.cmd
tools\SmokeViewportLifecycle.cmd
```

## Architecture

PrismDraft uses strict one-way layer dependencies.<br/>**PrismDraft 使用严格的单向层依赖。**

```text
app -> editor -> render -> engine -> core
export -> core
animation -> core
```

- `core`: pure C11 mesh data, storage, validation, fixtures, and topology mutation controllers.<br/>**`core`：纯 C11 网格数据、存储、校验、fixture 和拓扑变更控制器。**
- `engine`: Raylib-facing camera and window configuration boundaries.<br/>**`engine`：面向 Raylib 的相机与窗口配置边界。**
- `render`: mesh flattening, shader configuration, render targets, hard-step shading, edge pass, ground, and shadow configuration.<br/>**`render`：网格展开、shader 配置、渲染目标、硬阶梯光照、描边 pass、地面和阴影配置。**
- `editor`: selection, picking, scene state, transform state, visual state, panel state, and modeling service orchestration.<br/>**`editor`：选择、拾取、场景状态、变换状态、视觉状态、面板状态和建模服务编排。**
- `app`: application context, lifecycle, viewport frame loop, export smoke, and timeline demo wiring.<br/>**`app`：应用上下文、生命周期、视口帧循环、导出 smoke 和时间线演示连接。**
- `export`: glTF and GLB serialization from flattened mesh buffers.<br/>**`export`：从展开网格 buffer 序列化 glTF 和 GLB。**
- `animation`: object-level PRS data and timeline sampling.<br/>**`animation`：对象级 PRS 数据与时间线采样。**

Every source file and public symbol should follow the project naming grammar, documented in `docs/codex-architecture-workflow.md`.<br/>**所有源码文件和公开符号都应遵循项目命名语法，详见 `docs/codex-architecture-workflow.md`。**

## Controls

- `F1` Model, `F2` Move, `F3` Visual, `F4` Light, `F5` View, `F6` Ground, and `Tab` toggles the operation panel.<br/>**`F1` 模型、`F2` 移动、`F3` 视觉、`F4` 光照、`F5` 视图、`F6` 地面，`Tab` 切换操作面板显示。**
- The Model panel applies inset, extrude, bevel, loop cut, and creates new cube objects.<br/>**Model 面板可执行内缩、挤出、倒角、loop cut，并创建新的方块对象。**
- The Move, Visual, Light, View, and Ground panels expose direct controls for transform, color, outline, lighting, camera projection, ground visibility, ground color, and ground height.<br/>**Move、Visual、Light、View 和 Ground 面板提供变换、颜色、描边、光照、相机投影、地面显隐、地面颜色和地面高度的直接控制。**
- Mouse controls include left-click face picking, right-drag orbit, middle-drag pan, and wheel zoom.<br/>**鼠标控制包括左键面拾取、右键拖拽环绕、中键拖拽平移和滚轮缩放。**

## Repository Map

- `include/prismdraft/`: public C headers grouped by architecture layer.<br/>**`include/prismdraft/`：按架构层组织的公开 C 头文件。**
- `src/`: C11 implementation files, named with `pd_<layer>_<business>_<role>`.<br/>**`src/`：C11 实现文件，按 `pd_<layer>_<business>_<role>` 命名。**
- `tests/`: CTest-backed unit and focused behavior tests.<br/>**`tests/`：基于 CTest 的单元测试和聚焦行为测试。**
- `shaders/`: GLSL shader assets for hard-step, depth, normal, and edge passes.<br/>**`shaders/`：硬阶梯、深度、法线和描边 pass 使用的 GLSL shader 资源。**
- `tools/`: Windows PowerShell and CMD wrappers for configure, build, test, smoke, lifecycle, and architecture checks.<br/>**`tools/`：用于配置、构建、测试、smoke、生命周期和架构检查的 Windows PowerShell/CMD 包装脚本。**
- `docs/`: design mirror, architecture workflow, operation workflow, TODO, visual checks, and smoke documentation.<br/>**`docs/`：设计镜像、架构工作流、操作工作流、TODO、视觉检查和 smoke 文档。**
- `sample/`: user-provided visual references for the target modeling and rendering style.<br/>**`sample/`：用户提供的建模与渲染风格视觉参考。**

## Development Notes

- Keep core code pure C11 and independent of Raylib, OpenGL, UI, and app-layer state.<br/>**保持 core 代码为纯 C11，并独立于 Raylib、OpenGL、UI 和 app 层状态。**
- Preserve indexed, contiguous mesh storage; do not introduce pointer-linked topology or per-element heap allocation in modeling operations.<br/>**保持索引式、连续的网格存储；不要在建模操作中引入指针链接拓扑或逐元素堆分配。**
- Run the architecture guard when changing source files or architecture-sensitive directories.<br/>**修改源码或架构敏感目录时运行架构守卫。**

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\CheckArchitecture.ps1
```

- After each completed small transaction, commit and push through the configured project git workflow.<br/>**每完成一个小事务后，通过已配置的项目 git workflow 提交并推送。**

