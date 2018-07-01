/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/******************************************************************************

                  版权所有 (C), 2001-2012, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ScInterface.h
  版 本 号   : 初稿
  作    者   : 王笑非
  生成日期   : 2012年4月3日
  最近修改   :
  功能描述   : SC模块提供给外部模块的接口
  函数列表   :
  修改历史   :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增

******************************************************************************/

#ifndef __SCINTERFACE_H__
#define __SCINTERFACE_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "vos.h"
#include "UsimPsInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)


/*****************************************************************************
  1 消息头定义
*****************************************************************************/


/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define   SC_MAX_SUPPORT_CATEGORY   (3)   /* 支持的锁网锁卡CATEGORY类别数，目前只支持三种:network/network subset/SP */

#define   SC_MAX_CODE_LEN           (4)   /* 号段的最大长度(锁网为3，锁子网为4，锁SP为4，取上限) */

#define   SC_CP_MAX_CODE_LEN        (6)   /* 锁CP为5个字节 + 1字节对齐*/

#define   SC_MAX_CODE_NUM           (20)  /* 锁网锁卡号段最大个数 */

#define   SC_SIGNATURE_LEN          (32)  /* 签名长度 */

#if (FEATURE_ON == FEATURE_SC_SEC_UPDATE)

#define   SC_AUTH_PUB_KEY_LEN       (516) /* 单个鉴权公钥长度(AK明文加lenth) */

#define   SC_AUTH_PUB_KEY_TOTAL_LEN (SC_AUTH_PUB_KEY_LEN * 2) /* 2个鉴权公钥长度(内容加lenth) */

#define   SC_RSA_ENCRYPT_LEN        (256) /* 经过RSA加密后的密文长度升级版本，落地曼哈顿及后续新版本 */

#define   SC_CRYPTO_PWD_LEN         (32)  /* 密码的密文长度,pbkdf2加密 */
#else

#define   SC_AUTH_PUB_KEY_LEN       (260) /* 单个鉴权公钥长度(AK明文加lenth) */

#define   SC_AUTH_PUB_KEY_TOTAL_LEN (SC_AUTH_PUB_KEY_LEN * 2) /* 2个鉴权公钥长度(内容加lenth) */

#define   SC_RSA_ENCRYPT_LEN        (128) /* 经过RSA加密后的密文长度 */

#define   SC_CRYPTO_PWD_LEN         (16)  /* 密码的密文长度,AES_ECB加密 */
#endif

#define   SC_PERS_PWD_LEN           (16)  /* CK/UK/DK密码的明文长度 */

#define   SC_PI_HEAD_FLAG           (0x5A5AA5A5)

#define   SC_PIN_PWD_LEN            (16)  /* PIN密码的明文长度 */

#define   SC_IV_LEN                 (4)  /* IV长度 注:IV长度为16字节，用INT32存储 */

#define   SC_CRYPTO_CBC_PIN_LEN     (16)  /* 密码的密文长度AES_CCB加密 */

/*****************************************************************************
  3 枚举定义
*****************************************************************************/
/*****************************************************************************
枚举名    : SC_ERROR_CODE_ENUM
枚举说明  : SC模块错误码枚举定义

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
enum SC_ERROR_CODE_ENUM
{
    SC_ERROR_CODE_NO_ERROR              = 0,        /*    操作成功 */
    SC_ERROR_CODE_OPEN_FILE_FAIL,                   /* 1  打开文件失败 */
    SC_ERROR_CODE_READ_FILE_FAIL,                   /* 2  读取文件失败 */
    SC_ERROR_CODE_WRITE_FILE_FAIL,                  /* 3  写入文件失败 */
    SC_ERROR_CODE_VERIFY_SIGNATURE_FAIL,            /* 4  签名校验失败 */
    SC_ERROR_CODE_DK_INCORRECT,                     /* 5  DK密码不正确 */
    SC_ERROR_CODE_UNLOCK_KEY_INCORRECT,             /* 6  CK/UK密码不正确 */
    SC_ERROR_CODE_UNLOCK_STATUS_ABNORMAL,           /* 7  解锁/UNBLOCK时，状态不正确 */
    SC_ERROR_CODE_RSA_ENCRYPT_FAIL,                 /* 8  RSA加密失败 */
    SC_ERROR_CODE_RSA_DECRYPT_FAIL,                 /* 9  RSA解密失败 */
    SC_ERROR_CODE_VERIFY_PUB_KEY_SIGNATURE_FAIL,    /* 10 鉴权公钥的摘要签名校验失败 */
    SC_ERROR_CODE_GET_RAND_NUMBER_FAIL,             /* 11 获取随机数失败(crypto_rand) */
    SC_ERROR_CODE_IDENTIFY_FAIL,                    /* 12 产线鉴权失败 */
    SC_ERROR_CODE_IDENTIFY_NOT_FINISH,              /* 13 还未完成产线鉴权 */
    SC_ERROR_CODE_LOCK_CODE_INVALID,                /* 14 锁网号段本身取值不正确 */
    SC_ERROR_CODE_CREATE_KEY_FAIL,                  /* 15 密钥生成失败(create_crypto_key) */
    SC_ERROR_CODE_GENERATE_HASH_FAIL,               /* 16 生成HASH失败(crypto_hash) */
    SC_ERROR_CODE_HMAC_ENCRYPT_FAIL,                /* 17 hmac算法生成完整性签名失败(crypto_encrypt) */
    SC_ERROR_CODE_PBKDF2_ENCRYPT_FAIL,              /* 18 PBKDF2算法加密失败(crypto_encrypt) */
    SC_ERROR_CODE_AES_ECB_ENCRYPT_FAIL,             /* 19 AES ECB算法加密失败(crypto_encrypt) */
    SC_ERROR_CODE_WRITE_HUK_FAIL,                   /* 20 HUK写入错误 */
    SC_ERROR_CODE_ALLOC_MEM_FAIL,                   /* 21 申请内存失败 */
    SC_ERROR_CODE_PARA_FAIL,                        /* 22 传入参数错误 */
    SC_ERROR_CODE_WRITE_FILE_IMEI_FAIL,             /* 23 IMEI密文写入文件错误 */
    SC_ERROR_CODE_CMP_IMEI_FAIL,                    /* 24 比较IMEI字符串失败 */
    SC_ERROR_CODE_MODEM_ID_FAIL,                    /* 25 ModemID 错误 */
    SC_ERROR_CODE_NV_READ_FAIL,                     /* 26 NV读失败 */
    SC_ERROR_CODE_NV_WRITE_FAIL,                    /* 27 NV写失败 */
    SC_ERROR_CODE_SCCONTENT_WRITE_FAIL,             /* 28 SC文件写入备份区失败 */
    SC_ERROR_CODE_SCBACKUP_READ_FAIL,               /* 29 读取SC备份区失败 */
    SC_ERROR_CODE_MAGNUM_CMP_FAIL,                  /* 30 比较SC备份区标记失败 */
    SC_ERROR_CODE_SCFILE_RESTORE_FAIL,              /* 31 SC文件写入使用区失败 */
    SC_ERROR_CODE_SC_NO_NEED_RESTORE,               /* 32 SC文件不需要恢复 */
    SC_ERROR_CODE_CACHE_FILE_NOT_EXIT,              /* 33 缓存中不存在SC文件 */
    SC_ERROR_CODE_CACHE_FILE_LEN_ERR,               /* 34 缓存中SC文件内容长度与预期不一致 */
    SC_ERROR_CODE_CACHE_FILE_ERR,                   /* 35 缓存中SC文件内容与预期不一致 */
    SC_ERROR_CODE_NW_UPDATE_FORBIDDEN,              /* 36 网络锁未开启*/
    SC_ERROR_CODE_NONEED_CHECK_SIMSLOT,             /* 37 不需要检查随卡匹配 */
    SC_ERROR_CODE_HMAC_CHECK_FAIL,                  /* 38 HMAC完整性检查失败 */
    SC_ERROR_CODE_AES_CBC_ENCRYPT_FAIL,             /* 39 AES CBC算法加密失败 */
    SC_ERROR_CODE_AES_CBC_DECCRYPT_FAIL,            /* 40 AES CBC算法解密失败 */

    SC_ERROR_CODE_OTHER_ERROR,                      /*    其他错误 */

    SC_ERROR_CODE_BUTT
};
typedef VOS_UINT32  SC_ERROR_CODE_ENUM_UINT32;

