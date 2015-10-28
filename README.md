#ASK 1.0
#Authentication Sessions Keeper Server
Copyright (C) 2015, Luca Stasio joshuagame AT gmail DOT com //The CodeGazoline Team/

ASK is a simple session server mantaining authentication info per session.
It exposes a simple HTTP REST-like interface with few API methods: it is not a full REST interface, but a simple
HTTP endpoint with simple routes.
It is intended as a sort of simple Single Sign-On session server in a corporate.

##Roadmap
* FORM-based authentication
* Session Store
    * filesystem
    * Redis
* Use Presence information mgmt and propagation
* Cluster and session replication
