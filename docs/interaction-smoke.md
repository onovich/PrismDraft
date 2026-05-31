# PrismDraft Interaction Smoke

This workflow covers user-facing regressions that are easy to miss in the broad smoke path.

## Command

```powershell
tools\SmokeInteraction.cmd
```

It configures and builds the project, runs the focused interaction tests, then captures the viewport through `tools\SmokeViewport.cmd`.

## Coverage

- `pd_core_face_loop_cut_controller_test`: `4` must split the selected quad into two quads with midpoint vertices. It must not turn the selected face into two triangles.
- `pd_core_face_bevel_controller_test`: `3` must create an inward chamfer with sloped ring-face normals. It must not behave as an outward extrusion.
- `pd_editor_modeling_service_test`: the editor modeling service must keep all modeled meshes valid and pickable.
- `pd_editor_visual_state_test`: visual controls must continue to clamp and step through valid outline, light, dark, shadow, face, and background values.
- `tools\SmokeViewport.cmd`: the shader path must compile and produce `captures/phase2_cube.png`.

## Manual Follow-Up

When a visual issue is reported, run this command first, then launch:

```powershell
tools\SmokeViewportInteractive.cmd
```

Manual checks:

- Press `-` / `=` and confirm outline strength visibly fades or strengthens, not only the overlay value.
- Press `;` / `'` and confirm crease/normal edge sensitivity visibly changes.
- Press `U` / `I` and confirm shaded faces visibly lighten or darken.
- Select a face and press `4`; the selected face should show a midline split into two four-sided regions.
- Select a face and press `3`; the selected face should chamfer inward around the original boundary, not protrude outward.
