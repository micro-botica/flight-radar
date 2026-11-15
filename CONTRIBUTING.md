# Contributing

These are the working agreements for Maarten and Mick. They apply to everyone who changes this repository, including Claude.

## Branches

Never push directly to `main`. Every change gets its own branch, named `<type>/<short-description>` in lowercase with hyphens:

| Type | Use for | Example |
|---|---|---|
| `feat/` | New functionality | `feat/radar-sweep` |
| `fix/` | Bug fixes | `fix/wifi-reconnect` |
| `docs/` | Documentation only | `docs/readme` |
| `ci/` | Workflows and build tooling | `ci/claude-review` |
| `chore/` | Anything else, such as cleanup | `chore/update-lvgl` |

Branches created by `@claude` on an issue are named `claude/issue-<nr>-<timestamp>` automatically. Leave those names as they are.

## Pull requests

- Everything goes through a pull request to `main`, with no exceptions.
- Use an English title that says what changed.
- Put `Closes #nr` in the description, so the issue is closed and moves along the [project board](https://github.com/orgs/micro-botica/projects/1) automatically.
- Check with `gh pr list` or on GitHub whether a PR for the issue already exists. If so, push to that branch instead of opening a new one.
- Keep a PR small and focused on one issue.

## Reviews

- The other person reviews every PR. You do not merge your own PR without a review.
- The reviewer approves, or leaves comments that the author resolves.
- The author merges after the approval.
- The automatic Claude review is a help, not a replacement for the human review.

## Commits

- Write commit messages in English, short and in the imperative ("Add radar sweep").
- Commits are authored by the person who asked for the change. Do not add `Co-Authored-By` trailers or other Claude attribution.

## Secrets

Never commit Wi-Fi passwords or API keys. They only go in `firmware/include/secrets.h`, which is ignored by Git. Use `firmware/include/secrets.example.h` as a template.

## Style

- Code, comments, commit messages and documentation are in English.
- Do not use em dashes or en dashes. Use a comma, colon or period instead.
- Only use the hardware that is on the board itself. No extra sensors or modules.
