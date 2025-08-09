# Reminders for Team:

## Branching:

### Pulling branch
```bash
git checkout "<branch>"
git pull origin "<branch>"
```

### Pushing branch
```bash
git add .
git commit -m "<message>"
git push origin "<branch>"
```

### Merging
Merge using Gitlab

## Conventional Commit Syntax:
```
****************************************
** <type>(scope): <short description> **
****************************************
```
### Commit Types
```md
| Type       | Use Case                                                                 |
|------------|--------------------------------------------------------------------------|
| `feat`     | A new feature (e.g: `feat: add user login functionality`)                |
| `fix`      | A bug fix (e.g: `fix: correct password validation logic`)                |
| `perf`     | A code change that improves performance                                  |
| `refactor` | A code change that neither fixes a bug nor adds a feature                |
| `chore`    | Maintenance tasks (e.g: `build system config, tooling, etc`)             |
| `docs`     | Documentation only changes                                               |
| `style`    | Changes that don’t affect meaning (e.g: `whitespace, formatting, etc`)   |
| `test`     | Adding or updating tests                                                 |
| `build`    | Changes that affect the build system or dependencies (e.g: `npm`)        |
| `ci`       | Changes to CI/CD configuration (e.g: `GitHub Actions, CircleCI`)         |
| `revert`   | Reverting a previous commit                                              |
```
