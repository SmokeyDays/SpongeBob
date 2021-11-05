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

#### executor

`executoin_stage.cpp` 事实上可以发现所有 _stage 的文件结构都非常类似，应该都是处理对应的记录和回溯问题

`execution_node.cpp` 不太懂，看起来是一个涉及到 table, trx, tuple 的集成式处理文件

`tuple.cpp` tuple 是元组，可以理解为一行中 field 的某个集合，或者更广泛的理解，就是一行，看起来是处理"加入关系"操作的文件。

这里涉及到 schema，翻译为模式，实际语义很广，尚未分析。

`value.cpp` .cpp 是个空文件，.h 是处理不同类型数据的大小比较

#### optimizer

`optimize_stage.cpp` 又一个 _stage (与"查询优化，调整重写语法树"相关，是使用教程中提到的需要着重做的点)

#### parser

`lex.yy.c` 词法解析

`parse_stage.cpp` 又一个 _stage

`parse.cpp` parse 翻译为词法，文件里涉及到了 SQL 的各种操作，看上去像是把用户指令翻译成数据库内部的信息，但我没搞清它在哪里被用到

`reslove_stage.cpp` 又一个 _stage (查询缓存，决赛优化项目)

`yacc_sql.tab.c` 语法解析

#### plan_cache

`plan_cache_stage.cpp` 又一个 _stage (执行计划缓存，决赛优化项目)

#### query_cache

`query_cache_stage.cpp` 又一个 _stage (查询缓存，决赛优化项目)

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
