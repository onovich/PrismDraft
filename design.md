PrismDraft：独立式矢量图形化 3D 建模引擎设计文档 (PRD & Architecture Specification)

本文件旨在为 AI 智能体（如 Claude Code、Cursor、OpenClaw）提供清晰、严谨的工程指令，用以完全自研一套独立于任何商业游戏引擎的极简 3D 建模与渲染工具。

1. 项目概述与愿景 (Project Vision)

1.1 核心目标

构建一个轻量级、跨平台的独立 3D 软件，专注于特定艺术风格（极简图形化、三维矢量插画风、平直多边形海报风）的几何体建模、动态演示与资产导出。产出的资产能够无缝导入任何现代游戏引擎（如 Godot、Unity、Unreal）。

1.2 核心做减法原则（不包含的功能）

无贴图、无 UV、无传统材质： 彻底剥离纹理采样系统与复杂的 PBR（基于物理的渲染）材质节点。

无雕刻功能： 放弃高模雕刻与动态网格（Dynamic Topology），专注于硬表面与精确的几何拓扑结构。

无毛发、无高级粒子物理系统： 剔除所有毛发动力学、布料模拟及复杂的流体/粒子系统。

1.3 核心做加法原则（必须包含的功能）

纯顶点色（Vertex Color）驱动： 颜色信息完全依赖几何顶点，随拓扑结构一同计算与传递。

极致的刻面硬光影（Faceted Hard-Step Shading）： 纯粹由法线与光照方向决定的二值化光影，呈现出矢量图形般的平面色块视觉效果，拒绝任何表面平滑渐变（Smooth Shading）。

高精度边缘表现（Graphic Edge）： 视口与导出效果必须具备极强的海报级排版感，模型轮廓和明暗交界线清晰硬朗。

1.4 项目名称与品牌定位 (Naming & Positioning)

项目代号：PrismDraft

定位解析： Prism（棱柱/多面体切面）+ Draft（排版起草/设计绘制）。该名称摒弃了传统的 CAD 或 Maker 等易混淆后缀，将 3D 建模的行为升华为“空间中的矢量排版起草”。它直接向用户传递了产品的视觉内核：硬边缘切割的纯净几何体，以及像绘制平面海报一样精准、轻量的 3D 构建体验。专为 UI 设计师、矢量插画师和追求极简硬表面美学的独立游戏开发者打造。

2. 技术选型 (Tech Stack)

核心语言： 纯 C 语言 (C11 标准)。内核部分严禁引入 C++ 面向对象的类层次、虚函数多态与复杂模板，确保执行流的绝对透明性与可预测性。

图形与窗口库： Raylib。仅将其作为窗口管理、输入处理、基础数学库以及底层图形 API（OpenGL 3.3+）的包装层，避免引入任何重型引擎生态。

渲染着色器： GLSL (OpenGL 3.3+ 标准)。

资产导出格式： glTF 2.0 规范 (.gltf / .glb)。仅序列化几何数据（位置、法线、顶点色），不附加纹理通道，确保全引擎通用。

3. 架构设计原则 (Architectural Principles)

为了确保 AI 智能体在 Vibe Coding 范式下能够产出高稳定、无内存越界、极具可读性的代码，整套系统必须硬性锁定以下设计规范：

3.1 面向数据编程 (DOP) 内存布局

拒绝分散的堆内存分配： 禁止在建模操作中频繁为单个点、线、面执行 malloc 或 free。

连续内存块（Flat Arrays）： 所有的几何体元素必须存储在预先分配、连续的结构体数组（紧凑的内存块）中。

符号化索引引用： 元素之间的引用（例如半边指向顶点、面指向半边）通过 数组索引 (uint32_t) 替代裸指针（Raw Pointers）。这能防止因数组扩容（Realloc）导致内存地址搬移时的指针失效问题，并天然支持无限撤销/重做（Undo/Redo）的快照复制。

3.2 单向线性控制流 (Unidirectional Control Flow)

无单例模式 (No Singletons)： 所有的全局状态（编辑器上下文、相机、激活的工具、选中的网格）必须显式包裹在一个 AppContext 结构体中，并通过指针显式向下传递。

有序的单向调用： 每一帧的生命周期严格自上而下运行，不允许跨层级回调：

$$\text{Frame Start} \rightarrow \text{Update Input} \rightarrow \text{Modify Geometry Data} \rightarrow \text{Upload to GPU} \rightarrow \text{Render Pass} \rightarrow \text{UI Pass} \rightarrow \text{Frame End}$$

分层解耦架构：

Layer 0: Core Data：纯粹的数学与半边几何结构，不包含任何图形库头文件，纯计算。

