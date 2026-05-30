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
