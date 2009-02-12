#!/usr/local/bin/io

SVN
args := System args //at(1)=repo_path, at(2)=txn, at(3)=log_message
SVNrepos clone open(args at(1)) fs openTxn(args at(2)) changeProp("svn:log", args at(3))
System exit(0)

