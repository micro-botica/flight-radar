# GitHub templates

Claude cannot write to `.github/` through the GitHub App, so files that belong there are kept here. Copy them into place from the repository root:

```
mkdir -p .github && cp docs/github-templates/pull_request_template.md .github/pull_request_template.md
```

After copying, commit `.github/pull_request_template.md` yourself. If a template here changes, copy it again.