/*****************************************************************************
枚举名    : SC_PERSONALIZATION_CATEGORY_ENUM
枚举说明  : 锁网锁卡的category类型

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
enum SC_PERSONALIZATION_CATEGORY_ENUM
{
    SC_PERSONALIZATION_CATEGORY_NETWORK                     = 0x00, /* category:锁网 */
    SC_PERSONALIZATION_CATEGORY_NETWORK_SUBSET              = 0x01, /* category:锁子网 */
    SC_PERSONALIZATION_CATEGORY_SERVICE_PROVIDER            = 0x02, /* category:锁SP */
    SC_PERSONALIZATION_CATEGORY_CORPORATE                   = 0x03, /* category:锁团体 */
    SC_PERSONALIZATION_CATEGORY_SIM_USIM                    = 0x04, /* category:锁(U)SIM卡 */

    SC_PERSONALIZATION_CATEGORY_BUTT
};
typedef VOS_UINT8 SC_PERSONALIZATION_CATEGORY_ENUM_UINT8;

/*****************************************************************************
枚举名    : SC_PERSONALIZATION_SUPPORT_LIST_ENUM
枚举说明  : 锁网锁卡支持名单类型

  1.日    期   : 2016年4月3日
    作    者   : d00212987
    修改内容   : 锁网锁卡项目新增
*****************************************************************************/
enum SC_PERSONALIZATION_SUPPORT_CORPORATE_ENUM
{
    SC_PERSONALIZATION_NOT_SUPPORT_CORPORATE          = 0x00, /* 不支持团体锁 */
    SC_PERSONALIZATION_SUPPORT_CORPORATE              = 0x01, /* 支持团体锁 */

    SC_PERSONALIZATION_SUPPORT_CORPORATE_BUTT
};
typedef VOS_UINT8 SC_PERSONALIZATION_SUPPORT_CORPORATE_ENUM_UINT8;

/*****************************************************************************
枚举名    : SC_PERSONALIZATION_SUPPORT_LIST_ENUM
枚举说明  : 锁网锁卡支持名单类型

  1.日    期   : 2016年4月3日
    作    者   : d00212987
    修改内容   : 锁网锁卡项目新增
*****************************************************************************/
enum SC_PERSONALIZATION_SUPPORT_LIST_ENUM
{
    SC_PERSONALIZATION_SUPPORT_WHITELIST              = 0x00, /* 白名单 */
    SC_PERSONALIZATION_SUPPORT_BLACKLIST              = 0x01, /* 黑名单 */

    SC_PERSONALIZATION_SUPPORT_BUTT
};
typedef VOS_UINT8 SC_PERSONALIZATION_SUPPORT_LIST_ENUM_UINT8;

/*****************************************************************************
 枚举名    : SC_PERSONALIZATION_PLATFORM_ENUM_UINT32
 枚举说明  : SC Modem锁制式枚举

  1.日    期   : 2016年01月06日
    作    者   : d00212987
    修改内容   : SC增加对CDMA制式支持新增
*****************************************************************************/
enum SC_PERSONALIZATION_PLATFORM_ENUM
{
    SC_PERSONALIZATION_PLATFORM_G,   /* 非电信模式锁网锁卡 */

    SC_PERSONALIZATION_PLATFORM_C,   /* 电信模式锁网锁卡 */

    SC_PERSONALIZATION_PLATFORM_BUT,
};
typedef VOS_UINT8 SC_PERSONALIZATION_PLATFORM_ENUM_UINT8;

/*****************************************************************************
枚举名    : SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM
枚举说明  : 锁网锁卡网络解锁功能枚举

  1.日    期   : 2016年4月3日
    作    者   : d00212987
    修改内容   : 锁网锁卡项目新增
*****************************************************************************/
enum SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM
{
    SC_PERSONALIZATION_NETWORT_UNLOCK_UNSUPPORT       = 0x00, /* 不支持网络解锁 */
    SC_PERSONALIZATION_NETWORT_UNLOCK_SUPPORT         = 0x01, /* 支持网络解锁 */

   SC_PERSONALIZATION_NETWORT_UNLOCK_BUTT
};
typedef VOS_UINT8 SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM_UINT8;

/*****************************************************************************
枚举名    : SC_PERSONALIZATION_INDICATOR_ENUM
枚举说明  : 指定的锁网锁卡的锁卡模式

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
enum SC_PERSONALIZATION_INDICATOR_ENUM
{
    SC_PERSONALIZATION_INDICATOR_INACTIVE                   = 0x00, /* 全不锁 */
    SC_PERSONALIZATION_INDICATOR_ACTIVE                     = 0x01, /* 全锁 */
    SC_PERSONALIZATION_INDICATOR_ACTIVE_MODEM0              = 0x02, /* 锁主卡 */

    SC_PERSONALIZATION_INDICATOR_BUTT
};
typedef VOS_UINT8 SC_PERSONALIZATION_INDICATOR_ENUM_UINT8;


/*****************************************************************************
枚举名    : SC_PERSONALIZATION_STATUS_ENUM
枚举说明  : 指定的锁网锁卡类型的状态

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
enum SC_PERSONALIZATION_STATUS_ENUM
{
    SC_PERSONALIZATION_STATUS_READY     = 0x00, /* 已经解锁 */
    SC_PERSONALIZATION_STATUS_PIN       = 0x01, /* 未解锁状态，需输入解锁码 */
    SC_PERSONALIZATION_STATUS_PUK       = 0x02, /* Block状态，需输入UnBlock码 */

    SC_PERSONALIZATION_STATUS_BUTT
};
typedef VOS_UINT8 SC_PERSONALIZATION_STATUS_ENUM_UINT8;


/*****************************************************************************
枚举名    : SC_VERIFY_SIMLOCK_RESULT_ENUM
枚举说明  : 锁卡校验的结果

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
enum SC_VERIFY_SIMLOCK_RESULT_ENUM
{
    SC_VERIFY_SIMLOCK_RESULT_READY            = 0x00, /* READY: MT is not pending for any simlock */
    SC_VERIFY_SIMLOCK_RESULT_PH_NET_PIN       = 0x01, /* PH-NET PIN: MT is waiting network personalization password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_NET_PUK       = 0x02, /* PH-NET PUK: MT is waiting network personalization unblocking password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PIN    = 0x03, /* PH-NETSUB PIN: MT is waiting network subset personalization password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_NETSUB_PUK    = 0x04, /* PH-NETSUB PUK: MT is waiting network subset personalization unblocking password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_SP_PIN        = 0x05, /* PH-SP PIN: MT is waiting service provider personalization password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_SP_PUK        = 0x06, /* PH-SP PUK: MT is waiting service provider personalization unblocking password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_CP_PIN        = 0x07, /* PH-CP PIN: MT is waiting corporate personalization password to be given */
    SC_VERIFY_SIMLOCK_RESULT_PH_CP_PUK        = 0x08, /* PH-CP PUK: MT is waiting corporate personalization unblocking password to be given */

    SC_VERIFY_SIMLOCK_RESULT_BUTT
};
typedef VOS_UINT8 SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8;


