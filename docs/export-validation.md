# PrismDraft Export Validation Path

This document defines the validation path for PrismDraft glTF and GLB delivery. Use it whenever export code changes, before claiming engine import readiness.

## Automated Gate

Run the configured project validation workflow from the repository root:

```powershell
C:\Users\Administrator\.codex\skills\project-ops-workflow\scripts\ops\Validate.cmd
```

The gate must pass:

- `pd_export_mesh_buffer_test`
- `pd_export_gltf_writer_test`
- `pd_export_glb_writer_test`
- `prismdraft_architecture_check`

The export writer tests prove that the generated assets contain:

- glTF 2.0 asset metadata.
- A single triangle-list mesh primitive.
- `POSITION`, `NORMAL`, and `COLOR_0` float accessors.
- Cube fixture vertex count of `36`.
- Embedded glTF binary data for `.gltf`.
- A valid GLB header, JSON chunk, and BIN chunk for `.glb`.

## External Import Check

Use this manual path when a persistent exported artifact exists from the app, a demo command, or a temporary local harness:

1. Export both `.gltf` and `.glb` for the same cube or edited mesh.
2. Import each file into at least one external glTF consumer, such as a game engine, DCC tool, or dedicated glTF viewer.
3. Confirm the mesh appears as faceted hard-surface geometry, not smoothed geometry.
4. Confirm vertex colors are visible or available as `COLOR_0`.
5. Confirm normals import without flipped or missing faces.
6. Confirm there are no required texture, UV, skeletal, or PBR material dependencies.
7. Confirm the object scale and orientation are stable between `.gltf` and `.glb`.

If the importer requires material setup to display vertex colors, record that setup in the relevant release or demo notes instead of changing the exporter toward a texture-first pipeline.

## Architecture Self-Check

Export validation must preserve these boundaries:

- Export code may depend on `core` data and `PdExportMeshBuffer`.
- Export code must not depend on `render`, `editor`, `engine`, `app`, Raylib, OpenGL, UI state, or shader assets.
- Export files must keep the `pd_export_<business>_<role>` naming shape.
- Export must remain independent from the visual renderer; render buffers are not export buffers.
