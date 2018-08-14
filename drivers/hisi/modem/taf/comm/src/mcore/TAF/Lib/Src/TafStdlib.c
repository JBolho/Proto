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

/*****************************************************************************
  1 头文件包含
*****************************************************************************/

#include "TafStdlib.h"
#include "MnErrorCode.h"
/* Added by s00217060 for VoLTE_PhaseIII  项目, 2013-12-24, begin */
#include "MnMsgApi.h"
/* Added by s00217060 for VoLTE_PhaseIII  项目, 2013-12-24, end */
#include "MnMsgTs.h"



/*****************************************************************************
  2 常量定义
*****************************************************************************/

#define    THIS_FILE_ID        PS_FILE_ID_TAF_STD_LIB_C

/*****************************************************************************
  3 类型定义
*****************************************************************************/

/*****************************************************************************
  4 函数声明
*****************************************************************************/
LOCAL TAF_ERROR_CODE_ENUM_UINT32 TAF_STD_DecodeUtf8CharacterFirstByte(
    VOS_UINT8                          *pucUtf8FirstByte,
    VOS_UINT16                         *pusUtf8CharacterContent,
    VOS_UINT32                         *pulUtf8CharacterByteNum
);

/*****************************************************************************
  5 变量定义
*****************************************************************************/
/* Modified by s00217060 for VoLTE_PhaseIII  项目, 2013-12-24, begin */
LOCAL VOS_UINT8 f_aucMsgAsciiSfxDefAlpha[TAF_STD_MAX_GSM7BITDEFALPHA_NUM] =
{
    '@',  163,   '$',  165,  232,  233,  249,  236,  242,  199,  0x0a, 216,  248,  0x0d, 197,  229,
    0xff, '_',   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 198,  230,  223,  201,
    ' ',  '!',   '"',  '#',  164,  '%',  '&',  0x27, '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
    '0',  '1',   '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
    161,  'A',   'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
    'P',  'Q',   'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  196,  214,  209,  220,  167,
    191,  'a',   'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
    'p',  'q',   'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  228,  246,  241,  252,  224
};
/* Modified by s00217060 for VoLTE_PhaseIII  项目, 2013-12-24, end */
LOCAL VOS_UINT8 f_aucMsgAsciiSfxDefAlphaExt[TAF_STD_MAX_GSM7BITDEFALPHA_NUM] =
{
    0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0c, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,  0xff, 0xff, '^',  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, '{',  '}',  0xff, 0xff, 0xff, 0xff, 0xff, '\\',
    0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, '[',  '~',  ']',  0xff,
    '|',  0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,  0xff, 0xff, 0xff, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff,  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

};


/* 从1980年开始，以四年为单位，每年的天数 */
LOCAL VOS_UINT16 g_ausTafStdDaysElapsedOfALeapYearSetTab[] = {
  /* 年 0 (闰年) */  0,
  /* 年 1        */  366,
  /* 年 2        */  366+365,
  /* 年 3        */  366+365+365,
  /* 单元周期年  */  366+365+365+365
};

/* 平年中每月的天数 */
LOCAL VOS_UINT16 g_ausTafStdNormMonthTab[] = {
  /* ---    */ 0,
  /* 一月   */ 31,
  /* 二月   */ 31+28,
  /* 三月   */ 31+28+31,
  /* 四月   */ 31+28+31+30,
  /* 五月   */ 31+28+31+30+31,
  /* 六月   */ 31+28+31+30+31+30,
  /* 七月   */ 31+28+31+30+31+30+31,
  /* 八月   */ 31+28+31+30+31+30+31+31,
  /* 九月   */ 31+28+31+30+31+30+31+31+30,
  /* 十月   */ 31+28+31+30+31+30+31+31+30+31,
  /* 十一月 */ 31+28+31+30+31+30+31+31+30+31+30,
  /* 十二月 */ 31+28+31+30+31+30+31+31+30+31+30+31
};

/* 闰年中每月的天数 */
LOCAL VOS_UINT16 g_ausTafStdLeapMonthTab[] = {
  /* ---    */ 0,
  /* 一月   */ 31,
  /* 二月   */ 31+29,
  /* 三月   */ 31+29+31,
  /* 四月   */ 31+29+31+30,
  /* 五月   */ 31+29+31+30+31,
  /* 六月   */ 31+29+31+30+31+30,
  /* 七月   */ 31+29+31+30+31+30+31,
  /* 八月   */ 31+29+31+30+31+30+31+31,
  /* 九月   */ 31+29+31+30+31+30+31+31+30,
  /* 十月   */ 31+29+31+30+31+30+31+31+30+31,
  /* 十一月 */ 31+29+31+30+31+30+31+31+30+31+30,
  /* 十二月 */ 31+29+31+30+31+30+31+31+30+31+30+31
};


/*****************************************************************************
  6 函数定义
*****************************************************************************/
/*****************************************************************************
 函 数 名  : TAF_STD_Itoa
 功能描述  : 将整数转换为ASCII码, 将结果输出至字符串
 输入参数  : VOS_UINT32                          ulDigit            - 待转换为ASCII码的整数
 输出参数  : VOS_CHAR                           *pcDigitStr         - 输出结果的字符串
             VOS_UINT32                         *pulDigitStrLength  - 输出结果的字符串长度
 返 回 值  : VOS_FALSE  - 转换失败
             VOS_TRUE   - 转换成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月06日
    作    者   : f62575
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_Itoa(
    VOS_UINT32                          ulDigit,
    VOS_CHAR                           *pcDigitStr,
    VOS_UINT32                         *pulDigitStrLength
)
{
    VOS_INT32                           lCharNum;

    lCharNum = VOS_sprintf_s(pcDigitStr, TAF_SPM_SSC_MAX_LEN, "%d", (VOS_INT)ulDigit);

    *pulDigitStrLength = (VOS_UINT32)lCharNum;

    return VOS_TRUE;
}

/*****************************************************************************
 函 数 名  : TAF_STD_AsciiNum2HexString
 功能描述  : 完成16进制字符串转换功能
 输入参数  : pucSrc                          - 待转换为ASCII码的整数
 输出参数  : pucSrc                          - 转换后的数字
 返 回 值  : VOS_FALSE  - 转换失败
             VOS_TRUE   - 转换成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月06日
    作    者   : f62575
    修改内容   : 新生成函数

*****************************************************************************/