/*****************************************************************************
枚举名    : SC_PORT_STATUS_ENUM
枚举说明  : 端口锁状态(HSIC的OM端口)

  1.日    期   : 2012年4月4日
    作    者   : w00184875
    修改内容   : V7R1C51 锁网锁卡项目新增
*****************************************************************************/
enum SC_PORT_STATUS_ENUM
{
    SC_PORT_STATUS_OFF                  = 0x00, /* 端口关闭 */
    SC_PORT_STATUS_ON                   = 0x01, /* 端口打开 */

    SC_PORT_STATUS_BUTT
};
typedef VOS_UINT32 SC_PORT_STATUS_ENUM_UINT32;

/* Added by L47619 for V9R1 vSIM Project, 2013-8-27, begin */
/*****************************************************************************
枚举名    : SC_DH_KEY_TYPE_ENUM
枚举说明  : DH算法使用的秘钥类型

  1.日    期   : 2013年8月27日
    作    者   : L47619
    修改内容   : V9R1 vSIM项目修改

*****************************************************************************/
enum SC_DH_KEY_TYPE_ENUM
{
    SC_DH_KEY_SERVER_PUBLIC_KEY         = 0x00,             /* 服务器公钥 */
    SC_DH_KEY_MODEM_PUBLIC_KEY          = 0x01,             /* MODEM侧公钥 */
    SC_DH_KEY_MODEM_PRIVATE_KEY         = 0x02,             /* MODEM侧私钥 */

    SC_DH_KEY_BUTT
};
typedef VOS_UINT32 SC_DH_KEY_TYPE_ENUM_UINT32;
/* Added by L47619 for V9R1 vSIM Project, 2013-8-27, end */


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/*****************************************************************************
结构名    : SC_PH_LOCK_CODE_STRU
结构说明  : 锁网锁卡号段结构

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
   VOS_UINT8                            aucPhLockCodeBegin[SC_MAX_CODE_LEN];
   VOS_UINT8                            aucPhLockCodeEnd[SC_MAX_CODE_LEN];
}SC_PH_LOCK_CODE_STRU;

/*****************************************************************************
 结构名    : SC_SIMLOCK_CARD_FILE_INFO_STRU
 结构说明  : 锁网锁卡校验所需SIM卡文件内容

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
typedef struct
{
    USIMM_CARDAPP_STATUS_STRU           stUsimSimInfo;  /* GUL SIM状态 */
    USIMM_CARDAPP_STATUS_STRU           stCsimUimInfo;  /* CDMA SIM状态 */
    USIMM_CARDAPP_STATUS_STRU           stIsimInfo;     /* ISIM状态 */
}SC_SIMLOCK_CARD_FILE_INFO_STRU;

#if (FEATURE_ON == FEATURE_BOSTON_AFTER_FEATURE)
/*****************************************************************************
结构名    : SC_WRITE_SIMLOCK_DATA_STRU
结构说明  : 提供给NAS命令AT^SIMLOCKDATAWRITE写入SIMLOCK信息的数据结构，

  2.日    期   : 2016年05月07日
    作    者   : d00212987
    修改内容   : 锁网锁卡算法升级项目修改
*****************************************************************************/
typedef struct
{
    VOS_UINT32                                              ulHealdFlag;           /* 锁卡数据头信息标志 默认值0x5A5AA5A5 */
    VOS_UINT32                                              ulVersionInfo;         /* 锁网锁卡版本  默认值：1*/
    VOS_UINT8                                               ucSupportModemNum;     /* 支持modem数量，最多支持8个modem */
    VOS_UINT8                                               ucNeedSimlockModemNum; /* 需要锁卡的MODEM数量,用于工具回读校验 */
    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 acIndicator[8];        /* 每个数组下标对应modem Id,值表示锁网锁卡状态：未激活0/激活1/解锁2
                                                                                                例：ucSupportModemNum指示支持3个modem,则主modem0锁，modem1不锁,modem2锁的配置是
                                                                                                acIndicator[8] = {1,0,1,0,0,0,0,0}*/
    VOS_UINT8                                               ucSCDataCfg;           /* 所有modem使用相同锁卡数据：0 / 每个modem使用单独锁卡数据：1
                                                                                                 如头信息中ucSCDataCfg置1（针对一些奇葩运营商，每个modem使用单独锁卡数据），
                                                                                                 比如 3个modem，则每一层数据3份，modem number对应数组下标 */

    VOS_UINT8                                               ucSupportCategoryNum;  /* 支持多少层锁: 3/4/5/...层 */
    VOS_UINT16                                              ausCategoryDataSize[6];/* 每个数组下标填写对应每一层锁，数据填写该层锁的数据大小,单位字节
                                                                                                 例：ucSupportCategoryNum指示支持3层锁，则表示支持锁网、锁子网、锁SP,如果每层锁的数据大小为1k,2k,3k
                                                                                                 则 ausCategoryDataSize[6];  = {1024,2048,3072,0,0,0}
                                                                                                 这里面的每一层锁数据大小，如果不同的modem使用不同的锁卡数据，那大小是几个modem 锁数据的总和 */

    SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM_UINT8            enNetworkUnlock;       /* 不支持 0/支持1 网络解锁(如不支持网络解锁，空口不能加解锁，只要有一层锁支持网络加解锁，则该字段设成支持) */
    VOS_UINT8                                               aucRsv[31];            /* 保留字段 默认值为0*/

}SC_WRITE_SIMLOCK_HEAD_STRU;
#endif

#if (FEATURE_ON == FEATURE_SC_DATA_STRUCT_EXTERN)

/*****************************************************************************
结构名    : SC_PH_LOCK_CP_CODE_STRU
结构说明  : 锁网锁卡号段结构

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
typedef struct
{
   VOS_UINT8                            aucPhLockCodeBegin[SC_CP_MAX_CODE_LEN];
   VOS_UINT8                            aucPhLockCodeEnd[SC_CP_MAX_CODE_LEN];
}SC_PH_LOCK_CP_CODE_STRU;

/*****************************************************************************
结构名    : SC_SIMLOCK_CATEGORY_STRU
结构说明  : 提供给NAS获取SIMLOCK信息的基础数据结构，
            包括锁网锁卡的Catetory、对应Indicator、对应Status、最大解锁次数、剩余解锁次数
            号段数、号段数组的begin/end

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
typedef struct
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory          : 3;        /* 锁网锁卡的category类别指示 0/1/2/3/4层锁 */
    SC_PERSONALIZATION_PLATFORM_ENUM_UINT8                  enPlatform          : 1;        /* 锁网锁卡类型G:0 /C:1*/
    SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM_UINT8            enNetworkUnlock     : 1;        /* 网络加解锁 不支持0/支持1 */
    SC_PERSONALIZATION_SUPPORT_LIST_ENUM_UINT8              enSupportListType   : 2;        /* 锁网锁卡的名单模式 白名单0/黑名单1 */
    SC_PERSONALIZATION_SUPPORT_CORPORATE_ENUM_UINT8         enSupportCorporate  : 1;        /* 锁网锁卡是否支持团体锁 不支持0/支持1 */

    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator         : 4;        /* 锁网锁卡的激活指示  未激活0/激活1/锁主卡2 ... */
    VOS_UINT8                                               ucRsv               : 4;

    SC_PERSONALIZATION_STATUS_ENUM_UINT8                    enStatus;                    /* 锁网锁卡的当前状态 */
    VOS_UINT8                                               ucMaxUnlockTimes;            /* 锁网锁卡的最大解锁次数 */
    VOS_UINT8                                               ucRemainUnlockTimes;         /* 锁网锁卡的剩余解锁次数 */
    VOS_UINT8                                               aucReserved[2];              /* 保留字节，用于四字节对齐 */
    VOS_UINT8                                               ucGroupNum;                  /* 号段个数，一个begin/end算一个号段 */
    SC_PH_LOCK_CODE_STRU                                    astLockCode[SC_MAX_CODE_NUM];/* 锁网锁卡的号段内容*/
}SC_SIMLOCK_CATEGORY_STRU;

