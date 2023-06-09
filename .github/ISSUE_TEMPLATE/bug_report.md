---
name: Bug report
about: Create a report to help us improve
title: ''
labels: ''
assignees: ''
body:
- type: input
  id: build_tag
  attributes:
    label: 'Build ID'
    description: 'Either commit hash of the current build or the game version marked by the release tag (eg.: v1.0.1-pre)'
    placeholder: 'v1.0.0'
  validations:
    required: true
---

**Describe the bug**
A clear and concise description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:


**Expected behavior**
A clear and concise description of what you expected to happen.


**Screenshots**
If applicable, add screenshots to help explain your problem.

**Additional context**
Add any other context about the problem here.
