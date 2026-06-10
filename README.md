# base64_summary
关于文本、文件的base64编码、解码

## 字符串文本

### 编码

#### 具体流程
- 创建过滤器 b64
- 创建内存 bio
- 将内存 bio 与过滤器 b64 链接
- 对 bio 进行不换行
- 将字符串文本 str 写进内存 bio
- 刷新一遍bio
- 创建内存缓冲区 buf
- 把内存 bio 写进缓冲区 buf
- 动态分配堆空间 data
- 把 buf 的内容写到 data
- 在 data 结尾加上 '\0';
- 释放 bio
- 返回堆空间 data
```c
char *encode(char *str)
{
    // 过滤器
    BIO *b64=BIO_new(BIO_f_base64());
    // 内存bio
    BIO *bio=BIO_new(BIO_s_mem());
    // 链接过滤器和内存
    bio=BIO_push(b64,bio);
    // 被bio不换行
    BIO_set_flags(bio,BIO_FLAGS_BASE64_NO_NL);

    // 把str内容写道bio
    BIO_write(bio,str,strlen(str));
    // 刷新bio
    BIO_flush(bio);

    // 内存缓冲区
    BUF_MEM *buf;
    // 把bio的内容写到buf
    BIO_get_mem_ptr(bio,&buf);

    // 动态分配堆空间
    char *data=malloc(buf->length);

    // 将buf里面的内容给到data
    memcpy(data,buf->data,buf->length);

    // 给字符串结尾加上'/0'
    data[buf->length]=0;
    BIO_free_all(bio);

    return data;
}
```

### 解码

#### 具体流程
- 创建 b64 过滤器
- 获取输入数据长度
- 创建内存 bio 并写入数据
- 将内存 bio 与过滤器 b64 链接
- 对 bio 进行不换行
- 动态分配堆空间 buf
- 从 bio 中读取解码后的数据到 buf，返回值存入 len
- 释放 bio
- 返回堆空间 buf

```c
char *decode(char *data, int *len)
{
    // b64 过滤器
    BIO *b64 = BIO_new(BIO_f_base64());
    // 获取输入数据长度
    int input_len = strlen(data);

    // 把data数据写进到bio里面
    BIO *bio = BIO_new_mem_buf(data, input_len);
    // 链接过滤器和内存
    bio = BIO_push(b64, bio);
    // bio不换行
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    char *buf = malloc(input_len);
    *len = BIO_read(bio, buf, input_len);
    BIO_free_all(bio);

    return buf;
}
```

## 文件文本

### 编码

#### 具体流程
- 打开要读取的文件（二进制模式）
- 打开要生成的文件（文本模式）
- 创建 b64 过滤器
- 将过滤器与输出文件链接
- 循环读取输入文件内容到缓冲区
- 将缓冲区内容写入输出文件（自动进行 base64 编码）
- 刷新输出文件
- 释放所有 BIO 资源

```c
void encode_file(char *in_file, char *out_file)
{
    // 打开读取的文件
    BIO *in = BIO_new(BIO_new_file(in_file, "rb"));
    // 打开要生成的文件
    BIO *out = BIO_new(BIO_new_file(out_file, "w"));
    // 过滤器 b64
    BIO *b64 = BIO_new(BIO_f_base64());
    // 过滤器连接out
    out = BIO_push(b64, out);

    char buf[1024 * 4];
    int n;
    while ((n = BIO_read(in, buf, sizeof(buf))) > 0)
    {
        BIO_write(out, buf, n);
    }
    BIO_flush(out);
    BIO_free_all(in);
    BIO_free_all(out);
    printf("ok\n");
}
```

### 解码

#### 具体流程
- 打开要读取的 base64 文件（文本模式）
- 打开要生成的文件（二进制模式）
- 创建 b64 过滤器
- 将过滤器与输入文件链接
- 循环读取输入文件内容（自动进行 base64 解码）
- 将解码后的内容写入输出文件
- 刷新输出文件
- 释放所有 BIO 资源

```c
void decode_file(char *in_file, char *out_file)
{
    BIO *in = BIO_new_file(in_file, "r");
    BIO *out = BIO_new_file(out_file, "wb");
    // 链接 b64 和 in
    in = BIO_push(b64, in);

    char buf[1024];
    int n;
    while ((n = BIO_read(in, buf, sizeof(buf))) > 0)
    {
        BIO_write(out, buf, n);
    }
    BIO_flush(out);
    BIO_free_all(in);
    BIO_free_all(out);
    printf("ok\n");
}
```