/*****************************************************************************
结构名    : SC_SIMLOCK_CP_CATEGORY_STRU
结构说明  : 提供给NAS获取SIMLOCK信息的基础数据结构，
            包括锁网锁卡的Catetory、对应Indicator、对应Status、最大解锁次数、剩余解锁次数
            号段数、号段数组的begin/end

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
typedef struct
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory          : 3;        /* 锁网锁卡的category类别指示 0/1/2/3/4层锁 */
    SC_PERSONALIZATION_PLATFORM_ENUM_UINT8                  enPlatform          : 1;        /* 锁网锁卡类型G:0 /C:1*/
    SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM_UINT8            enNetworkUnlock     : 1;        /* 网络加解锁 不支持0/支持1 */
    SC_PERSONALIZATION_SUPPORT_LIST_ENUM_UINT8              enSupportListType   : 2;        /* 锁网锁卡的名单模式 白名单0/黑名单1 */
    SC_PERSONALIZATION_SUPPORT_CORPORATE_ENUM_UINT8         enSupportCorporate  : 1;        /* 锁网锁卡是否支持团体锁 不支持0/支持1 */

    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator         : 4;        /* 锁网锁卡的激活指示  未激活0/激活1/锁主卡2 ... */
    VOS_UINT8                                               ucRsv               : 4;

    SC_PERSONALIZATION_STATUS_ENUM_UINT8                    enStatus;                    /* 锁网锁卡的当前状态 */
    VOS_UINT8                                               ucMaxUnlockTimes;            /* 锁网锁卡的最大解锁次数 */
    VOS_UINT8                                               ucRemainUnlockTimes;         /* 锁网锁卡的剩余解锁次数 */
    VOS_UINT8                                               aucReserved[2];              /* 保留字节，用于四字节对齐 */
    VOS_UINT8                                               ucGroupNum;                  /* 号段个数，一个begin/end算一个号段 */
    SC_PH_LOCK_CP_CODE_STRU                                 astLockCode[SC_MAX_CODE_NUM];/* 锁网锁卡的号段内容*/
}SC_SIMLOCK_CP_CATEGORY_STRU;

/*****************************************************************************
结构名    : SC_WRITE_SIMLOCK_CP_DATA_STRU
结构说明  : 提供给NAS命令AT^SIMLOCKDATAWRITE写入SIMLOCK信息的数据结构，

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

  2.日    期   : 2016年05月07日
    作    者   : d00212987
    修改内容   : 锁网锁卡算法升级项目修改
*****************************************************************************/
typedef struct
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory          : 3;        /* 锁网锁卡的category类别指示 0/1/2/3/4层锁 */
    SC_PERSONALIZATION_PLATFORM_ENUM_UINT8                  enPlatform          : 1;        /* 锁网锁卡类型G:0 /C:1*/
    SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM_UINT8            enNetworkUnlock     : 1;        /* 网络加解锁 不支持0/支持1 */
    SC_PERSONALIZATION_SUPPORT_LIST_ENUM_UINT8              enSupportListType   : 2;        /* 锁网锁卡的名单模式 白名单0/黑名单1 */
    SC_PERSONALIZATION_SUPPORT_CORPORATE_ENUM_UINT8         enSupportCorporate  : 1;        /* 锁网锁卡是否支持团体锁 不支持0/支持1 */

    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator         : 4;        /* 锁网锁卡的激活指示  未激活0/激活1/锁主卡2 ... */
    VOS_UINT8                                               ucRsv               : 4;

    VOS_UINT8                                               ucMaxUnlockTimes;            /* 锁网锁卡的最大解锁次数 */
    VOS_UINT8                                               ucGroupNum;                  /* 号段个数，一个begin/end算一个号段 */
    SC_PH_LOCK_CP_CODE_STRU                                 astLockCode[SC_MAX_CODE_NUM];/* 锁网锁卡的号段内容*/
    /* astLockCode摘要密文，生成过程：aucSimlockRsaData是由astLockCode先经过SHA256算法生成摘要，
       然后使用USB dog中的私钥进行加密的校验数据。 在simlock code合法性检查的时候，先使用AK-File中
       的公钥对aucSimlockRsaData进行解密，然后与astLockCode使用SHA256生成的摘要进行比较。*/
    VOS_UINT8                                               aucSimlockRsaData[SC_RSA_ENCRYPT_LEN];
    /* 锁卡的解锁密码，32位pbkdf2加密后的密文 */
    VOS_UINT8                                               aucScLockPin[SC_CRYPTO_PWD_LEN];
    /* 锁卡的解锁密码，32位pbkdf2加密后的密文 */
    VOS_UINT8                                               aucScLockPuk[SC_CRYPTO_PWD_LEN];
    /* 鉴权公钥的摘要经HMAC算法(密钥为SSK)计算生成的摘要 */
    VOS_UINT8                                               aucSignature[SC_SIGNATURE_LEN];
}SC_WRITE_SIMLOCK_CP_DATA_STRU;

/*****************************************************************************
结构名    : SC_WRITE_SIMLOCK_DATA_STRU
结构说明  : 提供给NAS命令AT^SIMLOCKDATAWRITE写入SIMLOCK信息的数据结构，

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

  2.日    期   : 2016年05月07日
    作    者   : d00212987
    修改内容   : 锁网锁卡算法升级项目修改
*****************************************************************************/
typedef struct
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory          : 3;        /* 锁网锁卡的category类别指示 0/1/2/3/4层锁 */
    SC_PERSONALIZATION_PLATFORM_ENUM_UINT8                  enPlatform          : 1;        /* 锁网锁卡类型G:0 /C:1*/
    SC_PERSONALIZATION_NETWORT_UNLOCK_ENUM_UINT8            enNetworkUnlock     : 1;        /* 网络加解锁 不支持0/支持1 */
    SC_PERSONALIZATION_SUPPORT_LIST_ENUM_UINT8              enSupportListType   : 2;        /* 锁网锁卡的名单模式 白名单0/黑名单1 */
    SC_PERSONALIZATION_SUPPORT_CORPORATE_ENUM_UINT8         enSupportCorporate  : 1;        /* 锁网锁卡是否支持团体锁 不支持0/支持1 */

    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator         : 4;        /* 锁网锁卡的激活指示  未激活0/激活1/锁主卡2 ... */
    VOS_UINT8                                               ucRsv               : 4;

    VOS_UINT8                                               ucMaxUnlockTimes;            /* 锁网锁卡的最大解锁次数 */
    VOS_UINT8                                               ucGroupNum;                  /* 号段个数，一个begin/end算一个号段 */
    SC_PH_LOCK_CODE_STRU                                    astLockCode[SC_MAX_CODE_NUM];/* 锁网锁卡的号段内容*/
    /* astLockCode摘要密文，生成过程：aucSimlockRsaData是由astLockCode先经过SHA256算法生成摘要，
       然后使用USB dog中的私钥进行加密的校验数据。 在simlock code合法性检查的时候，先使用AK-File中
       的公钥对aucSimlockRsaData进行解密，然后与astLockCode使用SHA256生成的摘要进行比较。*/
    VOS_UINT8                                               aucSimlockRsaData[SC_RSA_ENCRYPT_LEN];
