#ifndef GETINFO_H
#define GETINFO_H

typedef struct MEMPACKED //定义一个mem occupy的结构体
{
    char name1[20]; //定义一个char类型的数组名name有20个元素
    unsigned long MemTotal;
    char name2[20];
    unsigned long MemFree;
    char name3[20];
    unsigned long Buffers;
    char name4[20];
    unsigned long Cached;
    char name5[20];
    unsigned long SwapCached;
} MEM_OCCUPY;

//proc/stat文件结构
//cpu  633666 46912 249878 176813696 782884 2859 19625 0
//cpu0 633666 46912 249878 176813696 782884 2859 19625 0
//intr 5812844
//ctxt 265816063
//btime 1455203832
//processes 596625
//procs_running 1
//procs_blocked 0

typedef struct CPUPACKED //定义一个cpu occupy的结构体
{
    char name[20];       //定义一个char类型的数组名name有20个元素
    unsigned int user;   //定义一个无符号的int类型的user
    unsigned int nice;   //定义一个无符号的int类型的nice
    unsigned int system; //定义一个无符号的int类型的system
    unsigned int idle;   //定义一个无符号的int类型的idle
    unsigned int lowait;
    unsigned int irq;
    unsigned int softirq;
} CPU_OCCUPY;

void get_memoccupy(MEM_OCCUPY *mem); //对无类型get函数含有一个形参结构体类弄的指针O

int get_cpuoccupy(CPU_OCCUPY *cpust); //对无类型get函数含有一个形参结构体类弄的指针O

double cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n);

#endif // GETINFO_H