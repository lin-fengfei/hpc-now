/*
 * Copyright (C) 2022-present Shanghai HPC-NOW Technologies Co., Ltd.
 * This code is distributed under the license: MIT License
 * Originally written by Zhenrong WANG
 * mailto: zhenrongwang@live.com | wangzhenrong@hpc-now.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include "now_macros.h"
#include "cluster_general_funcs.h"
#include "general_funcs.h"
#include "general_print_info.h"
#include "cluster_init.h"
#include "components.h"

extern char url_code_root_var[LOCATION_LENGTH];
extern char url_shell_scripts_var[LOCATION_LENGTH];
extern char url_initutils_root_var[LOCATION_LENGTH];
extern char url_app_pkgs_root_var[LOCATION_LENGTH];
extern char url_app_inst_root_var[LOCATION_LENGTH];
extern char az_environment[16];
extern int code_loc_flag_var;

/*
 * 
 *
 */
int cluster_init_conf(char* cluster_name, int argc, char* argv[]){
//    char* region_id, char* zone_id, char* node_num, char* hpc_user_num, char* master_inst, char* compute_inst, char* os_image, char* ht_flag
    char cloud_flag[16]="";
    char workdir[DIR_LENGTH]="";
    get_workdir(workdir,cluster_name);
    get_cloud_flag(workdir,cloud_flag);
    if(strcmp(cloud_flag,"CLOUD_A")!=0&&strcmp(cloud_flag,"CLOUD_B")!=0&&strcmp(cloud_flag,"CLOUD_C")!=0&&strcmp(cloud_flag,"CLOUD_D")!=0&&strcmp(cloud_flag,"CLOUD_E")!=0&&strcmp(cloud_flag,"CLOUD_F")!=0&&strcmp(cloud_flag,"CLOUD_G")!=0){
        return -5;
    }
    char tf_prep_conf[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    sprintf(cmdline,"%s %s%sconf %s",MKDIR_CMD,workdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    sprintf(tf_prep_conf,"%s%sconf%stf_prep.conf",workdir,PATH_SLASH,PATH_SLASH);
    if(file_exist_or_not(tf_prep_conf)==0){
        if(cmd_flag_check(argc,argv,"--force")!=0){
            return -3; // If the conf file already exists, exit, unless force specified.
        }
    }
    FILE* file_p=fopen(tf_prep_conf,"w+");
    if(file_p==NULL){
        return -1;
    }
    char default_region[32]="";
    char real_region[128]="";
    char default_zone[36]="";
    char real_zone[128]="";
    int default_node_num=1;
    char real_node_num_string[128]="";
    int default_user_num=3;
    char real_user_num_string[128]="";
    char* default_master_inst="a8c16g";
    char* default_master_inst_hwcloud="i8c16g";
    char* default_master_inst_azure="i4c8g";
    char* default_master_inst_gcp="a4c8g";
    char real_master_inst[128]="";
    char* default_compute_inst="a4c8g";
    char* default_compute_inst_hwcloud="i4c8g";
    char* default_compute_inst_azure="i4c8g";
    char* default_compute_inst_gcp="a4c8g";
    char real_compute_inst[128]="";
    char* default_os_image="centoss9";
    char* default_os_image_hwcloud="rocky9";
    char real_os_image[128]="";
    char* default_ht_flag="ON";
    char real_ht_flag[128]="";
    char real_nfs_volume[128]="";
    char app_inst_script_url_specified[LOCATION_LENGTH]="";
    char app_inst_pkgs_url_specified[LOCATION_LENGTH]="";
    int sum_temp;

    cmd_keyword_check(argc,argv,"--rg",real_region);
    cmd_keyword_check(argc,argv,"--az",real_zone);
    cmd_keyword_check(argc,argv,"--nn",real_node_num_string);
    cmd_keyword_check(argc,argv,"--un",real_user_num_string);
    cmd_keyword_check(argc,argv,"--mi",real_master_inst);
    cmd_keyword_check(argc,argv,"--ci",real_compute_inst);
    cmd_keyword_check(argc,argv,"--os",real_os_image);
    cmd_keyword_check(argc,argv,"--ht",real_ht_flag);
    cmd_keyword_check(argc,argv,"--vol",real_nfs_volume);
    cmd_keyword_check(argc,argv,"--inst",app_inst_script_url_specified);
    cmd_keyword_check(argc,argv,"--repo",app_inst_pkgs_url_specified);

    if(strlen(real_node_num_string)!=0){
        sum_temp=string_to_positive_num(real_node_num_string);
        if(sum_temp<MINIMUM_ADD_NODE_NUMBER||sum_temp>MAXIMUM_ADD_NODE_NUMBER){
            fclose(file_p);
            sprintf(cmdline,"%s %s %s",DELETE_FILE_CMD,tf_prep_conf,SYSTEM_CMD_REDIRECT);
            system(cmdline);
            return 1;
        }
    }

    if(strlen(real_user_num_string)!=0){
        sum_temp=string_to_positive_num(real_user_num_string);
        if(sum_temp<MINIMUM_ADD_USER_NUNMBER||sum_temp>MAXIMUM_ADD_USER_NUMBER){
            fclose(file_p);
            sprintf(cmdline,"%s %s %s",DELETE_FILE_CMD,tf_prep_conf,SYSTEM_CMD_REDIRECT);
            system(cmdline);
            return 1;
        }
    }
    
    if(strcmp(real_ht_flag,"ON")!=0&&strcmp(real_ht_flag,"OFF")!=0){
        strcpy(real_ht_flag,"");
    }
    if(strcmp(cloud_flag,"CLOUD_A")==0){
        strcpy(default_region,"cn-hangzhou");
        strcpy(default_zone,"cn-hangzhou-j");
    }
    else if(strcmp(cloud_flag,"CLOUD_B")==0){
        strcpy(default_region,"ap-guangzhou");
        strcpy(default_zone,"ap-guangzhou-6");
    }
    else if(strcmp(cloud_flag,"CLOUD_C")==0){
        strcpy(default_region,"cn-northwest-1");
        strcpy(default_zone,"cn-northwest-1a");
    }
    else if(strcmp(cloud_flag,"CLOUD_D")==0){
        strcpy(default_region,"cn-north-4");
        strcpy(default_zone,"cn-north-4a");
    }
    else if(strcmp(cloud_flag,"CLOUD_E")==0){
        strcpy(default_region,"bj");
        strcpy(default_zone,"cn-bj-a");
    }
    else if(strcmp(cloud_flag,"CLOUD_F")==0){
        strcpy(default_region,"az.Japan-East"); //Azure
        strcpy(default_zone,"*NULL*");
    }
    else{
        strcpy(default_region,"us-central1");
        strcpy(default_zone,"us-central1-a");
    }

    if(strlen(real_region)==0){
        strcpy(real_region,default_region);
    }
    if(strlen(real_zone)==0){
        strcpy(real_zone,default_zone);
    }
    if(strlen(real_node_num_string)==0){
        sprintf(real_node_num_string,"%d",default_node_num);
    }
    if(strlen(real_user_num_string)==0){
        sprintf(real_user_num_string,"%d",default_user_num);
    }
    if(strlen(real_master_inst)==0){
        if(strcmp(cloud_flag,"CLOUD_D")!=0&&strcmp(cloud_flag,"CLOUD_F")!=0&&strcmp(cloud_flag,"CLOUD_G")!=0){
            strcpy(real_master_inst,default_master_inst);
        }
        else if(strcmp(cloud_flag,"CLOUD_D")==0){
            strcpy(real_master_inst,default_master_inst_hwcloud);
        }
        else if(strcmp(cloud_flag,"CLOUD_F")==0){
            strcpy(real_master_inst,default_master_inst_azure);
        }
        else{
            strcpy(real_master_inst,default_master_inst_gcp);
        }
    }
    if(strlen(real_compute_inst)==0){
        if(strcmp(cloud_flag,"CLOUD_D")!=0&&strcmp(cloud_flag,"CLOUD_F")!=0&&strcmp(cloud_flag,"CLOUD_G")!=0){
            strcpy(real_compute_inst,default_compute_inst);
        }
        else if(strcmp(cloud_flag,"CLOUD_D")==0){
            strcpy(real_compute_inst,default_compute_inst_hwcloud);
        }
        else if(strcmp(cloud_flag,"CLOUD_F")==0){
            strcpy(real_compute_inst,default_compute_inst_azure);
        }
        else{
            strcpy(real_compute_inst,default_compute_inst_gcp);
        }
    }
    if(strlen(real_os_image)==0){
        if(strcmp(cloud_flag,"CLOUD_D")!=0&&strcmp(cloud_flag,"CLOUD_F")!=0){
            strcpy(real_os_image,default_os_image);
        }
        else if(strcmp(cloud_flag,"CLOUD_D")==0){
            strcpy(real_os_image,default_os_image_hwcloud);
        }
        else{
            strcpy(real_os_image,"*Oracle_Linux_9.2*");
        }
    }
    if(strlen(real_ht_flag)==0){
        strcpy(real_ht_flag,default_ht_flag);
    }
    if(strlen(app_inst_script_url_specified)>0&&valid_loc_format_or_not(app_inst_script_url_specified)==0){
        strncpy(url_app_inst_root_var,app_inst_script_url_specified,LOCATION_LENGTH);
    }
    if(strlen(app_inst_pkgs_url_specified)>0&&valid_loc_format_or_not(app_inst_pkgs_url_specified)==0){
        strncpy(url_app_pkgs_root_var,app_inst_pkgs_url_specified,LOCATION_LENGTH);
    }

    if(strcmp(cloud_flag,"CLOUD_D")==0||strcmp(cloud_flag,"CLOUD_F")==0||strcmp(cloud_flag,"CLOUD_G")==0){
        sum_temp=string_to_positive_num(real_nfs_volume);
        if(sum_temp==0||sum_temp<0){
            if(strcmp(cloud_flag,"CLOUD_G")!=0){
                strcpy(real_nfs_volume,"300");
            }
            else{
                strcpy(real_nfs_volume,"100");
            }
        }
    }
    fprintf(file_p,"#IMPORTANT: THERE *MUST* BE 22 CHARACTERs (including spaces) BEFORE THE VALUE OF PARAMETERS.\n");
    fprintf(file_p,"#FOR EXAMPLE        : PARAMETER_VALUE (*WITHOUT* ANY SPACE OR OTHER CHARACTORS AFTER THE PARAMETER_VALUE!)\n");
    fprintf(file_p,"#                   : |<---THIS IS THE STANDARD START POINT! YOU NEED TO *ABSOLUTELY* ALIGN WITH THIS LINE.\n");
    fprintf(file_p,"CLUSTER_ID          : %s\n",cluster_name);
    fprintf(file_p,"REGION_ID           : %s\n",real_region);
    fprintf(file_p,"ZONE_ID             : %s\n",real_zone);
    fprintf(file_p,"NODE_NUM            : %s\n",real_node_num_string);
    fprintf(file_p,"HPC_USER_NUM        : %s\n",real_user_num_string);
    fprintf(file_p,"master_init_param   : db skip\n");
    fprintf(file_p,"master_passwd       : *AUTOGEN*\n");
    fprintf(file_p,"compute_passwd      : *AUTOGEN*\n");
    fprintf(file_p,"master_inst         : %s\n",real_master_inst);
    if(strcmp(cloud_flag,"CLOUD_C")!=0&&strcmp(cloud_flag,"CLOUD_F")!=0&&strcmp(cloud_flag,"CLOUD_G")!=0){
        fprintf(file_p,"master_bandwidth    : 50\n");
    }
    fprintf(file_p,"compute_inst        : %s\n",real_compute_inst);
    fprintf(file_p,"os_image            : %s\n",real_os_image);
    if(strcmp(cloud_flag,"CLOUD_C")==0){
        fprintf(file_p,"hyperthreading      : %s\n",real_ht_flag);
    }
    if(strcmp(cloud_flag,"CLOUD_D")==0||strcmp(cloud_flag,"CLOUD_F")==0||strcmp(cloud_flag,"CLOUD_G")==0){
        fprintf(file_p,"nfs_volume          : %s\n",real_nfs_volume);
    }
    fclose(file_p);
    return 0;
}

int get_tf_prep_conf(char* conf_file, char* reconf_list, char* cluster_id, char* region_id, char* zone_id, int* node_num, int* hpc_user_num, char* master_init_param, char* master_passwd, char* compute_passwd, char* master_inst, char* master_bandwidth, char* compute_inst, char* os_image_raw, char* ht_flag, int* nfs_volume){
    if(file_exist_or_not(conf_file)!=0){
        return -3; // If the conf file doesn't exist, exit.
    }
    FILE* file_p=fopen(conf_file,"r");
    char conf_line_buffer[256]="";
    char header[64]="";
    char tail[128]="";
    char tail_ext[144]="";
    char node_inst_ext[144]="";
    int read_conf_lines=0;
    int sum_temp=0;
    int i;
    while(!feof(file_p)){
        fgetline(file_p,conf_line_buffer);
        get_seq_string(conf_line_buffer,' ',1,header);
        get_seq_string(conf_line_buffer,' ',3,tail);
        if(strcmp(header,"CLUSTER_ID")==0){
            strcpy(cluster_id,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"REGION_ID")==0){
            if(*(tail+0)=='a'&&*(tail+1)=='z'&&*(tail+2)=='.'){
                for(i=3;i<strlen(tail);i++){
                    if(*(tail+i)=='-'){
                        *(region_id+i-3)=' ';
                    }
                    else{
                        *(region_id+i-3)=*(tail+i);
                    }
                }
                if(contain_or_not(tail,"China")==0){
                    strcpy(az_environment,"china");
                }
                else{
                    strcpy(az_environment,"public");
                }
            }
            else{
                strcpy(region_id,tail);
            }
            read_conf_lines++;
        }
        else if(strcmp(header,"ZONE_ID")==0){
            strcpy(zone_id,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"NODE_NUM")==0){
            sum_temp=string_to_positive_num(tail);
            if(sum_temp<MINIMUM_ADD_NODE_NUMBER||sum_temp>MAXIMUM_ADD_NODE_NUMBER){
                fclose(file_p);
                return 1;
            }
            *node_num=sum_temp;
            read_conf_lines++;
        }
        else if(strcmp(header,"HPC_USER_NUM")==0){
            sum_temp=string_to_positive_num(tail);
            if(sum_temp<MINIMUM_ADD_USER_NUNMBER||sum_temp>MAXIMUM_ADD_USER_NUMBER){
                fclose(file_p);
                return 1;
            }
            *hpc_user_num=sum_temp;
            read_conf_lines++;
        }
        else if(strcmp(header,"master_init_param")==0){
            get_seq_string(conf_line_buffer,' ',4,tail_ext);
            sprintf(master_init_param,"%s %s",tail,tail_ext);
            read_conf_lines++;
        }
        else if(strcmp(header,"master_passwd")==0){
            strcpy(master_passwd,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"compute_passwd")==0){
            strcpy(compute_passwd,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"master_inst")==0){
            sprintf(node_inst_ext," %s ",tail);
            if(find_multi_keys(reconf_list,node_inst_ext,"","","","")<1){
                return 2;
            }
            strcpy(master_inst,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"master_bandwidth")==0){
            sum_temp=string_to_positive_num(tail);
            if(sum_temp>50||sum_temp<0){
                strcpy(master_bandwidth,"50");
            }
            else{
                strcpy(master_bandwidth,tail);
            }
            read_conf_lines++;
        }
        else if(strcmp(header,"compute_inst")==0){
            sprintf(node_inst_ext," %s ",tail);
            if(find_multi_keys(reconf_list,node_inst_ext,"","","","")<1){
                return 2;
            }
            strcpy(compute_inst,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"os_image")==0){
            strcpy(os_image_raw,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"hyperthreading")==0){
            strcpy(ht_flag,tail);
            read_conf_lines++;
        }
        else if(strcmp(header,"nfs_volume")==0){
            sum_temp=string_to_positive_num(tail);
            if(sum_temp<1||sum_temp>32000){
                fclose(file_p);
                return 1;
            }
            *nfs_volume=sum_temp;
            read_conf_lines++;
        }
        else{
            continue;
        }
    }
    fclose(file_p);
    if(read_conf_lines<CONF_LINE_NUM){
        return 3;
    }
    else{
        return 0;
    }
}

int save_bucket_info(char* bucket_id, char* region_id, char* bucket_ak, char* bucket_sk, char* az_subscription_id, char* az_tenant_id, char* bucket_info_file, char* cloud_flag){
    FILE* file_p=fopen(bucket_info_file,"w+");
    if(file_p==NULL){
        return -1;
    }
    if(strcmp(cloud_flag,"CLOUD_A")!=0&&strcmp(cloud_flag,"CLOUD_B")!=0&&strcmp(cloud_flag,"CLOUD_C")!=0&&strcmp(cloud_flag,"CLOUD_D")!=0&&strcmp(cloud_flag,"CLOUD_E")!=0&&strcmp(cloud_flag,"CLOUD_F")!=0&&strcmp(cloud_flag,"CLOUD_G")!=0){
        fclose(file_p);
        return -3;
    }
    if(strcmp(cloud_flag,"CLOUD_A")==0){
        fprintf(file_p,"BUCKET: oss://%s\nREGION: \"%s\"\nBUCKET_AK: %s\nBUCKET_SK: %s\n",bucket_id,region_id,bucket_ak,bucket_sk);
    }
    else if(strcmp(cloud_flag,"CLOUD_B")==0){
        fprintf(file_p,"BUCKET: cos://%s\nREGION: \"%s\"\nBUCKET_AK: %s\nBUCKET_SK: %s\n",bucket_id,region_id,bucket_ak,bucket_sk);
    }
    else if(strcmp(cloud_flag,"CLOUD_C")==0){
        fprintf(file_p,"BUCKET: s3://%s\nREGION: \"%s\"\nBUCKET_AK: %s\nBUCKET_SK: %s\n",bucket_id,region_id,bucket_ak,bucket_sk);
    }
    else if(strcmp(cloud_flag,"CLOUD_D")==0){
        fprintf(file_p,"BUCKET: obs://%s\nREGION: \"%s\"\nBUCKET_AK: %s\nBUCKET_SK: %s\n",bucket_id,region_id,bucket_ak,bucket_sk);
    }
    else if(strcmp(cloud_flag,"CLOUD_E")==0){
        fprintf(file_p,"BUCKET: bos://%s\nREGION: \"%s\"\nBUCKET_AK: %s\nBUCKET_SK: %s\n",bucket_id,region_id,bucket_ak,bucket_sk);
    }
    else if(strcmp(cloud_flag,"CLOUD_F")==0){
        fprintf(file_p,"BUCKET: %s\nREGION: \"%s\"\nBUCKET_AK: %s\nBUCKET_SK: %s\nAZ_SUBSCRIPTION_ID: %s\nAZ_TENANT_ID: %s\n",bucket_id,region_id,bucket_ak,bucket_sk,az_subscription_id,az_tenant_id);
    }
    else{
        fprintf(file_p,"BUCKET: gs://%s\nREGION: \"%s\"\nBUCKET_LINK: %s\n",bucket_id,region_id,bucket_ak);
    }
    fclose(file_p);
    return 0;
}

void node_user_num_fix(int* node_num, int* hpc_user_num){
    if(*node_num>MAXIMUM_ADD_NODE_NUMBER){
        printf(WARN_YELLO_BOLD "[ -WARN- ] The number of compute nodes %d exceeds the maximum value %d, reset to %d.\n" RESET_DISPLAY,*node_num, MAXIMUM_ADD_NODE_NUMBER,MAXIMUM_ADD_NODE_NUMBER);
        *node_num=MAXIMUM_ADD_NODE_NUMBER;
    }
    else if(*node_num<MINIMUM_ADD_NODE_NUMBER){
        printf(WARN_YELLO_BOLD "[ -WARN- ] The number of compute nodes %d is less than %d, reset to %d.\n" RESET_DISPLAY,*node_num,MINIMUM_ADD_USER_NUNMBER,MINIMUM_ADD_NODE_NUMBER);
        *node_num=MINIMUM_ADD_NODE_NUMBER;
    }
    if(*hpc_user_num>MAXIMUM_ADD_USER_NUMBER){
        printf(WARN_YELLO_BOLD "[ -WARN- ] The number of HPC users %d exceeds the maximum value %d, reset to %d.\n" RESET_DISPLAY,*hpc_user_num,MAXIMUM_ADD_USER_NUMBER,MAXIMUM_ADD_USER_NUMBER);
        *hpc_user_num=MAXIMUM_ADD_USER_NUMBER;
    }
    else if(*hpc_user_num<MINIMUM_ADD_USER_NUNMBER){
        printf(WARN_YELLO_BOLD "[ -WARN- ] The number of HPC users %d is less than %d, reset to %d.\n" RESET_DISPLAY,*hpc_user_num,MINIMUM_ADD_USER_NUNMBER,MINIMUM_ADD_USER_NUNMBER);
        *hpc_user_num=MINIMUM_ADD_USER_NUNMBER;
    }
}

void clear_if_failed(char* stackdir, char* confdir, char* vaultdir, int condition_flag){
    char cmdline[CMDLINE_LENGTH]="";
    if(condition_flag==1){
        sprintf(cmdline,"%s %s%s* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    else if(condition_flag==2){
        sprintf(cmdline,"%s %s%s* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(cmdline,"%s %s%stf_prep.conf %s%stf_prep.conf.failed %s",MOVE_FILE_CMD,confdir,PATH_SLASH,confdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    else{
        sprintf(cmdline,"%s %s%s* %s",DELETE_FILE_CMD,DESTROYED_DIR,PATH_SLASH,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(cmdline,"%s %s%s*.tmp %s %s",MOVE_FILE_CMD,stackdir,PATH_SLASH,DESTROYED_DIR,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(cmdline,"%s %s%s*.tf %s %s",MOVE_FILE_CMD,stackdir,PATH_SLASH,DESTROYED_DIR,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(cmdline,"%s %s%sUCID_LATEST.txt %s %s",MOVE_FILE_CMD,vaultdir,PATH_SLASH,DESTROYED_DIR,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(cmdline,"%s %s%stf_prep.conf %s%stf_prep.conf.destroyed %s",MOVE_FILE_CMD,confdir,PATH_SLASH,confdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
}

void generate_tf_files(char* stackdir){
    char cmdline[CMDLINE_LENGTH]="";
    sprintf(cmdline,"%s %s%shpc_stack.base %s%shpc_stack_base.tf %s",MOVE_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    sprintf(cmdline,"%s %s%shpc_stack.database %s%shpc_stack_database.tf %s",MOVE_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    sprintf(cmdline,"%s %s%shpc_stack.master %s%shpc_stack_master.tf %s",MOVE_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    sprintf(cmdline,"%s %s%shpc_stack.natgw %s%shpc_stack_natgw.tf %s",MOVE_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    sprintf(cmdline,"%s %s%shpc_stack.compute %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
}

int aws_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char secret_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_aws_root[LOCATION_LENGTH_EXTENDED]="";
    char access_key[AKSK_LENGTH]="";
    char secret_key[AKSK_LENGTH]="";
    char cloud_flag[16]="";
    int read_conf_flag=0;
    char conf_param_buffer[32]="";
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    char region_flag[16]="";
    char os_image[128]="";
    char db_os_image[64]="";
    char nat_os_image[64]="";
    char zone_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char os_image_raw[64]="";
    char htflag[CONF_STRING_LENTH]="";
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    int number_of_vcpu=0;
    int cpu_core_num=0;
    int threads;
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char bucket_id[32]="";
    char bucket_ak[AKSK_LENGTH]="";
    char bucket_sk[AKSK_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int num_temp;
    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_aws_root,"%s%saws%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_aws_root,"%saws/",url_code_root_var);
    }

    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    sprintf(secret_file,"%s%s.secrets.key",vaultdir,PATH_SLASH);
    get_ak_sk(secret_file,crypto_keyfile,access_key,secret_key,cloud_flag);
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one." RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s", COPY_FILE_CMD,url_aws_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s", url_aws_root,conf_file);
        }
        if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_aws.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_aws_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_aws.base -o %s%shpc_stack.base -s",url_aws_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_aws.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_aws_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_aws.master -o %s%shpc_stack.master -s",url_aws_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_aws.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_aws_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_aws.compute -o %s%shpc_stack.compute -s",url_aws_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_aws.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_aws_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_aws.database -o %s%shpc_stack.database -s",url_aws_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_aws.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_aws_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_aws.natgw -o %s%shpc_stack.natgw -s",url_aws_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_aws_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_aws_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,zone_id,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,conf_param_buffer,compute_inst,os_image_raw,htflag,&num_temp);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    number_of_vcpu=get_cpu_num(compute_inst);
    cpu_core_num=number_of_vcpu/2;
    if(strcmp(htflag,"OFF")==0){
        threads=1;
    }
    else{
        threads=2;
    }
    if(contain_or_not(zone_id,region_id)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Availability Zone ID doesn't match with Region ID, please double check.\n");
        printf("[ FATAL: ] Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    if(strcmp(region_id,"cn-northwest-1")==0){
        strcpy(region_flag,"cn_regions");
        if(strcmp(os_image_raw,"centos7")==0||strcmp(os_image_raw,"centoss9")==0){
            sprintf(os_image,"ami = \"${var.%scn.0}\"",os_image_raw);
        }
        else{
            sprintf(os_image,"ami = \"%s\"",os_image_raw);
        }
        strcpy(db_os_image,"ami = \"${var.centos7cn.0}\"");
        strcpy(nat_os_image,"ami = \"${var.centos7cn.0}\"");
    }
    else if(strcmp(region_id,"cn-north-1")==0){
        strcpy(region_flag,"cn_regions");
        if(strcmp(os_image_raw,"centos7")==0||strcmp(os_image_raw,"centoss9")==0){
            sprintf(os_image,"ami = \"${var.%scn.1}\"",os_image_raw);
        }
        else{
            sprintf(os_image,"ami = \"%s\"",os_image_raw);
        }
        strcpy(db_os_image,"ami = \"${var.centos7cn.1}\"");
        strcpy(nat_os_image,"ami = \"${var.centos7cn.1}\"");
    }
    else{
        strcpy(region_flag,"global_regions");
        if(strcmp(os_image_raw,"centos7")==0||strcmp(os_image_raw,"centoss9")==0||strcmp(os_image_raw,"openEuler")==0){
            sprintf(os_image,"ami = data.aws_ami.%s_x86_glb.image_id",os_image_raw);
        }
        else{
            sprintf(os_image,"ami = \"%s\"",os_image_raw);
        }
        strcpy(db_os_image,"ami = data.aws_ami.centos7_x86_glb.image_id");
        strcpy(nat_os_image,"ami = data.aws_ami.centos7_x86_glb.image_id");
    }
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Availability Zone:     %s\n",zone_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n" RESET_DISPLAY,os_image_raw);
    
    generate_sshkey(sshkey_folder,pubkey);

    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);
    sprintf(string_temp,"vpc-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_VPC_NAME",string_temp);
    sprintf(string_temp,"subnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_SUBNET_NAME",string_temp);
    sprintf(string_temp,"pubnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_PUB_SUBNET_NAME",string_temp);
    global_replace(filename_temp,"BLANK_ACCESS_KEY_ID",access_key);
    global_replace(filename_temp,"BLANK_SECRET_KEY",secret_key);
    global_replace(filename_temp,"BUCKET_ACCESS_POLICY",randstr);
    global_replace(filename_temp,"BUCKET_USER_ID",randstr);
    global_replace(filename_temp,"BUCKET_ID",randstr);
    sprintf(string_temp,"%s-public",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_PUBLIC",string_temp);
    sprintf(string_temp,"%s-natgw",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_NATGW",string_temp);
    sprintf(string_temp,"%s-intra",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_INTRA",string_temp);
    sprintf(string_temp,"%s-mysql",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_MYSQL",string_temp);
    sprintf(string_temp,"%s-nag",randstr);
    global_replace(filename_temp,"NAS_ACCESS_GROUP",string_temp);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"RG_NAME",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);
    if(strcmp(region_flag,"global_regions")==0){
        delete_lines_by_kwd(filename_temp,"DELETE_FOR_CN_REGIONS",1);
    }
    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    insert_lines(filename_temp,"#INSERT_AMI_HERE",os_image);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"RG_NAME",unique_cluster_id);
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    insert_lines(filename_temp,"#INSERT_AMI_HERE",os_image);
    global_replace(filename_temp,"RG_NAME",unique_cluster_id);
    sprintf(string_temp,"%d",cpu_core_num);
    global_replace(filename_temp,"CPU_CORE_NUM",string_temp);
    sprintf(string_temp,"%d",threads);
    global_replace(filename_temp,"THREADS_PER_CORE",string_temp);
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    insert_lines(filename_temp,"#INSERT_AMI_HERE",db_os_image);
    global_replace(filename_temp,"RG_NAME",unique_cluster_id);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    insert_lines(filename_temp,"#INSERT_AMI_HERE",nat_os_image);
    global_replace(filename_temp,"RG_NAME",unique_cluster_id);
    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
        sprintf(string_temp,"comp%d",i+1);
        global_replace(filename_temp,"NUMBER",string_temp);
        /*sprintf(line_temp,"echo -e \"export SCRIPTS_URL_ROOT=%s\" >> /etc/profile",url_shell_scripts_var);
        insert_lines(filename_temp,"var.cluster_init_scripts",line_temp);*/
    }
    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    getstate(workdir,crypto_keyfile);
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"bucket\"","","",1,"\"bucket\"","","",'\"',4,bucket_id);
    find_and_get(filename_temp,"aws_iam_access_key","","",15,"\"id\":","","",'\"',4,bucket_ak);
    find_and_get(filename_temp,"aws_iam_access_key","","",15,"\"secret\":","","",'\"',4,bucket_sk);
    if(strcmp(region_flag,"global_regions")==0){
        printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",AWS_SLEEP_TIME_GLOBAL);
        for(i=0;i<AWS_SLEEP_TIME_GLOBAL;i++){
            printf("[ -WAIT- ] Still need to wait %d seconds ... \r",AWS_SLEEP_TIME_GLOBAL-i);
            fflush(stdout);
            sleep(1);
        }
        printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    }
    else{
        printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",AWS_SLEEP_TIME_CN);
        for(i=0;i<AWS_SLEEP_TIME_CN;i++){
            printf("[ -WAIT- ] Still need to wait %d seconds ... \r",AWS_SLEEP_TIME_CN-i);
            fflush(stdout);
            sleep(1);
        }
        printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    }
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(bucket_id,region_id,bucket_ak,bucket_sk,"","",filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: s3:// %s\nNetDisk Region: %s\nNetDisk AccessKey ID: %s\nNetDisk Secret Key: %s\n",master_address,master_passwd,bucket_id,region_id,bucket_ak,bucket_sk);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p); 
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    sprintf(filename_temp,"%s%shpc_stack_database.tf",stackdir,PATH_SLASH);
    reset_string(string_temp);
    find_and_get(filename_temp,"instance_type","","",1,"instance_type","","",'.',3,string_temp);
    database_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    sprintf(filename_temp,"%s%shpc_stack_natgw.tf",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"instance_type","","",1,"instance_type","","",'.',3,string_temp);
    natgw_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    delete_decrypted_files(workdir,crypto_keyfile);
    return 0;
}

int qcloud_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char secret_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_qcloud_root[LOCATION_LENGTH_EXTENDED];
    char access_key[AKSK_LENGTH]="";
    char secret_key[AKSK_LENGTH]="";
    char cloud_flag[16]="";
    int read_conf_flag=0;
    char conf_param_buffer[32]="";
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    char os_image[64]="";
    char zone_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char master_bandwidth[CONF_STRING_LENTH];
    char NAS_Zone[CONF_STRING_LENTH]="";
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char bucket_id[32]="";
    char bucket_ak[AKSK_LENGTH]="";
    char bucket_sk[AKSK_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int num_temp;
    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_qcloud_root,"%s%sqcloud%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_qcloud_root,"%sqcloud/",url_code_root_var);
    }
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one. " RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s", url_qcloud_root,conf_file);
        }
            if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_qcloud.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_qcloud.base -o %s%shpc_stack.base -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_qcloud.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_qcloud.master -o %s%shpc_stack.master -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_qcloud.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_qcloud.compute -o %s%shpc_stack.compute -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_qcloud.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_qcloud.database -o %s%shpc_stack.database -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_qcloud.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_qcloud.natgw -o %s%shpc_stack.natgw -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sNAS_Zones_QCloud.txt %s%sNAS_Zones_QCloud.txt %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sNAS_Zones_QCloud.txt -o %s%sNAS_Zones_QCloud.txt -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_qcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_qcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(secret_file,"%s%s.secrets.key",vaultdir,PATH_SLASH);
    get_ak_sk(secret_file,crypto_keyfile,access_key,secret_key,cloud_flag);
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,zone_id,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,master_bandwidth,compute_inst,os_image,conf_param_buffer,&num_temp);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    sprintf(filename_temp,"%s%sNAS_Zones_QCloud.txt",stackdir,PATH_SLASH);
    if(find_multi_keys(filename_temp,zone_id,"","","","")>0){
        strcpy(NAS_Zone,zone_id);
    }
    else{
        find_and_get(filename_temp,region_id,"","",1,region_id,"","",' ',1,NAS_Zone);
    }
 
    if(contain_or_not(zone_id,region_id)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Availability Zone ID doesn't match with Region ID, please double check.\n");
        printf("[ FATAL: ] Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Availability Zone:     %s\n",zone_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n" RESET_DISPLAY,os_image);
    generate_sshkey(sshkey_folder,pubkey);
    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);
    sprintf(string_temp,"vpc-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_VPC_NAME",string_temp);
    sprintf(string_temp,"subnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_SUBNET_NAME",string_temp);
    sprintf(string_temp,"pubnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_PUB_SUBNET_NAME",string_temp);
    global_replace(filename_temp,"CFSID",unique_cluster_id);
    global_replace(filename_temp,"BLANK_ACCESS_KEY_ID",access_key);
    global_replace(filename_temp,"BLANK_SECRET_KEY",secret_key);
    global_replace(filename_temp,"BUCKET_ACCESS_POLICY",randstr);
    global_replace(filename_temp,"BUCKET_USER_ID",randstr);
    global_replace(filename_temp,"BUCKET_ID",randstr);
    sprintf(string_temp,"%s-public",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_PUBLIC",string_temp);
    sprintf(string_temp,"%s-intra",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_INTRA",string_temp);
    sprintf(string_temp,"%s-mysql",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_MYSQL",string_temp);
    sprintf(string_temp,"%s-natgw",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_NATGW",string_temp);
    sprintf(string_temp,"%s-nag",randstr);
    global_replace(filename_temp,"NAS_ACCESS_GROUP",string_temp);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"DEFAULT_NAS_ZONE",NAS_Zone);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);

    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.tencentcloud_images.OS_IMAGE.images.0.image_id",string_temp);
    }
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.tencentcloud_images.OS_IMAGE.images.0.image_id",string_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);

    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
        global_replace(filename_temp,"RUNNING_FLAG","true");
    }
    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);   
    system(cmdline);
    getstate(workdir,crypto_keyfile);
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"bucket\"","","",1,"\"bucket\"","","",'\"',4,bucket_id);
    find_and_get(filename_temp,"secret_id","","",1,"secret_id","","",'\"',4,bucket_ak);
    find_and_get(filename_temp,"secret_key","","",1,"secret_key","","",'\"',4,bucket_sk);
    printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",QCLOUD_SLEEP_TIME);
    for(i=0;i<QCLOUD_SLEEP_TIME;i++){
        printf("[ -WAIT- ] Still need to wait %d seconds ... \r",QCLOUD_SLEEP_TIME-i);
        fflush(stdout);
        sleep(1);
    }
    printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(bucket_id,region_id,bucket_ak,bucket_sk,"","",filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: cos: %s\nNetDisk Region: %s\nNetDisk AccessKey ID: %s\nNetDisk Secret Key: %s\n",master_address,master_passwd,bucket_id,region_id,bucket_ak,bucket_sk);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p);
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    sprintf(filename_temp,"%s%shpc_stack_database.tf",stackdir,PATH_SLASH);
    reset_string(string_temp);
    find_and_get(filename_temp,"instance_type","","",1,"instance_type","","",'.',3,string_temp);
    database_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    sprintf(filename_temp,"%s%shpc_stack_natgw.tf",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"instance_type","","",1,"instance_type","","",'.',3,string_temp);
    natgw_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    delete_decrypted_files(workdir,crypto_keyfile);
    return 0;
}

int alicloud_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char secret_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_alicloud_root[LOCATION_LENGTH_EXTENDED]="";
    char access_key[AKSK_LENGTH]="";
    char secret_key[AKSK_LENGTH]="";
    char cloud_flag[16]="";
    char conf_param_buffer[32]="";
    int read_conf_flag=0;
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    char os_image[64]="";
    char zone_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char master_bandwidth[CONF_STRING_LENTH]="";
    char NAS_Zone[CONF_STRING_LENTH]="";
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char bucket_id[32]="";
    char bucket_ak[AKSK_LENGTH]="";
    char bucket_sk[AKSK_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int num_temp;
    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_alicloud_root,"%s%salicloud%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_alicloud_root,"%salicloud/",url_code_root_var);
    }
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one. " RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s",url_alicloud_root,conf_file);
        }
        if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stackv2.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stackv2.base -o %s%shpc_stack.base -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stackv2.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stackv2.master -o %s%shpc_stack.master -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stackv2.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stackv2.compute -o %s%shpc_stack.compute -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stackv2.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stackv2.database -o %s%shpc_stack.database -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stackv2.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stackv2.natgw -o %s%shpc_stack.natgw -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sNAS_Zones_ALI.txt %s%sNAS_Zones_ALI.txt %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sNAS_Zones_ALI.txt -o %s%sNAS_Zones_ALI.txt -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_alicloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_alicloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(secret_file,"%s%s.secrets.key",vaultdir,PATH_SLASH);
    get_ak_sk(secret_file,crypto_keyfile,access_key,secret_key,cloud_flag);
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,zone_id,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,master_bandwidth,compute_inst,os_image,conf_param_buffer,&num_temp);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    sprintf(filename_temp,"%s%sNAS_Zones_ALI.txt",stackdir,PATH_SLASH);
    if(find_multi_keys(filename_temp,zone_id,"","","","")>0){
        strcpy(NAS_Zone,zone_id);
    }
    else{
        find_and_get(filename_temp,region_id,"","",1,region_id,"","",' ',1,NAS_Zone);
    }
    if(contain_or_not(zone_id,region_id)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Availability Zone ID doesn't match with Region ID, please double check.\n");
        printf("[ FATAL: ] Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Availability Zone:     %s\n",zone_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n" RESET_DISPLAY,os_image);
    generate_sshkey(sshkey_folder,pubkey);
    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);
    sprintf(string_temp,"vpc-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_VPC_NAME",string_temp);
    global_replace(filename_temp,"BLANK_ACCESS_KEY_ID",access_key);
    global_replace(filename_temp,"BLANK_SECRET_KEY",secret_key);
    global_replace(filename_temp,"BUCKET_ACCESS_POLICY",randstr);
    global_replace(filename_temp,"BUCKET_USER_ID",randstr);
    global_replace(filename_temp,"BUCKET_ID",randstr);
    sprintf(string_temp,"%s-public",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_PUBLIC",string_temp);
    sprintf(string_temp,"%s-intra",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_INTRA",string_temp);
    global_replace(filename_temp,"RG_NAME",unique_cluster_id);
    global_replace(filename_temp,"RG_DISPLAY_NAME",unique_cluster_id);
    sprintf(string_temp,"%s-mysql",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_MYSQL",string_temp);
    sprintf(string_temp,"%s-nag",randstr);
    global_replace(filename_temp,"NAS_ACCESS_GROUP",string_temp);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"DEFAULT_NAS_ZONE",NAS_Zone);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);
    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RG_DISPLAY_NAME",unique_cluster_id);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        global_replace(filename_temp,"${data.alicloud_images.OS_IMAGE.images.0.id}",os_image);
    }
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RG_DISPLAY_NAME",unique_cluster_id);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        global_replace(filename_temp,"${data.alicloud_images.OS_IMAGE.images.0.id}",os_image);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RG_DISPLAY_NAME",unique_cluster_id);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);
    global_replace(filename_temp,"RG_DISPLAY_NAME",unique_cluster_id);
    reset_string(filename_temp);
    reset_string(string_temp);

    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
    }
    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    getstate(workdir,crypto_keyfile);
    printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",ALI_SLEEP_TIME);
    for(i=0;i<ALI_SLEEP_TIME;i++){
        printf("[ -WAIT- ] Still need to wait %d seconds ... \r",ALI_SLEEP_TIME-i);
        fflush(stdout);
        sleep(1);
    }
    printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"bucket\"","","",1,"\"bucket\"","","",'\"',4,bucket_id);
    sprintf(filename_temp,"%s%sbucket_secrets.txt",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"AccessKeyId","","",1,"AccessKeyId","","",'\"',4,bucket_ak);
    find_and_get(filename_temp,"AccessKeySecret","","",1,"AccessKeySecret","","",'\"',4,bucket_sk);
    sprintf(cmdline,"%s %s %s",DELETE_FILE_CMD,filename_temp,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(bucket_id,region_id,bucket_ak,bucket_sk,"","",filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: oss:// %s\nNetDisk Region: %s\nNetDisk AccessKey ID: %s\nNetDisk Secret Key: %s\n",master_address,master_passwd,bucket_id,region_id,bucket_ak,bucket_sk);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p);
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    sprintf(filename_temp,"%s%shpc_stack_database.tf",stackdir,PATH_SLASH);
    reset_string(string_temp);
    find_and_get(filename_temp,"instance_type","","",1,"instance_type","","",'.',3,string_temp);
    database_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    sprintf(filename_temp,"%s%shpc_stack_natgw.tf",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"instance_type","","",1,"instance_type","","",'.',3,string_temp);
    natgw_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    delete_decrypted_files(workdir,crypto_keyfile);
    return 0;
}

int hw_intel_amd_generation(const char* region_id, char* intel_generation, char* amd_generation, int* amd_flag){
    if(strcmp(region_id,"cn-north-4")==0||strcmp(region_id,"cn-east-3")==0||strcmp(region_id,"cn-south-1")==0){
        *amd_flag=0;
        strcpy(amd_generation,"ac7");
    }
    else{
        *amd_flag=1;
        strcpy(amd_generation,"");
    }
    if(strcmp(region_id,"na-mexico-1")==0||strcmp(region_id,"na-mexico-2")==0||strcmp(region_id,"sa-brazil-1")==0||strcmp(region_id,"la-south-2")==0||strcmp(region_id,"af-south-1")==0){
        strcpy(intel_generation,"c6");
        return 1;
    }
    else if(strcmp(region_id,"ap-southeast-1")==0){
        strcpy(intel_generation,"c7");
        return 2;
    }
    else if(strcmp(region_id,"tr-west-1")==0||strcmp(region_id,"ap-southeast-4")==0||strcmp(region_id,"ap-southeast-3")==0||strcmp(region_id,"ap-southeast-2")==0){
        strcpy(intel_generation,"c7");
        return 3;
    }
    else{
        strcpy(intel_generation,"c7");
        return 4;
    }
}

int hwcloud_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char secret_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_hwcloud_root[LOCATION_LENGTH_EXTENDED];
    char access_key[AKSK_LENGTH]="";
    char secret_key[AKSK_LENGTH]="";
    char cloud_flag[16]="";
    int read_conf_flag=0;
    char conf_param_buffer[32]="";
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    char os_image[64]="";
    char zone_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char master_bandwidth[CONF_STRING_LENTH];
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char bucket_id[32]="";
    char bucket_ak[AKSK_LENGTH]="";
    char bucket_sk[AKSK_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    int amd_flavor_flag=1;
    char intel_generation[8]="";
    char amd_generation[8]="";
    int intel_flavor_flag=0;
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int nfs_volume;
    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_hwcloud_root,"%s%shwcloud%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_hwcloud_root,"%shwcloud/",url_code_root_var);
    }
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one. " RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s", url_hwcloud_root,conf_file);
        }
            if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_hwcloud.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_hwcloud.base -o %s%shpc_stack.base -s",url_hwcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_hwcloud.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_hwcloud.master -o %s%shpc_stack.master -s",url_hwcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_hwcloud.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_hwcloud.compute -o %s%shpc_stack.compute -s",url_hwcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_hwcloud.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_hwcloud.database -o %s%shpc_stack.database -s",url_hwcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_hwcloud.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_hwcloud.natgw -o %s%shpc_stack.natgw -s",url_hwcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_hwcloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_hwcloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(secret_file,"%s%s.secrets.key",vaultdir,PATH_SLASH);
    get_ak_sk(secret_file,crypto_keyfile,access_key,secret_key,cloud_flag);
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,zone_id,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,master_bandwidth,compute_inst,os_image,conf_param_buffer,&nfs_volume);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    if(contain_or_not(zone_id,region_id)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Availability Zone ID doesn't match with Region ID, please double check.\n");
        printf("[ FATAL: ] Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Availability Zone:     %s\n",zone_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n",os_image);
    printf("|          NFS Volume:            %d\n" RESET_DISPLAY,nfs_volume);

    intel_flavor_flag=hw_intel_amd_generation(region_id,intel_generation,amd_generation,&amd_flavor_flag);
    generate_sshkey(sshkey_folder,pubkey);
    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);
    sprintf(string_temp,"vpc-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_VPC_NAME",string_temp);
    sprintf(string_temp,"subnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_SUBNET_NAME",string_temp);
    sprintf(string_temp,"pubnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_PUB_SUBNET_NAME",string_temp);
    global_replace(filename_temp,"BLANK_ACCESS_KEY_ID",access_key);
    global_replace(filename_temp,"BLANK_SECRET_KEY",secret_key);
    global_replace(filename_temp,"BUCKET_USER_ID",randstr);
    global_replace(filename_temp,"BUCKET_ID",randstr);
    sprintf(string_temp,"%s-public",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_PUBLIC",string_temp);
    sprintf(string_temp,"%s-intra",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_INTRA",string_temp);
    sprintf(string_temp,"%s-mysql",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_MYSQL",string_temp);
    sprintf(string_temp,"%s-natgw",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_NATGW",string_temp);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    sprintf(string_temp,"%d",nfs_volume);
    global_replace(filename_temp,"DEFAULT_STORAGE_VOLUME",string_temp);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    if(amd_flavor_flag==1){
        insert_lines(filename_temp,"#AMD_MACHINE_START","/*");
        insert_lines(filename_temp,"#AMD_MACHINE_END","*/");
    }
    else{
        global_replace(filename_temp,"AMD_GENERATION",amd_generation);
    }
    if(intel_flavor_flag==1||intel_flavor_flag==3){
        insert_lines(filename_temp,"#C7_SPECIFIC","/*");
        insert_lines(filename_temp,"#C6S_SPECIFIC","*/");
    }
    else if(intel_flavor_flag==2){
        insert_lines(filename_temp,"#C7_SPECIFIC","/*");
        insert_lines(filename_temp,"#C7N_HK_SPECIFIC","*/");
    }
    global_replace(filename_temp,"INTEL_GENERATION",intel_generation);

    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);
    if(strcmp(os_image,"rocky9")==0||strcmp(os_image,"euleros")==0||strcmp(os_image,"centos7")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.huaweicloud_images_images.OS_IMAGE.images[0].id",string_temp);
    }
    
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    if(strcmp(os_image,"rocky9")==0||strcmp(os_image,"euleros")==0||strcmp(os_image,"centos7")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.huaweicloud_images_images.OS_IMAGE.images[0].id",string_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);

    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
        global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
        /*sprintf(line_temp,"echo -e \"export SCRIPTS_URL_ROOT=%s\" >> /etc/profile",url_shell_scripts_var);
        insert_lines(filename_temp,"var.cluster_init_scripts",line_temp);*/
    }

    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);   
    system(cmdline);
    getstate(workdir,crypto_keyfile);
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"bucket\"","","",1,"\"bucket\"","","",'\"',4,bucket_id);
    find_and_get(filename_temp,"access_key","","",20,"\"id\":","","",'\"',4,bucket_ak);
    find_and_get(filename_temp,"access_key","","",20,"\"secret\":","","",'\"',4,bucket_sk);
    printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",GENERAL_SLEEP_TIME);
    for(i=0;i<GENERAL_SLEEP_TIME;i++){
        printf("[ -WAIT- ] Still need to wait %d seconds ... \r",GENERAL_SLEEP_TIME-i);
        fflush(stdout);
        sleep(1);
    }
    printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(bucket_id,region_id,bucket_ak,bucket_sk,"","",filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: obs: %s\nNetDisk Region: %s\nNetDisk AccessKey ID: %s\nNetDisk Secret Key: %s\n",master_address,master_passwd,bucket_id,region_id,bucket_ak,bucket_sk);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p);
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    sprintf(filename_temp,"%s%shpc_stack_database.tf",stackdir,PATH_SLASH);
    reset_string(string_temp);
    find_and_get(filename_temp,"flavor_id","","",1,"flavor_id","","",'.',3,string_temp);
    database_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    sprintf(filename_temp,"%s%shpc_stack_natgw.tf",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"flavor_id","","",1,"flavor_id","","",'.',3,string_temp);
    natgw_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    delete_decrypted_files(workdir,crypto_keyfile);
    return 0;
}

int baiducloud_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char secret_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_baiducloud_root[LOCATION_LENGTH_EXTENDED];
    char access_key[AKSK_LENGTH]="";
    char secret_key[AKSK_LENGTH]="";
    char cloud_flag[16]="";
    int read_conf_flag=0;
    char conf_param_buffer[32]="";
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    char os_image[64]="";
    char zone_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char master_bandwidth[CONF_STRING_LENTH];
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char bucket_id[32]="";
    char bucket_ak[AKSK_LENGTH]="";
    char bucket_sk[AKSK_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    char natgw_inst[16]="";
    char db_inst[16]="";
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int num_temp;
    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_baiducloud_root,"%s%sbaidu%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_baiducloud_root,"%sbaidu/",url_code_root_var);
    }
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one. " RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s", url_baiducloud_root,conf_file);
        }
            if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_baidu.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_baidu.base -o %s%shpc_stack.base -s",url_baiducloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_baidu.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_baidu.master -o %s%shpc_stack.master -s",url_baiducloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_baidu.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_baidu.compute -o %s%shpc_stack.compute -s",url_baiducloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_baidu.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_baidu.database -o %s%shpc_stack.database -s",url_baiducloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_baidu.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_baidu.natgw -o %s%shpc_stack.natgw -s",url_baiducloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_baiducloud_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_baiducloud_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(secret_file,"%s%s.secrets.key",vaultdir,PATH_SLASH);
    get_ak_sk(secret_file,crypto_keyfile,access_key,secret_key,cloud_flag);
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,zone_id,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,master_bandwidth,compute_inst,os_image,conf_param_buffer,&num_temp);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    if(contain_or_not(zone_id,region_id)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Availability Zone ID doesn't match with Region ID, please double check.\n");
        printf("[ FATAL: ] Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    if(strcmp(region_id,"hk")==0){
        strcpy(db_inst,"i2c2g-hk");
        strcpy(natgw_inst,"i2c2g-hk");
    }
    else{
        strcpy(db_inst,"i2c2g");
        strcpy(natgw_inst,"i2c2g");
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Availability Zone:     %s\n",zone_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n" RESET_DISPLAY,os_image);
    generate_sshkey(sshkey_folder,pubkey);

    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);
    sprintf(string_temp,"vpc-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_VPC_NAME",string_temp);
    sprintf(string_temp,"subnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_SUBNET_NAME",string_temp);
    sprintf(string_temp,"pubnet-%s",unique_cluster_id);
    global_replace(filename_temp,"DEFAULT_PUB_SUBNET_NAME",string_temp);
    global_replace(filename_temp,"BLANK_ACCESS_KEY_ID",access_key);
    global_replace(filename_temp,"BLANK_SECRET_KEY",secret_key);
    global_replace(filename_temp,"BUCKET_USER_ID",randstr);
    global_replace(filename_temp,"BUCKET_ID",randstr);
    sprintf(string_temp,"%s-public",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_PUBLIC",string_temp);
    sprintf(string_temp,"%s-intra",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_INTRA",string_temp);
    sprintf(string_temp,"%s-mysql",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_MYSQL",string_temp);
    sprintf(string_temp,"%s-natgw",randstr);
    global_replace(filename_temp,"SECURITY_GROUP_NATGW",string_temp);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    if(strcmp(region_id,"gz")==0){
        global_replace(filename_temp,"DEFAULT_NAS_ZONE","zoneC");
    }
    else{
        global_replace(filename_temp,"DEFAULT_NAS_ZONE","zoneA");
    }
    global_replace(filename_temp,"NFSID",randstr);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    global_replace(filename_temp,"MASTER_BANDWIDTH",master_bandwidth);

    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.baiducloud_images.OS_IMAGE.images[0].id",string_temp);
    }
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.baiducloud_images.OS_IMAGE.images[0].id",string_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    global_replace(filename_temp,"DB_INST",db_inst);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
    global_replace(filename_temp,"NATGW_INST",natgw_inst);

    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
        global_replace(filename_temp,"RESOURCETAG",unique_cluster_id);
        /*sprintf(line_temp,"echo -e \"export SCRIPTS_URL_ROOT=%s\" >> /etc/profile",url_shell_scripts_var);
        insert_lines(filename_temp,"var.cluster_init_scripts",line_temp);*/
    }
    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);   
    system(cmdline);
    getstate(workdir,crypto_keyfile);
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"bucket\":","","",1,"\"bucket\":","","",'\"',4,bucket_id); 
    sprintf(filename_temp,"%s%saccess-key.txt",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"AccessKeyId","","",1,"AccessKeyId","","",'\"',4,bucket_ak);
    find_and_get(filename_temp,"AccessKeySecret","","",1,"AccessKeySecret","","",'\"',4,bucket_sk);
    sprintf(cmdline,"%s %s %s",DELETE_FILE_CMD,filename_temp,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",GENERAL_SLEEP_TIME);
    for(i=0;i<GENERAL_SLEEP_TIME;i++){
        printf("[ -WAIT- ] Still need to wait %d seconds ... \r",GENERAL_SLEEP_TIME-i);
        fflush(stdout);
        sleep(1);
    }
    printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(bucket_id,region_id,bucket_ak,bucket_sk,"","",filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: bos: %s\nNetDisk Region: %s\nNetDisk AccessKey ID: %s\nNetDisk Secret Key: %s\n",master_address,master_passwd,bucket_id,region_id,bucket_ak,bucket_sk);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p);
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    sprintf(filename_temp,"%s%shpc_stack_database.tf",stackdir,PATH_SLASH);
    reset_string(string_temp);
    find_and_get(filename_temp,"instance_spec","","",1,"instance_spec","","",'.',3,string_temp);
    database_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    sprintf(filename_temp,"%s%shpc_stack_natgw.tf",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"instance_spec","","",1,"instance_spec","","",'.',3,string_temp);
    natgw_vcpu=get_cpu_num(string_temp);
    reset_string(string_temp);
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    generate_bceconfig(vaultdir,region_id,bucket_ak,bucket_sk);
    sprintf(filename_temp,"%s%scredentials",vaultdir,PATH_SLASH);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket_creds/credentials","root","put","",0);
    sprintf(filename_temp,"%s%sconfig",vaultdir,PATH_SLASH);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket_creds/config","root","put","",0);
    delete_decrypted_files(workdir,crypto_keyfile);
    return 0;
}

int azure_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char secret_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_azure_root[LOCATION_LENGTH_EXTENDED];
    char access_key[AKSK_LENGTH]="";
    char secret_key[AKSK_LENGTH]="";
    char subscription_id[AKSK_LENGTH]="";
    char tenant_id[AKSK_LENGTH]="";
    char cloud_flag[16]="";
    int read_conf_flag=0;
    char conf_param_buffer1[32]="";
    char conf_param_buffer2[32]="";
    char conf_param_buffer3[32]="";
    char os_image[64]="";
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char random_storage_account[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char bucket_id[128]="";
    char bucket_ak[AKSK_LENGTH]="";
    char bucket_sk[AKSK_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int nfs_volume;

    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_azure_root,"%s%sazure%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_azure_root,"%sazure/",url_code_root_var);
    }
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one. " RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s", url_azure_root,conf_file);
        }
        if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_azure.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_azure.base -o %s%shpc_stack.base -s",url_azure_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_azure.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_azure.master -o %s%shpc_stack.master -s",url_azure_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_azure.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_azure.compute -o %s%shpc_stack.compute -s",url_azure_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_azure.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_azure.database -o %s%shpc_stack.database -s",url_azure_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_azure.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_azure.natgw -o %s%shpc_stack.natgw -s",url_azure_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_azure_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_azure_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(secret_file,"%s%s.secrets.key",vaultdir,PATH_SLASH);
    get_ak_sk(secret_file,crypto_keyfile,access_key,secret_key,cloud_flag);
    get_azure_info(workdir,subscription_id,tenant_id);
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,conf_param_buffer1,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,conf_param_buffer2,compute_inst,os_image,conf_param_buffer3,&nfs_volume);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n",os_image);
    printf("|          NFS Volume:            %d\n" RESET_DISPLAY,nfs_volume);
    generate_sshkey(sshkey_folder,pubkey);
    generate_random_string(random_storage_account);

    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);
    global_replace(filename_temp,"BLANK_CLIENT_ID",access_key);
    global_replace(filename_temp,"BLANK_SECRET_KEY",secret_key);
    global_replace(filename_temp,"DEFAULT_TENANT_ID",tenant_id);
    global_replace(filename_temp,"DEFAULT_SUBCRIPTION_ID",subscription_id);
    global_replace(filename_temp,"ENVIRONMENT_OPTION",az_environment);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"RANDOM_STORAGE_ACCOUNT",random_storage_account);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    sprintf(string_temp,"%d",nfs_volume);
    global_replace(filename_temp,"DEFAULT_STORAGE_VOLUME",string_temp);

    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"RANDOM_STRING",randstr);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"RANDOM_STRING",randstr);

    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
        /*sprintf(line_temp,"echo -e \"export SCRIPTS_URL_ROOT=%s\" >> /etc/profile",url_shell_scripts_var);
        insert_lines(filename_temp,"var.cluster_init_scripts",line_temp);*/
    }
    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);   
    system(cmdline);

    getstate(workdir,crypto_keyfile);
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"type\": \"azurerm_storage_container\",","","",20,"\"id\": \"","","",'\"',4,bucket_id);
    find_and_get(filename_temp,"\"type\": \"azuread_application\",","","",20,"\"application_id\":","","",'\"',4,bucket_ak);
    find_and_get(filename_temp,"\"type\": \"azuread_application_password\",","","",20,"\"value\":","","",'\"',4,bucket_sk);
    
    printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",2*GENERAL_SLEEP_TIME);
    for(i=0;i<2*GENERAL_SLEEP_TIME;i++){
        printf("[ -WAIT- ] Still need to wait %d seconds ... \r",2*GENERAL_SLEEP_TIME-i);
        fflush(stdout);
        sleep(1);
    }
    printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(bucket_id,region_id,bucket_ak,bucket_sk,subscription_id,tenant_id,filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: %s\nNetDisk Region: %s\nNetDisk AccessKey ID: %s\nNetDisk Secret Key: %s\n",master_address,master_passwd,bucket_id,region_id,bucket_ak,bucket_sk);
    fprintf(file_p,"Azure Subscription ID: %s\nAzure Tenant ID: %s\n",subscription_id,tenant_id);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p);
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    database_vcpu=1;
    natgw_vcpu=1;
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    delete_decrypted_files(workdir,crypto_keyfile);
    return 0;
}

