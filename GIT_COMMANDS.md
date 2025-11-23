# Git Commands Quick Reference

Quick reference for common Git commands you'll use daily.

## 🔄 Basic Push & Pull

### Pull (Get Latest Changes from GitHub)

```bash
# Pull latest changes from main branch
git pull origin main

# Or if you're already on main and it's set as upstream:
git pull
```

### Push (Send Your Changes to GitHub)

```bash
# Push current branch to GitHub
git push origin main

# Or if branch is already tracking:
git push

# Push a new branch for the first time:
git push -u origin branch-name
```

## 📋 Complete Daily Workflow Commands

### Start of Day - Get Latest Changes

```bash
# Make sure you're on main branch
git checkout main

# Pull latest changes
git pull origin main
```

### Working on a Feature

```bash
# Create and switch to new branch
git checkout -b feature/my-feature-name

# Make your changes, then:
git add .                    # Stage all changes
git commit -m "Add: Description"  # Commit with message
git push origin feature/my-feature-name  # Push to GitHub
```

### After PR is Merged

```bash
# Switch back to main
git checkout main

# Pull the merged changes
git pull origin main

# Delete old feature branch (optional)
git branch -d feature/my-feature-name
```

## 🔍 Useful Status Commands

```bash
# Check current status
git status

# See what branch you're on
git branch

# See commit history
git log --oneline

# See remote repository info
git remote -v
```

## ⚠️ Common Scenarios

### Undo Uncommitted Changes

```bash
# Discard changes to a specific file
git checkout -- filename

# Discard ALL uncommitted changes (careful!)
git checkout .
```

### Update Branch with Latest Main

```bash
# While on your feature branch:
git checkout feature/my-feature
git merge main              # Merge main into your branch
# Resolve any conflicts, then:
git push origin feature/my-feature
```

### See What Changed

```bash
# See changes in working directory
git diff

# See changes staged for commit
git diff --staged

# See changes between branches
git diff main..feature/my-feature
```