#if (FEATURE_ON == FEATURE_SC_SEC_UPDATE)
    /* 锁卡的解锁密码，32位pbkdf2加密后的密文 */
    VOS_UINT8                                               aucScLockPin[SC_CRYPTO_PWD_LEN];
    /* 锁卡的解锁密码，32位pbkdf2加密后的密文 */
    VOS_UINT8                                               aucScLockPuk[SC_CRYPTO_PWD_LEN];
    /* 鉴权公钥的摘要经HMAC算法(密钥为SSK)计算生成的摘要 */
    VOS_UINT8                                               aucSignature[SC_SIGNATURE_LEN];

#else
    /* 锁卡的解锁密码，由16位的PIN码使用USB dog中的私钥进行加密的数据 */
    VOS_UINT8                                               aucScLockPin[SC_RSA_ENCRYPT_LEN];
    /* 锁卡的UnBlock密码，由16位的PUK码使用USB dog中的私钥进行加密的数据 */
    VOS_UINT8                                               aucScLockPuk[SC_RSA_ENCRYPT_LEN];
#endif
}SC_WRITE_SIMLOCK_DATA_STRU;
#else
/*****************************************************************************
结构名    : SC_SIMLOCK_CATEGORY_STRU
结构说明  : 提供给NAS获取SIMLOCK信息的基础数据结构，
            包括锁网锁卡的Catetory、对应Indicator、对应Status、最大解锁次数、剩余解锁次数
            号段数、号段数组的begin/end

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory;                  /* 锁网锁卡的category类别 */
    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator;                 /* 锁网锁卡的激活指示 */
    SC_PERSONALIZATION_STATUS_ENUM_UINT8                    enStatus;                    /* 锁网锁卡的当前状态 */
    VOS_UINT8                                               ucMaxUnlockTimes;            /* 锁网锁卡的最大解锁次数 */
    VOS_UINT8                                               ucRemainUnlockTimes;         /* 锁网锁卡的剩余解锁次数 */
    VOS_UINT8                                               aucReserved[2];              /* 保留字节，用于四字节对齐 */
    VOS_UINT8                                               ucGroupNum;                  /* 号段个数，一个begin/end算一个号段 */
    SC_PH_LOCK_CODE_STRU                                    astLockCode[SC_MAX_CODE_NUM];/* 锁网锁卡的号段内容*/
}SC_SIMLOCK_CATEGORY_STRU;

/*****************************************************************************
结构名    : SC_WRITE_SIMLOCK_DATA_STRU
结构说明  : 提供给NAS命令AT^SIMLOCKDATAWRITE写入SIMLOCK信息的数据结构，

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

  2.日    期   : 2016年05月07日
    作    者   : d00212987
    修改内容   : 锁网锁卡算法升级项目修改
*****************************************************************************/
typedef struct
{
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8                  enCategory;                  /* 锁网锁卡的category类别 */
    SC_PERSONALIZATION_INDICATOR_ENUM_UINT8                 enIndicator;                 /* 锁网锁卡的激活指示 */
    VOS_UINT8                                               ucMaxUnlockTimes;            /* 锁网锁卡的最大解锁次数 */
    VOS_UINT8                                               ucGroupNum;                  /* 号段个数，一个begin/end算一个号段 */
    SC_PH_LOCK_CODE_STRU                                    astLockCode[SC_MAX_CODE_NUM];/* 锁网锁卡的号段内容*/
    /* astLockCode摘要密文，生成过程：aucSimlockRsaData是由astLockCode先经过SHA256算法生成摘要，
       然后使用USB dog中的私钥进行加密的校验数据。 在simlock code合法性检查的时候，先使用AK-File中
       的公钥对aucSimlockRsaData进行解密，然后与astLockCode使用SHA256生成的摘要进行比较。*/
    VOS_UINT8                                               aucSimlockRsaData[SC_RSA_ENCRYPT_LEN];
#if (FEATURE_ON == FEATURE_SC_SEC_UPDATE)
    /* 锁卡的解锁密码，32位pbkdf2加密后的密文 */
    VOS_UINT8                                               aucScLockPin[SC_CRYPTO_PWD_LEN];
    /* 锁卡的解锁密码，32位pbkdf2加密后的密文 */
    VOS_UINT8                                               aucScLockPuk[SC_CRYPTO_PWD_LEN];
    /* 鉴权公钥的摘要经HMAC算法(密钥为SSK)计算生成的摘要 */
    VOS_UINT8                                               aucSignature[SC_SIGNATURE_LEN];

#else
    /* 锁卡的解锁密码，由16位的PIN码使用USB dog中的私钥进行加密的数据 */
    VOS_UINT8                                               aucScLockPin[SC_RSA_ENCRYPT_LEN];
    /* 锁卡的UnBlock密码，由16位的PUK码使用USB dog中的私钥进行加密的数据 */
    VOS_UINT8                                               aucScLockPuk[SC_RSA_ENCRYPT_LEN];
#endif
}SC_WRITE_SIMLOCK_DATA_STRU;

#endif

/*****************************************************************************
结构名    : SC_SET_FAC_AUTH_PUB_KEY_STRU
结构说明  : 提供给NAS命令AT^FACAUTHPUBKEY写入鉴权公钥的数据结构

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

  2.日    期   : 2016年05月07日
    作    者   : d00212987
    修改内容   : 锁网锁卡算法升级项目修改
*****************************************************************************/
typedef struct
{
    VOS_UINT8       aucAuthPubKey[SC_AUTH_PUB_KEY_TOTAL_LEN];   /* 鉴权公钥:包含2个公钥及长度 */
    VOS_UINT8       aucSignature[SC_SIGNATURE_LEN];       /* 鉴权公钥的摘要经AES-ECB算法(密钥为SSK)计算生成的摘要签名，新版本是hmac生成 */
}SC_SET_FAC_AUTH_PUB_KEY_STRU;


/*****************************************************************************
结构名    : SC_IDENTIFY_START_RESPONSE_STRU
结构说明  : 提供给NAS命令AT^IDENTIFYSTART用于获取<identify_start_response>码流的数据结构

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
    /* SC获取随机数并采用公钥进行RSA加密后的码流 */
    VOS_UINT8       aucIdentifyStartRsp[SC_RSA_ENCRYPT_LEN];
}SC_IDENTIFY_START_RESPONSE_STRU;


/*****************************************************************************
结构名    : SC_IDENTIFY_END_REQUEST_STRU
结构说明  : NAS将命令AT^IDENTIFYEND中的<identify_end_request>码流提交给SC时使用的数据结构

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
    /* 后台通过私钥和<identify_start_response>解密生成随机数，再用私钥对随机数加密生成<identify_end_request>，
       通过命令^IDENTIFYEND参数<identify_end_request>发送给协议栈，协议栈使用该数据和产线鉴权公钥还原随机数，
       若还原随机数与原随机数一致则鉴权通过，否则，鉴权失败 */
    VOS_UINT8       aucIdentifyEndReq[SC_RSA_ENCRYPT_LEN];
}SC_IDENTIFY_END_REQUEST_STRU;


