# PrismDraft Visual Checks

Use this document to record screenshot-based visual checks against [docs/visual-reference.md](visual-reference.md). The checks are not a replacement for automated tests; they confirm whether the viewport is moving toward the target look.

## Phase 1 / Phase 2 Cube Check

Screenshot command:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\RunPhase2Viewport.ps1
```

Expected output:

```text
captures/phase2_cube.png
```

Current comparison:

- Pass: Orthographic or near-isometric composition.
- Pass: Flat poster-like background.
- Pass: Clean primitive-derived cube silhouette.
- Pass: Vertex/base-color-style surfaces with no texture, UV, or PBR detail.
- Pass: Hard face separation from face normals and hard-step lighting.
- Pass: Visible screen-space edge overlay composited from depth and normal render targets.
- Pass: Designed hard projected shadow similar to `sample/sample2.png` and `sample/sample4.png`.

Next visual target:

- Move into Phase 3 modeling checks while keeping the current screenshot as the baseline for flat color, hard edges, and designed shadow behavior.

## Phase 3 Face Highlight Check

Screenshot command:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\RunPhase2Viewport.ps1
```

Expected output:

```text
captures/phase2_cube.png
```

Current comparison:

- Pass: Selected face highlight is visible as a flat, semi-transparent color block.
- Pass: Highlight follows the selected face surface without replacing edge separation.
- Pass: Shadow, background, hard-step lighting, and crisp silhouette remain aligned with the `sample/` references.

## Manual Smoke Feedback: 2026-05-31

Observed through `tools\SmokeViewportInteractive.cmd`.

Pass:

- Window opens correctly.
- Basic cube composition, flat poster background, hard-step face lighting, and hard projected shadow are directionally correct.
- No black screen, flicker, or obvious broken faces were observed.

Issues:

- Window resizing is not supported in the current interactive smoke build.
- Edge overlay and face colors visibly interleave at some boundaries; the line currently reads as being painted on top of the face instead of resolving as a clean outer or structural edge.
- Diagonal face boundaries and diagonal edge overlay are visibly jagged.
- The hard projected shadow also needs anti-aliasing.
- Edge alignment may be slightly off; the likely cause is a mix of screen-space edge sampling mismatch and missing anti-aliasing.

Follow-up target:

- Treat this as Phase 5 visual polish before expanding the interactive editor surface. Fix render-target resize handling, edge alignment, and anti-aliasing, then capture a new `captures/phase2_cube.png` and compare again with `sample/`.

Process follow-up:

- 2026-05-31: After the user closed the interactive window, a local process check found no remaining PrismDraft or Raylib viewport process.

## Phase 5 Edge And Anti-Aliasing Check

Screenshot command:

```powershell
tools\SmokeViewport.cmd
```

Expected output:

```text
captures/phase2_cube.png
```

Current comparison:

- Pass: The viewport renders through a 2x internal target and downsamples to the window, improving diagonal face boundaries and the hard projected shadow edge.
- Pass: The edge shader uses an 8-neighbor smooth edge mask instead of binary four-neighbor `step`, reducing stair-step artifacts and hard overpaint.
- Pass: The output preserves flat poster-like surfaces, hard geometric silhouette, and the designed shadow shape from the visual references.
- Watch: Edge placement should be re-checked by the user in the interactive smoke, especially at the center vertical crease and high-contrast face boundaries.

## Phase 5 Interaction Smoke

Interactive command:

```powershell
tools\SmokeViewportInteractive.cmd
```

Controls:

- Right mouse drag: orbit camera around the model.
- Middle mouse drag: pan camera target and position.
- Mouse wheel: zoom the orthographic camera.
- Left click: pick a cube face and rebuild selected-face highlight.
- `W` / `S`: move the object along smoke Z.
- `A` / `D`: move the object along smoke X.
- `Q` / `E`: move the object vertically.
- `R` / `F`: rotate the object around Y.
- `Z` / `X`: scale the object down or up.
- `T`: reset object transform.
- `C`: cycle the selected face color and rebuild the viewport mesh.
- `B`: cycle the background color.
- `[` / `]`: decrease or increase edge sample radius for outline thickness.
- `-` / `=`: decrease or increase depth edge threshold.
- `;` / `'`: decrease or increase normal edge threshold.
- `U` / `I`: decrease or increase dark-side lighting intensity.
- Arrow keys: adjust light direction.
- `O` / `P`: decrease or increase shadow strength.
- `K` / `L`: move the shadow left or right.
- `N` / `M`: move the shadow backward or forward.
- `1`: apply inset to the selected face.
- `2`: apply extrude to the selected face.
- `3`: apply bevel to the selected face.
- `4`: apply loop cut to the selected quad face.

Current comparison:

- Pass: The interaction layer uses existing `editor` face picking and render face highlight buffers instead of duplicating business logic in the app layer.
- Pass: The default screenshot smoke remains non-interactive and still exits after rendering `captures/phase2_cube.png`.
- Pass: Face color, background color, outline thickness, edge thresholds, light direction, dark intensity, shadow strength, and shadow offset are editable through keyboard controls.
- Pass: Interactive mode draws a compact state overlay for selection, transform, color, outline, lighting, and shadow values without changing the automated screenshot baseline.
- Pass: Interactive modeling shortcuts call the existing core inset, extrude, bevel, and loop cut controllers, then rebuild the viewport mesh and selected-face highlight.
- Watch: The user still needs to manually re-check edge placement and anti-aliasing in the interactive viewport after the Phase 5 render pass.

## Phase 6 Export Smoke

Command:

```powershell
tools\SmokeExport.cmd
```

Expected output:

```text
captures/phase6_export_smoke.gltf
captures/phase6_export_smoke.glb
```

Current comparison:

- Pass: The smoke initializes the app context, applies an editor modeling command to the selected face, flattens the edited mesh through the export buffer, and writes both glTF and GLB files.
- Pass: The project smoke workflow runs viewport screenshot smoke, export smoke, and timeline smoke as separate commands.

## Phase 6 Timeline Smoke

Command:

```powershell
tools\SmokeTimeline.cmd
```

Current comparison:

- Pass: The smoke initializes the app context, samples a two-keyframe PRS timeline at the midpoint, and applies the sampled pose to the app transform state.

## Phase 5-6 Completion Audit: 2026-05-31

Commands:

```powershell
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Validate.cmd
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Smoke.cmd
tools\SmokeViewportLifecycle.cmd
```

Artifacts:

```text
captures/phase2_cube.png
captures/phase6_export_smoke.gltf
captures/phase6_export_smoke.glb
```

Current comparison:

- Pass: `captures/phase2_cube.png` was re-opened after the latest smoke run and checked against the `sample/` style notes.
- Pass: Silhouette and crease edges read as structural boundaries instead of obvious face-overpaint artifacts.
- Pass: Diagonal face boundaries, edge overlay, and hard projected shadow show anti-aliased edges through the 2x internal render target and smoothed edge shader sampling.
- Pass: Lifecycle smoke verifies that the interactive viewport process exits after a close message.
- Pass: Project smoke now covers viewport screenshot, edited-mesh export, and timeline demo paths.
