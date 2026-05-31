# CLAUDE.md — 项目宪法

一个保留 IBus 核心设计的轻量级分布式通信框架。此文件是项目宪法（Project Constitution），所有开发任务都必须遵守。AI 工具每次启动时必须先读此文件。

---

## 一、项目目标

构建一个轻量级、可学习、含多设备拓扑合并的分布式通信中间件，作为 IBus 的精简版实现。

### 核心特性

- NodeAddress = (Role, Name) 双层命名
- Pub/Sub + RPC 双通信模型
- MsgDispatcher 多 lane 优先级隔离
- 每设备一个 ibusd + 共享内存路由表
- 多设备拓扑合并 通过 gossip + Dijkstra
- 心跳 + 故障自动重路由

### 非目标（明确不做）

- ❌ SPI/UART 等嵌入式链路（只支持 UDS + TCP）
- ❌ Reliable Tunnel（TCP 已经可靠）
- ❌ 消息切片（限制单消息 64KB）
- ❌ 多产品配置（单一拓扑配置文件）
- ❌ 加密 / 鉴权
- ❌ 跨平台（仅 Linux）

---

## 二、当前状态

> 每完成一个任务，请更新此区域

**当前 Phase**：Phase 0（项目骨架）

**进行中任务**：
- 任务 0.2: 定义全局错误码

**已完成任务**：
- 任务 0.1: 初始化项目结构 + CMake

**已完成模块**：
- CMakeLists.txt（顶层 + 各子目录）
- .gitignore
- README.md

**下一步任务**：任务 0.2 → 0.3 → 0.4 → Phase 1

---

## 三、技术栈

| 类别 | 选型 | 备注 |
|---|---|---|
| 语言 | C++17 | 禁用异常、禁用 RTTI |
| 构建 | CMake 3.16+ | |
| 序列化 | Protobuf 3.x | 必需 |
| 日志 | spdlog | 通过封装宏使用 |
| 测试 | GoogleTest | 所有模块必带单测 |
| 配置 | YAML 或 JSON | nlohmann/json 或简易解析 |
| 并发 | std::thread + std::mutex | 不引入 boost::asio |

### 禁止引入的依赖

- ❌ boost（除非有不可替代的必要性，需先讨论）
- ❌ Qt
- ❌ ZeroMQ / DDS / gRPC
- ❌ 任何 ROS 相关库
- ❌ liburing（避免增加复杂度）

---

## 四、代码规范

### 4.1 命名约定

| 类型 | 规则 | 示例 |
|---|---|---|
| 类 | PascalCase | MsgLooper |
| 函数 | PascalCase | CalculateShortestPaths |
| 成员变量 | snake_case_ | next_hop_role_ |
| 局部变量 | snake_case | peer_role |
| 常量 | kPascalCase | kHeartbeatIntervalMs |
| 枚举值 | PascalCase | Role::Camera |
| 文件 | snake_case | msg_looper.cc |
| 命名空间 | snake_case | minibus::ibusd |

### 4.2 文件组织

- 头文件用 `#pragma once`，不用 include guards
- 一个类一个文件（小工具类可以合并）
- `.h` 只放声明 + inline 实现
- 模板实现放 `.h` 中

### 4.3 错误处理

强制使用错误码模式，禁用异常：

```cpp
// 推荐
tl::expected<Result, ErrorCode> DoSomething();
// 简单场景
std::optional<Value> Lookup(Key key);
// 仅状态码
ErrorCode SendMessage(...);
```

错误码统一在 `core/errors.h` 定义。

### 4.4 注释规范

- 函数注释：解释"为什么"而不是"是什么"
- 头文件必须有简短说明，如：`// msg_looper.h - 单线程消息循环，按时间戳排序消费`

**重要原则：宁可不写注释也不要写垃圾注释。**

### 4.5 多线程规则

- 共享状态必须用 `std::mutex` 或 `std::atomic` 保护
- 禁止使用全局可变变量（const 除外）
- 锁的粒度尽量小
- 长任务（IO、计算）**不能持锁**
- 死锁优先考虑用 `std::scoped_lock` 多锁同时获取

### 4.6 内存管理

- 优先用智能指针（`unique_ptr` / `shared_ptr`）
- 跨线程共享对象用 `shared_ptr`
- 禁用 `new` / `delete`（用 `make_unique` / `make_shared`）
- 缓冲区用 `std::vector<uint8_t>` 或 `std::string`

### 4.7 错误处理具体规则

