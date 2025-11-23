# GitHub Repository Setup Guide

This guide will help you set up the LibreEffects repository on GitHub for collaboration.

## 🚀 Initial GitHub Setup

### Step 1: Create GitHub Repository

1. Go to [GitHub.com](https://github.com) and sign in
2. Click the **"+"** icon in the top right corner
3. Select **"New repository"**
4. Fill in the repository details:
   - **Repository name**: `LibreEffects`
   - **Description**: "Open-source alternative to Adobe Creative Suite"
   - **Visibility**: Choose Public (recommended) or Private
   - **DO NOT** initialize with README, .gitignore, or license (we already have these)
5. Click **"Create repository"**

### Step 2: Connect Local Repository to GitHub

After creating the repository on GitHub, you'll see setup instructions. Run these commands in your terminal:

```bash
# Navigate to your project directory
cd "C:\Users\Cadan\Documents\Cursor Projects\LibreEffects"

# Add the remote repository (replace YOUR_USERNAME with your GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/LibreEffects.git

# Rename branch to main (if needed)
git branch -M main

# Push your code to GitHub
git push -u origin main
```

### Step 3: Add Collaborator

1. Go to your repository on GitHub
2. Click on **"Settings"** tab
3. Click on **"Collaborators"** in the left sidebar
4. Click **"Add people"**
5. Enter your friend's GitHub username or email
6. Click **"Add [username] to this repository"**

Your friend will receive an invitation email and can accept it to start collaborating.

## 🔄 Daily Workflow

### Making Changes

```bash
# Pull latest changes from GitHub
git pull origin main

# Create a new branch for your feature
git checkout -b feature/your-feature-name

# Make your changes, then stage them
git add .

# Commit your changes
git commit -m "Add: Description of your changes"

# Push your branch to GitHub
git push origin feature/your-feature-name
```

### Creating Pull Requests

1. After pushing your branch, go to GitHub
2. You'll see a prompt to create a Pull Request
3. Click **"Compare & pull request"**
4. Fill in the PR description
5. Request review from your collaborator
6. After review and approval, merge the PR

### Syncing Changes

```bash
# Always pull before starting work
git pull origin main

# If you're on a feature branch, merge main into it
git checkout feature/your-feature-name
git merge main
```

## 📋 Branch Strategy

- **main**: Stable, production-ready code
- **develop**: Integration branch (optional)
- **feature/***: New features
- **bugfix/***: Bug fixes

## 🔐 Authentication

If you encounter authentication issues:

1. **Use Personal Access Token** (recommended):
   - Go to GitHub Settings → Developer settings → Personal access tokens
   - Generate a new token with `repo` permissions
   - Use the token as your password when pushing

2. **Or use SSH**:
   ```bash
   # Generate SSH key (if you don't have one)
   ssh-keygen -t ed25519 -C "your_email@example.com"
   
   # Add SSH key to GitHub (Settings → SSH and GPG keys)
   # Then use SSH URL:
   git remote set-url origin git@github.com:YOUR_USERNAME/LibreEffects.git
   ```

## ✅ Verification

After setup, verify everything works:

```bash
# Check remote is set correctly
git remote -v

# Should show:
# origin  https://github.com/YOUR_USERNAME/LibreEffects.git (fetch)
# origin  https://github.com/YOUR_USERNAME/LibreEffects.git (push)
```

## 🆘 Troubleshooting

**Problem**: "Permission denied" when pushing
- **Solution**: Check that you're authenticated and have write access

**Problem**: "Updates were rejected"
- **Solution**: Pull first: `git pull origin main --rebase`, then push again

**Problem**: Merge conflicts
- **Solution**: Resolve conflicts in your editor, then `git add .` and `git commit`

---

Happy coding! 🎨

