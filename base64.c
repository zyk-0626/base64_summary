#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

// 字符串文本编码
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
int main(int argc, char const *argv[])
{
    // 字符串文本
    char *str="hello";
    char *base64=encode(str);
    printf("%s\n",base64);
    return 0;
}
