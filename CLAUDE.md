# Source SDK Agent Instructions

Every task in this repository follows the `source-sdk` skill, imported below. Its references live
in [external/skills/source-sdk/references/](external/skills/source-sdk/references/) — read the
ones the skill points to for the task at hand.

@external/skills/source-sdk/SKILL.md

If [external/skills/source-sdk/SKILL.md](external/skills/source-sdk/SKILL.md) is missing, the
`external/skills` submodule is not checked out. Fetch it before doing anything else, then read the
skill:

```sh
git submodule update --init --depth 1 external/skills
```
