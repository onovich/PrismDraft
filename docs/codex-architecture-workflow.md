# Codex Architecture Workflow

This document is the mandatory operating contract for Codex work in PrismDraft. It exists to keep the codebase readable, layered, and faithful to the design mirror in [docs/prismdraft-design.md](prismdraft-design.md).

## Required Reading Order

Before changing code, Codex must read these files in order:

1. [AGENTS.md](../AGENTS.md)
2. [docs/project-brief.md](project-brief.md)
3. [docs/project-todo.md](project-todo.md)
4. [docs/visual-reference.md](visual-reference.md) when the task affects rendering, viewport presentation, modeling style, or design explanation.
5. [docs/codex-architecture-workflow.md](codex-architecture-workflow.md)
6. The specific module files being edited.

If a task conflicts with this workflow or the design mirror, stop and update the workflow/design deliberately before implementing code.

## Work Loop

Every implementation task must follow this sequence:

1. Identify the target layer and business noun.
2. State which files will be created or edited.
3. Check whether the dependency direction remains legal.
4. Implement the smallest complete vertical slice.
5. Add or update focused tests, fixtures, or validation notes when the change touches shared behavior.
6. Run the configured validation workflow when commands exist.
7. For visual/rendering work, compare the result against the `sample/` references.
8. Commit and push the completed task-scoped change through `gitflow` / `project-git-workflow`.
9. Summarize architectural impact in the final response.

Do not introduce broad refactors, dependency changes, alternate frameworks, generated code, or new naming schemes inside feature work.

## Layer Contract

PrismDraft uses strict one-way dependencies. Lower layers must never include or call higher layers.

```text
app -> editor -> render -> engine -> core
export -> core
animation -> core
```

Layer responsibilities:

- `core`: pure C11 data, math, memory, indexed half-edge topology, validation, and fixtures. No Raylib, no OpenGL, no UI.
- `engine`: Raylib window, input, time, file IO wrapper, and platform boundary. No mesh topology mutation.
- `render`: GPU buffer flattening, GLSL shader loading, FBOs, hard-step shading, normal/depth edge post-process.
- `editor`: tools, picking, selection, commands, undo/redo orchestration, and user interaction state.
- `app`: `AppContext`, startup/shutdown, frame lifecycle, and top-level control flow.
- `export`: glTF/GLB serialization from flattened mesh data. No editor or UI dependencies.
- `animation`: object-level PRS timeline data and interpolation. No skeletal animation.

If a module wants to cross a forbidden boundary, move the shared data into `core` or pass a narrow value struct through the caller.

## Directory Layout

Use this layout when source code begins:

```text
src/
  app/
  core/
  engine/
  render/
  editor/
  export/
  animation/
include/
  prismdraft/
shaders/
tests/
  core/
  render/
  editor/
```

Do not create catch-all folders such as `common`, `misc`, `helpers`, `utils`, or `manager`. A file must belong to a named layer and a named business noun.

## Naming Grammar

Names must make the module obvious from the prefix, the business obvious from the first noun, and the role obvious from the suffix.

### Files

File names use:

```text
pd_<layer>_<business>_<role>.h
pd_<layer>_<business>_<role>.c
```

Examples:

- `pd_core_mesh_entity.h`
- `pd_core_mesh_storage_controller.c`
- `pd_core_halfedge_validator.c`
- `pd_render_mesh_buffer.c`
- `pd_render_hardstep_shader.c`
- `pd_editor_selection_state.h`
- `pd_editor_extrude_controller.c`
- `pd_app_context_entity.h`
- `pd_export_gltf_writer.c`

Allowed layer tokens:

- `app`
- `core`
- `engine`
- `render`
- `editor`
- `export`
- `animation`

Allowed role suffixes:

- `entity`: persistent data model structs.
- `state`: mutable runtime state.
- `config`: immutable or startup configuration.
- `controller`: orchestrates a workflow and mutates state through lower-level APIs.
- `system`: frame-level subsystem with explicit update/shutdown functions.
- `service`: stateless operation group with no ownership of long-lived state.
- `buffer`: CPU or GPU contiguous storage.
- `shader`: GLSL loading or shader-specific bindings.
- `validator`: invariant checks and debug assertions.
- `fixture`: deterministic test/demo data.
- `writer`: serialization output.
- `reader`: serialization input.
- `test`: automated tests under `tests/`.

Do not invent a new suffix until existing suffixes are clearly insufficient.

### Types

Public C types use:

```text
Pd<Layer><Business><Role>
```

Examples:

- `PdCoreMeshEntity`
- `PdCoreHalfEdgeEntity`
- `PdCoreMeshStorageController`
- `PdRenderMeshBuffer`
- `PdEditorSelectionState`
- `PdAppContextEntity`

Enums use the same prefix and singular enum values:

