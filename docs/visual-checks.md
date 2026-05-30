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
- Pending: Designed projected shadows like `sample/sample2.png` and `sample/sample4.png`.

Next visual target:

- Add a simple planar shadow or shadow-like hard projection that matches the sample images without introducing soft physically realistic lighting.
