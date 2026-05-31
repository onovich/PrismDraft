# PrismDraft Interaction Smoke

This workflow covers user-facing regressions that are easy to miss in the broad smoke path.

## Command

```powershell
tools\SmokeInteraction.cmd
```

It configures and builds the project, runs the focused interaction tests, captures multiple viewport smoke cases, then compares screenshot pixel differences for visible feedback.

## Coverage

- `pd_core_face_loop_cut_controller_test`: `4` must split the selected quad into two quads with midpoint vertices. It must not turn the selected face into two triangles.
- `pd_core_face_bevel_controller_test`: `3` must create an inward chamfer with sloped ring-face normals. It must not behave as an outward extrusion.
- `pd_editor_panel_state_test`: operation panels must initialize open, switch between grouped panels, toggle visibility, and expose stable names for the viewport UI.
- `pd_editor_modeling_service_test`: the editor modeling service must keep all modeled meshes valid and pickable.
- `pd_editor_visual_state_test`: visual controls must continue to clamp and step through valid outline, light, dark, shadow, face, and background values.
- `visual-edge-low` / `visual-edge-high`: depth edge threshold changes must produce a localized visible screenshot difference.
- `visual-normal-low` / `visual-normal-high`: normal edge threshold changes must produce a localized visible screenshot difference.
- `visual-dark-low` / `visual-dark-high`: dark intensity changes must produce a broad screenshot difference.
- `modeling-bevel`: applying `3` through the viewport smoke case must render a screenshot that differs from the default viewport.
- `modeling-loop-cut`: applying `4` through the viewport smoke case must render a screenshot that differs from the default viewport.
- `tools\SmokeViewport.cmd`: the default shader path must compile and produce `captures/phase2_cube.png`.

The visual difference gates intentionally combine average difference, changed-pixel ratio, and max difference. Edge changes are local by nature, so a low full-image average is acceptable only when enough sampled edge pixels move by a large amount.

## Manual Follow-Up

When a visual issue is reported, run this command first, then launch:

```powershell
tools\SmokeViewportInteractive.cmd
```

Manual checks:

- Use the operation panel as the primary control surface. `F1` opens Model, `F2` opens Move, `F3` opens Visual, `F4` opens Light, and `Tab` hides or shows the panel.
- Click panel tabs and controls; clicks inside the panel should not pick or clear model faces behind the panel.
- Resize the window while dragging the border; the viewport should continue repainting and should not leave persistent black exposed regions after resize settles.
- In Model, select a face and click `Inset`, `Extrude`, `Bevel`, and `Loop cut`; each button should update the mesh and selected-face highlight.
- In Move, drag position, rotate, and scale sliders; the object should update in the next frame, and `Reset` should restore the default transform.
- In Visual, drag face RGB sliders, background RGB sliders, and outline sliders; selected-face color, background, and edge response should visibly change.
- In Light, drag light direction, dark intensity, shadow strength, and shadow offset sliders; hard-step lighting and the projected shadow should visibly update.
- Press `-` / `=` and confirm outline strength visibly fades or strengthens, not only the overlay value.
- Press `;` / `'` and confirm crease/normal edge sensitivity visibly changes.
- Press `U` / `I` and confirm shaded faces visibly lighten or darken.
- Select a face and press `4`; the selected face should show a midline split into two four-sided regions.
- Select a face and press `3`; the selected face should chamfer inward around the original boundary, not protrude outward.
