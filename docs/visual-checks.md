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