```cpp
// 1. 检查所有可能失败的返回值
auto result = SomeOperation();
if (!result) {
  MINIBUS_ERROR("operation failed: {}", ErrorCodeToString(result.error()));
  return result.error();
}

// 2. 资源 RAII
{
  std::lock_guard lock(mutex_);
  // 自动释放
}

// 3. 网络/IO 错误必须处理 EINTR
while (recv(...) < 0 && errno == EINTR) continue;

// 4. SIGPIPE 必须屏蔽
signal(SIGPIPE, SIG_IGN);
// 或使用 MSG_NOSIGNAL
```

---

## 五、目录结构

```
mini-ibus/
├── CLAUDE.md       ← 本文件
├── CMakeLists.txt  ← 顶层构建文件
├── README.md
├── .gitignore
├── docs/           ← 详细文档
│   ├── full_plan.md    ← 完整实施方案
│   ├── task_breakdown.md ← 任务拆解清单
│   ├── architecture.md
│   └── protocol.md
├── proto/          ← Protobuf 定义
│   ├── CMakeLists.txt
│   ├── message.proto
│   └── topology.proto
├── core/           ← 核心库（业务进程链接）
│   ├── CMakeLists.txt
│   ├── address.h/.cc
│   ├── errors.h/.cc
│   ├── logging.h
│   ├── message_codec.h/.cc
│   ├── connection.h/.cc
│   ├── tcp_connection.h/.cc
│   ├── thread_pool.h/.cc
│   ├── msg_looper.h/.cc
│   ├── client.h/.cc
│   ├── client_config.h/.cc
│   └── route_table.h/.cc
├── addons/         ← 业务侧可选增强（不属于框架核心）
│   ├── CMakeLists.txt
│   ├── README.md
│   ├── msg_dispatcher.h/.cc
│   └── ...
├── ibusd/          ← 守护进程
│   ├── CMakeLists.txt
│   ├── main.cc
│   ├── config.h/.cc
│   ├── connection_manager.h/.cc
│   ├── peer_manager.h/.cc
│   ├── subscription_table.h/.cc
│   ├── topology.h/.cc
│   ├── undirected_graph.h/.cc
│   ├── route_calculator.h/.cc
│   ├── topology_collector.h/.cc
│   ├── topology_broadcaster.h/.cc
│   ├── external_topology_store.h/.cc
│   ├── topology_merger.h/.cc
│   ├── merge_thread.h/.cc
│   ├── heartbeat.h/.cc
│   └── diag_server.h/.cc
├── tools/          ← 命令行工具
│   ├── CMakeLists.txt
│   └── mini-ibus-cli.cc
├── examples/       ← Demo 应用
│   ├── echo_service/
│   ├── pubsub_demo/
│   ├── two_device_demo/
│   ├── three_device_demo/
│   └── distributed_recorder/
└── tests/
    ├── unit/       ← 单元测试
    ├── integration/ ← 集成测试 + Docker Compose
    └── perf/       ← 性能基准
```

---

## 六、关键设计决策

### 6.1 地址模型：NodeAddress = (Role, Name) 拼成 uint32

- 高 16 位是 Role（Camera/Screen/Box/GUI）
- 低 16 位是 Name（具体程序名）
- 编译期已知，不做动态服务发现

**为什么**：嵌入式场景节点固定，编译期可以确定，避免运行时发现的复杂度。

### 6.2 单 ibusd + 跨 ibusd 拓扑合并

- 每台设备一个 ibusd 守护进程
- 业务进程通过 UDS 连本机 ibusd
- ibusd 之间通过 TCP 互连，gossip 拓扑信息
- 每个 ibusd 独立跑 Dijkstra，写入本机共享内存路由表

**为什么**：去中心化，无单点故障。

### 6.3 共享内存路由表

- ibusd 是 writer，业务进程是 reader
- 用 `version` 双校验做无锁读
- 业务进程 O(1) 查路由，不走 IPC

**为什么**：性能敏感路径不能走 IPC。

### 6.4 拓扑版本号

- 每个 publisher_role 维护单调递增 `uint64` version
- ExternalTopologyStore 按版本号决定是否更新
- 跨 role 的版本号不可比较

**为什么**：防止网络延迟导致旧消息覆盖新消息。

### 6.5 Peer 连接是配置驱动

- 启动时按配置文件主动连
- 自动重连，每 3 秒重试一次
- 不做动态发现（不像 DDS 的多播）

**为什么**：避免发现协议的复杂度。

### 6.6 第一版不做拓扑转发

- ibusd-A 只广播给直连的 peer
- A 通过 B 把拓扑传到 C？**第一版不支持**
- 假设所有 ibusd 之间至少有一条直连路径

