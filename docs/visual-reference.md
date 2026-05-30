# PrismDraft Visual Reference

The `sample/` directory contains user-provided target images for PrismDraft's modeling and rendering style. Treat these files as visual references for design explanation, rendering checks, and UI decisions that affect the final viewport look.

## Reference Files

| File | Size | Use |
| --- | --- | --- |
| `sample/sample1.png` | 609 x 512 | Circular object, shallow depth, poster-like flat background, clean rim geometry, restrained edge detail. |
| `sample/sample2.png` | 275 x 326 | Simple block composition, hard projected shadow, low-detail geometric planes, warm flat-color scene. |
| `sample/sample3.png` | 320 x 329 | Cool monochrome scene, clear silhouette, block geometry, hard light/dark face separation. |
| `sample/sample4.png` | 377 x 378 | Iconic object modeling target, strong red vertex-color-like body, crisp silhouette, high-contrast hard shadow. |

## Style Signals

Use the samples to explain and evaluate these PrismDraft goals:

- Orthographic or near-isometric composition with graphic-design clarity.
- Flat poster backgrounds instead of realistic environments.
- Low-detail geometric forms with deliberate hard edges.
- Quantized light and shadow with clear bright/dark regions.
- Vertex-color-like surfaces, not texture-driven surfaces.
- Minimal material complexity: no UV detail, no photographic texture, no PBR-first look.
- Clean silhouettes and readable object identity at small sizes.
- Shadows that feel designed and blocky rather than physically soft or noisy.

## Rendering Checkpoints

When implementing viewport or shader work, compare output against the samples and ask:

- Does the scene read as flat graphic 3D rather than conventional smooth 3D?
- Are face boundaries and silhouettes clear without relying on dense outlines?
- Does lighting use hard thresholds instead of smooth gradients?
- Can object color be explained as vertex/base color instead of texture detail?
- Are shadows simple, directional, and compositionally useful?
- Does the camera framing support icon-like or poster-like presentation?

## Modeling Checkpoints

When implementing modeling tools, use the samples to keep scope narrow:

- Prefer clean primitive-derived objects over sculpted organic forms.
- Keep topology operations focused on hard-surface edits such as extrude, inset, bevel, and loop cut.
- Preserve readable planar faces after topology mutation.
- Avoid features whose primary value depends on texture painting, UV editing, hair, cloth, fluid, or high-poly sculpting.

## Documentation Use

When explaining PrismDraft to future agents or contributors, cite the samples as the visual target set. The synced design document describes the rules; these images show the desired rendering feel.