Layer 1: Engine Wrapper：Raylib 输入、时间轴与窗口事件包装。

Layer 2: Render Pipeline：自定义 GLSL 着色器、帧缓冲（FBO）管理与屏幕空间后处理。

Layer 3: UI & Interaction：立即模式 UI（如 Raygui 或自定义状态机）与编辑工具集。

4. 核心数据结构设计 (Data Structures)

4.1 CPU 端几何核心：索引型半边结构 (Indexed Half-Edge)

为了高效支持网格编辑（挤出、倒角、内插），网格底层采用半边数据结构，但全部通过索引符号化：

#include <stdint.h>
#include <stdbool.h>

#define INVALID_INDEX 0xFFFFFFFF

typedef struct {
    float position[3];     // X, Y, Z 坐标
    float normal[3];       // 顶点法线
    uint8_t color[4];      // RGBA 顶点色，取代常规贴图
    uint32_t half_edge;    // 指向该顶点出发的一条半边的索引
} Vertex;

typedef struct {
    uint32_t next_half_edge; // 同一多边形内的下一条半边索引
    uint32_t pair_half_edge; // 对应的反向半边索引
    uint32_t vertex;         // 该半边指向的源顶点（起点）索引
    uint32_t face;           // 该半边所属的面索引
} HalfEdge;

typedef struct {
    uint32_t half_edge;      // 属于该面的任意一条半边索引
    float face_normal[3];    // 面法线，用于硬着色计算
    uint8_t base_color[4];   // 面基础底色
} Face;

typedef struct {
    Vertex* vertices;
    HalfEdge* half_edges;
    Face* faces;
    
    uint32_t vertex_count;
    uint32_t vertex_capacity;
    
    uint32_t half_edge_count;
    uint32_t half_edge_capacity;
    
    uint32_t face_count;
    uint32_t face_capacity;
} MeshContext;


4.2 GPU 端渲染缓冲结构

为了保证最高效的缓存命中率与流水线吞吐，每一帧网格修改完成后，半边结构会被线性打平为标准的顶点缓冲送入 GPU：

typedef struct {
    float pos[3];
    float normal[3];      // 提供面法线以实现完美的平直着色 (Faceted Shading)
    uint8_t color[4];     // 传入 Shader 的顶点颜色
} RenderVertex;


5. 核心模块与功能明细 (Functional Requirements)

5.1 建模核心功能 (Modeling Core)

面选择与挤出 (Extrude)： 沿当前面的法线方向生成新顶点，创建四周的新面，自动断开原拓扑并重新缝合半边索引。

内插 (Inset)： 沿面边缘向内等比收缩，在原有面内部生成一个缩小的新面，并在四周产生一圈过渡面。

平滑倒角 (Bevel)： 针对选中的边或顶点进行切角，通过插入新面产生硬朗的倒角过渡，支持控制倒角的分段数。

循环切线 (Loop Cut)： 识别交错的半边闭环（Quad Loop），在网格中间安全割裂并插入一圈新边与新顶点。

5.2 渲染管线 (Graphic & Faceted Pipeline)

必须实现两条硬编码的自定义着色器管线，不依赖高级图形接口的复杂光照模型：

5.2.1 基础着色器 (Hard-Step Shader)

矢量化光影 (Hard-Quantized Shading)：
在片元着色器中计算面法线与世界光源方向的点积（$N \cdot L$）。不使用连续渐变，直接使用硬阶跃函数（Step/Threshold），模拟矢量插画中的硬投影：

$$\text{Intensity} = \text{dot}(N, L) > 0.0 ? 1.0 : 0.35;$$

最终像素颜色 = $\text{Vertex Color} \times \text{Intensity}$。

纯色块表现： 确保暗面与亮面具有完美的平面几何刻面感（Faceted），没有任何因双线性插值带来的微弱过渡，呈现纯粹的高反差图形排版效果。

5.2.2 屏幕空间辅助着色器 (Screen-Space Edge Shader)

图形化轮廓： 相比于二次元的全局黑线勾描，本软件的后处理主要用于增强“纯几何块面”之间的分割感（基于深度 Depth 和法线 Normal 的差异）。

边缘提取算法： 在后处理中，使用 Sobel 算子 检测几何体的边界与转折面，用于在同色块重叠时提供极细的几何分割线，保持画面硬朗的设计感。

5.3 基础动画模块 (Keyframe Animation)

层级变换： 支持对象级（Object-level）的平移、旋转、缩放（PRS）层级树。

线性数据驱动： 动画通道由纯粹的时间轴-数值对数组构成，使用简单的线性插值（Lerp）或缓动函数（Easing Functions）驱动变换矩阵，剔除任何复杂的骨骼蒙皮与权重计算。

