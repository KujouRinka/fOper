# 配置

这个文件用于描述 `task-type: copy-file` 时的配置文件格式

## 行为

复制 `source` 到 `target`

## 目标与源文件

```yaml
target: ./data/dir
source: ./action
```

用于描述目标与源文件的位置，可以使文件或文件夹

## 排除文件

```yaml
exclude:
  - ./data/src_dir/exclude1
  - ./data/src_dir/exclude2
```

排除掉不需要复制的文件

## 文件权限信息

```yaml
permission:
  action: [default|mod|copy]
  # action: mod only
  owner: root #(option)
  group: user #(option)
  mode: r--rw---- #(option)
```

`action` 为必填字段，描述复制时的行为

- `default` 系统默认
- `mod` 进行修改
- `copy` 保持与源文件一致

`owner`，`group`，`mode` 为 `mod` 下的可选字段，用于修改文件权限信息，格式与 chown，chgrp，chmod 一致

## 文件属性

```yaml
attr:
  action: [default|mod|copy]
  # action: mode only
  operation: [+ia+A]|[=iaAe]
```

`action` 必填，含义同上，`operation` 为操作：

- `+` 为文件添加属性
- `-` 为文件移除属性
- `=` 设置文件属性

格式与 chattr 调用一致

注意，`+` 与 `-` 可出现在同一 `operation` 中，但不能与 `=` 同时存在

## 文件拓展属性

```yaml
xattr:
  action: [default|mod|copy]
  # action: mod only
  operation:
    - verb: set
      name: user.foo
      value: bar
    - verb: del
      name: user.114514
```

`action` 必填，含义同上，`operation` 为包含一组操作的列表，其中每个元素为一个表示单个操作的字典：

- `verb` 必填，有 `set` 和 `del` 两个操作
- `name` 操作的拓展属性名称
- `value` 仅 `set` 有效，拓展属性的值

与 setfattr 行为一致

## ACL

```yaml
acl:
  action: [default|mod|copy]
  # action: mod only
  operation: "u::rwx,u:rinka:r--,g::---,o::---,m::rwx"
```

`operation` 与 setfacl 一致

## 时间

```yaml
time:
  action: [default|mod|copy]
  # action: mod only
  fmt: "%Y-%m-%d %H:%M:%S" #(option)
  mtime: 2000-01-01 00:00:00
  atime: 2020-12-25 11:45:14
```

修改文件的时间属性：

- `fmt` 时间格式，可选，默认为 `%Y%m%d %H%M%S`
- `mtime` 文件的修改时间
- `atime` 文件的访问时间

# 注意事项！！

修改时间 `time` 的操作在修改文件属性 `attr` 的操作之后，请务必保证所修改的文件属性支持修改时间（例如为文件属性添加 `i` flag 后无法修改文件时间），否则修改不会生效。