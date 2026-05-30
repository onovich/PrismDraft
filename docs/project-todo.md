# PrismDraft TODO

This TODO translates the design mirror and visual references into small Codex-sized transactions. Each completed transaction must be committed and pushed through `gitflow` / `project-git-workflow` before starting the next independent transaction.

## Current Focus

Phase 1: Memory skeleton and viewport.

The first milestone is not a full editor. It is a clean, testable foundation: C11 data types, indexed mesh storage, deterministic cube fixture, and a minimal Raylib viewport that can display flattened hard-face geometry.

## Phase 0: Preparation

- [x] Mirror the Google Doc design locally.
- [x] Add Codex architecture workflow and naming guardrails.
- [x] Add visual reference documentation for `sample/`.
- [x] Add architecture structure checker.
- [x] Add project TODO.
- [x] Add initial repository scaffold directories.
- [x] Add line-ending and ignore rules.
- [x] Add minimal CMake/CTest build and test entrypoints.

## Phase 1: Memory Skeleton And Viewport

- [x] Define core result/status types.
- [x] Define `PdCoreMeshEntity`, `PdCoreVertexEntity`, `PdCoreHalfEdgeEntity`, and `PdCoreFaceEntity`.
- [x] Define mesh storage config and explicit count/capacity ownership rules.
- [x] Implement mesh init/free/reserve routines.
- [x] Implement mesh invariant validator for index ranges, closed face loops, and pair symmetry.
- [x] Add a deterministic cube half-edge fixture.
- [x] Add tests for init/free/reserve, cube fixture counts, and validator failure cases.
- [x] Define `PdAppContextEntity` as the root of long-lived state.
- [x] Add the minimal app lifecycle controller.
- [ ] Add Raylib window/camera wrapper under `engine`.
- [ ] Add render mesh buffer flattening from core mesh data.
- [ ] Add a first hard-step shader pass for vertex color and face normal lighting.
- [ ] Display the cube fixture in an orthographic or near-isometric viewport.

## Phase 2: Visual Loop

- [ ] Split shader assets into named files under `shaders/`.
- [ ] Add render config for background color, light direction, and hard-step threshold.
- [ ] Add depth/normal render target plan.
- [ ] Implement screen-space edge separation using depth and normal differences.
- [ ] Capture viewport reference screenshots and compare them against `sample/` style checkpoints.

## Phase 3: Modeling Tools

- [ ] Define editor selection state for vertex, edge, and face selection.
- [ ] Implement ray picking against flattened or core mesh geometry.
- [ ] Add face highlight rendering path.
- [ ] Implement face inset with topology proof comment and validator call.
- [ ] Implement face extrude with topology proof comment and validator call.
- [ ] Implement bevel with topology proof comment and validator call.
- [ ] Implement loop cut only after validator coverage is strong enough.

## Phase 4: Export And Demonstration

- [ ] Define flattened export mesh data independent of render buffers.
- [ ] Write glTF 2.0 JSON export with `POSITION`, `NORMAL`, and `COLOR_0`.
- [ ] Write GLB binary export.
- [ ] Add import checks in an external viewer or documented validation path.
- [ ] Define object-level PRS animation data.
- [ ] Add a minimal timeline playback controller.

## Always-On Checks

- [ ] File names match `pd_<layer>_<business>_<role>`.
- [ ] Public symbols match their file prefix.
- [ ] `core` has no Raylib, OpenGL, UI, or app dependency.
- [ ] Mesh topology mutations include proof comments and validator calls.
- [ ] Rendering changes are compared with [docs/visual-reference.md](visual-reference.md).
- [ ] Each small transaction is committed and pushed before moving on.
