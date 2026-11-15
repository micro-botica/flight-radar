# CI workflow ready to install

`build.yml` here is the workflow for issue #8. Claude cannot write to
`.github/workflows`, so it is staged in this folder. To enable it:

    git mv docs/ci/build.yml .github/workflows/build.yml
    git rm docs/ci/README.md

Commit and push that, then enable "Require status checks" in the ruleset
with the check name `build` (it only shows in the picker after one run).
