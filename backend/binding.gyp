{
    "targets": [{
        "target_name": "backend",
        "sources": ["<!@(ls -1 *.cpp)"],
        "cflags": ["-std=c++11", "-Wall", "-Wextra"],
        "configurations": {
            "Debug": {
                "cflags": ["-g"],
            },
            "Release": {
                "cflags": ["-Ofast"],
                "defines": ["NDEBUG"],
            }
        }
    }]
}
