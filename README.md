# 拼车

## 依赖（用于前端）

- Node.js （版本必须 >= 6.0.0）
- npm （Node.js 的包管理器）
- make

仅在Linux下通过测试，其他系统的兼容性未知。

## 编译

将数据文件`car.txt`、`road.cnode`和`road.nedge`复制进`data/`目录，然后在项目根目录执行：

```
make build # 需要联网，会自动下载所需的Node.js包
```

## 运行

在项目根目录执行：

```
make run # 需要X Window环境（不能在纯字符环境执行）
```

弹出的窗口中会出现一张地图。在地图上点击标记出发地和目的地，地图上会显示候选出租车。点击某一辆出租车，即可显示其原最佳路径、拼车后最佳路径，和各项绕路距离。点击右下角的“×”以清除候选并重新选取路径。
