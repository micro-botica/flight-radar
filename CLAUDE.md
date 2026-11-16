# Instructions for Claude

This repository is the Flight Radar project by Maarten (@Remotev2) and Mick (@MVelden1): a live ADS-B aircraft radar on a Waveshare ESP32-S3-Touch-LCD-2.1 round display. See README.md for the overview and the project board for planned work.

## Rules

- Never add `Co-Authored-By` trailers or any other Claude attribution to commits or pull requests. Commits must be authored by the person who asked.
- Never push directly to `main`. Work on a branch and open a pull request.
- Link pull requests to their issue with `Closes #nr` in the description.
- When you have changed files for an issue, open the pull request yourself with `gh pr create` (not as a draft): an English title, a short summary of what changed and `Closes #nr`. First check with `gh pr list` whether a PR for that issue already exists; if so, push to its branch instead of opening a new one.
- Do not push a branch or open a pull request when nothing changed.
- Never commit secrets. Wi-Fi credentials and API keys only go in `firmware/include/secrets.h`, which is ignored by Git.
- Only use the hardware that is on the board itself. No extra sensors or modules.
- Write code, comments, commit messages and documentation in English.
- Do not use em dashes or en dashes in any text. Use a comma, colon or period instead.

## Using @claude on GitHub

Mention `@claude` in an issue, a PR comment or a PR review. The workflow in `.github/workflows/claude.yml` runs on the Claude subscription of whoever wrote the comment.

Each of us adds our own token once:

1. Install Claude Code and run `claude setup-token`. Log in with your own Claude account (Pro or Max).
2. Copy the token that it prints.
3. In this repository go to Settings > Secrets and variables > Actions > New repository secret.
4. Name: `CLAUDE_TOKEN_MICK` (Mick) or `CLAUDE_TOKEN_MAARTEN` (Maarten). Paste the token and save.

Secrets are write-only, so neither of us can see the other's token.

## Unverified work and documentation

Claude has no memory between runs. Only what is in the repository or in an issue exists for the next run, so follow these rules whenever something could not be checked (no `pio run`, no board, no radio, no display):

1. Document the open check in `docs/system-testing.md`: what to test, how to test it, the expected result and the issue it belongs to.
2. Run `gh issue list` first. If no issue covers the open check, create one with the label and milestone of the source issue. Never create duplicates.
3. Update the related documentation (`README.md`, `docs/`) in the same pull request, so code and docs merge together.
4. State in the pull request description what was and was not verified, and which issue covers the rest.
5. When a check is verified later, remove it from the document and tick or close the issue.
6. Every new feature gets an entry in `docs/system-testing.md` in the same pull request.
7. Claude cannot change `.github/workflows`. Put files that belong under `.github/` in `docs/github-templates/` and give the user the command to copy them into place.
