## observer

### event

`execution_plan_event.cpp` `session_event.cpp` `sql_event.cpp`  `storage_event.cpp` 都是与 SEDA 相关的东西，不是很懂

### handler

`handler.h` 标识了 storage/default/default_handler 要实现的接口，也就是我们任务应该实现的东西 (也许?)

### net

`server.cpp` 与 client 连接，接收和发送数据

### session

`session_stage.cpp` SEDA 相关，看上去是记录当前事件的执行阶段

有一段注释也许对理解其他地方有用：

```
/**
 * seda::stage使用说明：
 * 这里利用seda的线程池与调度。stage是一个事件处理的几个阶段。
 * 目前包括session,parse,execution和storage
 * 每个stage使用handleEvent函数处理任务，并且使用StageEvent::pushCallback注册回调函数。
 * 这时当调用StageEvent::done(Immediate)时，就会调用该事件注册的回调函数。
 */
```

`session.cpp` 传统意义上的 session，记录对应 server 当前操作的库以及当前执行的事务之类，不得不记录的信息

### sql



### storage

#### common

`blpus_tree_index.cpp` `bplus_tree.cpp` B+ 树

`condition_fliter.cpp` condition 在这里翻译为"条件"，主要是对输入和实际数据类型的比较判定

在 init 函数中有注释

```
NOTE：这里没有实现不同类型的数据比较，比如整数跟浮点数之间的对比
  // 但是选手们还是要实现。这个功能在预选赛中会出现
```

`db.cpp` 在数据库的角度执行对 table 的处理

`field_meta.cpp` `index_manager.cpp` `record_manager.cpp` `table.cpp` 这几个放在一起比较，field 是属性，也就是一列，index 是索引 (句柄?)，record 是记录，也就是一行，table 是表，这些文件是对他们内部的操作或者对其中结构的辅助定义

#### default

`default_handler.cpp ` 与 handler/handler.h 对应

`default_storage_stage.cpp` (也许) 与 mem/mem_storage_stage.cpp  有一点相关，看起来是数据的储存

`disk_buffer_pool.cpp` 直译为 "硬盘缓冲池"，我不是很懂()

#### mem

`mem_storage_stage.cpp` SEDA 相关，出现了在 /session/session_stage.cpp 的注释中提到的函数 handle_event, 似乎是对一个 stage 的记录和回溯

#### trx

`trx.cpp` 事务记录

### ...

`init.cpp` 初始化日志，SEDA 以及各种(与任务无关的)配置

`main.cpp` 接收命令行运行参数，以及线程处理相关

`rc` 错误类型打印
