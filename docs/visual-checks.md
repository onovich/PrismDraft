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

Current comparison:

- Pass: The interaction layer uses existing `editor` face picking and render face highlight buffers instead of duplicating business logic in the app layer.
- Pass: The default screenshot smoke remains non-interactive and still exits after rendering `captures/phase2_cube.png`.
- Watch: User-facing tool UI and explicit transform/color/lighting controls are still Phase 6 work.
