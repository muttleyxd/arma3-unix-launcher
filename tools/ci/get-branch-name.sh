#!/usr/bin/env bash
is_detached_head()
{
  IS_DETACHED_HEAD=$(git rev-parse --symbolic-full-name --abbrev-ref HEAD)
  if [ "$IS_DETACHED_HEAD" == "HEAD" ]; then
    return 0
  fi
  return 1
}

if is_detached_head; then
  git show -s --pretty=%D HEAD | awk '{gsub("origin/", ""); print $2}'
else
  git branch | grep "*" | cut -c 3-
fi
