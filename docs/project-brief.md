# PrismDraft Project Brief

Source design mirror: [docs/prismdraft-design.md](prismdraft-design.md)
Synced source: https://docs.google.com/document/d/1gocSWlCi_SNYCkqoY9bJzcjeV5UcEzL5ovkk7YyUKnA/edit?usp=sharing
Visual reference: [docs/visual-reference.md](visual-reference.md)

## Intent

PrismDraft is a lightweight standalone 3D modeling and rendering tool for vector-like, faceted hard-surface art. It should feel closer to spatial poster drafting than to a general DCC package: precise geometry, flat color blocks, hard edges, and clean export into engines such as Godot, Unity, and Unreal.

The user-provided images under `sample/` are the visual target set. Use them to explain and check the intended modeling and rendering style: icon-like objects, flat poster backgrounds, hard quantized shadows, clean silhouettes, and low-detail geometric surfaces.

## Hard Constraints

- Implement the core in pure C11. Do not use C++ features such as classes, namespaces, templates, `new/delete`, or STL containers.
- Use Raylib only as a window, input, math, and OpenGL wrapper. Do not build on a heavyweight game-engine ecosystem.
- Store geometry in contiguous arrays and reference topology with `uint32_t` indices, not raw cross-object pointers.
- Avoid per-vertex, per-edge, or per-face `malloc/free` inside modeling operations.
- Pass all application state explicitly through `AppContext`; avoid singletons, hidden globals, and static global state.
- Render from vertex color, face normals, and custom GLSL hard-step shading. No textures, UV workflow, smooth lighting, or PBR material graph.
- Compare viewport and shader work against the `sample/` visual references before treating the look as correct.

## Architecture Shape

- Layer 0: Core data and math. Pure C data structures, indexed half-edge topology, no graphics-library headers.
- Layer 1: Engine wrapper. Raylib windowing, input, timing, and camera integration.
- Layer 2: Render pipeline. Custom GLSL, GPU buffer flattening, FBOs, depth/normal edge detection.
- Layer 3: UI and interaction. Immediate-mode UI or a small explicit state machine for tools.

## Roadmap

1. Memory skeleton and viewport: define `AppContext`, mesh storage, safe growth/free routines, a cube half-edge fixture, Raylib window, and isometric orbit camera.
2. Visual loop: flatten mesh data into render buffers, implement hard-step vertex-color shader, then add depth/normal edge post-processing.
3. Modeling tools: implement ray picking, selection highlighting, face extrude, bevel, inset, and loop cut with strict half-edge index maintenance.
4. Delivery loop: add simple PRS keyframe animation and glTF/GLB export with `POSITION`, `NORMAL`, and `COLOR_0`.

## Current Status

- Google Doc design has been mirrored locally.
- Project initialization and workflow wiring should treat the mirrored design as the source of truth for implementation constraints.
- Codex architecture work must follow [docs/codex-architecture-workflow.md](codex-architecture-workflow.md).
- Rendering and design explanations should reference [docs/visual-reference.md](visual-reference.md) and the images in `sample/`.
