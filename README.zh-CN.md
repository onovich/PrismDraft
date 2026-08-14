# PrismDraft

[English](README.md)

用 C11 编写的低面硬表面 3D 建模与渲染原型，强调矢量海报式造型。

![PrismDraft 封面](docs/cover.png)

## 项目包含什么

- 低面硬表面建模。
- C11 核心与 Shader 管线。
- 自动化与视觉校验。

## 快速开始

使用 CMake 生成并构建本地工程：

```bash
cmake -S . -B build
cmake --build build
```

## 仓库结构

- `src/` — 应用与库的源代码。
- `include/` — 公开 C 头文件。
- `tools/` — 开发与内容工具。
- `docs/` — 项目文档与设计说明。
- `tests/` — 自动化测试与校验材料。

## 文档

- [`design.md`](design.md)
- [`docs/prismdraft-design.md`](docs/prismdraft-design.md)
- [`docs/project-brief.md`](docs/project-brief.md)
- [`docs/project-todo.md`](docs/project-todo.md)
- [`docs/export-validation.md`](docs/export-validation.md)

## 当前状态

这是一个仍在开发或验证方向的项目，现有仓库已经包含可检查的实现与内容，但不应被理解为商业成品。仓库内包含自动化测试；具体兼容性仍取决于目标运行时、编辑器或平台。

## 许可证

当前仓库未包含开源许可证。
