/*
 * This code is written and maintained by Zhenrong WANG
 * mailto: zhenrongwang@live.com (*preferred*) | wangzhenrong@hpc-now.com
 * The founder of Shanghai HPC-NOW Technologies Co., Ltd (website: https://www.hpc-now.com)
 * This code is distributed under the license: GNU Public License - v2.0
 * Bug report: info@hpc-now.com
 */

#ifndef TRANSFER_H
#define TRANSFER_H

int export_cluster(char* cluster_name, char* user_name, char* trans_key);
int import_cluster(char* cluster_name, char* trans_key);

#endif