**为什么**：转发需要 TTL + 去重 + 环路检测，第一版不引入。后续可加。

### 6.7 MsgDispatcher 4 lane（业务侧增强，可选，放在 addons/）

- **不是 mini-ibus 框架核心组件**，而是业务侧的最佳实践封装
- 源自 IBus 的 GUI 进程设计：用 4 个独立 lane 隔离慢任务和快任务
- Urgent / Critical / Normal / Blocking 各一个独立线程
- 提供 OrFallback 兜底（投递失败时同步执行）
- 简单业务进程可以不用，直接在 Client 接收线程跑

**目录归属**：`addons/msg_dispatcher.h`，不放在 `core/`

**为什么放在 addons/**：
1. IBus 项目里只有 GUI 进程用，mediad/hald/BLA 等都没用，证明它不是通用框架组件
2. Client 接口只暴露 `std::function<void(std::function<void()>)>` 通用回调，与 MsgDispatcher 解耦
3. 保持 core 简洁，便于学习者理解核心架构

**为什么仍然实现**：作为业务侧最佳实践的示范，且是 IBus 设计的重要亮点。

### 6.8 State 用 shared_ptr 隔离生命周期

- MsgLooper 内部有 `shared_ptr<State>`
- 线程入口只 capture State，不 capture this
- 防止对象析构后线程还在访问

**为什么**：避免 self-destruction 时的 UAF。

---

## 七、协议设计要点

### 7.1 LinkHeader（固定 16 字节）

```cpp
struct LinkHeader {
  uint8_t magic[2];      // "MI"
  uint8_t version;        // 协议版本（当前=1）
  uint8_t type;           // MessageType
  uint32_t src_addr;
  uint32_t dst_addr;
  uint32_t payload_size;  // 限制 64KB
} __attribute__((packed));
```

### 7.2 消息类型

- REGISTER (1) - 业务进程注册
- RPC_REQUEST (2)
- RPC_RESPONSE (3)
- PUBLISH (4)
- SUBSCRIBE (5)
- HEARTBEAT (6)
- TOPOLOGY (7) - ibusd 之间的拓扑广播
- GOODBYE (8) - 优雅退出通知（Phase 4 引入）

### 7.3 编解码原则

- LinkHeader 用裸字节（手动 packed）
- Payload 用 Protobuf
- TCP 拆包：先读 16 字节 header，再按 payload_size 读 body

---

## 八、测试要求

### 8.1 必须的单元测试

每个核心模块都要有单测：

- `address_test.cc` - Address 编解码
- `errors_test.cc` - 错误码到字符串
- `message_codec_test.cc` - LinkHeader 编解码
- `connection_test.cc` - UDS/TCP 收发
- `thread_pool_test.cc` - 线程池
- `msg_looper_test.cc` - 消息循环
- `msg_dispatcher_test.cc` - 4 lane 分发（addons）
- `route_table_test.cc` - 共享内存路由表
- `config_test.cc` - 配置文件加载
- `client_timeout_test.cc` - RPC 超时取消
- `subscription_table_test.cc` - 订阅表
- `undirected_graph_test.cc` - 无向图
- `route_calculator_test.cc` - **Dijkstra（最关键）**
- `topology_merger_test.cc` - 拓扑合并
- `topology_version_test.cc` - 拓扑版本号防回环

### 8.2 必须的集成测试

每个 Phase 完成后跑一遍：

- Phase 1: 单设备 RPC + PubSub
- Phase 2: 双设备直连
- Phase 3: 三设备拓扑合并（A-B-C）
- Phase 4: 故障注入 + 自动恢复
- Phase 5: 性能隔离验证

### 8.3 测试覆盖率目标

- 核心模块单测覆盖率 > 80%
- Dijkstra 必须覆盖所有边界 case
- 故障路径必须有专门测试

### 8.4 测试运行约定

```bash
# 单元测试
cd build && ctest --output-on-failure

# 集成测试
./tests/integration/test_xxx.sh

# 性能测试
./tests/perf/run_bench.sh
```

---

## 九、禁止事项

### 9.1 严格禁止

- ❌ **不要"顺便重构"已完成的模块** - 任务边界要严格
- ❌ **不要引入新依赖** - 除非 PROJECT 宪法允许，必须先讨论
- ❌ **不要写无单元测试的代码** - 单测是任务完成的硬性标准
- ❌ **不要使用异常** - 全部用错误码
- ❌ **不要在 ibusd 里加业务逻辑** - ibusd 保持纯路由 + 拓扑
- ❌ **不要跨设备比较时钟** - 时间戳只能本地用
- ❌ **不要在持锁状态调用慢操作** - IO/RPC/计算必须在锁外

### 9.2 警惕事项

- ⚠️ 任何 `new` / `delete` 都需要解释（优先用智能指针）
- ⚠️ 任何全局变量都需要解释（优先用单例 + DI）
- ⚠️ 任何 sleep/busy-wait 都需要解释（优先用 cv）
- ⚠️ 任何 `std::async` / `std::thread` 都需要明确生命周期
- ⚠️ 任何裸 socket fd 都要 RAII 包装

---

## 十、AI 工具协作约定

### 10.1 任务粒度

- 每次任务必须能在 30-60 分钟内完成
- 一个任务产出 100-300 行代码（含测试）
- 一个任务一个 git commit

### 10.2 上下文要求

每次任务开始前，AI 必须：
1. 读 CLAUDE.md（本文件）
2. 读任务相关的已完成模块
3. 读 `docs/task_breakdown.md` 中本任务的描述

### 10.3 产出要求

每次任务完成时，AI 必须：
1. 列出新建/修改的文件
2. 解释关键设计决策
3. 跑通对应的单元测试
4. 提示需要更新 CLAUDE.md 的"已完成模块"清单

### 10.4 禁止事项

AI **不允许**：
- 修改任务范围外的文件
- 引入新依赖
- 跳过单元测试
- "通读项目代码然后大改"
- 主动重构已完成模块
- 在不询问的情况下做架构决策

### 10.5 鼓励事项

AI **应当**：
- 主动指出代码中的潜在问题
- 主动询问模糊的需求
- 主动建议更好的设计（但不擅自实施）
- 主动跑测试验证

---

## 十一、Git 工作流

### 11.1 Commit 信息规范

```
<scope>: <description>

[可选 body]
```

例子：
```
core: implement Address encoding/decoding
ibusd: add peer connection manager
test: add Dijkstra edge cases
docs: update CLAUDE.md after Phase 1
```

### 11.2 Branch 策略

- `main` - 稳定分支，每个 Phase 完成后合并
- `phase-N-feature-X` - 任务分支
- 每个任务一个 commit，每个 Phase 一次合并

### 11.3 必须提交的内容

- 所有源代码（.h/.cc）
- 所有 CMakeLists.txt
- 所有单元测试
- 必要的配置文件

### 11.4 不要提交

- `build/` 目录
- `*.pb.h` / `*.pb.cc`（生成代码）
- 编辑器配置（`.vscode/` 个性化部分）
- 个人调试日志

---

## 十二、性能目标

仅供参考，不强制达到：

| 指标 | 目标 |
|---|---|
| RPC 延迟（单机 P99） | < 1ms |
| RPC 吞吐量（单机） | > 10K QPS |
| Pub/Sub 延迟 | < 500us |
| 拓扑收敛时间 | < 5 秒 |
| 共享内存路由查询 | < 1us |
| 启动时间 | < 2 秒 |
| 内存占用（ibusd） | < 50 MB |

---

## 十三、相关文档

- `docs/full_plan.md` - 完整实施方案（全设计细节）
- `docs/task_breakdown.md` - 55 个任务的详细拆解
- `docs/architecture.md` - 架构文档（Phase 7 写）
- `docs/protocol.md` - 协议文档（Phase 7 写）

---

## 十四、快速开始（给 AI 的标准 Prompt）

每次开始一个新任务时，使用以下模板：

> 请先阅读 CLAUDE.md 了解项目宪法。
> 我们现在要做任务 X.Y：[任务名]
> 请按 docs/task_breakdown.md 中该任务的描述完成：
>
> - [目标]
> - [输入]
> - [产出]
> - [测试]
>
> 约束（必须遵守）：
> - 不修改已完成模块
> - 不引入新依赖
> - 必须配套单元测试
> - 一次产出不超过 300 行代码
>
> 完成后请：
> 1. 列出新建/修改的文件
> 2. 解释关键设计决策
> 3. 跑单测验证
> 4. 提示我更新 CLAUDE.md

---

## 十五、变更记录

> 项目宪法本身的修改记录，重大决策要在这里追溯

| 日期 | 变更 | 原因 |
|---|---|---|
| 2026-05-29 | 初始版本 | 项目启动 |
| 2026-05-29 | 补 5 个高优先级任务（1.13、1.14、2.9、3.11、5.1 调整） | 完备性增强；MsgDispatcher 重新归类到 addons/ |
| 2026-05-31 | 整理为标准 markdown 格式 | 修复 Unicode 分隔符问题 |