int gcp_cluster_init(char* cluster_id_input, char* workdir, char* crypto_keyfile){
    char stackdir[DIR_LENGTH]="";
    char vaultdir[DIR_LENGTH]="";
    char logdir[DIR_LENGTH]="";
    char confdir[DIR_LENGTH]="";
    char compute_template[FILENAME_LENGTH]="";
    char cmdline[CMDLINE_LENGTH]="";
    char conf_file[FILENAME_LENGTH]="";
    char filename_temp[FILENAME_LENGTH]="";
    char user_passwords[FILENAME_LENGTH]="";
    char* tf_exec=TOFU_EXEC;
    char url_gcp_root[LOCATION_LENGTH_EXTENDED];
    char cloud_flag[16]="";
    int read_conf_flag=0;
    char conf_param_buffer1[32]="";
    char conf_param_buffer2[32]="";
    char cluster_id_temp[24]="";
    char unique_cluster_id[96]="";
    char string_temp[128]="";
    char keyfile_path[FILENAME_LENGTH]="";
    char keyfile_path_ext[FILENAME_LENGTH_EXT]="";
    char gcp_project_id[128]="";
    char cluster_id[CONF_STRING_LENTH]="";
    char region_id[CONF_STRING_LENTH]="";
    char os_image[120]="";
    char zone_id[CONF_STRING_LENTH]="";
    int node_num=0;
    int hpc_user_num=0;
    char master_init_param[CONF_STRING_LENTH]="";
    char master_passwd[CONF_STRING_LENTH]="";
    char compute_passwd[CONF_STRING_LENTH]="";
    char master_inst[CONF_STRING_LENTH]="";
    char compute_inst[CONF_STRING_LENTH]="";
    char randstr[RANDSTR_LENGTH_PLUS]="";
    char* sshkey_folder=SSHKEY_DIR;
    char pubkey[LINE_LENGTH]="";
    FILE* file_p=NULL;
    FILE* file_p_2=NULL;
    char database_root_passwd[PASSWORD_STRING_LENGTH]="";
    char database_acct_passwd[PASSWORD_STRING_LENGTH]="";
    char user_passwd_temp[PASSWORD_STRING_LENGTH]="";
    char line_temp[LINE_LENGTH]="";
    char master_address[32]="";
    time_t current_time_long;
    struct tm* time_p=NULL;
    char current_date[12]="";
    char current_time[12]="";
    char master_cpu_vendor[8]="";
    char compute_cpu_vendor[8]="";
    int master_vcpu,database_vcpu,natgw_vcpu,compute_vcpu;
    char usage_logfile[FILENAME_LENGTH]="";
    int i;
    int nfs_volume;
    char bucket_selflink[128]="";
    char bucket_private_key[LINE_LENGTH]="";
    if(folder_exist_or_not(workdir)==1){
        return -1;
    }
    create_and_get_stackdir(workdir,stackdir);
    create_and_get_vaultdir(workdir,vaultdir);
    sprintf(logdir,"%s%slog%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(confdir,"%s%sconf%s",workdir,PATH_SLASH,PATH_SLASH);
    sprintf(compute_template,"%s%scompute_template",stackdir,PATH_SLASH);
    printf("[ START: ] Start initializing the cluster ...\n");
    if(folder_exist_or_not(stackdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,stackdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(vaultdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,vaultdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(logdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,logdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(folder_exist_or_not(confdir)!=0){
        sprintf(cmdline,"%s %s %s",MKDIR_CMD,confdir,SYSTEM_CMD_REDIRECT);
        system(cmdline);
    }
    if(code_loc_flag_var==1){
        sprintf(url_gcp_root,"%s%sgcp%s",url_code_root_var,PATH_SLASH,PATH_SLASH);
    }
    else{
        sprintf(url_gcp_root,"%sgcp/",url_code_root_var);
    }
    sprintf(conf_file,"%s%stf_prep.conf",confdir,PATH_SLASH);
    if(file_exist_or_not(conf_file)==1){
        printf(GENERAL_BOLD "[ -INFO- ] IMPORTANT: No configure file found. Use the default one. " RESET_DISPLAY "\n");
        if(code_loc_flag_var==1){
            sprintf(cmdline,"%s %s%stf_prep.conf %s %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,conf_file,SYSTEM_CMD_REDIRECT);
        }
        else{
            sprintf(cmdline,"curl %stf_prep.conf -s -o %s", url_gcp_root,conf_file);
        }
            if(system(cmdline)!=0){
            printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
            clear_if_failed(stackdir,confdir,vaultdir,1);
            return 2;
        }
    }
    printf("[ STEP 1 ] Creating initialization files now ...\n");
    sprintf(cmdline,"%s %s%shpc_stack* %s",DELETE_FILE_CMD,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_gcp.base %s%shpc_stack.base %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_gcp.base -o %s%shpc_stack.base -s",url_gcp_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_gcp.master %s%shpc_stack.master %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_gcp.master -o %s%shpc_stack.master -s",url_gcp_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_gcp.compute %s%shpc_stack.compute %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_gcp.compute -o %s%shpc_stack.compute -s",url_gcp_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_gcp.database %s%shpc_stack.database %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_gcp.database -o %s%shpc_stack.database -s",url_gcp_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%shpc_stack_gcp.natgw %s%shpc_stack.natgw %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %shpc_stack_gcp.natgw -o %s%shpc_stack.natgw -s",url_gcp_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    if(code_loc_flag_var==1){
        sprintf(cmdline,"%s %s%sreconf.list %s%sreconf.list %s",COPY_FILE_CMD,url_gcp_root,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);
    }
    else{
        sprintf(cmdline,"curl %sreconf.list -o %s%sreconf.list -s",url_gcp_root,stackdir,PATH_SLASH);
    }
    if(system(cmdline)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Failed to download/copy necessary file(s). Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,1);
        return 2;
    }
    sprintf(filename_temp,"%s%sreconf.list",stackdir,PATH_SLASH);
    read_conf_flag=get_tf_prep_conf(conf_file,filename_temp,cluster_id,region_id,zone_id,&node_num,&hpc_user_num,master_init_param,master_passwd,compute_passwd,master_inst,conf_param_buffer1,compute_inst,os_image,conf_param_buffer2,&nfs_volume);
    if(read_conf_flag!=0){
        if(read_conf_flag==-3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Configuration file not found. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==1){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid format for NODE_NUM and/or HPC_USER_NUM. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==2){
            printf(FATAL_RED_BOLD "[ FATAL: ] Invalid node configuration string. Exit now." RESET_DISPLAY "\n");
        }
        else if(read_conf_flag==3){
            printf(FATAL_RED_BOLD "[ FATAL: ] Insufficient configuration params. Exit now." RESET_DISPLAY "\n");
        }
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    node_user_num_fix(&node_num,&hpc_user_num);
    if(contain_or_not(zone_id,region_id)!=0){
        printf(FATAL_RED_BOLD "[ FATAL: ] Availability Zone ID doesn't match with Region ID, please double check.\n");
        printf("[ FATAL: ] Exit now." RESET_DISPLAY "\n");
        clear_if_failed(stackdir,confdir,vaultdir,2);
        return 3;
    }
    reset_string(database_root_passwd);
    generate_random_db_passwd(database_root_passwd);
    usleep(10000);
    reset_string(database_acct_passwd);
    generate_random_db_passwd(database_acct_passwd);
    if(strcmp(master_passwd,"*AUTOGEN*")==0||strlen(master_passwd)<8){
        reset_string(master_passwd);
        generate_random_passwd(master_passwd);
    }
    if(strcmp(compute_passwd,"*AUTOGEN*")==0||strlen(compute_passwd)<8){
        reset_string(compute_passwd);
        generate_random_passwd(compute_passwd);
    }
    if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MAX){
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id_temp+i)=*(cluster_id_input+i);
        }
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        for(i=0;i<CLUSTER_ID_LENGTH_MAX;i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)>CLUSTER_ID_LENGTH_MIN||strlen(cluster_id_input)==CLUSTER_ID_LENGTH_MIN){
        global_replace(conf_file,cluster_id,cluster_id_input);
        reset_string(cluster_id);
        for(i=0;i<strlen(cluster_id_input);i++){
            *(cluster_id+i)=*(cluster_id_input+i);
        }
    }
    else if(strlen(cluster_id_input)<CLUSTER_ID_LENGTH_MIN&&strlen(cluster_id_input)>0){
        sprintf(cluster_id_temp,"%s-hpcnow",cluster_id_input);
        global_replace(conf_file,cluster_id,cluster_id_temp);
        reset_string(cluster_id);
        strcpy(cluster_id,cluster_id_temp);
    }
    sprintf(filename_temp,"%s%sUCID_LATEST.txt",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)==1){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Creating a Unique Cluster ID now...\n");
        reset_string(randstr);
        generate_random_string(randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        file_p=fopen(filename_temp,"w+");
        fprintf(file_p,"%s",randstr);
        fclose(file_p);
    }
    else{
        file_p=fopen(filename_temp,"r");
        fscanf(file_p,"%s",randstr);
        sprintf(unique_cluster_id,"%s-%s",cluster_id,randstr);
        fclose(file_p);
    }
    printf(HIGH_GREEN_BOLD "[ STEP 2 ] Cluster Configuration:\n");
    printf("|          Cluster ID:            %s\n",cluster_id);
    printf("|          Region:                %s\n",region_id);
    printf("|          Availability Zone:     %s\n",zone_id);
    printf("|          Number of Nodes:       %d\n",node_num);
    printf("|          Number of Users:       %d\n",hpc_user_num);
    printf("|          Master Node Instance:  %s\n",master_inst);
    printf("|          Compute Node Instance: %s\n",compute_inst);
    printf("|          OS Image:              %s\n",os_image);
    printf("|          NFS Volume:            %d\n" RESET_DISPLAY,nfs_volume);
//    printf("---%s %s %s %s---\n",stackdir,vaultdir,logdir,confdir);
    generate_sshkey(sshkey_folder,pubkey);
    gcp_credential_convert(workdir,"decrypt",0);
    get_cloud_flag(workdir,cloud_flag);
    sprintf(filename_temp,"%s%shpc_stack.base",stackdir,PATH_SLASH);

    sprintf(keyfile_path,"%s%s.key.json",vaultdir,PATH_SLASH);
    windows_path_to_string(keyfile_path,keyfile_path_ext);
    global_replace(filename_temp,"BLANK_CREDENTIAL_PATH",keyfile_path_ext);
    find_and_get(keyfile_path,"\"project_id\":","","",1,"\"project_id\":","","",'\"',4,gcp_project_id);
    global_replace(filename_temp,"BLANK_PROJECT",gcp_project_id);
    global_replace(filename_temp,"DEFAULT_REGION_ID",region_id);
    global_replace(filename_temp,"DEFAULT_ZONE_ID",zone_id);
    sprintf(string_temp,"%d",node_num);
    global_replace(filename_temp,"DEFAULT_NODE_NUM",string_temp);
    sprintf(string_temp,"%d",hpc_user_num);
    global_replace(filename_temp,"DEFAULT_USER_NUM",string_temp);
    global_replace(filename_temp,"DEFAULT_MASTERINI",master_init_param);
    global_replace(filename_temp,"DEFAULT_MASTER_PASSWD",master_passwd);
    global_replace(filename_temp,"DEFAULT_COMPUTE_PASSWD",compute_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ROOT_PASSWD",database_root_passwd);
    global_replace(filename_temp,"DEFAULT_DB_ACCT_PASSWD",database_acct_passwd);
    global_replace(filename_temp,"BLANK_URL_SHELL_SCRIPTS",url_shell_scripts_var);
    sprintf(string_temp,"%d",nfs_volume);
    global_replace(filename_temp,"DEFAULT_STORAGE_VOLUME",string_temp);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"RESOURCE_LABEL",unique_cluster_id);

    file_p=fopen(filename_temp,"a");
    sprintf(user_passwords,"%s%suser_passwords.txt",vaultdir,PATH_SLASH);
    file_p_2=fopen(user_passwords,"w+");
    for(i=0;i<hpc_user_num;i++){
        reset_string(user_passwd_temp);
        generate_random_passwd(user_passwd_temp);
        fprintf(file_p,"variable \"user%d_passwd\" {\n  type = string\n  default = \"%s\"\n}\n\n",i+1,user_passwd_temp);
        fprintf(file_p_2,"username: user%d %s STATUS:ENABLED\n",i+1,user_passwd_temp);
    }
    fclose(file_p);
    fclose(file_p_2);

    sprintf(filename_temp,"%s%shpc_stack.master",stackdir,PATH_SLASH);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"MASTER_INST",master_inst);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"RESOURCE_LABEL",unique_cluster_id);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.google_compute_image.OS_IMAGE.self_link",string_temp);
    }
    global_replace(filename_temp,"PUBLIC_KEY",pubkey);
    for(i=0;i<hpc_user_num;i++){
        sprintf(line_temp,"echo -e \"username: user%d ${var.user%d_passwd} STATUS:ENABLED\" >> /root/user_secrets.txt",i+1,i+1);
        insert_lines(filename_temp,"master_private_ip",line_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);
    sprintf(line_temp,"echo -e \"export APPS_INST_SCRIPTS_URL=%s\\nexport APPS_INST_PKGS_URL=%s\" > /usr/hpc-now/appstore_env.sh",url_app_inst_root_var,url_app_pkgs_root_var);
    insert_lines(filename_temp,"master_private_ip",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.compute",stackdir,PATH_SLASH);
    global_replace(filename_temp,"CLOUD_FLAG",cloud_flag);
    global_replace(filename_temp,"COMPUTE_INST",compute_inst);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"RESOURCE_LABEL",unique_cluster_id);
    if(strcmp(os_image,"centos7")==0||strcmp(os_image,"centoss9")==0){
        global_replace(filename_temp,"OS_IMAGE",os_image);
    }
    else{
        sprintf(string_temp,"\"%s\"",os_image);
        global_replace(filename_temp,"data.google_compute_image.OS_IMAGE.self_link",string_temp);
    }
    sprintf(line_temp,"echo -e \"export INITUTILS_REPO_ROOT=%s\" >> /etc/profile",url_initutils_root_var);
    insert_lines(filename_temp,"mount",line_temp);

    sprintf(filename_temp,"%s%shpc_stack.database",stackdir,PATH_SLASH);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"RESOURCE_LABEL",unique_cluster_id);

    sprintf(filename_temp,"%s%shpc_stack.natgw",stackdir,PATH_SLASH);
    global_replace(filename_temp,"RANDOM_STRING",randstr);
    global_replace(filename_temp,"RESOURCE_LABEL",unique_cluster_id);

    for(i=0;i<node_num;i++){
        sprintf(cmdline,"%s %s%shpc_stack.compute %s%shpc_stack_compute%d.tf %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,i+1,SYSTEM_CMD_REDIRECT);
        system(cmdline);
        sprintf(filename_temp,"%s%shpc_stack_compute%d.tf",stackdir,PATH_SLASH,i+1);
        sprintf(string_temp,"compute%d",i+1);
        global_replace(filename_temp,"COMPUTE_NODE_N",string_temp);
        /*sprintf(line_temp,"echo -e \"export SCRIPTS_URL_ROOT=%s\" >> /etc/profile",url_shell_scripts_var);
        insert_lines(filename_temp,"var.cluster_init_scripts",line_temp);*/
    }
    generate_tf_files(stackdir);
    if(tofu_execution(tf_exec,"init",workdir,crypto_keyfile,0)!=0){
        return 5;
    }
    if(tofu_execution(tf_exec,"apply",workdir,crypto_keyfile,1)!=0){
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Rolling back and exit now ...\n");
        if(tofu_execution(tf_exec,"destroy",workdir,crypto_keyfile,1)==0){
            delete_decrypted_files(workdir,crypto_keyfile);
            clear_if_failed(stackdir,confdir,vaultdir,3);
            printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Successfully rolled back and destroyed the residual resources.\n");
            printf("|          Please run " HIGH_GREEN_BOLD "hpcopr viewlog --err --hist" RESET_DISPLAY " for details.\n");
            return 7;
        }
        printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " Failed to roll back. Please try 'hpcopr destroy' later.\n");
        delete_decrypted_files(workdir,crypto_keyfile);
        return 9;
    }
    sprintf(cmdline,"%s %s%shpc_stack_compute1.tf %s%scompute_template %s",COPY_FILE_CMD,stackdir,PATH_SLASH,stackdir,PATH_SLASH,SYSTEM_CMD_REDIRECT);   
    system(cmdline);
    getstate(workdir,crypto_keyfile);
    printf("[ STEP 3 ] Remote executing now, please wait %d seconds for this step ...\n",GENERAL_SLEEP_TIME*3);
    for(i=0;i<GENERAL_SLEEP_TIME*3;i++){
        printf("[ -WAIT- ] Still need to wait %d seconds ... \r",GENERAL_SLEEP_TIME*3-i);
        fflush(stdout);
        sleep(1);
    }
    printf(GENERAL_BOLD "[ -DONE- ]" RESET_DISPLAY " Remote execution commands sent.\n");
    sprintf(filename_temp,"%s%sterraform.tfstate",stackdir,PATH_SLASH);
    find_and_get(filename_temp,"\"name\": \"hpc_storage\",","","",40,"\"self_link\":","","",'\"',4,bucket_selflink);
    find_and_get(filename_temp,"\"name\": \"hpc_storage_key\",","","",20,"\"private_key\":","","",'\"',4,bucket_private_key);

    sprintf(filename_temp,"%s%sbucket_key.txt",vaultdir,PATH_SLASH);
    base64decode(bucket_private_key,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket_key.json","root","put","",0);

    sprintf(filename_temp,"%s%sbucket_info.txt",vaultdir,PATH_SLASH);
    save_bucket_info(randstr,region_id,bucket_selflink,"","","",filename_temp,cloud_flag);
    remote_copy(workdir,sshkey_folder,filename_temp,"/hpc_data/cluster_data/.bucket.info","root","put","",0);
    
    get_state_value(workdir,"master_public_ip:",master_address);
    sprintf(filename_temp,"%s%sCLUSTER_SUMMARY.txt",vaultdir,PATH_SLASH);
    file_p=fopen(filename_temp,"w+");
    fprintf(file_p,"HPC-NOW CLUSTER SUMMARY\nMaster Node IP: %s\nMaster Node Root Password: %s\n\nNetDisk Address: gs: %s\nNetDisk Region: %s\nNetDisk Self Link: %s\nNetDisk Short Link: gs://%s\n",master_address,master_passwd,randstr,region_id,bucket_selflink,randstr);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",database_root_passwd,database_acct_passwd);
    fprintf(file_p,"+----------------------------------------------------------------+\n");
    fprintf(file_p,"%s\n%s\n",master_passwd,compute_passwd);
	fclose(file_p);
    remote_exec(workdir,sshkey_folder,"connect",7);
    remote_exec(workdir,sshkey_folder,"all",8);
    sprintf(filename_temp,"%s%scloud_flag.flg",vaultdir,PATH_SLASH);
    if(file_exist_or_not(filename_temp)!=0){
        sprintf(cmdline,"echo %s > %s%scloud_flag.flg",cloud_flag,vaultdir,PATH_SLASH);
        system(cmdline);
    }
    printf(GENERAL_BOLD "[ -INFO- ]" RESET_DISPLAY " After the initialization:\n|\n");
    graph(workdir,crypto_keyfile,0);
    printf("|\n");
    time(&current_time_long);
    time_p=gmtime(&current_time_long);
    sprintf(current_date,"%d-%d-%d",time_p->tm_year+1900,time_p->tm_mon+1,time_p->tm_mday);
    sprintf(current_time,"%d:%d:%d",time_p->tm_hour,time_p->tm_min,time_p->tm_sec);
    master_vcpu=get_cpu_num(master_inst);
    compute_vcpu=get_cpu_num(compute_inst);
    database_vcpu=2;
    natgw_vcpu=2;
    if(*(master_inst+0)=='a'){
        strcpy(master_cpu_vendor,"amd64");
    }
    else{
        strcpy(master_cpu_vendor,"intel64");
    }
    if(*(compute_inst+0)=='a'){
        strcpy(compute_cpu_vendor,"amd64");
    }
    else{
        strcpy(compute_cpu_vendor,"intel64");
    }
    strcpy(usage_logfile,USAGE_LOG_FILE);
    file_p=fopen(usage_logfile,"a+");
    fprintf(file_p,"%s-%s,%s,master,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,master_vcpu,current_date,current_time,master_cpu_vendor,region_id);
    fprintf(file_p,"%s-%s,%s,database,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,database_vcpu,current_date,current_time,region_id);
    fprintf(file_p,"%s-%s,%s,natgw,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,intel64,%s\n",cluster_id,randstr,cloud_flag,natgw_vcpu,current_date,current_time,region_id);
    for(i=0;i<node_num;i++){
        fprintf(file_p,"%s-%s,%s,compute%d,%d,%s,%s,RUNNING_DATE,RUNNING_TIME,NULL1,NULL2,%s,%s\n",cluster_id,randstr,cloud_flag,i+1,compute_vcpu,current_date,current_time,compute_cpu_vendor,region_id);
    }
    fclose(file_p);
    get_latest_hosts(stackdir,filename_temp);
    remote_copy(workdir,sshkey_folder,filename_temp,"/root/hostfile","root","put","",0);
    sync_statefile(workdir,sshkey_folder);
    sprintf(cmdline,"%s %s%s.%s %s", MKDIR_CMD,sshkey_folder,PATH_SLASH,cluster_id,SYSTEM_CMD_REDIRECT);
    system(cmdline);
    get_user_sshkey(cluster_id,"root","ENABLED",sshkey_folder);
    for(i=0;i<hpc_user_num;i++){
        sprintf(string_temp,"user%d",i+1);
        get_user_sshkey(cluster_id,string_temp,"ENABLED",sshkey_folder);
    }
    print_cluster_init_done();
    delete_decrypted_files(workdir,crypto_keyfile);
    gcp_credential_convert(workdir,"delete",0);
    return 0;
}