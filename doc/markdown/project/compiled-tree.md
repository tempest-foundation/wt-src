# Wind/Tempest Project - Tree

> [!WARNING]
> This tree is not final and will probably change as the project evolves.

```
/
├── Applications
│   ├── app1 -> ...
│   ├── app2 -> ...
│   ├── app3 -> ...
│   ├── SystemApps -> ...
│   └── Utilities -> ...
├── System
│   ├── lib
│   ├── boot
│   ├── mount
│   ├── devices
│   ├── tmp
│   ├── drivers
│   ├── kernel
│   ├── logs
│   │   ├── kernel
│   │   └── user
│   ├── volumes
│   │   ├── volume1
│   │   ├── volume2
│   │   └── volume3
│   ├── services
│   └── config
├── Home
│   ├── user1
│   │   ├── .config
│   │   ├── .local
│   │   ├── Desktop
│   │   ├── Documents
│   │   ├── Downloads
│   │   ├── Music
│   │   ├── Pictures
│   │   └── Videos
│   ├── user2
│   │   ├── .config
│   │   ├── .local
│   │   ├── Desktop
│   │   ├── Documents
│   │   ├── Downloads
│   │   ├── Music
│   │   ├── Pictures
│   │   └── Videos
│   └── user3
│       ├── .config
│       ├── .local
│       ├── Desktop
│       ├── Documents
│       ├── Downloads
│       ├── Music
│       ├── Pictures
│       └── Videos
```

## Why is not POSIX?

> **yes.**

Because POSIX tree is too big and complex.  It's a mess.  
Wind/Tempest is designed for humans, not for 40-year-old standards made for UNIX servers.
