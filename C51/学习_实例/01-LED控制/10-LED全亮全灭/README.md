# LED全亮全灭

## 功能说明

用两种不同的延时函数控制LED闪烁节奏，展示快慢对比。

## 硬件接线

| 引脚 | 连接 |
|:---|:---|
| P1.0 ~ P1.7 | 8个LED（低电平点亮） |

## 代码要点

```c
void DelayLong(void)   { /* 约500ms */ }
void DelayShort(void)  { /* 约100ms */ }

// 慢闪3次 → 快闪3次 → 全亮全灭各3次
```

## 踩坑记录

- `DelayLong` 用 `unsigned int` 循环，延时更长
- `DelayShort` 用嵌套 `unsigned char` 循环，延时较短
- 两种延时函数可复用到其他项目
