{
    "targets": [{
        "target_name": "backend",
        "sources": ["src/backend/backend.cpp"],
        "cflags": ["-std=c++11", "-Wall", "-Wextra"],
        "libraries": ["-lbackend"],
        "configurations": {
            "Debug": {
                "cflags": ["-g"],
                "ldflags": ["-LDebug"]
            },
            "Release": {
                "cflags": ["-Ofast"],
                "ldflags": ["-LRelease"],
                "defines": ["NDEBUG"],
            }
        }
    }]
}