VOS_UINT32 TAF_STD_AsciiNum2HexString(
    VOS_UINT8                          *pucSrc,
    VOS_UINT16                         *pusSrcLen
)
{
    VOS_UINT16                          usChkLen;
    VOS_UINT8                           ucTmp;
    VOS_UINT8                           ucLeft;
    VOS_UINT16                          usSrcLen;
    VOS_UINT8                          *pucDst = VOS_NULL_PTR;

    usChkLen        = 0;
    ucTmp           = 0;
    ucLeft          = 0;
    usSrcLen        = *pusSrcLen;
    pucDst          = pucSrc;

    /* 如果是奇数个半字节则返回错误 */
    if (0 != (usSrcLen % 2))
    {
        return VOS_FALSE;
    }

    while (usChkLen < usSrcLen)
    {
        /* the number is 0-9 */
        if(isdigit(pucSrc[usChkLen]))
        {
            ucLeft = pucSrc[usChkLen] - '0';
        }
        /* the number is a-f */
        else if( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') )
        {
            ucLeft = (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        /* the number is A-F */
        else if( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') )
        {
            ucLeft = (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return VOS_FALSE;
        }

        pucDst[ucTmp] = 0xf0 & (ucLeft << 4);
        usChkLen += 1;

        /* the number is 0-9 */
        if(isdigit(pucSrc[usChkLen]))
        {
            pucDst[ucTmp] |= pucSrc[usChkLen] - '0';
        }
        /* the number is a-f */
        else if( (pucSrc[usChkLen] >= 'a') && (pucSrc[usChkLen] <= 'f') )
        {
            pucDst[ucTmp] |= (pucSrc[usChkLen] - 'a') + 0x0a;
        }
        /* the number is A-F */
        else if( (pucSrc[usChkLen] >= 'A') && (pucSrc[usChkLen] <= 'F') )
        {
            pucDst[ucTmp] |= (pucSrc[usChkLen] - 'A') + 0x0a;
        }
        else
        {
            return VOS_FALSE;
        }

        usChkLen += 1;
        ucTmp += 1;
    }

    *pusSrcLen = usSrcLen / 2;

    return VOS_TRUE;
}

/*****************************************************************************
 函 数 名  : TAF_STD_HexAlpha2AsciiString
 功能描述  : 完成16进制字符串转换功能
 输入参数  : pucSrc                          - 待转换为ASCII码的数字
             usSrcLen                        - 待转换为ASCII码的数字长度
 输出参数  : pucDst                          - 转换后的ASCII码
 返 回 值  : 转换后的ASCII码长度
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年5月06日
    作    者   : f62575
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT16 TAF_STD_HexAlpha2AsciiString(
    VOS_UINT8                          *pucSrc,
    VOS_UINT16                          usSrcLen,
    VOS_UINT8                          *pucDst
)
{
    VOS_UINT16                          usLen;
    VOS_UINT16                          usChkLen;
    VOS_UINT8                           ucHigh;
    VOS_UINT8                           ucLow;
    VOS_UINT8                          *pucWrite = VOS_NULL_PTR;
    VOS_UINT8                          *pucRead = VOS_NULL_PTR;

    pucWrite = pucDst;
    pucRead  = pucSrc;
    usLen    = 0;
    usChkLen = 0;
    ucHigh   = 0;
    ucLow    = 0;

    /* 扫完整个字串 */
    while ( usChkLen++ < usSrcLen )
    {
        ucHigh = 0x0F & (*pucRead >> 4);
        ucLow  = 0x0F & *pucRead;

        usLen += 2;    /* 记录长度 */

        if (0x09 >= ucHigh)   /* 0-9 */
        {
            *pucWrite++ = ucHigh + 0x30;
        }
        else if (0x0A <= ucHigh)    /* A-F */
        {
            *pucWrite++ = ucHigh + 0x37;
        }
        else
        {

        }

        if (0x09 >= ucLow)   /* 0-9 */
        {
            *pucWrite++ = ucLow + 0x30;
        }
        else if (0x0A <= ucLow)    /* A-F */
        {
            *pucWrite++ = ucLow + 0x37;
        }
        else
        {

        }

        /* 下一个字符 */
        pucRead++;
    }

    return usLen;
}

/* Added by s00217060 for 边境搜网优化PhaseI, 2016-9-1, begin */
/*****************************************************************************
 函 数 名  : TAF_STD_ConvertStrToDecInt
 功能描述  : 字符串转成10进制数，例"123"-->123(一百二十三)
 输入参数  : VOS_UINT8                          *pucSrc
             VOS_UINT16                          usSrcLen
             VOS_UINT32                         *pulDec
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年8月23日
    作    者   : s00217060
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertStrToDecInt(
    VOS_UINT8                          *pucSrc,
    VOS_UINT32                          ulSrcLen,
    VOS_UINT32                         *pulDec
)
{
    VOS_UINT32                          ulTmp;
    VOS_UINT32                          ultotal;
    VOS_UINT8                           ulLength;

    ulTmp       = 0;
    ultotal     = 0;
    ulLength    = 0;

    ulTmp = (VOS_UINT32)*pucSrc++;

    while (ulLength++ < ulSrcLen)
    {
        if( (ulTmp  >= '0') && (ulTmp  <= '9') )
        {
            ulTmp  = ulTmp  - '0';
        }
        else
        {
            return VOS_FALSE;
        }

        if (ultotal > 0x0FFFFFFF)
        {
            return VOS_FALSE;
        }
        else
        {
            ultotal = (ultotal * 10) + ulTmp;
            ulTmp = (VOS_UINT32)(*(pucSrc++));
        }
    }

    *pulDec = ultotal;
    return VOS_TRUE;
}

/* Added by s00217060 for 边境搜网优化PhaseI, 2016-9-1, end */

/* Added by f62575 for V9R1 STK升级, 2013-6-26, begin */
/* MN_UnPack7Bit从MnMsgDecode.c移到本文件，更名为TAF_STD_UnPack7Bit */
/* MN_Pack7Bit从MnMsgEncode.c移到本文件，更名为TAF_STD_Pack7Bit */

/*****************************************************************************
 函 数 名  : TAF_STD_UnPack7Bit
 功能描述  : 同步函数,将7bit编码方式的字符转换为8bit字符
 输入参数  : pucOrgChar            - 指向需要转换的字符指针
             ulLen                 - 需要转换的字符长度，7位位组个数
             ucFillBit             - 需要填充的Bit个数
 输出参数  : pucUnPackedChar       - 指向转换后的指针
 返 回 值  : VOS_UINT32:函数返回的结果,成功以及失败的原因值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月26日
    作    者   : f62575
    修改内容   : V9R1 STK升级
*****************************************************************************/
VOS_UINT32  TAF_STD_UnPack7Bit(
    const VOS_UINT8                     *pucOrgChar,
    VOS_UINT32                          ulLen,
    VOS_UINT8                           ucFillBit,
    VOS_UINT8                           *pucUnPackedChar
)
{
    /*存放字节地址*/
    VOS_UINT32                          ulPos = 0;
     /*存放位偏移*/
    VOS_UINT32                          ulOffset;
    VOS_UINT32                          ulLoop;

    if ((VOS_NULL_PTR == pucOrgChar)
     || (VOS_NULL_PTR == pucUnPackedChar))
    {
        return VOS_ERR;
    }

    /*根据协议23040 9.2.3.24 UDHL和UDH后面是Fill Bits和SM，去掉Fill Bits后就是SM(Unit: Septet),可以获得SM中包含字符个数*/
    ulOffset = ucFillBit % 8;

    /*第一步，移出当前无效的偏移位ulOffset，得到字符的低(8 - ulOffset)位，
      第二步，若(8 - ulOffset)小于7位，需要从下一个OCTET中获取高(7 - (8 - ulOffset))位
      第三步，获取下一个数据源的下标(ulPos)和需要去除的数据位(偏移位ulOffset)*/
    for (ulLoop = 0; ulLoop < ulLen; ulLoop++)
    {
        pucUnPackedChar[ulLoop] = (VOS_UINT8)(pucOrgChar[ulPos] >> ulOffset);
        if (ulOffset > 1)
        {
            /*lint -e701*/
            pucUnPackedChar[ulLoop] |= (VOS_UINT8)((pucOrgChar[ulPos + 1UL] << (8 - ulOffset)) & TAF_STD_7BIT_MASK);
            /*lint +e701*/
        }
        else
        {
            pucUnPackedChar[ulLoop] &= TAF_STD_7BIT_MASK;
        }

        ulPos   += (ulOffset + 7) / 8;
        ulOffset = (ulOffset + 7) % 8;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : TAF_STD_Pack7Bit
 功能描述  : 同步函数,将字符转换为7bit编码方式
 输入参数  : pucOrgChar          - 指向需要转换的字符指针
             ulLen               - 需要转换的字符长度，单位septet 注意:数组元素仅低7bit有效，仍然占用1OCTET的内存空间
             ucFillBit           - 需要填充的Bit个数
 输出参数  : pucPackedChar       - 指向转换后的指针
             pulLen              - 指向转换后的字符长度，单位octet
 返 回 值  : VOS_UINT32:函数返回的结果,成功以及失败的原因值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年6月26日
    作    者   : f62575
    修改内容   : V9R1 STK升级
*****************************************************************************/
VOS_UINT32  TAF_STD_Pack7Bit(
    const VOS_UINT8                     *pucOrgChar,
    VOS_UINT32                          ulLen,
    VOS_UINT8                           ucFillBit,
    VOS_UINT8                           *pucPackedChar,
    VOS_UINT32                          *pulLen
)
{
    /*存放字节地址*/
    VOS_UINT32                          ulPos = 0;
    /*存放位偏移*/
    VOS_UINT32                          ulOffset;
    VOS_UINT32                          ulLoop;

    if ((VOS_NULL_PTR == pucOrgChar)
     || (VOS_NULL_PTR == pucPackedChar)
     || (VOS_NULL_PTR == pulLen))
    {
        return VOS_ERR;
    }

    *pulLen = (VOS_UINT32)((((ulLen * 7) + ucFillBit) + 7) / 8);/* [false alarm]:ignore buffer overflow */

    ulOffset = ucFillBit % 8;

    /*bit 7   6   5   4   3   2   1   0 */
    /*    |digit1L|   |---ulOffset1---| */ /*左移ulOffset1位*/
    /*                |(0)  digit1H   | */ /*右移(8-ulOffset1位)*/
    /*    |-digit2L-  |   |-ulOffset2-| */ /*偏移量为(8-1+ulOffset1)%8*/

    /*第一步，空出当前已经填充的偏移位ulOffset，并从数据源中取出一个OCTET填充高(8 - ulOffset)位，
      第二步，若(8 - ulOffset)小于7位，需要将当前数据源字符余下高(7 - (8 - ulOffset))位填充到目的数据的下一个OCTET中
      第三步，获取下一个目标数据的下标(ulPos)和已经填充的数据位(偏移位ulOffset)*/
    for (ulLoop = 0; ulLoop < ulLen; ulLoop++)
    {
        if ((pucOrgChar[ulLoop] & (~TAF_STD_7BIT_MASK)) != 0)
        {
            return VOS_ERR;
        }

        pucPackedChar[ulPos] |= (VOS_UINT8)((VOS_UINT32)(pucOrgChar[ulLoop] & TAF_STD_7BIT_MASK) << ulOffset);
        if (ulOffset > 1)
        {
            pucPackedChar[ulPos + 1UL] = (VOS_UINT8)((pucOrgChar[ulLoop] & TAF_STD_7BIT_MASK) >> (8 - ulOffset));
        }

        ulPos   += (ulOffset + 7) / 8;
        ulOffset = (ulOffset + 7) % 8;
    }

    return VOS_OK;
}
/* Added by f62575 for V9R1 STK升级, 2013-6-26, end */

/* Added by s00217060 for VoLTE_PhaseIII  项目, 2013-12-16, begin */

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertBcdNumberToAscii
 功能描述  : 将BCD编码的号码转换成Ascii编码的号码
 输入参数  : pBcdNumber     - BCD号码
             ucBcdLen       - BCD号码的长度
 输出参数  : pcAsciiNumber  - 转换得到的ASCII号码(以'\0'结尾)
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertBcdNumberToAscii(
    const VOS_UINT8                    *pucBcdNumber,
    VOS_UINT8                           ucBcdLen,
    VOS_CHAR                           *pcAsciiNumber
)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucLen;
    VOS_UINT8                           ucBcdCode;
    VOS_UINT32                          ulRet;

    if ((VOS_NULL_PTR == pucBcdNumber)
     || (VOS_NULL_PTR == pcAsciiNumber))
    {
        return MN_ERR_NULLPTR;
    }

    /*整理号码字符串，去除无效的0XFF数据*/
    while (ucBcdLen > 1)
    {
        if (0xFF == pucBcdNumber[ucBcdLen - 1])
        {
            ucBcdLen--;
        }
        else
        {
            break;
        }
    }

    /*判断pucBcdAddress所指向的字符串的最后一个字节的高位是否为1111，
    如果是，说明号码位数为奇数，否则为偶数*/
    if ((pucBcdNumber[ucBcdLen - 1] & 0xF0) == 0xF0)
    {
        ucLen = (VOS_UINT8)((ucBcdLen * 2) - 1);
    }
    else
    {
        ucLen = (VOS_UINT8)(ucBcdLen * 2);
    }

    /*解析号码*/
    for (ucLoop = 0; ucLoop < ucLen; ucLoop++)
    {
        /*判断当前解码的是奇数位号码还是偶数位号码，从0开始，是偶数*/
        if (1 == (ucLoop % 2))
        {
            /*如果是奇数位号码，则取高4位的值*/
            ucBcdCode = ((pucBcdNumber[(ucLoop / 2)] >> 4) & 0x0F);
        }
        else
        {
            /*如果是偶数位号码，则取低4位的值*/
            ucBcdCode = (pucBcdNumber[(ucLoop / 2)] & 0x0F);
        }

        /*将二进制数字转换成Ascii码形式*/
        ulRet = TAF_STD_ConvertBcdCodeToAscii(ucBcdCode, &(pcAsciiNumber[ucLoop]));
        if (MN_ERR_NO_ERROR != ulRet)
        {
            return ulRet;
        }
    }

    pcAsciiNumber[ucLoop] = '\0';      /*字符串末尾为0*/

    return MN_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertBcdCodeToAscii
 功能描述  : 将BCD编码的字符转换成Ascii码字符
 输入参数  : ucBcdCode   - BCD编码的字符
 输出参数  : pcAsciiCode - 转换得到的ASCII码字符
 返 回 值  : VOS_UINT32:函数返回的结果,成功以及失败的原因值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 傅映君 62575
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertBcdCodeToAscii(
    VOS_UINT8                           ucBcdCode,
    VOS_CHAR                           *pcAsciiCode
)
{
    VOS_CHAR                            cAsciiCode;

    if (VOS_NULL_PTR == pcAsciiCode)
    {
        return MN_ERR_NULLPTR;
    }

    if (ucBcdCode <= 0x09)
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x30);
    }
    else if (0x0A == ucBcdCode)
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x20);    /*字符'*'*/
    }
    else if (0x0B == ucBcdCode)
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x18);    /*字符'#'*/
    }
    else if ((0x0C == ucBcdCode)
          || (0x0D == ucBcdCode)
          || (0x0E == ucBcdCode))
    {
        cAsciiCode = (VOS_CHAR)(ucBcdCode + 0x55);    /*字符'a', 'b', 'c'*/
    }
    else
    {
        return MN_ERR_INVALID_BCD;
    }

    *pcAsciiCode = cAsciiCode;

    return MN_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertAsciiNumberToBcd
 功能描述  : 将Ascii编码的号码转换成BCD编码的号码
 输入参数  : pcAsciiNumber - 以'\0'结尾的ASCII字符号码
 输出参数  : pucBcdNumber   - 转换得到的BCD号码
             pucBcdLen      - 转换得到的BCD号码的长度
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 丁庆 49431
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertAsciiNumberToBcd(
    const VOS_CHAR                     *pcAsciiNumber,
    VOS_UINT8                          *pucBcdNumber,
    VOS_UINT8                          *pucBcdLen
)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucBcdCode;
    VOS_UINT32                          ulRet;

    if ((VOS_NULL_PTR == pcAsciiNumber)
     || (VOS_NULL_PTR == pucBcdNumber)
     || (VOS_NULL_PTR == pucBcdLen))
    {
        return MN_ERR_NULLPTR;
    }

    for (ucLoop = 0; pcAsciiNumber[ucLoop] != '\0'; ucLoop++)
    {
        ulRet = TAF_STD_ConvertAsciiCodeToBcd(pcAsciiNumber[ucLoop], &ucBcdCode);
        if (MN_ERR_NO_ERROR != ulRet)
        {
            return ulRet;
        }

        /*将当前需要填入的空间清0*/
        pucBcdNumber[(ucLoop / 2)] &= ((ucLoop % 2) == 1) ? 0x0F : 0xF0;

        /*将数字填入相应的空间*/
        pucBcdNumber[(ucLoop / 2)] |= (((ucLoop % 2) == 1) ? ((ucBcdCode << 4) & 0xF0) : (ucBcdCode & 0x0F));
    }

    /*如果长度为奇数，则最后一个字符需要填 F */
    if (1 == (ucLoop % 2))
    {
        pucBcdNumber[(ucLoop / 2)] |= 0xF0;
    }

    *pucBcdLen = (ucLoop + 1) / 2;

    return MN_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertAsciiAddrToBcd
 功能描述  : 同步函数,将MN_MSG_ASCII_ADDR_STRU类型地址转换成MN_MSG_BCD_ADDR_STRU类型地址
 输入参数  : pstAsciiAddr   - MN_MSG_ASCII_ADDR_STRU类型地址
 输出参数  : pstBcdAddr     - MN_MSG_BCD_ADDR_STRU类型地址
 返 回 值  : MN_ERR_NO_ERROR转换操作成功，否则失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年2月13日
    作    者   : fuyingjun
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertAsciiAddrToBcd(
    MN_MSG_ASCII_ADDR_STRU             *pstAsciiAddr,
    MN_MSG_BCD_ADDR_STRU               *pstBcdAddr
)
{
    VOS_UINT32 ulRet;

    if ((VOS_NULL_PTR == pstAsciiAddr)
     || (VOS_NULL_PTR == pstBcdAddr))
    {
        return MN_ERR_NULLPTR;
    }

    MN_GET_ADDRTYPE(pstBcdAddr->addrType,
                    pstAsciiAddr->enNumType,
                    pstAsciiAddr->enNumPlan);

    ulRet = TAF_STD_ConvertAsciiNumberToBcd((VOS_CHAR *)(pstAsciiAddr->aucAsciiNum),
                              pstBcdAddr->aucBcdNum,
                              &pstBcdAddr->ucBcdLen);

    return ulRet;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertAsciiCodeToBcd
 功能描述  : 将Ascii码字符转换成BCD码字符
 输入参数  : ucAsciiCode  - ASCII字符
 输出参数  : pucBcdCode   - 转换得到的BCD码
 返 回 值  : VOS_UINT32:函数返回的结果,成功以及失败的原因值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年9月20日
    作    者   : 傅映君 62575
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertAsciiCodeToBcd(
    VOS_CHAR                            cAsciiCode,
    VOS_UINT8                          *pucBcdCode
)
{
    if (VOS_NULL_PTR == pucBcdCode)
    {
        return MN_ERR_NULLPTR;
    }

    if ((cAsciiCode >= '0')
     && (cAsciiCode <= '9'))
    {
        *pucBcdCode = (VOS_UINT8)(cAsciiCode - '0');
    }
    else if ('*' == cAsciiCode)
    {
        *pucBcdCode = 0x0a;
    }
    else if ('#' == cAsciiCode)
    {
        *pucBcdCode = 0x0b;
    }
    else if (('a' == cAsciiCode)
          || ('b' == cAsciiCode)
          || ('c' == cAsciiCode))
    {
        *pucBcdCode = (VOS_UINT8)((cAsciiCode - 'a') + 0x0c);
    }
    else
    {
        return MN_ERR_INVALID_ASCII;
    }

    return MN_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertDeciDigitToBcd
 功能描述  : 将十进制数字转换成BCD码
 输入参数  : ucDeciDigit 十进制数字
             bReverseOrder TAF_TRUE反序转换,即BCD码的高4BIT对应十进制的个位;
                           TAF_FALSE顺序转换，即BCD码的高4BIT对应十进制的十位;
 输出参数  : 无
 返 回 值  : 十进制数字转换得到的BCD码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月15日
    作    者   : fuyingjun
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 TAF_STD_ConvertDeciDigitToBcd(
    VOS_UINT8                           ucDeciDigit,
    VOS_BOOL                            bReverseOrder
)
{
    VOS_UINT8 ucBcd;

    if (VOS_TRUE == bReverseOrder)
    {
        /*低4BIT存储十进制数的高位*/
        ucBcd  = ucDeciDigit / 10;
        /*高4BIT存储十进制数的低位*/
        ucBcd |= (ucDeciDigit % 10) << 4;
    }
    else
    {
        /*低4BIT存储十进制数的地位*/
        ucBcd  = ucDeciDigit % 10;
        /*高4BIT存储十进制数的高位*/
        ucBcd |= (VOS_UINT8)((ucDeciDigit / 10) << 4);
    }

    return ucBcd;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertBcdToDeciDigit
 功能描述  : 将BCD编码的数字转换成十进制数字
 输入参数  : ucBcdDigit BCD编码的数字
             bReverseOrder MN_TRUE反序转换,即BCD码的高4BIT对应十进制的个位;
                           MN_FALSE顺序转换，即BCD码的高4BIT对应十进制的十位;
 输出参数  : pucDigit      转换后得到的十进制数字
 返 回 值  : BCD码转换得到的十进制数字
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2008年1月15日
    作    者   : 傅映君 62575
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertBcdToDeciDigit(
    VOS_UINT8                           ucBcdDigit,
    VOS_BOOL                            bReverseOrder,
    VOS_UINT8                          *pucDigit
)
{
    VOS_UINT8                           ucHigher;
    VOS_UINT8                           ucLower;
    VOS_UINT32                          ulRet;

    ucHigher = (ucBcdDigit >> 4) & 0x0f;
    ucLower  = ucBcdDigit & 0x0f;
    if ((ucHigher > 9) || (ucLower > 9))
    {
        ulRet = MN_ERR_INVALIDPARM;
    }
    else
    {
        ulRet = MN_ERR_NO_ERROR;
    }

    if (VOS_TRUE == bReverseOrder)
    {
        *pucDigit  = (VOS_UINT8)((VOS_UINT8)(ucLower * 10) + ucHigher);
    }
    else
    {
        *pucDigit  = (VOS_UINT8)((VOS_UINT8)ucHigher * 10) + ucLower;
    }

    return ulRet;
}

/*****************************************************************************
 函 数 名  : TAF_STD_RemapAsciiToSelectTable
 功能描述  : 根据提供的字符表将Ascii码转换到对应字符表给定的编码
 输入参数  : ucAsciiChar                给定Ascii码
            *pucAlphaTable              字符表
             ulAlphaTableSize           字符表大小
             pucAlphaChar               转换后编码
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月17日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_RemapAsciiToSelectTable(
    VOS_UINT8                           ucAsciiChar,
    VOS_UINT8                          *pucAlphaTable,
    VOS_UINT32                          ulAlphaTableSize,
    VOS_UINT8                          *pucAlphaChar
)
{
    VOS_UINT32                          ulLoop1;

    if (VOS_NULL_PTR == pucAlphaTable)
    {
        return VOS_FALSE;
    }

    for (ulLoop1 = 0 ; ulLoop1 < ulAlphaTableSize ; ulLoop1++)
    {
        if (pucAlphaTable[ulLoop1] == (ucAsciiChar))
        {
            *pucAlphaChar = (VOS_UINT8)ulLoop1;
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertAsciiToDefAlpha
 功能描述  : 同步函数,将ASCII码的值转换为23038协议规定的Default Alphabet
 输入参数  : pucAsciiChar          - 指向ASCII码的指针
             ulLen                 - ASCII字符的长度
 输出参数  : pucDefAlpha           - 指向Default Alphabet的指针
 返 回 值  : VOS_UINT32:函数返回的结果,成功以及失败的原因值
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年10月8日
    作    者   : z40661
    修改内容   : 新生成函数
  2.日    期   : 2017年5月17日
    作    者   : z00385378
    修改内容   : USSI

*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertAsciiToDefAlpha(
    const VOS_UINT8                    *pucAsciiChar,
    VOS_UINT32                          ulLen,
    VOS_UINT8                          *pucDefAlpha,
    VOS_UINT32                         *pulDefAlphaLen,
    VOS_UINT32                          ulDefAlphaBuffLen
)
{
    VOS_UINT32                          ulLoop2;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulDefAlphaIdx;

    if ((VOS_NULL_PTR == pucAsciiChar)
     || (VOS_NULL_PTR == pucDefAlpha)
     || (VOS_NULL_PTR == pulDefAlphaLen))
    {
        return MN_ERR_NULLPTR;
    }

    ulRet         = MN_ERR_NO_ERROR;
    ulDefAlphaIdx = 0;
    /*
    目前参考其他平台仅支持23038 6.2.1 GSM 7 bit Default Alphabet表中存在的字符集，
    不支持扩展表中的字符；
    ASCII表与GSM 7 bit Default Alphabet交集部分的标准ASCII码可以转换为GSM 7 bit Default Alphabet；
    键盘能够输入的字符已测试通过，其他未测试；
    ASCII表中其他字符，如{，则记录为00，回读出来是@，其他则失败；
    键盘能够输入的字符已测试通过，其他未测试；
    需要参考其他平台的处理；
    */
    for (ulLoop2 = 0; ulLoop2 < ulLen; ulLoop2++)
    {
        /*7BIT convert into ASCII ,consider later!!!!!*/
        /* 缓存满 */
        if (ulDefAlphaIdx >= ulDefAlphaBuffLen)
        {
            ulRet = MN_ERR_INVALIDPARM;
            break;
        }

        /* 使用Basic Character Set表 */
        if (VOS_TRUE == TAF_STD_RemapAsciiToSelectTable(*pucAsciiChar,
                                                        f_aucMsgAsciiSfxDefAlpha,
                                                        TAF_STD_MAX_GSM7BITDEFALPHA_NUM,
                                                        pucDefAlpha))
        {
            ulDefAlphaIdx++;
            pucDefAlpha++;
        }
        else
        {
            /* 剩余缓存长度不够支持使用扩展编码表 */
            if ((ulDefAlphaIdx + 1) >= ulDefAlphaBuffLen)
            {
                ulRet = MN_ERR_INVALIDPARM;
                MN_INFO_LOG("TAF_STD_ConvertAsciiToDefAlpha ExtTable need larger buffer");
                break;
            }

            /* 使用Basic Character Set Extension表 */
            if (VOS_TRUE == TAF_STD_RemapAsciiToSelectTable(*pucAsciiChar,
                                                            f_aucMsgAsciiSfxDefAlphaExt,
                                                            TAF_STD_MAX_GSM7BITDEFALPHA_NUM,
                                                            pucDefAlpha + 1))
            {
                *pucDefAlpha   = 0x1B;
                ulDefAlphaIdx += 2;
                pucDefAlpha   += 2;
            }
            else
            {
                /* 扩展编码表也无法包含 */
                MN_INFO_LOG1("TAF_STD_ConvertAsciiToDefAlpha Encode Failed ", *pucAsciiChar);
                ulRet = MN_ERR_INVALIDPARM;
            }
        }

        pucAsciiChar++;
    }

    *pulDefAlphaLen = ulDefAlphaIdx;

    return ulRet;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertDefAlphaToAscii
 功能描述  : 同步函数,将23038协议规定的Default Alphabet转换为ASCII码的值
 输入参数  : pucDefAlpha    - 指向Default Alphabet的指针
             ulDefAlphaLen  - Default Alphabet的长度
 输出参数  : pucAsciiChar   - 指向ASCII码的指针
 			 pulAsciiCharLen- 指向ASCII码字符串长度
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2007年10月8日
    作    者   : z40661
    修改内容   : 新生成函数
  2.日    期   : 2009-09-08
    作    者   : F62575
    修改内容   : 问题单号:设置TE和MT的字符集类型为IRA，短信编码类型为7BIT编码，输入特殊字符@等短信内容，写入到SIM卡中的数据错误；
  3.日    期   : 2009-09-24
    作    者   : F62575
    修改内容   : 问题单号AT2D14728:文本格式列表或读取短信异常
  4.日    期   : 2010年04月10日
    作    者   : f62575
    修改内容   : 问题单号AT2D18035
                 写PDU短信到SIM卡,BALONG对TP-SCTS的检查与标杆不一致；
  5.日    期   : 2013年08月22日
    作    者   : l65478
    修改内容   : 问题单号DTS2013081408291,无法显示的字符没有显示为空格
  6.日    期   : 2017年5月17日
    作    者   : z00385378
    修改内容   : USSI
*****************************************************************************/
VOS_VOID  TAF_STD_ConvertDefAlphaToAscii(
    const VOS_UINT8                     *pucDefAlpha,
    VOS_UINT32                          ulDefAlphaLen,
    VOS_UINT8                           *pucAsciiChar,
    VOS_UINT32                          *pulDstLen
)
{
    VOS_UINT32                          ulLoop;
    VOS_UINT8                           ucPos;
    VOS_UINT32                          ulValidLen = 0;
    VOS_UINT8                           ucExtFlg;

    if ((VOS_NULL_PTR == pucDefAlpha)
     || (VOS_NULL_PTR == pucAsciiChar))
    {
        return;
    }
    ucExtFlg = VOS_FALSE;

    /*
    目前参考其他平台仅支持23038 6.2.1 GSM 7 bit Default Alphabet表中存在的字符集，
    不支持扩展表中的字符；
    与ASCII表交集部分的GSM 7 bit Default Alphabet可以转换为标准ASCII码供显示用；
    扩展字符标记转换为0XFE，不同设备上对其有私有解释；
    其他非标准字符转换为0XFF，不同设备上对其有私有解释；
    需要参考其他平台的处理；
    */
    for (ulLoop = 0; ulLoop < ulDefAlphaLen; ulLoop++)
    {
        ucPos           = TAF_STD_7BIT_MASK & pucDefAlpha[ulLoop];
        *pucAsciiChar   = (VOS_FALSE == ucExtFlg) ? f_aucMsgAsciiSfxDefAlpha[ucPos] : f_aucMsgAsciiSfxDefAlphaExt[ucPos];

        if (TAF_STD_NOSTANDARD_ASCII_CODE == (*pucAsciiChar))
        {
            /* 对比标杆,对无法显示的字符使用空格替换 */
            ucExtFlg        = VOS_FALSE;
            (*pucAsciiChar) = ' ';
            pucAsciiChar++;
            ulValidLen++;
        }
        else if (TAF_STD_GSM_7BIT_EXTENSION_FLAG == (*pucAsciiChar))
        {
            /* 下一个Default Alpha使用扩展表解析 */
            (*pucAsciiChar) = ' ';
            ucExtFlg        = VOS_TRUE;
        }
        else
        {
            ucExtFlg        = VOS_FALSE;
            pucAsciiChar++;
            ulValidLen++;
        }
    }


    if (ulValidLen < ulDefAlphaLen)
    {
        *pucAsciiChar = 0;
    }

    *pulDstLen = ulValidLen;

    return;
}

/* Added by s00217060 for VoLTE_PhaseIII  项目, 2013-12-16, end */

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertBcdCodeToDtmf
 功能描述  : 把BCD码转换成3GPP2协议中的DTMF格式编码
 输入参数  : VOS_UINT8                           ucBcdCode
             VOS_UINT8                          *pucDtmfCode
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月30日
    作    者   : y00218312
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertBcdCodeToDtmf(
    VOS_UINT8                           ucBcdCode,
    VOS_UINT8                          *pucDtmfCode
)
{
    VOS_UINT8                            ucDtmfCode;

    if (VOS_NULL_PTR == pucDtmfCode)
    {
        return MN_ERR_NULLPTR;
    }

    /*数字'0' BCD:0x00, DTMF:0x0A*/
    /*字符'*' BCD:0x0A, DTMF:0x0B*/
    /*字符'#' BCD:0x0B, DTMF:0x0C*/

    if (ucBcdCode == 0x00)
    {
        ucDtmfCode = 0x0A;
    }
    else if (ucBcdCode <= 0x09)
    {
        ucDtmfCode = ucBcdCode;
    }
    else if (0x0A == ucBcdCode)         /*字符'*'*/
    {
        ucDtmfCode = ucBcdCode + 1;
    }
    else if (0x0B == ucBcdCode)
    {
        ucDtmfCode = ucBcdCode + 1;     /*字符'#'*/
    }
    else
    {
        return MN_ERR_INVALID_BCD;
    }

    *pucDtmfCode = ucDtmfCode;

    return MN_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertBcdNumberToDtmf
 功能描述  : 把BCD格式的编码字符串转换为3GPP2协议定义的DTMF格式的字符串
 输入参数  : const VOS_UINT8                    *pucBcdNumber
             VOS_UINT8                           ucBcdLen
             VOS_UINT8                          *pucDtmfNumber
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年9月30日
    作    者   : y00218312
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32  TAF_STD_ConvertBcdNumberToDtmf(
    const VOS_UINT8                    *pucBcdNumber,
    VOS_UINT8                           ucBcdLen,
    VOS_UINT8                          *pucDtmfNumber
)
{
    VOS_UINT8                           ucLoop;
    VOS_UINT8                           ucLen;
    VOS_UINT8                           ucBcdCode;
    VOS_UINT32                          ulRet;

    if ((VOS_NULL_PTR == pucBcdNumber)
     || (VOS_NULL_PTR == pucDtmfNumber))
    {
        return MN_ERR_NULLPTR;
    }

    /*整理号码字符串，去除无效的0XFF数据*/
    while (ucBcdLen > 1)
    {
        if (0xFF == pucBcdNumber[ucBcdLen - 1])
        {
            ucBcdLen--;
        }
        else
        {
            break;
        }
    }

    /*判断pucBcdAddress所指向的字符串的最后一个字节的高位是否为1111，
    如果是，说明号码位数为奇数，否则为偶数*/
    if ((pucBcdNumber[ucBcdLen - 1] & 0xF0) == 0xF0)
    {
        ucLen = (VOS_UINT8)((ucBcdLen * 2) - 1);
    }
    else
    {
        ucLen = (VOS_UINT8)(ucBcdLen * 2);
    }

    /*解析号码*/
    for (ucLoop = 0; ucLoop < ucLen; ucLoop++)
    {
        /*判断当前解码的是奇数位号码还是偶数位号码，从0开始，是偶数*/
        if (1 == (ucLoop % 2))
        {
            /*如果是奇数位号码，则取高4位的值*/
            ucBcdCode = ((pucBcdNumber[(ucLoop / 2)] >> 4) & 0x0F);
        }
        else
        {
            /*如果是偶数位号码，则取低4位的值*/
            ucBcdCode = (pucBcdNumber[(ucLoop / 2)] & 0x0F);
        }

        /*将二进制数字转换成DTMF码形式*/
        ulRet = TAF_STD_ConvertBcdCodeToDtmf(ucBcdCode, pucDtmfNumber);
        if (MN_ERR_NO_ERROR != ulRet)
        {
            return ulRet;
        }

        pucDtmfNumber++;
    }

    return MN_ERR_NO_ERROR;
}



/*****************************************************************************
 函 数 名  :TAF_STD_GetDaysOfLeapMonthTabAddr
 功能描述  :获取闰年各月的天数
 输入参数  :无
 输出参数  :无
 返 回 值  :返回g_ausTafStdLeapMonthTab表的地址信息
 调用函数  :无

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   : 新生成函数
*****************************************************************************/
LOCAL VOS_UINT16* TAF_STD_GetDaysOfLeapMonthTabAddr(VOS_VOID)
{
    return g_ausTafStdLeapMonthTab;
}

/*****************************************************************************
 函 数 名  :TAF_STD_GetDaysOfNormalMonthTabAddr
 功能描述  :获取非闰年各月的天数表的地址
 输入参数  :无
 输出参数  :无
 返 回 值  :返回g_ausTafStdNormMonthTab表的地址信息
 调用函数  :无

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   : 新生成函数
*****************************************************************************/
LOCAL VOS_UINT16* TAF_STD_GetDaysOfNormalMonthTabAddr(VOS_VOID)
{
    return g_ausTafStdNormMonthTab;
}

/*****************************************************************************
 函 数 名  :TAF_STD_GetDaysElapsedOfALeapYearSet
 功能描述  :获取g_ausTafStdDaysElapsedOfALeapYearSetTab表的地址信息
 输入参数  :无
 输出参数  :无
 返 回 值  :返回g_ausTafStdDaysElapsedOfALeapYearSetTab表的地址信息
 调用函数  :无

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   : 新生成函数
*****************************************************************************/
LOCAL VOS_UINT16* TAF_STD_GetDaysElapsedOfALeapYearSet(VOS_VOID)
{
    return g_ausTafStdDaysElapsedOfALeapYearSetTab;
}

/*****************************************************************************
 函 数 名  : TAF_STD_64Add32
 功能描述  : 一个64位的值加上一个32位的值
 输入参数  :
             ulHighAddend --被加数高位
             ulLowAddend  --被加数低位
             ulAddFactor

 输出参数  :
             pulHighRslt  --结果数高位
             pulLowRslt   --结果数低位
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   :
*****************************************************************************/
VOS_UINT32 TAF_STD_64Add32
(
    VOS_UINT32                          ulHighAddend,
    VOS_UINT32                          ulLowAddend,
    VOS_UINT32                          ulAddFactor,
    VOS_UINT32                         *pulHighRslt,
    VOS_UINT32                         *pulLowRslt
)
{
    if (0xFFFFFFFF - ulLowAddend >= ulAddFactor)
    {
        *pulLowRslt  = ulLowAddend + ulAddFactor;
        *pulHighRslt = ulHighAddend;
    }
    else
    {
        if (0xFFFFFFFF == ulHighAddend)
        {
            return VOS_ERR;
        }

        *pulHighRslt = ulHighAddend + 1;
        *pulLowRslt = ulAddFactor - (0xFFFFFFFF - ulLowAddend) - 1;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : TAF_STD_64Sub32
 功能描述  : 一个64位的值减去一个32位的值
 输入参数  :
             ulHighMinuend  --被减数高位
             ulLowMinuend   --被减数低位
             ulAddFactor
 输出参数  :
             pulHighRslt    --结果数高位
             pulLowRslt     --结果数低位
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   :
*****************************************************************************/
VOS_UINT32 TAF_STD_64Sub32
(
    VOS_UINT32                          ulHighMinuend,
    VOS_UINT32                          ulLowMinuend,
    VOS_UINT32                          ulSubFactor,
    VOS_UINT32                         *pulHighRslt,
    VOS_UINT32                         *pulLowRslt
)
{
    if(ulLowMinuend  >= ulSubFactor)
    {
        *pulHighRslt = ulHighMinuend;
        *pulLowRslt = ulLowMinuend - ulSubFactor;
    }
    else if (ulHighMinuend >= 1)
    {
        *pulHighRslt = ulHighMinuend - 1;
        *pulLowRslt  = ulLowMinuend + (0xFFFFFFFF - ulSubFactor) + 1;
    }
    else
    {
        return VOS_ERR;
    }

    return VOS_OK;
}
/*****************************************************************************
 函 数 名  : TAF_STD_ConvertTimeFromSecsToTimeZone
 功能描述  : 把从1980.1.6开始的秒数转化为相应的年月日 时分秒

 输入参数  : VOS_UINT32                          ulHighSystemTime,
             VOS_UINT32                          ulLowSystemTime,

 输出参数  : TAF_STD_TimeZoneType                 *pstUniversalTimeandLocalTimeZone
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   :
*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertTimeFromSecsToTimeZone
(
    VOS_UINT32                          ulHighSystemTime,
    VOS_UINT32                          ulLowSystemTime,
    TAF_STD_TIME_ZONE_TYPE_STRU        *pstUniversalTimeandLocalTimeZone
)
{
    VOS_UINT16                          i;
    VOS_UINT16                          usDays;
    VOS_UINT32                          ulQuadYyears;
    VOS_UINT16                         *pusMonthTable = VOS_NULL_PTR;
    VOS_UINT16                         *pusLeapMonthTable = VOS_NULL_PTR;
    VOS_UINT16                         *pusNormalMonthTable = VOS_NULL_PTR;
    VOS_UINT16                         *pusDaysElapsedTable = VOS_NULL_PTR;
    VOS_UINT32                          ulHighTmp;
    VOS_UINT32                          ulLowTmp;
    VOS_UINT32                          ulQuotientHigh;
    VOS_UINT32                          ulQuotientLow;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulRemainder;

    pusLeapMonthTable                 = TAF_STD_GetDaysOfLeapMonthTabAddr();

    pusNormalMonthTable               = TAF_STD_GetDaysOfNormalMonthTabAddr();

    pusDaysElapsedTable               = TAF_STD_GetDaysElapsedOfALeapYearSet();

    ulHighTmp      = 0;
    ulLowTmp       = 0;
    ulQuotientHigh = 0;
    ulQuotientLow  = 0;
    ulRemainder    = 0;
     /* 时间换算成从1980.1.1 开始的秒数 */
    ulResult = TAF_STD_64Add32(ulHighSystemTime, ulLowSystemTime, (VOS_UINT32)TAF_STD_TIME_ZONE_OFFSET_S, &ulHighTmp, &ulLowTmp);
    if ( VOS_OK != ulResult )
    {
        return VOS_ERR;
    }

     /* 根据系统时间的秒数计算当前时间的秒数 */
    ulResult = VOS_64Div32(ulHighTmp, ulLowTmp, TAF_STD_SECONDS_PER_MINUTE, &ulQuotientHigh, &ulQuotientLow, &ulRemainder);
    if ( VOS_OK != ulResult )
    {
        return VOS_ERR;
    }
    pstUniversalTimeandLocalTimeZone->usSecond = (VOS_UINT16)ulRemainder;

    /* 根据系统时间的秒数计算当前时间的分钟数 */
    ulResult = VOS_64Div32(ulQuotientHigh, ulQuotientLow, TAF_STD_MINUTES_PER_HOUR, &ulHighTmp, &ulLowTmp, &ulRemainder);
    if ( VOS_OK != ulResult )
    {
        return VOS_ERR;
    }
    pstUniversalTimeandLocalTimeZone->usMinute = (VOS_UINT16)ulRemainder;

    /* 根据系统时间的秒数计算当前时间的小时 */
    ulResult = VOS_64Div32(ulHighTmp, ulLowTmp, TAF_STD_HOURS_PER_DAY, &ulQuotientHigh, &ulQuotientLow, &ulRemainder);
    if ( VOS_OK != ulResult )
    {
        return VOS_ERR;
    }
    pstUniversalTimeandLocalTimeZone->usHour = (VOS_UINT16)ulRemainder;

    /* 计算自1980开始过去了多少闰年 */
    ulResult = VOS_64Div32(ulQuotientHigh, ulQuotientLow, TAF_STD_TIME_ZONE_QUAD_YEAR, &ulHighTmp, &ulLowTmp, &ulRemainder);
    if ( VOS_OK != ulResult )
    {
        return VOS_ERR;
    }
    usDays = (VOS_UINT16)ulRemainder;

    ulQuadYyears = ulLowTmp;

    pstUniversalTimeandLocalTimeZone->usYear = (VOS_UINT16)(TAF_STD_TIME_ZONE_BASE_YEAR + (4 * ulQuadYyears));

    /* 计算当前年处在哪个闰年中 */
    for ( i = 0; i < TAF_STD_DAYS_ELAPSED_OF_A_LEAP_YEAR_SET_TAB_COUNT - 1; i++ )
    {
        if (usDays < pusDaysElapsedTable[i + 1])
        {
            break;
        }
    }

    /* 获取当前年过去的天数 */
    usDays -= pusDaysElapsedTable[i];

    /* 计算是哪一年 */
    pstUniversalTimeandLocalTimeZone->usYear += (VOS_UINT16)i;

    /* 下面计算月份和哪一天 */

    /* 获取月份表 */
    pusMonthTable = (i == 0) ? pusLeapMonthTable : pusNormalMonthTable;

    /* 查找月份表 */
    for ( i = 0; i < TAF_STD_NORMAL_MONTH_TAB_COUNT - 1; i++ )
    {
        if (usDays < pusMonthTable[i + 1])
        {
            break;
        }
    }

    /* 计算出当前是哪一个天.*/
    pstUniversalTimeandLocalTimeZone->usDay = usDays - pusMonthTable[i] + 1;

    /* 计算出月份 */
    pstUniversalTimeandLocalTimeZone->usMonth = (VOS_UINT16)i + 1;

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertSystemTimeToHighLow
 功能描述  : 把system time通过高低32来保存
 输入参数  : VOS_UINT8                          *pucSysTimeByte
             VOS_UINT32                         *pulHighSystemTime,
             VOS_UINT32                         *pulLowSystemTime

 输出参数  :
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年12月26日
    作    者   : x00314862
    修改内容   :
*****************************************************************************/
VOS_VOID TAF_STD_ConvertSystemTimeToHighLow(
    VOS_UINT8                          *pucSysTimeByte,
    VOS_UINT32                         *pulHighSystemTime,
    VOS_UINT32                         *pulLowSystemTime
)
{
    VOS_UINT32                          ulTmp;
    VOS_UINT8                          *pucAddr = VOS_NULL_PTR;

    pucAddr = pucSysTimeByte + 1;

    ulTmp = *pucAddr++;
    ulTmp <<= 8;
    ulTmp |= *pucAddr++;
    ulTmp <<= 8;
    ulTmp |= *pucAddr++;
    ulTmp <<= 8;
    ulTmp |= *pucAddr;

    /* 取systemtime的高四位 */
    *pulHighSystemTime = (VOS_UINT32)((pucSysTimeByte[0] & 0xF0) >> 4);

    /* 取systemtime的低四位 */
    *pulLowSystemTime  = ((((VOS_UINT32)(((VOS_UINT32)(pucSysTimeByte[0] & 0x0F)) << 28)) & 0xF0000000)
                       | ((VOS_UINT32)(ulTmp >> 4) & 0x0FFFFFFF));

}


/*****************************************************************************
 函 数 名  : TAF_XCALL_TransformBcdMccToDeciDigit
 功能描述  : 转换BCD格式的MCC为十进制
 输入参数  : VOS_UINT32                          ulBcdMcc
 输出参数  : 无
 返 回 值  : VOS_UINT16

 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年4月14日
    作    者   : c00299063
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT16 TAF_STD_TransformBcdMccToDeciDigit(
    VOS_UINT32                          ulBcdMcc
)
{
    /*0x00 06 04 --> 460 */
    VOS_UINT16                          usMcc;

    usMcc = (ulBcdMcc & 0xff) * 100 + ((ulBcdMcc >> 8) & 0xff) * 10 + ((ulBcdMcc >> 16) & 0xff);

    if ( 999 < usMcc )
    {
        usMcc = 0xffff;
    }

    return usMcc;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertBcdMncToDeciDigit
 功能描述  : 转换BCD格式的MNC为十进制的格式
 输入参数  : VOS_UINT32                          ulBcdMnc
 输出参数  : 无
 返 回 值  : VOS_UINT16

 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年09月08日
    作    者   : w00351686
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT16 TAF_STD_TransformBcdMncToDeciDigit(
    VOS_UINT32                          ulBcdMnc
)
{
    /* 2位Mnc转换形式:0x00 0f 03 00 --> 03 */
    /* 3位Mnc转换形式:0x0f 03 00 00 --> 003 */
    VOS_UINT16                          usMnc;

    if (0x0f0000 == (ulBcdMnc & 0x0f0000))
    {
        usMnc = (ulBcdMnc & 0xff) * 10 + ((ulBcdMnc >> 8) & 0xff);

        if (usMnc >= 100)
        {
            usMnc = 0xffff;
        }
    }
    else
    {
        usMnc = (ulBcdMnc & 0xff) * 100 + ((ulBcdMnc >> 8) & 0xff) * 10 + ((ulBcdMnc >> 16) & 0xff);

        if (usMnc >= 1000)
        {
            usMnc = 0xffff;
        }
    }

    return usMnc;
}


/*****************************************************************************
 函 数 名  : TAF_STD_TransformBcdImsi1112ToDeciDigit
 功能描述  : 转换IMSI11_12格式的MCC为十进制
 输入参数  : VOS_UINT16                          usBcdImsi1112
 输出参数  : 无
 返 回 值  : VOS_UINT8
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年7月7日
    作    者   : l00301449
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT8 TAF_STD_TransformBcdImsi1112ToDeciDigit(
    VOS_UINT16                          usBcdImsi1112
)
{
    /*0x03 00 --> 3 */
    VOS_UINT16                          usImsi1112;

    usImsi1112 = ((usBcdImsi1112 ) & 0xff) * 10 + ((usBcdImsi1112 >> 8) & 0xff);

    if ( 100 < usImsi1112 )
    {
        usImsi1112 = 0xffff;
    }

    return (VOS_UINT8)usImsi1112;
}


/*****************************************************************************
 函 数 名  : TAF_STD_TransformCLBcdMncToDeciDigit
 功能描述  : 转换CL模式下的BCD格式的MNC为十进制
 输入参数  : VOS_UINT16                         usBcdMcc
 输出参数  : 无
 返 回 值  : VOS_UINT16

 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年6月1日
    作    者   : c00299063
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT16 TAF_STD_TransformCLBcdMncToDeciDigit(
    VOS_UINT16                          usBcdMnc
)
{
    /*0x03 00 --> 03 */
    VOS_UINT16                          usMnc;

    usMnc = (usBcdMnc & 0xff) * 10 + ((usBcdMnc >> 8) & 0xff);

    if ( 100 < usMnc )
    {
        usMnc = 0xffff;
    }

    return usMnc;
}


/*****************************************************************************
 函 数 名  : TAF_STD_TransformDeciDigitToBcdMcc
 功能描述  : 十进制的MCC转换为BCD格式
 输入参数  : VOS_UINT32                          ulDeciDigitMcc
 输出参数  : 无
 返 回 值  : VOS_UINT32

 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2015年12月28日
    作    者   : l00324781
    修改内容   : 新生成函数
*****************************************************************************/
VOS_UINT32 TAF_STD_TransformDeciDigitToBcdMcc(
    VOS_UINT32                          ulDeciDigitMcc
)
{
    /*460 --->0x00 06 04 */
    VOS_UINT32                          ulBcdMcc;

    if (TAF_STD_INVALID_MCC != ulDeciDigitMcc)
    {
        ulBcdMcc  = (ulDeciDigitMcc % 1000) / 100;

        ulBcdMcc += (ulDeciDigitMcc % 10) << 16;

        ulBcdMcc += ((ulDeciDigitMcc % 100) / 10) << 8;

        return ulBcdMcc;
    }
    else
    {
        return TAF_STD_INVALID_MCC;
    }
}


/*****************************************************************************
 函 数 名  : TAF_STD_ChangeEndian
 功能描述  : 改变大小端
 输入参数  :*pucSrcStr                  源串地址
             ulStrLen                   串长度(默认目标串长度与源串长度相同)
            *pucDstStr                  目标串地址
             ulDataSize                 数据大小(字节数)
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月26日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ChangeEndian(
    VOS_UINT8                          *pucSrcStr,
    VOS_UINT32                          ulStrLen,
    VOS_UINT8                          *pucDstStr,
    VOS_UINT8                           ucDataSize
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulInterOrder;
    VOS_UINT8                           ucTmp;

    if ((VOS_NULL_PTR == pucSrcStr)
     || (VOS_NULL_PTR == pucDstStr))
    {
        return TAF_ERR_NULL_PTR;
    }

    MN_INFO_LOG2("TAF_STD_ChangeEndian SLen DSize", ulStrLen, ucDataSize);

    if ((0 == ucDataSize)
     || (ulStrLen < ucDataSize))
    {
        return TAF_ERR_PARA_ERROR;
    }

    ulIndex = 0;

    while (ulIndex + ucDataSize <= ulStrLen)
    {
        for (ulInterOrder = 0; ulInterOrder < (ucDataSize / 2); ulInterOrder++)
        {
            ucTmp = *(pucSrcStr + ulIndex + ulInterOrder);
            *(pucSrcStr + ulIndex + ulInterOrder) = *(pucSrcStr + ulIndex + ucDataSize - ulInterOrder - 1);
            *(pucSrcStr + ulIndex + ucDataSize - ulInterOrder - 1) = ucTmp;
        }
        ulIndex += ucDataSize;
    }

    return TAF_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_EncodeUnicodeToUtf8
 功能描述  : 单个字符Unicode转Utf8(大端序)
 输入参数  : usUnicode                          Unicode码
            *pucUtf8Len                         Utf8存储空间长度(字节数)
 输出参数  :*pucUtf8Str                         Utf8存储地址
 返 回 值  : VOS_UINT32                         Utf8解码实际长度(字节数)
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月12日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_EncodeUnicodeToUtf8(
    VOS_UINT16                          usUnicode,
    VOS_UINT8                          *pucUtf8Str,
    VOS_UINT32                          ulUtf8Len
)
{
    if ((TAF_STD_UTF8_1BYTE_MAX >= usUnicode)
     && (1 <= ulUtf8Len))
    {
        * pucUtf8Str        = (VOS_UINT8)usUnicode;

        return 1;
    }
    else if ((TAF_STD_UTF8_2BYTE_MAX >= usUnicode)
          && (2 <= ulUtf8Len))
    {
        *(pucUtf8Str + 1)   = (VOS_UINT8)(0x80 |  (usUnicode &  0x3F));
        * pucUtf8Str        = (VOS_UINT8)(0xC0 |  (usUnicode >> 6 ));

        return 2;
    }
    else if (3 <= ulUtf8Len)
    {
        * pucUtf8Str        = (VOS_UINT8)(0xE0 |  (usUnicode >> 12));
        *(pucUtf8Str + 1)   = (VOS_UINT8)(0x80 | ((usUnicode >> 6 ) & 0x3F));
        *(pucUtf8Str + 2)   = (VOS_UINT8)(0x80 |  (usUnicode &  0x3F));

        return 3;
    }
    else
    {
        return 0;
    }
}
/*****************************************************************************
 函 数 名  : TAF_STD_DecodeUtf8CharacterFirstByte
 功能描述  : Utf8格式编码字符首字节解码，得到一个UTF8字符包含的字节数、和首字节中的数据bits内容
 输入参数  :*pucUtf8Char                 -UTF-8字符的首个byte
 输出参数  :*pusUtf8CharacterContent     -输出UTF-8字符首个byte解码出来的数据内容，
                                          如果只想判断UTF8字符包含的字节数，此变量传入VOS_NULL_PTR
            *pulUtf8CharacterByteNum     -UTF-8字符包含的字节数(包含首个字节)
 返 回 值  : TAF_ERROR_CODE_ENUM_UINT32  -解码结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年7月6日
    作    者   : t00323010
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL TAF_ERROR_CODE_ENUM_UINT32 TAF_STD_DecodeUtf8CharacterFirstByte(
    VOS_UINT8                          *pucUtf8FirstByte,
    VOS_UINT16                         *pusUtf8CharacterContent,
    VOS_UINT32                         *pulUtf8CharacterByteNum
)
{
    /* 计算高位bit起连续1的个数 */
    for (*pulUtf8CharacterByteNum = 0; *pulUtf8CharacterByteNum < 8; (*pulUtf8CharacterByteNum)++)
    {
        if (0 == ((*pucUtf8FirstByte) & (0x80 >> (*pulUtf8CharacterByteNum))))
        {
            break;
        }
    }

    /* 保证函数通用性，支持仅查询utf8字符包含的字节数，如果输出为NULL，仅返回字节数 */
    if (VOS_NULL_PTR == pusUtf8CharacterContent)
    {
        return TAF_ERR_NO_ERROR;
    }

    /* 不支持超过4个及4个以上字节组成一个字符的UTF8编码 */
    if (3 < *pulUtf8CharacterByteNum)
    {
        /* 数据帧头异常，超过目前支持的最大字节个数，返回失败 */
        return TAF_DECODE_ERR_UTF8_BEYOND_MAX_BYTE_LIMIT;
    }
    else if (1 < *pulUtf8CharacterByteNum)
    {
        /* utf8字符由由多个字节组成 */
        *pusUtf8CharacterContent = *pucUtf8FirstByte & (0x7F >> (*pulUtf8CharacterByteNum));
    }
    else if (0 == *pulUtf8CharacterByteNum)
    {
        /* utf8字符由单字节组成 */
        *pusUtf8CharacterContent = *pucUtf8FirstByte;

        *pulUtf8CharacterByteNum = 1;
    }
    else
    {
        /* 数据帧头异常(bit7~bit6: 10)，返回失败 */
        return TAF_DECODE_ERR_UTF8_ABNORMAL_BYTE_HEADER;
    }

    return TAF_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertUtf8ToUcs2
 功能描述  : Utf8码流转Ucs2码流
             大端序输入
 输入参数  :*pucUtf8Str                 UTF-8码流
             ulUtf8Len                  UTF-8码流长度(字节数)
             ulUcs2BuffLen              UCS2码流缓存大小(编码个数)
 输出参数  :*pucUcs2Str                 UCS2码流地址
             VOS_UINT32                 UCS2长度(编码个数)
 返 回 值  : VOS_UINT32                 编码结果
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月13日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertUtf8ToUcs2(
    VOS_UINT8                          *pucUtf8Str,
    VOS_UINT32                          ulUtf8Len,
    VOS_UINT16                         *pusUcs2Str,
    VOS_UINT32                          ulUcs2BuffLen,
    VOS_UINT32                         *pulUcs2Idx
)
{
    VOS_UINT32                          ulUcs2Idx;
    VOS_UINT32                          ulUtf8Idx;
    VOS_UINT32                          ulUtf8Num;
    VOS_UINT8                           ucUtf8Char;
    VOS_UINT32                          ulRslt;

    if ((VOS_NULL_PTR == pusUcs2Str)
     || (VOS_NULL_PTR == pucUtf8Str))
    {
        return TAF_ERR_NULL_PTR;
    }

    ulUtf8Num = 0;
    ulUcs2Idx = 0;

    for (ulUtf8Idx = 0; ulUtf8Idx < ulUtf8Len; ulUtf8Idx++)
    {
        ucUtf8Char = *(pucUtf8Str + ulUtf8Idx);

        /* Utf8第一个字节 */
        if (0 == ulUtf8Num)
        {
            ulRslt = TAF_STD_DecodeUtf8CharacterFirstByte(&ucUtf8Char, pusUcs2Str + ulUcs2Idx, &ulUtf8Num);

            /* 头字节解码通用函数输出的是utf8的字节数(包含头字节)，所以要减1 */
            ulUtf8Num--;


            if (TAF_ERR_NO_ERROR != ulRslt)
            {
                return ulRslt;
            }
        }
        else
        {
            /* Utf8后续字节 */
            if (0x2 == (ucUtf8Char >> 6))
            {
                *(pusUcs2Str + ulUcs2Idx) = (VOS_UINT16)(*(pusUcs2Str + ulUcs2Idx) << 6) | (ucUtf8Char & 0x3F);
                ulUtf8Num--;
            }
            else
            {
                MN_INFO_LOG("TAF_STD_ConvertUtf8ToUcs2 Invaild follow Byte");

                /* 非数据帧头异常，返回失败 */
                return TAF_DECODE_ERR_UTF8_ABNORMAL_BYTE_CONTENT;
            }
        }

        /* 当前Utf8字节是Utf8编码的最后一个字节 */
        if (0 == ulUtf8Num)
        {
            ulUcs2Idx++;
            if (ulUcs2Idx >= ulUcs2BuffLen)
            {
                break;
            }
        }
    }

    *pulUcs2Idx = ulUcs2Idx;


    return TAF_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertUcs2ToUtf8
 功能描述  : Ucs2转Utf8编码
             大端序输出
 输入参数  :*pucUcs2Str                 UCS2码流
             ulUcs2Len                  UCS2码流长度(编码个数)
             ulUtf8BuffLen              UTF-8码流缓存大小(字节数)
 输出参数  :*pucUtf8Str                 UTF-8码流
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月12日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertUcs2ToUtf8(
    VOS_UINT16                         *pusUcs2Str,
    VOS_UINT32                          ulUcs2Len,
    VOS_UINT8                          *pucUtf8Str,
    VOS_UINT32                          ulUtf8BuffLen
)
{
    VOS_UINT32                          ulUcs2Idx;
    VOS_UINT32                          ulUtf8Idx;
    VOS_UINT32                          ulEncodeLen;

    if ((VOS_NULL_PTR == pusUcs2Str)
     || (VOS_NULL_PTR == pucUtf8Str))
    {
        return 0;
    }

    ulUtf8Idx = 0;

    for (ulUcs2Idx = 0; ulUcs2Idx < ulUcs2Len; ulUcs2Idx++)
    {
        ulEncodeLen  = TAF_STD_EncodeUnicodeToUtf8(*(pusUcs2Str + ulUcs2Idx), pucUtf8Str + ulUtf8Idx, ulUtf8BuffLen - ulUtf8Idx);
        ulUtf8Idx   += ulEncodeLen;

        if (0 == ulEncodeLen)
        {
            break;
        }
    }

    return ulUtf8Idx;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertUcs2To8Bit
 功能描述  : 强制将UCS2码流转8Bit码流
 输入参数  :*pusUcs2Str                 UCS2码流
             ulUcs2Len                  UCS2码流长度(编码个数)
             ul8BitBuffLen              8Bit缓存大小(字节数)
 输出参数  :*puc8BitStr                 8Bit码流
 返 回 值  : VOS_UINT32                 8Bit码流长度(字节数)
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月15日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertUcs2To8Bit(
    VOS_UINT16                         *pusUcs2Str,
    VOS_UINT32                          ulUcs2Len,
    VOS_UINT8                          *puc8BitStr,
    VOS_UINT32                          ul8BitBuffLen
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ul8BitLen;

    ul8BitLen = ulUcs2Len > ul8BitBuffLen ? ul8BitBuffLen : ulUcs2Len;

    for (ulIndex = 0; ulIndex < ul8BitLen; ulIndex++)
    {
        *(puc8BitStr + ulIndex) = (VOS_UINT8)*(pusUcs2Str + ulIndex);
    }

    return ul8BitLen;
}

/*****************************************************************************
 函 数 名  : TAF_STD_Convert8BitToUcs2
 功能描述  : 8Bit转UCS2码流
 输入参数  :*puc8BitStr                 8Bit码流
             ul8BitLen                  8Bit码流长度(字节数)
             ulUcs2BuffLen              UCS2码流缓存大小(编码个数)
 输出参数  :*pusUcs2Str                 UCS2码流地址
 返 回 值  : VOS_UINT32                 UCS2码流长度(编码个数)
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月15日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_Convert8BitToUcs2(
    VOS_UINT8                          *puc8BitStr,
    VOS_UINT32                          ul8BitLen,
    VOS_UINT16                         *pusUcs2Str,
    VOS_UINT32                          ulUcs2BuffLen
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulUcs2Len;

    ulUcs2Len = ul8BitLen < ulUcs2BuffLen ? ul8BitLen : ulUcs2BuffLen;

    for (ulIndex = 0; ulIndex < ulUcs2Len; ulIndex++)
    {
        *(pusUcs2Str + ulIndex) = *(puc8BitStr + ulIndex);
    }

    return ulUcs2Len;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertStrToUcs2
 功能描述  : 将码流转换成UCS2编码
             大端序输入
 输入参数  :*pstSrcStr                  输入码流结构
             ulUcs2BuffLen              UCS2码流缓存大小(UCS2编码个数)
 输出参数  :*pusUcs2Str                 UCS2码流
            *pulUcs2Len                 UCS2码流实际长度(UCS2编码个数)
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月13日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertStrToUcs2(
    TAF_STD_STR_WITH_ENCODING_TYPE_STRU            *pstSrcStr,
    VOS_UINT16                                     *pusUcs2Str,
    VOS_UINT32                                      ulUcs2BuffLen,
    VOS_UINT32                                     *pulUcs2Len
)
{
    VOS_UINT8                                      *pucUnpackBuff       = VOS_NULL_PTR;

    if ((VOS_NULL_PTR == pstSrcStr )
     || (VOS_NULL_PTR == pusUcs2Str)
     || (VOS_NULL_PTR == pulUcs2Len)
     || (VOS_NULL_PTR == pstSrcStr->pucStr))
    {
        return TAF_ERR_NULL_PTR;
    }

    MN_INFO_LOG1("TAF_STD_ConvertStrToUcs2: encoding", pstSrcStr->enCoding);

    switch (pstSrcStr->enCoding)
    {
        case TAF_STD_ENCODING_7BIT:
            /* 7bit: 先解码成8bit,再将8bit变为16bit的UCS2 */
            pucUnpackBuff   = (VOS_UINT8*)PS_MEM_ALLOC(WUEPS_PID_TAF, ulUcs2BuffLen * sizeof(VOS_UINT8));

            if (VOS_NULL_PTR == pucUnpackBuff)
            {
                return TAF_ERR_ERROR;
            }

            if (VOS_ERR == TAF_STD_UnPack7Bit(pstSrcStr->pucStr, ulUcs2BuffLen, 0, pucUnpackBuff))
            {
                PS_MEM_FREE(WUEPS_PID_TAF, pucUnpackBuff);
                MN_ERR_LOG("TAF_STD_ConvertStrToUcs2: Unpack fail");
                return TAF_ERR_ERROR;
            }

            *pulUcs2Len = ulUcs2BuffLen;

            /* 去除7Bit压缩时填充位 */
            if (0x0d == (pucUnpackBuff[*pulUcs2Len - 1]))
            {
                (*pulUcs2Len)--;
            }

            /* DefaultAlpha->Ascii */
            TAF_STD_ConvertDefAlphaToAscii(pucUnpackBuff, *pulUcs2Len, pucUnpackBuff, pulUcs2Len);

            /* 单字节转双字节 */
            *pulUcs2Len = TAF_STD_Convert8BitToUcs2(pucUnpackBuff, *pulUcs2Len, pusUcs2Str, ulUcs2BuffLen);

            PS_MEM_FREE(WUEPS_PID_TAF, pucUnpackBuff);
            break;

        case TAF_STD_ENCODING_8BIT:
            TAF_STD_Convert8BitToUcs2(pstSrcStr->pucStr, ulUcs2BuffLen, pusUcs2Str, ulUcs2BuffLen);
            *pulUcs2Len = pstSrcStr->ulLen;
            break;

        case TAF_STD_ENCODING_UCS2:
            TAF_MEM_CPY_S(pusUcs2Str, ulUcs2BuffLen, pstSrcStr->pucStr, ulUcs2BuffLen);

            /* 输入为大端 内部为小端，需要转换 */
            (VOS_VOID)TAF_STD_ChangeEndian((VOS_UINT8*)pusUcs2Str, ulUcs2BuffLen, (VOS_UINT8*)pusUcs2Str, sizeof(VOS_UINT16));

            /* Ucs2Len指UINT16的UCS2码的个数， pstSrcStr->ulLen指码流字节数 */
            *pulUcs2Len = pstSrcStr->ulLen / 2;
            break;

        case TAF_STD_ENCODING_UTF8:
            /* 增加编码转换失败原因值 */
            return TAF_STD_ConvertUtf8ToUcs2(pstSrcStr->pucStr, pstSrcStr->ulLen, pusUcs2Str, ulUcs2BuffLen, pulUcs2Len);

        default:
            return TAF_ERR_ERROR;
    }

    return TAF_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertUcs2ToStr
 功能描述  : Unicode转到目标格式
             大端序输出
 输入参数  :*pusUcs2Str                 UCS2码流
             ulUcs2Len                  UCS2码流长度(编码个数)
             ulDstStrBuffLen            目标串缓存空间大小(字节数)
 输出参数  :*pstDstStr                  目标字符串
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月15日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertUcs2ToStr(
    TAF_STD_STR_WITH_ENCODING_TYPE_STRU            *pstDstStr,
    VOS_UINT16                                     *pusUcs2Str,
    VOS_UINT32                                      ulUcs2Len,
    VOS_UINT32                                      ulDstStrBuffLen
)
{
    VOS_UINT8                          *pucBuffStr  = VOS_NULL_PTR;
    VOS_UINT32                          ulUcs2BuffLen;
    VOS_UINT32                          ulAllocBuffLen;
    VOS_UINT32                          ul8BitLen;
    VOS_UINT32                          ulRslt;

    if ((VOS_NULL_PTR == pstDstStr )
     || (VOS_NULL_PTR == pusUcs2Str)
     || (VOS_NULL_PTR == pstDstStr->pucStr))
    {
        return TAF_ERR_NULL_PTR;
    }

    MN_INFO_LOG1("TAF_STD_ConvertUcs2ToStr: encoding", pstDstStr->enCoding);

    pstDstStr->ulLen = 0;
    TAF_MEM_SET_S(pstDstStr->pucStr, ulDstStrBuffLen, 0, ulDstStrBuffLen);

    switch (pstDstStr->enCoding)
    {
        case TAF_STD_ENCODING_UCS2:
            TAF_MEM_CPY_S(pstDstStr->pucStr, ulDstStrBuffLen, pusUcs2Str, ulUcs2Len * sizeof(VOS_UINT16));

            /* 增加CHR上报 */
            /* 内部是小端序，拷贝输出时候需要转成大端序 */
            ulRslt = TAF_STD_ChangeEndian(pstDstStr->pucStr, ulUcs2Len * sizeof(VOS_UINT16), pstDstStr->pucStr, sizeof(VOS_UINT16));

            pstDstStr->ulLen = ulUcs2Len * 2;

            return ulRslt;

        case TAF_STD_ENCODING_8BIT:
            pstDstStr->ulLen = TAF_STD_ConvertUcs2To8Bit(pusUcs2Str, ulUcs2Len, pstDstStr->pucStr, ulDstStrBuffLen);
            break;

        case TAF_STD_ENCODING_7BIT:
            /* 7Bit支持扩展表后最多用2个字节表示一个Unicode */
            ul8BitLen      = 0;
            ulUcs2BuffLen  = ulUcs2Len * 2;
            ulAllocBuffLen = ulUcs2BuffLen * 2;

            /* 申请2倍缓存分开使用 */
            pucBuffStr    = (VOS_UINT8*)PS_MEM_ALLOC(WUEPS_PID_TAF, ulAllocBuffLen * sizeof(VOS_UINT8));
            if (VOS_NULL_PTR == pucBuffStr)
            {
                return TAF_ERR_ERROR;
            }
            TAF_MEM_SET_S(pucBuffStr, ulUcs2BuffLen * sizeof(VOS_UINT8), 0, ulUcs2Len * sizeof(VOS_UINT8));

            /* 先转成8Bit编码 */
            ulUcs2Len = TAF_STD_ConvertUcs2To8Bit(pusUcs2Str, ulUcs2Len, pucBuffStr, ulUcs2BuffLen);

            /* 转成DefaultAlpha，使用后半段缓存 */
            if (MN_ERR_NO_ERROR != TAF_STD_ConvertAsciiToDefAlpha(pucBuffStr, ulUcs2Len, pucBuffStr + ulUcs2BuffLen, &ul8BitLen, ulUcs2BuffLen))
            {
                PS_MEM_FREE(WUEPS_PID_TAF, pucBuffStr);
                MN_ERR_LOG("TAF_STD_ConvertUcs2ToStr: Ascii->DefAlpha fail");
                return TAF_ERR_ERROR;
            }

            /* 压缩成7Bit编码 */
            if (VOS_OK != TAF_STD_Pack7Bit(pucBuffStr + ulUcs2BuffLen, ul8BitLen, 0, pstDstStr->pucStr, &(pstDstStr->ulLen)))
            {
                PS_MEM_FREE(WUEPS_PID_TAF, pucBuffStr);
                MN_ERR_LOG("TAF_STD_ConvertUcs2ToStr: pack fail");
                return TAF_ERR_ERROR;
            }

            /* 23038 6.1.2.3.1，最后7Bit如果没有数据需要填CR以免被识别成@ */
            if (7 == ul8BitLen % 8)
            {
                *(pstDstStr->pucStr + pstDstStr->ulLen - 1) |= 0x1A;
            }

            PS_MEM_FREE(WUEPS_PID_TAF, pucBuffStr);
            break;

        case TAF_STD_ENCODING_UTF8:
            pstDstStr->ulLen = TAF_STD_ConvertUcs2ToUtf8(pusUcs2Str, ulUcs2Len, pstDstStr->pucStr, ulDstStrBuffLen);
            break;

        default:
            return TAF_ERR_ERROR;
    }

    return TAF_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_IsSuitableEncodeForUcs2
 功能描述  : 判断目标码流是否是UCS2码流适合的编码
 输入参数  :*pusUcs2                    UCS2码流
             ulUcs2Len                  UCS2码流长度(编码个数)
             enCoding                   目标编码类型
 输出参数  : 无
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月15日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_IsSuitableEncodeForUcs2(
    VOS_UINT16                         *pusUcs2,
    VOS_UINT32                          ulUcs2Len,
    TAF_STD_ENCODING_TYPE_ENUM_U8       enCoding
)
{
    VOS_UINT32                          ulIndex;
    VOS_UINT32                          ulDecodeRet1;
    VOS_UINT32                          ulDecodeRet2;
    VOS_UINT8                           ucTmp;
    VOS_UINT16                          usUcs2Char;

    if (VOS_NULL_PTR == pusUcs2)
    {
        return TAF_ERR_NULL_PTR;
    }

    ucTmp = 0;

    for (ulIndex = 0; ulIndex < ulUcs2Len; ulIndex++)
    {
        usUcs2Char = pusUcs2[ulIndex];

        switch (enCoding)
        {
            case TAF_STD_ENCODING_7BIT:
                ulDecodeRet1 = TAF_STD_RemapAsciiToSelectTable((VOS_UINT8)usUcs2Char,
                                                                f_aucMsgAsciiSfxDefAlpha,
                                                                TAF_STD_MAX_GSM7BITDEFALPHA_NUM,
                                                                &ucTmp);
                ulDecodeRet2 = TAF_STD_RemapAsciiToSelectTable((VOS_UINT8)usUcs2Char,
                                                                f_aucMsgAsciiSfxDefAlphaExt,
                                                                TAF_STD_MAX_GSM7BITDEFALPHA_NUM,
                                                                &ucTmp);
                if (((VOS_FALSE == ulDecodeRet1)
                  && (VOS_FALSE == ulDecodeRet2))
                 || (0xFF < usUcs2Char))
                {
                    return TAF_ERR_ERROR;
                }
                break;

            case TAF_STD_ENCODING_8BIT:
                if (0x7F < usUcs2Char)
                {
                    return TAF_ERR_ERROR;
                }
                break;

            case TAF_STD_ENCODING_UCS2:
            case TAF_STD_ENCODING_UTF8:
                return TAF_ERR_NO_ERROR;

            default:
                return TAF_ERR_ERROR;
        }
    }

    return TAF_ERR_NO_ERROR;
}

/*****************************************************************************
 函 数 名  : TAF_STD_ConvertStrEncodingType
 功能描述  : 根据指定编码类型转换字符串，
             先将源字符串(UINT8)转到中间态UCS2(UINT16)编码串，
             再由UCS2转到目标编码(UINT8)
             大端输入 大端输出
 输入参数  : pstSrcStr           源字符串结构
             enDstCoding         目标编码类型
             ulDstBuffLen        目标字符串的缓存大小(字节数)
 输出参数  : pstDstStr           目标编码字符串结构
 返 回 值  : VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年5月11日
    作    者   : z00385378
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 TAF_STD_ConvertStrEncodingType(
    TAF_STD_STR_WITH_ENCODING_TYPE_STRU            *pstSrcStr,
    TAF_STD_ENCODING_TYPE_ENUM_U8                   enDstCoding,
    VOS_UINT32                                      ulDstBuffLen,
    TAF_STD_STR_WITH_ENCODING_TYPE_STRU            *pstDstStr
)
{
    VOS_UINT16                                     *pusUcs2Str      = VOS_NULL_PTR;
    VOS_UINT32                                      ulUcs2Len;
    VOS_UINT32                                      ulUcs2BuffLen;
    VOS_UINT32                                      ulRslt;

    if ((VOS_NULL_PTR == pstSrcStr)
     || (VOS_NULL_PTR == pstDstStr)
     || (VOS_NULL_PTR == pstSrcStr->pucStr)
     || (VOS_NULL_PTR == pstDstStr->pucStr))
    {
        return TAF_ERR_NULL_PTR;
    }

    /* 计算申请Unicode内存大小 */
    /* pstSrcStr->Len指字节数， ulUcs2Len指UCS2编码数， 前者UINT8 后者UINT16 */
    if (TAF_STD_ENCODING_7BIT == pstSrcStr->enCoding)
    {
        ulUcs2Len = pstSrcStr->ulLen * 8 / 7;
    }
    else
    {
        ulUcs2Len = pstSrcStr->ulLen;
    }
    ulUcs2BuffLen = ulUcs2Len;

    pusUcs2Str    = (VOS_UINT16*)PS_MEM_ALLOC(WUEPS_PID_TAF, ulUcs2BuffLen * sizeof(VOS_UINT16));
    if (VOS_NULL_PTR == pusUcs2Str)
    {
        return TAF_ERR_ERROR;
    }
    TAF_MEM_SET_S(pusUcs2Str, ulUcs2BuffLen * sizeof(VOS_UINT16), 0, ulUcs2BuffLen * sizeof(VOS_UINT16));

    /* 将输入码流转为Unicode(UCS2)格式 */
    ulRslt = TAF_STD_ConvertStrToUcs2(pstSrcStr, pusUcs2Str, ulUcs2BuffLen, &ulUcs2Len);

    if (TAF_ERR_NO_ERROR != ulRslt)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pusUcs2Str);/*lint !e516 */
        return ulRslt;
    }

    /* 是否能转成目标编码 */
    if (TAF_ERR_NO_ERROR != TAF_STD_IsSuitableEncodeForUcs2(pusUcs2Str, ulUcs2Len, enDstCoding))
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pusUcs2Str);/*lint !e516 */
        MN_INFO_LOG("TAF_STD_ConvertStrEncodingType Dst Encoding cannot be");
        return TAF_ERR_PARA_ERROR;
    }
    pstDstStr->enCoding = enDstCoding;

    /* 将Unicode(UCS2)码流转到指定格式 */
    ulRslt = TAF_STD_ConvertUcs2ToStr(pstDstStr, pusUcs2Str, ulUcs2Len, ulDstBuffLen);

    if (TAF_ERR_NO_ERROR != ulRslt)
    {
        PS_MEM_FREE(WUEPS_PID_TAF, pusUcs2Str);/*lint !e516 */
        return ulRslt;
    }

    PS_MEM_FREE(WUEPS_PID_TAF, pusUcs2Str);/*lint !e516 */
    return TAF_ERR_NO_ERROR;
}