```c
typedef enum PdEditorSelectionKind {
    PD_EDITOR_SELECTION_KIND_NONE,
    PD_EDITOR_SELECTION_KIND_VERTEX,
    PD_EDITOR_SELECTION_KIND_EDGE,
    PD_EDITOR_SELECTION_KIND_FACE
} PdEditorSelectionKind;
```

### Functions

Public functions use:

```text
pd_<layer>_<business>_<role>_<verb>
```

Examples:

- `pd_core_mesh_entity_init`
- `pd_core_mesh_entity_free`
- `pd_core_mesh_storage_controller_reserve_vertices`
- `pd_core_halfedge_validator_check_closed_face`
- `pd_render_mesh_buffer_upload`
- `pd_editor_extrude_controller_apply_face`
- `pd_app_lifecycle_controller_run_frame`

Private file-local helpers still use the module prefix:

```text
pd_<layer>_<business>_<role>_local_<verb>
```

Do not use vague verbs such as `handle`, `process`, `do`, or `manage` unless the file is a top-level frame system and the action is genuinely broad.

### Constants And Macros

Constants and macros use:

```text
PD_<LAYER>_<BUSINESS>_<ROLE>_<NAME>
```

Examples:

- `PD_CORE_MESH_ENTITY_INVALID_INDEX`
- `PD_CORE_MESH_STORAGE_CONFIG_DEFAULT_VERTEX_CAPACITY`
- `PD_RENDER_HARDSTEP_SHADER_LIGHT_THRESHOLD`

### Variables

Local variables use lower snake case and should preserve the business noun:

```c
uint32_t face_index;
PdCoreMeshEntity* mesh_entity;
PdEditorSelectionState* selection_state;
```

Avoid compressed names. Use `half_edge_index`, not `he`, except inside a tiny loop where the meaning is unambiguous.

## Header Rules

Every `.h` file must expose one primary public concept. If a header needs multiple unrelated primary concepts, split it.

Header order:

1. Include guard.
2. Standard library includes.
3. Lower-layer project includes.
4. Constants/macros.
5. Type declarations.
6. Public function declarations.

Implementation order in `.c` files:

1. Matching header include.
2. Standard library includes.
3. Additional lower-layer project includes.
4. Private constants.
5. Private helper declarations.
6. Public functions.
7. Private helpers.

Do not include Raylib headers from `core`.

## State And Ownership

- All long-lived state must be reachable from `PdAppContextEntity`.
- No hidden singleton state.
- No mutable static globals.
- File-local `static` helper functions are allowed.
- Ownership must be visible in function names or comments when memory crosses a module boundary.
- Geometry arrays must use explicit count/capacity pairs.
- Topology links must use `uint32_t` indices and `PD_CORE_MESH_ENTITY_INVALID_INDEX`.

## Topology Mutation Rule

Any function that mutates half-edge topology must begin with a short proof comment containing:

- Input invariants.
- New vertices, half-edges, and faces being allocated.
- `next_half_edge` rewiring plan.
- `pair_half_edge` rewiring plan.
- Output invariants.

The function must call a validator in debug/test builds before returning success.

## Error Handling

Use explicit status results for recoverable failures.

```c
typedef enum PdCoreResult {
    PD_CORE_RESULT_OK,
    PD_CORE_RESULT_ERROR_INVALID_ARGUMENT,
    PD_CORE_RESULT_ERROR_OUT_OF_MEMORY,
    PD_CORE_RESULT_ERROR_TOPOLOGY_INVALID
} PdCoreResult;
```

Do not use exceptions, `longjmp`, hidden global error state, or silent fallback behavior.

## Forbidden Patterns

- C++ syntax or C++ runtime dependencies.
- Per-element heap allocation in mesh editing operations.
- Raw pointer links between mesh elements.
- `common`, `misc`, `helpers`, `utils`, or `manager` modules.
- Cross-layer callbacks.
- Smooth shading as the default visual path.
- Texture, UV, or PBR-first implementation.
- Replacing the `sample/` target style with generic realistic, cinematic, or physically soft rendering.
- Static mutable global state.
- Business logic inside shaders, UI code, or Raylib wrappers.

## Codex Review Checklist

Before finalizing any code change, Codex must verify:

- File names match `pd_<layer>_<business>_<role>`.
- Public symbols match their file prefix.
- The dependency direction is legal.
- `core` remains independent of Raylib/OpenGL/UI.
- New state is reachable through `PdAppContextEntity`.
- Rendering or viewport changes still match [docs/visual-reference.md](visual-reference.md) when applicable.
- Mesh topology changes include proof comments and validator calls.
- No new vague catch-all module names were introduced.

Run the architecture guard when source files exist:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\CheckArchitecture.ps1
```

After the guard passes, use the configured git workflow to commit and push the completed small transaction before beginning another independent task.
