This directory holds Xinu sources for VirtualBox. This branch contains the multicore migration of these sources, which is a work in progress and is not guaranteed to be stable.

Current status of multicore migration & known TODOs:
-Networking is mostly functional, but network actions initiated in Xinu (such as ping and date) sometimes fail. The root cause of this has not yet been identified - it may be due to either issues in the networking code or issues in the multicore-specific inter-process communication code.
-Deadlocks happen occasionally when reading/writing to the shell. Some of these issues were resolved in 6f2f64b, but there still appears to be some deadlocking happening during ping/date calls. Whether this is an issue with the network stack or IPC has not been established and should be investigated.
