# PrismDraft TODO

This TODO translates the design mirror and visual references into small Codex-sized transactions. Each completed transaction must be committed and pushed through `gitflow` / `project-git-workflow` before starting the next independent transaction.

## Current Focus

Phase 5: Viewport polish and interaction foundation.

The current milestone is turning the proven data/render/export pieces into a user-testable editor loop, while fixing the viewport polish issues found in manual smoke and making the expected basic interactions explicit.

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
- [x] Add Raylib window/camera wrapper under `engine`.
- [x] Add render mesh buffer flattening from core mesh data.
- [x] Add a first hard-step shader pass for vertex color and face normal lighting.
- [x] Display the cube fixture in an orthographic or near-isometric viewport.

## Phase 2: Visual Loop

- [x] Split shader assets into named files under `shaders/`.
- [x] Add render config for background color, light direction, and hard-step threshold.
- [x] Add depth/normal render target plan.
- [x] Implement screen-space edge separation using depth and normal differences.
- [x] Capture viewport reference screenshots and compare them against `sample/` style checkpoints.
- [x] Composite edge separation into the viewport screenshot path.
- [x] Add a hard projected shadow or shadow-like graphic pass for stronger sample alignment.

## Phase 3: Modeling Tools

- [x] Define editor selection state for vertex, edge, and face selection.
- [x] Implement ray picking against flattened or core mesh geometry.
- [x] Add face highlight rendering path.
- [x] Implement face inset with topology proof comment and validator call.
- [x] Implement face extrude with topology proof comment and validator call.
- [x] Implement bevel with topology proof comment and validator call.
- [x] Implement constrained quad loop cut only after validator coverage is strong enough.

## Phase 4: Export And Demonstration

- [x] Define flattened export mesh data independent of render buffers.
- [x] Write glTF 2.0 JSON export with `POSITION`, `NORMAL`, and `COLOR_0`.
- [x] Write GLB binary export.
- [x] Add import checks in an external viewer or documented validation path.
- [x] Define object-level PRS animation data.
- [x] Add a minimal timeline playback controller.

## Phase 5: Viewport Polish And Interaction Foundation

- [x] Manually verify that the viewport process exits after the window closes.
- [x] Add an automated process lifecycle smoke check that verifies the viewport process exits after the window closes.
- [ ] Support viewport window resizing by recreating render targets and updating edge shader texel size.
- [ ] Fix edge compositing so silhouette and crease lines read as outer/structural boundaries instead of face-overpaint artifacts.
- [ ] Add anti-aliasing for diagonal face boundaries, edge overlay, and hard projected shadow.
- [ ] Re-run visual comparison against `sample/` after edge alignment and anti-aliasing changes.
- [ ] Add camera orbit, pan, and zoom controls for inspecting the model.
- [ ] Add an interactive editor smoke mode for face picking and selection feedback.

### Phase 5 Implementation Order

1. Stabilize smoke lifecycle: automate launch, wait, close, and process-exit verification.
2. Fix viewport resizing: support resizable windows, recreate render targets, and update edge texel size when size changes.
3. Fix edge alignment: make outline and crease edges read as structural boundaries, not as misregistered overpaint on faces.
4. Add anti-aliasing: treat diagonal face boundaries, edge overlay, and hard projected shadow as one visual polish pass.
5. Capture a new viewport image and compare it against `sample/` and the manual smoke notes.
6. Add camera orbit, pan, and zoom after the visual baseline is stable.
7. Add face picking and selected-face feedback as the first interactive editor smoke.

## Phase 6: Basic Editor Controls

- [ ] Add object transform state for move, rotate, and scale.
- [ ] Add transform controls or keyboard shortcuts for move, rotate, and scale.
- [ ] Add keyboard or tool commands in the editor smoke for inset, extrude, bevel, and loop cut.
- [ ] Add editable face color state and a simple face color adjustment control.
- [ ] Add editable background color state and a simple background color adjustment control.
- [ ] Add editable outline style config, including outline thickness and edge threshold controls.
- [ ] Add editable lighting config, including light direction, dark intensity, shadow strength, and shadow offset controls.
- [ ] Add a compact on-screen debug/control overlay for current tool, selection, and visual parameters.
- [ ] Add an app-level export smoke path for glTF and GLB from the current mesh.
- [ ] Add a minimal app-level timeline demo that applies PRS animation to an object.

### Phase 6 Implementation Order

1. Add object transform state and non-destructive move, rotate, and scale controls.
2. Add face color editing, then background color editing.
3. Add outline controls for thickness and edge thresholds.
4. Add lighting controls for direction, dark intensity, shadow strength, and shadow offset.
5. Add a compact overlay that exposes current selection, active tool, transform values, color, outline, and lighting values.
6. Wire modeling commands into the interactive smoke: inset, extrude, bevel, and loop cut.
7. Add app-level export smoke for glTF and GLB from the edited mesh.
8. Add the minimal PRS timeline demo after transform state is visible in the app layer.

## Always-On Checks

- [ ] File names match `pd_<layer>_<business>_<role>`.
- [ ] Public symbols match their file prefix.
- [ ] `core` has no Raylib, OpenGL, UI, or app dependency.
- [ ] Mesh topology mutations include proof comments and validator calls.
- [ ] Rendering changes are compared with [docs/visual-reference.md](visual-reference.md).
- [ ] Each small transaction is committed and pushed before moving on.