/*****************************************************************************
结构名    : SC_SET_PORT_ATTRIBUTE_STRU
结构说明  : NAS将命令AT^PORTATTRSET中的<onoff>和<port_passwd>提交给SC时使用的数据结构

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成
*****************************************************************************/
typedef struct
{
    /* AT命令^IDENTIFYEND中的参数<onoff>的值 */
    SC_PORT_STATUS_ENUM_UINT32          enStatus;

    /* AT命令^IDENTIFYEND中的参数<port_passwd>的值，<port_passwd>为16位密码（密码长度
       为固定的16个“0”-“9”字符）经过RSA加密生成的128字节密文 */
    VOS_UINT8                           aucRsaPwd[SC_RSA_ENCRYPT_LEN];
}SC_SET_PORT_ATTRIBUTE_STRU;

/*****************************************************************************
 结构名    : SC_CK_CATEGORY_STRU
 结构说明  : 每一个类别的CK/UK密码保存方式

  1.日    期   : 2012年4月7日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucCKCryptoPwd[SC_CRYPTO_PWD_LEN]; /* CK密文 */
    VOS_UINT8                           aucUKCryptoPwd[SC_CRYPTO_PWD_LEN]; /* UK密文 */
}SC_CK_CATEGORY_STRU;


/*****************************************************************************
 结构名    : SC_PI_FILE_STRU
 结构说明  : PI文件的数据结构

  1.日    期   : 2012年04月07日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构
*****************************************************************************/

typedef struct
{
    /* 目前支持3种category，结构体数组按照network->network subset->SP的顺序排列 */
    SC_SIMLOCK_CATEGORY_STRU            astSimlockCategory[SC_MAX_SUPPORT_CATEGORY];
}SC_PI_FILE_STRU;

/*****************************************************************************
 结构名    : SC_CK_FILE_STRU
 结构说明  : CK-FILE的存储结构
             包括3种Category的相应的CK/UK密码，密文存储

  1.日    期   : 2012年04月07日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构
*****************************************************************************/
typedef struct
{
    /* 目前支持3种category，结构体数组按照network->network subset->SP的顺序排列 */
    SC_CK_CATEGORY_STRU                 astCKCategory[SC_MAX_SUPPORT_CATEGORY];
}SC_CK_FILE_STRU;

/*****************************************************************************
 结构名    : SC_DK_FILE_STRU
 结构说明  : DK文件的数据结构

  1.日    期   : 2012年04月07日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构
*****************************************************************************/
typedef struct
{
    SC_PORT_STATUS_ENUM_UINT32          enPortStatus;       /* 端口锁状态，明文 */
    VOS_UINT8                           aucDKCryptoPwd[SC_CRYPTO_PWD_LEN]; /* DK密文 */
}SC_DK_FILE_STRU;

/*****************************************************************************
 结构名    : SC_AK_FILE_STRU
 结构说明  : AK文件的数据结构

  1.日    期   : 2012年04月07日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构

  2.日    期   : 2016年05月07日
    作    者   : d00212987
    修改内容   : 锁网锁卡算法升级项目修改
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucAuthPubKey_A[SC_AUTH_PUB_KEY_LEN];    /* AK1公钥及公钥长度 */
    VOS_UINT8                           aucAuthPubKey_B[SC_AUTH_PUB_KEY_LEN];    /* AK2公钥及公钥长度 */
}SC_AK_FILE_STRU;

/*****************************************************************************
 结构名    : SC_SIGN_FILE_STRU
 结构说明  : 签名文件的数据结构

  1.日    期   : 2012年04月09日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucSignature[SC_SIGNATURE_LEN];    /* 签名文件码流 */
}SC_SIGN_FILE_STRU;

/*****************************************************************************
结构名    : SC_SIMLOCK_INFO_STRU
结构说明  : 提供给NAS获取SIMLOCK信息的数据结构，
            包括3种Category的相应锁网锁卡信息

  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增结构
*****************************************************************************/
typedef SC_PI_FILE_STRU SC_SIMLOCK_INFO_STRU;

/*****************************************************************************
 结构名    : SC_IMEI_SEC_STRU
 结构说明  : 产线下发加密IMEI

  1.日    期   : 2017年07月07日
    作    者   : d00212987
    修改内容   : 新增结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           *pucRsaIMEI;                    /* Rsa加密后的秘文 */
    VOS_UINT32                           ulRsaIMEILen;                  /* Rsa加密后的秘文长度 */
    VOS_UINT8                           *pucHmac;                       /* hmac签名 */
    VOS_UINT32                           ulHmacLen;                     /* hmac签名长度 */
}SC_IMEI_SEC_STRU;

