# php-donie
A sample extension for PHP5.

## 生成擴展項目框架

```bash
# 下載PHP源碼
git clone git://github.com/php/php-src.git

# 生成擴展項目框架
cd php-src/ext
./ext_skel --extname=donie
```

去掉config.m4裡PHP_ARG_WITH或者PHP_ARG_ENABLE的註釋。

## 編譯

```bash
# 擴展源碼目錄中

phpize

./configure

make

make install
```

