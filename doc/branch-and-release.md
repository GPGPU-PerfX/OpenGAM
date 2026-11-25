# Branch policy and releases

This repository uses two long-lived branches. OpenGAM does not claim
compatibility with any vendor's firmware, driver, or cycle-accurate hardware
behavior.

## Long-lived branches

- `develop` is the default development branch and the default pull-request
  review target. Ordinary feature work lands here after review.
- `master` is the defined release / published-push branch. Published history on
  `master` is not rewritten. Ordinary feature PRs must not target `master`.

## Feature branches and review

- Feature branches are pushed for pull-request review against `develop`, not
  `master`.
- Do not force-push `develop` or `master`.

## Porting develop to master

Releases are a periodic, explicit port of `develop` onto `master` (a
reviewable pull request or an equivalent documented merge). There is no
automatic merge from `develop` to `master`.

Until the first official port, `master` may remain at the initial skeleton
while `develop` carries later reviewed work.