/*****************************************************************************
 结构名    : SC_CRYPTO_PIN_STRU
 结构说明  :

  1.日    期   : 2017年07月07日
    作    者   : d00212987
    修改内容   : 新增结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                            aucCryptoPin[SC_CRYPTO_CBC_PIN_LEN];   /* AES加密后的秘文 */
    VOS_UINT32                           aulPinIv[SC_IV_LEN];                   /* IV值 */
    VOS_UINT8                            aucHmacValue[SC_SIGNATURE_LEN];        /* hmac签名 */
}SC_CRYPTO_PIN_STRU;

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  10 函数声明
*****************************************************************************/
/*****************************************************************************
 函 数 名  : SC_PERS_GetSimlockInfo
 功能描述  : NAS通过该接口获取SIMLOCK信息
 输入参数  : pstSimlockInfo:获取SIMLOCK信息的数据结构，由调用者申请内存
 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_GetSimlockInfo(SC_SIMLOCK_INFO_STRU *pstSimlockInfo);


#if (FEATURE_ON == FEATURE_SC_NETWORK_UPDATE)
/*****************************************************************************
 函 数 名  : SC_PERS_WriteNWSimLockData
 功能描述  : NAS收到AT^SIMLOCKNWUPDATE命令，通过该接口写入SIMLOCK信息
 输入参数  : ulLayer 锁卡写那一层数据
             pucSimlockData:   锁网锁卡数据
             ulSimlockDataLen: 锁网锁卡数据长度
             puchmac:          锁网锁卡数据hmac完整性校验码流
             ulhmacLen:        锁网锁卡数据hmac完整性校验码流长度
 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32类型的SC错误码

  1.日    期   : 2017年03月6日
    作    者   : l00383907
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_WriteNWSimLockData(
    VOS_UINT32                           ulLayer,
    VOS_UINT8                           *pucSimlockData,
    VOS_UINT32                           ulSimlockDataLen,
    VOS_UINT8                           *pucHmac,
    VOS_UINT32                           ulHmacLen
);
#endif

#if ((FEATURE_ON == FEATURE_SC_DATA_STRUCT_EXTERN) || (FEATURE_ON == FEATURE_BOSTON_AFTER_FEATURE))
/*****************************************************************************
 函 数 名  : SC_PERS_WriteSimLockDataEx
 功能描述  : NAS收到AT^SIMLOCKDATAWRITEEX命令，通过该接口写入SIMLOCK信息
 输入参数  : ulLayer:          锁卡写那一层数据
             pucSimlockData:   锁网锁卡数据长度
             ulSimlockDataLen: 锁网锁卡数据
             puchmac:          锁网锁卡数据hmac完整性校验码流
             ulhmacLen:        锁网锁卡数据hmac完整性校验码流长度

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32类型的SC错误码

  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_WriteSimLockDataEx(
    VOS_UINT32                          ulLayer,
    VOS_UINT8                          *pucSimlockData,
    VOS_UINT32                          ulSimlockDataLen,
    VOS_UINT8                          *pucHmac,
    VOS_UINT32                          ulHmacLen
);

/*****************************************************************************
 函 数 名  : SC_PERS_ReadSimLockDataEx
 功能描述  : NAS通过该接口获取锁卡数据结构扩展后的某一层锁网锁卡信息
 输入参数  : ulLayer            : 指示获取那一层锁卡数据
             pucSimlockDatabuf  : 获取SIMLOCK信息空间，由调用者申请内存
             ulSimlockDatabufLen: 空间长度

 输出参数  : pulSimlockDataLen   : 真实锁卡数据长度

 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_ReadSimLockDataEx(
    VOS_UINT32                           ulLayer,
    VOS_UINT8                           *pucSimlockDatabuf,
    VOS_UINT32                           ulSimlockDatabufLen,
    VOS_UINT32                          *pulSimlockDataLen
);
#else

/*****************************************************************************
 函 数 名  : SC_PERS_WriteSimLockData
 功能描述  : NAS收到AT^SIMLOCKDATAWRITE命令，通过该接口写入SIMLOCK信息
 输入参数  : pstWriteSimLockData: 需写入PI-FILE和CK-File的SIMLOCK信息

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_WriteSimLockData(SC_WRITE_SIMLOCK_DATA_STRU *pstWriteSimLockData);
#endif

#if (FEATURE_ON == FEATURE_SC_DATA_STRUCT_EXTERN)

/*****************************************************************************
 函 数 名  : SC_PERS_GetSimLockDataLayerLen
 功能描述  : NAS通过该接口获取锁卡数据结构扩展后的某一层锁网锁卡信息大小
 输入参数  : ulLayer            : 指示获取那一层锁卡数据大小

 输出参数  : pulSimlockDataLen 锁网锁卡数据长度

 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_GetSimLockDataLayerLen(
    VOS_UINT32                           ulLayer,
    VOS_UINT32                          *pulSimlockDataLen
);

/*****************************************************************************
 函 数 名  : SC_PERS_GetSimlockCPInfo
 功能描述  : NAS通过该接口获取第四层 SIMLOCK信息
 输入参数  : pstSimlockInfo:获取SIMLOCK信息的数据结构，由调用者申请内存
 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

 修改历史      :
  1.日    期   : 2017年03月1日
    作    者   : d00212987
    修改内容   : Dallas C60 锁网锁卡数据结构扩展项目新增
*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_GetSimlockCPInfo(SC_SIMLOCK_CP_CATEGORY_STRU *pstSimlockCpInfo);
#endif

/*****************************************************************************
 函 数 名  : SC_PERS_VerifySimLock
 功能描述  : 进行锁网锁卡校验
 输入参数  : enModemID:Modem ID

 输出参数  : pucSimLockResult:锁网锁卡状态
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年04月07日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增函数

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_VerifySimLock(
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 *penVerifySimlockRslt,
    MODEM_ID_ENUM_UINT16                 enModemID
);

/*****************************************************************************
 函 数 名  : SC_PERS_NoVerifyNvReadImei
 功能描述  : 读取存在NV里的IMEI
             说明:不去校验，直接去读NV,区别于NV_Read接口，SC_PERS_NoVerifyNvReadImei接口在modem2 IMEI没写的情况下，读取modem0
 输入参数  : enModemID : Modem ID
             usID      : Nv id
             ulLength  : Nv 长度

 输出参数  : *pItem    : 获取NV 内容
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

 修改历史      :
  1.日    期   : 2015年11月30日
    作    者   : d00212987
    修改内容   : IMEI验证
*****************************************************************************/
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_NoVerifyNvReadImei(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT16                          usID,
    VOS_VOID                           *pItem,
    VOS_UINT32                          ulLength
);

/*****************************************************************************
 函 数 名  : SC_PERS_VerifyNvReadImei
 功能描述  : 读取存在NV里的IMEI,增加IMEI验证(返回值为:SC_ERROR_CODE_NO_ERROR,读取的NV内容可用，否则不可用)
             说明:先校验IMEI号的合法性，然后再读取NV，如果IMEI号非法返回全0 IMEI,并且在modem2 imei 没写的情况下，读取modem0
 输入参数  : enModemID : Modem ID
             usID      : Nv id
             ulLength  : Nv 长度

 输出参数  : *pItem    : 获取NV 内容
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

 修改历史      :
  1.日    期   : 2013年5月25日
    作    者   : d00212987
    修改内容   : IMEI验证
*****************************************************************************/
SC_ERROR_CODE_ENUM_UINT32 SC_PERS_VerifyNvReadImei(
    MODEM_ID_ENUM_UINT16                enModemID,
    VOS_UINT16                          usID,
    VOS_VOID                           *pItem,
    VOS_UINT32                          ulLength
);

