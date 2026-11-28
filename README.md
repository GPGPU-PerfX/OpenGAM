# OpenGAM

OpenGAM (Open-Source GPU Architecture Model) is an open-source GPU architecture
model. It is licensed under the Apache License 2.0.

## Branches

- `develop` is the default development branch and the default pull-request
  review target. Ordinary feature work lands here after review.
- `master` is the release / published-push branch. Published history on
  `master` is not rewritten. Ordinary feature PRs must not target `master`.

Releases are a periodic, explicit port of `develop` onto `master`. There is
no automatic merge from `develop` to `master`. See
[doc/branch-and-release.md](doc/branch-and-release.md).
