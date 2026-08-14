# PrismDraft

[简体中文](README.zh-CN.md)

A C11 modeling and rendering prototype for low-poly hard-surface forms with a vector-poster look.

![PrismDraft cover](docs/cover.png)

## What it includes

- Low-poly hard-surface modeling.
- C11 core and shader pipeline.
- Automated and visual validation.

## Getting started

Configure and build the project with CMake:

```bash
cmake -S . -B build
cmake --build build
```

## Repository map

- `src/` — Application and library source.
- `include/` — Public C headers.
- `tools/` — Development and content tools.
- `docs/` — Project documentation and design notes.
- `tests/` — Automated tests and validation fixtures.

## Documentation

- [`design.md`](design.md)
- [`docs/prismdraft-design.md`](docs/prismdraft-design.md)
- [`docs/project-brief.md`](docs/project-brief.md)
- [`docs/project-todo.md`](docs/project-todo.md)
- [`docs/export-validation.md`](docs/export-validation.md)

## Status

This project is still under development or validating its direction. The repository contains inspectable implementation and content, but it should not be treated as a commercial release. Automated tests are included; compatibility still depends on the target runtime, editor, or platform.

## License

No open-source license is currently included in this repository.
