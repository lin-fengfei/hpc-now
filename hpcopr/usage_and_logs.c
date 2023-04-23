/*
 * This code is written and maintained by Zhenrong WANG (mailto: wangzhenrong@hpc-now.com) 
 * The founder of Shanghai HPC-NOW Technologies Co., Ltd (website: https://www.hpc-now.com)
 * It is distributed under the license: GNU Public License - v2.0
 * Bug report: info@hpc-now.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include "now_macros.h"
#include "general_funcs.h"
#include "usage_and_logs.h"

int get_usage(char* usage_logfile){
    char cmdline[CMDLINE_LENGTH]="";
    if(file_exist_or_not(usage_logfile)!=0){
        printf("[ FATAL: ] Failed to get the usage record. Either you haven't initialize your first\n");
        printf("|          cluster, or there are internal errors. Exit now.\n");
        return 1;
    }
#ifdef _WIN32
    system("del /f /s /q c:\\hpc-now\\cluster_usage_temp.log > nul 2>&1");
    sprintf(cmdline,"copy /y %s c:\\hpc-now\\cluster_usage_temp.log > nul 2>&1",usage_logfile);
#elif __APPLE__
    system("rm -rf /Users/hpc-now/now-cluster-usage-latest.log >> /dev/null 2>&1");
    sprintf(cmdline,"/bin/cp %s /Users/hpc-now/cluster_usage_temp.log >> /dev/null 2>&1",usage_logfile);
#elif __linux__
    system("rm -rf /home/hpc-now/now-cluster-usage-latest.log >> /dev/null 2>&1");
    sprintf(cmdline,"/bin/cp %s /home/hpc-now/cluster_usage_temp.log >> /dev/null 2>&1",usage_logfile);
#endif
    system(cmdline);
#ifdef _WIN32
    system("more c:\\hpc-now\\cluster_usage_temp.log");
#elif __APPLE__
    system("more /Users/hpc-now/cluster_usage_temp.log");
#elif __linux__
    system("more /home/hpc-now/cluster_usage_temp.log");
#endif
    printf("[ -DONE- ] The latest usage summary has been printed to the file below:\n");
#ifdef _WIN32
    printf("|          c:\\hpc-now\\cluster_usage_temp.log\n");
    printf("|          You can use either MS Office Excel (*strongly recommended*) or other\n");
#elif __APPLE__
    printf("|          /Users/hpc-now/cluster_usage_temp.log\n");
    printf("|          You can use either any CSV file processing tools (i.e. LibreOffice) or\n");
#elif __linux__
    printf("|          /home/hpc-now/cluster_usage_temp.log\n");
    printf("|          You can use either any CSV file processing tools (i.e. LibreOffice) or\n");
#endif
    printf("|          plain text editors (for example, notepad) to view the usage details.\n");
    return 0;
}

int get_syslog(char* operation_logfile){
    char cmdline[CMDLINE_LENGTH]="";
    if(file_exist_or_not(operation_logfile)!=0){
        printf("[ FATAL: ] Failed to get the operation log. Exit now.\n");      
        return 1;
    }
#ifdef _WIN32
    system("del /f /s /q c:\\hpc-now\\cluster_syslog_temp.log > nul 2>&1");
    sprintf(cmdline,"copy /y %s c:\\hpc-now\\cluster_syslog_temp.log > nul 2>&1",operation_logfile);
#elif __APPLE__
    system("rm -rf /Users/hpc-now/now-cluster-operation-latest.log >> /dev/null 2>&1");
    sprintf(cmdline,"/bin/cp %s /Users/hpc-now/cluster_syslog_temp.log >> /dev/null 2>&1",operation_logfile);
#elif __linux__
    system("rm -rf /home/hpc-now/now-cluster-operation-latest.log >> /dev/null 2>&1");
    sprintf(cmdline,"/bin/cp %s /home/hpc-now/cluster_syslog_temp.log >> /dev/null 2>&1",operation_logfile);
#endif
    system(cmdline);
#ifdef _WIN32
    system("more c:\\hpc-now\\cluster_syslog_temp.log");
#elif __APPLE__
    system("more /Users/hpc-now/cluster_syslog_temp.log");
#elif __linux__
    system("more /home/hpc-now/cluster_syslog_temp.log");
#endif
    printf("[ -DONE- ] The latet operation log has been printed to the file below:\n");
#ifdef _WIN32
    printf("|          c:\\hpc-now\\cluster_syslog_temp.log\n");
    printf("|          You can use either MS Office Excel (*strongly recommended*) or other\n");
#elif __APPLE__
    printf("|          /Users/hpc-now/cluster_syslog_temp.log\n");
    printf("|          You can use either any CSV file processing tools (i.e. LibreOffice) or\n");
#elif __linux__
    printf("|          /home/hpc-now/cluster_syslog_temp.log\n");
    printf("|          You can use either any CSV file processing tools (i.e. LibreOffice) or\n");
#endif
    printf("|          plain text editors (for example, notepad) to view the detailed log.\n");
    return 0;
}

int system_cleanup(void){
#ifdef _WIN32
    FILE* file_p=NULL;
    char cmdline[CMDLINE_LENGTH]="";
    char appdata_dir[DIR_LENGTH]="";
    system("echo %APPDATA% > c:\\programdata\\appdata.txt.tmp");
    file_p=fopen("c:\\programdata\\appdata.txt.tmp","r");
    fscanf(file_p,"%s",appdata_dir);
    fclose(file_p);
    system("del /f /s /q c:\\programdata\\appdata.txt.tmp > nul 2>&1");
    
    sprintf(cmdline,"del /f /s /q %s\\Microsoft\\Windows\\Recent\\* > nul 2>&1",appdata_dir);
    system(cmdline);
    sprintf(cmdline,"rd /q /s %s\\Microsoft\\Windows\\Recent\\ > nul 2>&1",appdata_dir);
    system(cmdline);
    return 0;
#else
    //Keep it here for further use. 
    return 0;
#endif
}

int write_log(char* workdir, char* operation_logfile, char* operation, int runflag){
    time_t current_time_long;
    struct tm* time_p=NULL;
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    FILE* file_p=fopen(operation_logfile,"a+");
    if(file_p==NULL){
        printf("[ -WARN- ] Failed to write operation log to the records. The cluster operation may\n");
        printf("|          not be affected, but will not be recorded to your system.\n");
        return -1;
    }
    fprintf(file_p,"%d-%d-%d,%d:%d:%d,%s,%s,%d\n",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday,time_p->tm_hour,time_p->tm_min,time_p->tm_sec,workdir,operation,runflag);
    fclose(file_p);
    return 0;
}