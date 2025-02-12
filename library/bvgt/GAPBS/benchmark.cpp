//
// Created by sjf on 8/24/2022.
//
#include "benchmark.h"

// bool VerifyUnimplemented(...) {
//     std::cout << "** verify unimplemented **" << std::endl;
//     return false;
// }

// void write_array_to_file(const char *filename, long int *array, size_t array_size) {
//     // 打开文件，以追加模式
//     FILE *file = fopen(filename, "a");
//     if (file == NULL) {
//         perror("无法打开文件");
//         return;
//     }

//     // 遍历数组
//     for (size_t i = 0; i < array_size; ++i) {
//         // 将数字格式化为字符串，并添加换行符
//         fprintf(file, "%ld\n", array[i]);
//     }

//     // 关闭文件
//     fclose(file);
// }