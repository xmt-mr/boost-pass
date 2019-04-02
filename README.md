
# Overview
Boost.Pythonを用いた、C++とPython3の間の、様々なデータの受け渡し方法

# Dependency & Setup
・C++11
・Python3
・Boost.Python
・Opencv2

# Setup
Python3:
```bash
$ brew install python3
```

Boost.Python:
```bash
$ brew install boost-python3
```
or
```bash
$ sudo port install boost +python36
```

# Compile
g++ -o pass pass.cpp -std=c++11 -lboost_python37 -lboost_numpy37 `python3-config --cflags` `python3-config --ldflags` `pkg-config --libs opencv`
