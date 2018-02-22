#!/bin/bash

exists=`git show-ref refs/heads/test`
if [ ! -n "$exists" ]; then
    echo "Branch doesn't exists!"
    git branch test
fi

git checkout test && git branch -D master-lede && git fetch origin && git checkout -b master-lede origin/master-lede
