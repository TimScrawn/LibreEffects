# Collaboration Workflow Guide

This guide explains how you and your collaborator will work together on LibreEffects using Git, GitHub, and Cursor.

## 🔄 How Collaboration Works

### Initial Setup (Your Friend's First Time)

Once you've added your friend as a collaborator, they need to:

1. **Accept the GitHub invitation** (they'll receive an email)
2. **Clone the repository** to their local machine:
   ```bash
   git clone https://github.com/TimScrawn/LibreEffects.git
   cd LibreEffects
   ```
3. **Open in Cursor**: They can open the cloned folder in Cursor AI

### Cursor & GitHub Integration

**Yes, Cursor works seamlessly with Git/GitHub!** Here's how:

- **Cursor automatically detects Git repositories** - When you open a folder with a Git repo, Cursor shows Git status
- **Built-in Git support** - Cursor has integrated Git features:
  - See file changes (green = new, yellow = modified, red = deleted)
  - Stage/unstage files
  - Commit changes
  - Push/pull from GitHub
  - View commit history
  - See branch information

## 📋 Daily Collaboration Workflow

### Standard Workflow (Both of You)

#### 1. **Before Starting Work** - Always Pull Latest Changes

```bash
# Make sure you're on the main branch
git checkout main

# Pull the latest changes from GitHub
git pull origin main
```

**In Cursor**: You can use the Source Control panel (Ctrl+Shift+G) to pull changes.

#### 2. **Create a Feature Branch** - Work on Your Own Features

```bash
# Create and switch to a new branch
git checkout -b feature/your-feature-name

# Example branches:
# feature/add-layer-system
# feature/image-filters
# bugfix/fix-crash-on-export
```

**Why branches?** This prevents conflicts and lets you work independently!

#### 3. **Make Your Changes** - Code Away!

- Edit files in Cursor
- Cursor will show you which files have changed
- Test your changes

#### 4. **Commit Your Changes**

```bash
# Stage your changes
git add .

# Commit with a descriptive message
git commit -m "Add: Layer system for image editing"
```

**In Cursor**: Use the Source Control panel to:
- Click the "+" next to files to stage them
- Type a commit message
- Click the checkmark (✓) to commit

#### 5. **Push Your Branch to GitHub**

```bash
git push origin feature/your-feature-name
```

**In Cursor**: Use the Source Control panel → Click the "..." menu → "Push"

#### 6. **Create a Pull Request**

1. Go to your repository on GitHub
2. You'll see a banner: "feature/your-feature-name had recent pushes"
3. Click **"Compare & pull request"**
4. Fill in:
   - **Title**: Brief description
   - **Description**: What you changed and why
   - **Reviewer**: Tag your friend (@their-username)
5. Click **"Create pull request"**

#### 7. **Review & Merge**

- Your friend reviews the code
- They can comment, suggest changes, or approve
- Once approved, merge the PR:
  - Click **"Merge pull request"**
  - Click **"Confirm merge"**
  - Optionally delete the feature branch

#### 8. **Update Your Local Main Branch**

After merging, both of you should:

```bash
# Switch back to main
git checkout main

# Pull the merged changes
git pull origin main

# Delete the old feature branch (optional)
git branch -d feature/your-feature-name
```

## 🔔 Getting Live Updates

### Automatic Updates

**Cursor doesn't automatically pull changes** - you need to manually sync. Here's why this is good:
- You control when to update
- You can review changes before pulling
- Prevents unexpected conflicts

### How to Stay Synced

**Option 1: Manual Pull (Recommended)**
```bash
# Before starting work each day
git pull origin main
```

**Option 2: Set Up a Reminder**
- Pull changes at the start of each work session
- Pull before creating a new branch
- Pull after your friend merges a PR

**Option 3: GitHub Notifications**
- Enable GitHub email notifications
- You'll get emails when PRs are created/merged
- Then you know when to pull

## 🎯 Best Practices

### 1. **Communication is Key**
- Discuss major features before starting
- Use GitHub Issues to track tasks
- Comment on PRs with questions

### 2. **Small, Frequent Commits**
- Commit often (don't wait days)
- Each commit should do one thing
- Write clear commit messages

### 3. **Pull Before Push**
- Always pull before pushing
- Resolve conflicts immediately
- Don't let branches get too far behind

### 4. **Use Descriptive Branch Names**
- ✅ Good: `feature/add-undo-redo`, `bugfix/fix-memory-leak`
- ❌ Bad: `test`, `changes`, `fix`

### 5. **Review Code Together**
- Don't merge your own PRs (have your friend review)
- Discuss major architectural decisions
- Learn from each other's code

## 🛠️ Cursor-Specific Tips

### Using Cursor's Git Features

1. **Source Control Panel** (Ctrl+Shift+G):
   - See all changed files
   - Stage/unstage files
   - Write commit messages
   - Push/pull changes

2. **Status Bar**:
   - Bottom of Cursor shows current branch
   - Shows number of changes

3. **File Explorer**:
   - Changed files show:
     - **U** = Untracked (new file)
     - **M** = Modified
     - **D** = Deleted

### Handling Conflicts

If you both edit the same file:

1. **Pull changes**:
   ```bash
   git pull origin main
   ```

2. **Git will mark conflicts**:
   ```
   <<<<<<< HEAD
   Your changes
   =======
   Their changes
   >>>>>>> branch-name
   ```

3. **In Cursor**:
   - Open the conflicted file
   - Cursor will highlight conflicts
   - Choose which version to keep (or merge both)
   - Remove the conflict markers

4. **Resolve and commit**:
   ```bash
   git add .
   git commit -m "Resolve merge conflicts"
   git push
   ```

## 📱 Quick Reference

### Daily Commands

```bash
# Start of day
git checkout main
git pull origin main

# Start new feature
git checkout -b feature/my-feature
# ... make changes ...
git add .
git commit -m "Add: Description"
git push origin feature/my-feature

# End of day (if PR merged)
git checkout main
git pull origin main
```

### Emergency: Undo Local Changes

```bash
# Discard all uncommitted changes
git checkout .

# Reset to last commit (careful!)
git reset --hard HEAD
```

## 🎉 You're Ready!

Once your friend clones the repo and opens it in Cursor, you're both ready to collaborate! The key is:
- **Pull before you start**
- **Work on branches**
- **Create PRs for review**
- **Merge together**

Happy coding! 🚀

