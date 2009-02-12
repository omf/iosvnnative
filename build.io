AddonBuilder clone do(
	SystemCall setCommand("/usr/local/apr/bin/apr-1-config") setArguments(list("--cflags", "--cppflags", "--includes")) run
	aprcompopts := SystemCall stdout readLine
	
	SystemCall setCommand("/usr/local/apr/bin/apr-1-config") setArguments(list("--link-ld", "--libs")) run
	aprlinkopts := SystemCall stdout readLine

	l := List clone
	d := Directory setPath("/usr/local/lib")
	d files foreach(v, if(v name containsSeq("libsvn") and v name containsSeq(".a"), l append("/usr/local/lib/" .. v name)) )
	s := l join(" ")

	//dependsOnLib("apr-1")
	//dependsOnLib("svn_repos-1")
	//dependsOnLib("svn_fs-1")
	dependsOnInclude(aprcompopts .. " -I/usr/local/include/subversion-1")
	dependsOnLinkOption(aprlinkopts)
	dependsOnLinkOption("--verbose -lsvn_repos-1 -lsvn_fs-1")
	//dependsOnLinkOption(s)
)