/*****************************************************************************
 函 数 名  : SC_PERS_UnLock
 功能描述  : 解锁或UNBLOC操作
 输入参数  : pcPwd: 解锁密码
             ucLen: 密码长度
             enModemID:Modem ID
 输出参数  : pucSimLockResult:解锁操作后的锁网锁卡状态
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年04月07日
    作    者   : w00184875
    修改内容   : AP-Modem锁网锁卡项目新增函数

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_PERS_UnLock(
    VOS_UINT8                           *pucPwd,
    VOS_UINT8                            ucLen,
    SC_VERIFY_SIMLOCK_RESULT_ENUM_UINT8 *penVerifySimlockRslt,
    MODEM_ID_ENUM_UINT16                 enModemID
);

/*****************************************************************************
 函 数 名  : SC_FAC_SetFacAuthPubKey
 功能描述  : NAS收到AT^FACAUTHPUBKEY后，通过该接口写入鉴权公钥
 输入参数  : pstSetFacAuthPubKey: 鉴权公钥数据结构

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_SetFacAuthPubKey(SC_SET_FAC_AUTH_PUB_KEY_STRU *pstSetFacAuthPubKey);


/*****************************************************************************
 函 数 名  : SC_FAC_StartIdentify
 功能描述  : NAS收到AT^IDENTIFYSTART后，通过该接口获取<identify_start_response>码流
 输入参数  : NA

 输出参数  : pstIdentifyStartRsp: 用于获取<identify_start_response>码流的数据结构，
             由调用者申请内存
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_StartIdentify(SC_IDENTIFY_START_RESPONSE_STRU *pstIdentifyStartRsp);


/*****************************************************************************
 函 数 名  : SC_FAC_EndIdentify
 功能描述  : NAS收到AT^IDENTIFYEND命令后，通过该接口将<identify_end_request>码流提交给SC
 输入参数  : pstIdentifyEndReq: 用于存储<identify_end_request>码流的数据结构

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_EndIdentify(SC_IDENTIFY_END_REQUEST_STRU *pstIdentifyEndReq);


/*****************************************************************************
 函 数 名  : SC_FAC_RsaEncrypt
 功能描述  : NAS收到AT^PHONEPHYNUM?查询命令，通过该接口对物理号进行RSA加密
 输入参数  : pcRawData: 物理号，明文存储
             ulLen: 物理号长度
             pcCipherData: RSA加密后的密文
             pulCipherLen: RSA加密后的密文长度

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_RsaEncrypt(
    VOS_UINT8   *pucRawData,
    VOS_UINT32   ulLen,
    VOS_UINT8   *pucCipherData,
    VOS_UINT32  *pulCipherLen
);


/*****************************************************************************
 函 数 名  : SC_FAC_RsaDecrypt
 功能描述  : NAS收到AT^PHONEPHYNUM设置命令，通过该接口对物理号进行RSA解密
 输入参数  : pcCipherData: 物理号，密文存储
             ucCipherLen: 物理号密文长度
             pcRawData: RSA解密后的物理号明文
             pucLen: RSA解密后的物理号明文长度

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_RsaDecrypt(
    VOS_UINT8  *pucCipherData,
    VOS_UINT32  ulCipherLen,
    VOS_UINT8  *pucRawData,
    VOS_UINT32 *pulLen
);

/*****************************************************************************
 函 数 名  : SC_FAC_WriteHUK
 功能描述  : NAS收到AT^HUK命令，通过该SC封装的接口写入HUK
 输入参数  : pcHuk: HUK码流，128bit
             ucLen: HUK码流长度

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_WriteHUK(VOS_UINT8 *pucHuk, VOS_UINT32 ulLen);

/*****************************************************************************
 函 数 名  : SC_FAC_WriteIMEI
 功能描述  : NAS收到AT^PHONEPHYNUM令，通过该SC封装的接口把IMEI密文写入IMEIFile.bin并生成签名
 输入参数  : enModemID    : 卡0 或者卡1
             pucCipherData: IMEI密文码流
             ulCipherLen  : IMEI密文码流长度

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月9日
    作    者   : d00212987
    修改内容   : IMEI验证
*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_WriteIMEI(MODEM_ID_ENUM_UINT16    enModemID,
                                                  VOS_UINT8              *pucCipherData,
                                                  VOS_UINT32              ulCipherLen);

/*****************************************************************************
 函 数 名  : SC_FAC_SetPortAttrib
 功能描述  : NAS收到AT^PORTATTRIBSET设置命令后，通过该接口将端口锁属性信息提交给SC
 输入参数  : pstSetPortAttrib: 端口锁属性相关信息

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_SetPortAttrib(SC_SET_PORT_ATTRIBUTE_STRU *pstSetPortAttrib);


/*****************************************************************************
 函 数 名  : SC_FAC_GetPortAttrib
 功能描述  : 该函数用于如下两个场景:
             1.NAS收到AT^PORTATTRIBSET查询命令后，通过该接口读取端口锁属性信息中的端口锁状态
             2.上电初始化时，C核OM通过该接口获取端口锁状态后，通知A核OM根据端口锁状态决定是否挂接HSIC OM口
 输入参数  : NA

 输出参数  : penStatus: 端口状态
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_GetPortAttrib(SC_PORT_STATUS_ENUM_UINT32   *penStatus);

/*****************************************************************************
 函 数 名  : SC_FAC_VerifyPortPassword
 功能描述  : NAS收到AT^PORTCTRLTMP命令，调用该接口进行端口锁密码校验
 输入参数  : pcPwd: 端口锁密码，密码为明文传送(密码为16个“0”-“9”字符组成)
             ucLen: 端口锁密码长度

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年4月3日
    作    者   : w00184875
    修改内容   : 初始生成

*****************************************************************************/
extern SC_ERROR_CODE_ENUM_UINT32 SC_FAC_VerifyPortPassword(VOS_UINT8 *pucPwd, VOS_UINT8 ucLen);

/*****************************************************************************
 函 数 名  : SC_PERS_SimlockUnlock
 功能描述  : 解锁Simlock锁网锁卡
 输入参数  : enCategory         -- 锁网锁卡类型
             *pucPwd            -- 解锁密码
 输出参数  : 无
 返 回 值  : VOS_OK     -- 解锁成功
             VOS_ERR    -- 解锁失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年09月18日
    作    者   : l00198894
    修改内容   : 新生成函数

*****************************************************************************/
extern VOS_UINT32 SC_PERS_SimlockUnlock(
    SC_PERSONALIZATION_CATEGORY_ENUM_UINT8  enCategory,
    VOS_UINT8                              *pucPwd );

#if ((FEATURE_VSIM == FEATURE_ON) && (FEATURE_OFF == FEATURE_VSIM_ICC_SEC_CHANNEL))
/*****************************************************************************
 函 数 名  : SC_FAC_SetDhKey
 功能描述  : NAS收到AT^HVPDH设置命令后，通过该接口将DH KEY提交给SC
 输入参数  : MODEM_ID_ENUM_UINT16         enModemID,
             SC_DH_KEY_TYPE_ENUM_UINT32   enKeyType,
             VOS_UINT32                   ulKeyLen,
             VOS_UINT8                   *pucKey

 输出参数  : NA
 返 回 值  : SC_ERROR_CODE_ENUM_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年8月27日
    作    者   : L47619
    修改内容   : V9R1 vSIM项目修改

*****************************************************************************/
SC_ERROR_CODE_ENUM_UINT32 SC_FAC_SetDhKey(
    MODEM_ID_ENUM_UINT16                enModemID,
    SC_DH_KEY_TYPE_ENUM_UINT32          enKeyType,
    VOS_UINT32                          ulKeyLen,
    VOS_UINT8                          *pucKey
);
#endif

#if (FEATURE_ON != FEATURE_SC_SEC_UPDATE)
/*****************************************************************************
函 数 名  :SC_COMM_Backup
功能描述  : SC文件备份处理
输入参数  :

输出参数  :无
返 回 值  :

被调函数  :
修订记录  :
1.日    期   : 2014年6月5日
  作    者   : d00212987
  修改内容   : 初始生成
*****************************************************************************/
VOS_UINT32 SC_COMM_Backup(VOS_VOID);

#if ((OSA_CPU_ACPU == VOS_OSA_CPU))
/*****************************************************************************
函 数 名  : SC_COMM_Restore
功能描述  : SC文件恢复
输入参数  : 无

输出参数  : 无
返 回 值  : VOS_UINT32

被调函数  :
修订记录  :
1.日    期   : 2015年8月10日
  作    者   : d00212987
  修改内容   : SC备份到底软NV备份的扩展分区
*****************************************************************************/
VOS_UINT32 SC_COMM_Restore(VOS_VOID);

#endif

#endif

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
/*****************************************************************************
 函 数 名  : SC_COMM_CryptoEncPIN
 功能描述  : 生成PIN码的密文
 输入参数  : pucPin  : pin明文
             ulPinLen: pin明文长度

 输出参数  : pstCryptoPin:生成密文信息

 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

  1.日    期   : 2017年09月07日
    作    者   : d00212987
    修改内容   : PIN加密
*****************************************************************************/
SC_ERROR_CODE_ENUM_UINT32 SC_COMM_CryptoEncPIN(
    VOS_UINT8                          *pucPin,
    SC_CRYPTO_PIN_STRU                 *pstCryptoPin
);

/*****************************************************************************
 函 数 名  : SC_COMM_CryptoDecPIN
 功能描述  : 生成PIN码的密文
 输入参数  : pucPin  : pin明文
             ulPinLen: pin明文长度

 输出参数  : pstCryptoPin:生成密文信息

 返 回 值  : SC_ERROR_CODE_ENUM_UINT32

  1.日    期   : 2017年09月07日
    作    者   : d00212987
    修改内容   : PIN加密
*****************************************************************************/
SC_ERROR_CODE_ENUM_UINT32 SC_COMM_CryptoDecPIN(
    SC_CRYPTO_PIN_STRU                 *pstCryptoPin,
    VOS_UINT8                          *pucPin);

#endif

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif



