# 配置

## 语法

使用 `yaml` 作为配置文件格式，`yaml` 大小写敏感，使用缩进表示层级关系

## 全局配置

### 生成的目标文件

必须规定生成的目标文件名称

```yaml
gen-file: ./dir/myfile.bin
```

生成目标文件的指定目录和文件名

### 需要依赖的外部文件

其他字段中需要使用的外部文件的列表

```yaml
external-file:
  - ./dir/file1
  - ./dir/file2
```

### 创建文件的方法

规定使用何种方式创建文件

```yaml
create-method: default
```

目前只支持 `default`，后续可拓展

### 对文件的操作

一个列表，列表中每个元素代表对单个文件进行一次何种操作

```yaml
operation:
  OperationObj
  OperationObj
  ...
```

#### OperationObj

规定了如何对文件进行操作，通常必须包含一个 `action` 字段，表示这是何种类型的操作，后续几个该操作的参数

```yaml
- action: some_action
  par1: par1
  par2: par2
  ...
```

##### action: syscall

系统调用

###### sys_write

```yaml
# string data
- action: syscall
  call: sys_write
  data: helloworld
  len: 10

# binary data
- action: syscall
  call: sys_write
  data: "\x68\x65\x6c\x6c\x6f\x77\x6f\x72\x6c\x64"
  len: 10
```

##### action: file

自定义的写操作

###### 从其他文件复制

```yaml
- action: write
  verb: file
  filename: ./dir/file1
  offset: 233
  len: 1024
```

###### 复制随机字节

```yaml
- action: write
  verb: random
  len: 1024
```





