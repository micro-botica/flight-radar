# Instructions for Claude

This repository is the Flight Radar project by Maarten (@Remotev2) and Mick (@MVelden1): a live ADS-B aircraft radar on a Waveshare ESP32-S3-Touch-LCD-2.1 round display. See README.md for the overview and the project board for planned work.

## Rules

- Never add `Co-Authored-By` trailers or any other Claude attribution to commits or pull requests. Commits must be authored by the person who asked.
- Never push directly to `main`. Work on a branch and open a pull request.
- Link pull requests to their issue with `Closes #nr` in the description.
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