5.4 资产导出模块 (glTF Exporter)

将 CPU 端的 MeshContext 拓扑结构打平为标准的三角面索引数组。

严格按照 glTF 2.0 规范，将位置数据映射到 POSITION 属性，将顶点色写入 COLOR_0 属性，法线写入 NORMAL 属性。

导出的材质（Material）部分不写入任何纹理参数，仅保留 pbrMetallicRoughness 中硬编码的低粗糙度、无金属度参数，以确保外部引擎（如 Godot/Unity）导入时能直接读取到纯正的顶点颜色底色。

6. 开发路线图 (Roadmap & Implementation Stages)

[阶段 1: 内存骨架与视口] ──> [阶段 2: 扁平切面与阶跃光影] ──> [阶段 3: 半边拓扑与核心工具] ──> [阶段 4: glTF 闭环]



阶段 1：内存骨架与基础视口 (基础奠定)

目标： 构建单向控制流主循环，建立扁平连续内存的网格数据存储。

AI 任务清单：

编写 AppContext 及内存分配、安全扩容与释放函数。

使用 Raylib 初始化基础窗口、视口、等距视角（Isometric）风格倾向的轨道相机控制。

实现一个手动构造基础标准立方体（Cube）半边结构的测试用例，并将其正确打平渲染。

阶段 2：渲染管线打通 (视觉闭环)

目标： 实现完全不需要任何外部贴图的矢量扁平色块渲染与硬几何投影。

AI 任务清单：

编写自定义加载 GLSL 顶点的 Forward Shader，实现平直顶点色与无渐变阶跃（Hard-Step）矢量光影。

配置离屏 FBO 渲染管线，将场景的深度和世界空间法线写入独立的渲染目标（Render Texture）。

编写基于深度与法线的边界分割后处理着色器，完成整体图形化海报画风在软件内部的呈现。

阶段 3：拓扑编辑工具集开发 (核心功能)

目标： 让工具具备真正的手工几何建模能力。

AI 任务清单：

编写基于屏幕射线的点、边、面拾取（Raycasting）和高亮选中逻辑。

基于扁平数组半边数据结构，严格实现 Extrude（面挤出）算法，要求正确更新和缝合所有受影响的 HalfEdge, Vertex, Face 索引，杜绝野索引。

实现 Bevel（硬倒角）与 Inset（内插面）的核心几何数学计算。

阶段 4：序列化与工程闭环 (交付阶段)

目标： 资产能够安全走出本工具，被其他引擎高效读取，达成完全独立的生态链。

AI 任务清单：

编写一个简单的基于 PRS 关键帧的时间轴控制器，允许对模型进行基础的动态演示。

编写通用纯 C 的 glTF 2.0 / GLB 导出器，将打平后的顶点流及 COLOR_0 写入二进制文件，确保在第三方查看器或游戏引擎中自带完整的顶点色。

7. AI 智能体开发 Prompt 指导模板 (给 AI 读的元提示)

当你将此文档交给 AI 智能体（如 Claude Code、Cursor、OpenClaw）开始编写代码时，请附加以下具体约束提示词：

"执行任务指令：
你现在是一个精通低阶图形学与纯 C 语言（C11）的顶级架构师。我们需要从零开始构建名为 PrismDraft 的图形化极简 3D 建模软件。请严格按照上述提供的设计文档进行系统级编码。

核心硬性约束：
1. 严禁引入任何 C++ 特性（如 class, namespace, std::vector, new/delete）。整个项目必须基于纯 C 语言编写。
2. 核心建模与几何拓扑操作中，严禁针对单个点、单条边或单个面调用 malloc/free。所有几何元素必须存放在连续的结构体动态数组中，元素之间的互指必须使用 uint32_t 类型的数组索引。
3. 任何修改网格拓扑的函数（特别是 Extrude 和 Bevel），必须在代码头部以清晰的注释详细推导半边索引的裂变与重排逻辑，确保重排后所有 pair_half_edge 和 next_half_edge 处于闭环状态，严防死锁或野索引。
4. 所有的状态管理必须显式通过 AppContext 指针向下传递，严禁使用 static 全局变量或隐藏的单例模式。
5. 着色器逻辑必须完全去除连续渐变，必须通过 step 或 threshold 函数实现极其硬朗的无光照（Unlit）底色与纯色块硬投影（Hard-Step Shading），追求矢量插画/海报设计般的平直刻面（Faceted）视觉效果，绝不是传统的平滑光照。

请首先为我生成【阶段 1】的内存骨架代码与核心数据结构的定义文件。"
