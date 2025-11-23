# Recommended Cursor IDE Extensions

Since Cursor is based on VS Code, you can install VS Code extensions! Here are the best ones for Git/GitHub workflow:

## 🔥 Essential GitHub Extensions

### 1. **GitHub Pull Requests and Issues**
- **Extension ID**: `GitHub.vscode-pull-request-github`
- **What it does**:
  - View and manage Pull Requests directly in Cursor
  - Create PRs without leaving the editor
  - Review PRs with inline comments
  - See GitHub issues
  - Merge PRs from Cursor
- **Why you need it**: Makes PR workflow seamless!

### 2. **GitLens — Git Supercharged**
- **Extension ID**: `eamodio.gitlens`
- **What it does**:
  - See who wrote each line (blame annotations)
  - View file history and commit details
  - Compare branches visually
  - See recent changes
  - Interactive rebase editor
- **Why you need it**: Makes Git history and collaboration much easier!

### 3. **Git Graph**
- **Extension ID**: `mhutchie.git-graph`
- **What it does**:
  - Visual branch graph
  - See all branches and commits
  - Easy branch management
  - Cherry-pick commits visually
- **Why you need it**: Visual representation of your Git history!

## 🎨 Additional Helpful Extensions

### 4. **Git History**
- **Extension ID**: `donjayamanne.githistory`
- **What it does**: View file history, search commits, compare versions

### 5. **GitHub Copilot** (if you want AI assistance)
- **Extension ID**: `GitHub.copilot`
- **What it does**: AI pair programmer (though Cursor has built-in AI)

### 6. **Error Lens**
- **Extension ID**: `usernamehw.errorlens`
- **What it does**: Shows errors inline as you type
- **Why it's useful**: Catch issues before committing!

## 📦 How to Install Extensions in Cursor

### Method 1: Extension Marketplace
1. Press `Ctrl+Shift+X` (or `Cmd+Shift+X` on Mac)
2. Search for the extension name
3. Click "Install"

### Method 2: Command Palette
1. Press `Ctrl+Shift+P` (or `Cmd+Shift+P` on Mac)
2. Type "Extensions: Install Extensions"
3. Search and install

### Method 3: Direct Install
1. Press `Ctrl+Shift+P`
2. Type "Extensions: Install from VSIX..." (if you have the file)
3. Or use the extension ID directly

## 🚀 Quick Setup Steps

1. **Install GitHub Pull Requests extension**:
   ```
   Ctrl+Shift+X → Search "GitHub Pull Requests" → Install
   ```

2. **Sign in to GitHub**:
   - After installing, you'll be prompted to sign in
   - Authorize Cursor to access GitHub
   - This enables PR management

3. **Install GitLens**:
   ```
   Ctrl+Shift+X → Search "GitLens" → Install
   ```

4. **Install Git Graph**:
   ```
   Ctrl+Shift+X → Search "Git Graph" → Install
   ```

## 💡 Using the Extensions

### GitHub Pull Requests Panel
- **Access**: Click the GitHub icon in the sidebar
- **Create PR**: Right-click your branch → "Create Pull Request"
- **View PRs**: See all open PRs in the panel
- **Review**: Click on a PR to see changes and add comments

### GitLens Features
- **Blame**: Hover over code to see who wrote it
- **File History**: Right-click file → "Open File History"
- **Compare**: Right-click file → "Compare with..."

### Git Graph
- **Open**: Command Palette → "Git Graph: View Git Graph"
- **Visualize**: See your entire Git tree
- **Actions**: Right-click commits/branches for options

## 🎯 Recommended Workflow with Extensions

1. **Start work**: Pull latest (`git pull origin main`)
2. **Create branch**: Use Git Graph or command line
3. **Make changes**: Code in Cursor
4. **Commit**: Use Source Control panel (Ctrl+Shift+G)
5. **Push**: Use Source Control panel or command line
6. **Create PR**: Use GitHub Pull Requests extension (right-click branch)
7. **Review**: Use GitHub Pull Requests panel
8. **Merge**: Use GitHub Pull Requests extension
9. **Update**: Pull latest changes

## 🔐 Authentication

When you first use GitHub extensions:
- You'll be prompted to sign in
- Authorize Cursor/GitHub extension
- You may need a Personal Access Token
- Follow the prompts in the extension

---

**Pro Tip**: Start with GitHub Pull Requests and GitLens - they'll make the biggest difference in your workflow!

