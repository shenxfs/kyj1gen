/**
 * @brief COFF File class
 * @fn coff.cpp
 * @author shenxfsn@@163.com
 * @version V1.1.0
 */
#include "coff.h"
#include <QDateTime>
#include <QFile>

/**
 * @brief CCoff::CCoff
 */
CCoff::CCoff()
{
    pCoffBuffer = NULL;
    pCoffOptionalHeader = NULL;
    pSectionHeader = NULL;
    pSmallSectionHeader = NULL;
    pSymbolsTab = NULL;
    strList = NULL;
    isGood = false;
}

/**
 * @brief CCoff::~CCoff
 */
CCoff::~CCoff()
{
    if(pCoffBuffer != NULL)
    {
        delete [] pCoffBuffer;
    }
}

/**
 * @brief CCoff::CCoff
 * @param FileName
 */
CCoff::CCoff(QString &FileName)
{
    isGood = this->open(FileName);
}

/**
 * @brief CCoff::open
 * @param CoffFileName
 * @return
 */
bool CCoff::open(QString &CoffFileName)
{
    QFile file(CoffFileName);
    bool ret = false;
    if(file.open( QIODevice::ReadOnly))
    {
        if(pCoffBuffer != NULL)
        {
            delete [] pCoffBuffer;
        }
        isGood = false;
        pCoffBuffer = NULL;
        pCoffOptionalHeader = NULL;
        pSectionHeader = NULL;
        pSmallSectionHeader = NULL;
        pSymbolsTab = NULL;
        strList = NULL;
        TCoffHeader head;
        memset(&head,0,sizeof(TCoffHeader));
        file.read((char*)&head,sizeof(TCoffHeader));
        if((head.usVersionID == 0x00c1) || (head.usVersionID == 0x00c2))
        {
            uiCoffFileSize = (quint32)file.size();
            file.seek(0);
            pCoffBuffer = new qint8[uiCoffFileSize];
            if(pCoffBuffer != NULL)
            {
                memset(pCoffBuffer,0,uiCoffFileSize);
                file.read((char*)pCoffBuffer,uiCoffFileSize);
                pCoffHeader = (TCoffHeader*)pCoffBuffer;
                if(pCoffHeader->uiOptionalHeaderBytes != 0)
                {
                    pCoffOptionalHeader = (TCoffOptionalHeader*)&pCoffBuffer[sizeof(TCoffHeader)];
                    if(pCoffHeader->usVersionID == 0x00c2)
                    {
                        pSectionHeader = (TSectionHeader*)&pCoffBuffer[sizeof(TCoffHeader)+sizeof(TCoffOptionalHeader)];
                    }
                    else
                    {
                        pSmallSectionHeader = (TSmallSectionHeader*)&pCoffBuffer[sizeof(TCoffHeader)+sizeof(TCoffOptionalHeader)];
                    }
                }
                else if(pCoffHeader->usVersionID == 0x00c2)
                {
                    pSectionHeader = (TSectionHeader*)&pCoffBuffer[sizeof(TCoffHeader)];
                }
                else
                {
                    pSmallSectionHeader = (TSmallSectionHeader*)&pCoffBuffer[sizeof(TCoffHeader)];
                }
                pSymbolsTab = (TSymbolTable*)&pCoffBuffer[pCoffHeader->uiSymbolPointer];
                strList = (char*)&pCoffBuffer[pCoffHeader->uiSymbolPointer+pCoffHeader->uiSymbolEntryNumber*sizeof(TSymbolTable)];
                isGood = true;
                ret = true;
            }
        }
        file.close();
    }
    return ret;
}

/**
 * @brief CCoff::GetCoffInfo
 * @return
 */
QString CCoff::GetCoffInfo(void)
{
    QString res("");
    if(isGood)
    {
        QString str;
        str = QString("\r\nCOFF 版本      :0x%1").arg(pCoffHeader->usVersionID,4,16);
        res += str;
        str = QString("\r\n段数目         :%1").arg(pCoffHeader->usSectionNumber,4,10);
        res += str;
    }
    return res;
}

/**
 * @brief CCoff::FindSymbol
 * @param sym
 * @return
 */
TSymbolTable* CCoff::FindSymbol(QString &sym)
{
    TSymbolTable *pRec = NULL;
    quint32 ind = 0;
    QString str("");
    if(isGood && (sym.size() != 0))
    {
        while((ind < pCoffHeader->uiSymbolEntryNumber)&&(pRec == NULL))
        {
            str = (const char*)&pSymbolsTab[ind].sName.sName;
            if(str.size() == 0)
            {
                str = (const char*)&strList[pSymbolsTab[ind].sName.uiIndex[1]];
            }
            else if(str.size() > 8)
            {
                str = str.left(8);
            }
            if(str == sym)
            {
                pRec = &pSymbolsTab[ind];
            }
            if(pSymbolsTab[ind].cMEM != 0)
            {
              ind++;
            }
            ind++;
        }
    }
    return pRec;
}

/**
 * @brief CCoff::GetSymbolValue
 * @param sym
 * @param pValue
 * @param len
 * @return
 */
bool CCoff::GetSymbolValue(QString &sym,void *pValue,quint16 len)
{
    TSymbolTable *pSym;
    bool rec = false;
    pSym = FindSymbol(sym);
    if(pSym != NULL)
    {
        quint16 ind = pSym->siNum - 1;
        if(pCoffHeader->usVersionID == 0x00c1)
        {
            if((pSmallSectionHeader[ind].uiRawDataPointer !=0) && (pSym->cStorClass == 6) )
            {
                int offset = pSym->lSymValue - pSmallSectionHeader[ind].uiPysicalAddress;
                offset *= 2;
                offset += pSmallSectionHeader[ind].uiRawDataPointer;
                if(len == 1)
                {
                    *((qint16*)pValue) = *((qint16*)(&pCoffBuffer[offset]));
                }
                else if (len == 2)
                {
                    *((qint32*)pValue) = *((qint32*)(&pCoffBuffer[offset]));
                }
                else
                {
                    for (quint16 i = 0;i < len;i++)
                    {
                        qint16 *den = (qint16*)pValue;
                        den[i] = *((qint16 *)(&pCoffBuffer[offset + i*2]));
                    }
                }
                if(len != 0)
                {
                    rec = true;
                }
            }
        }
        else
        {
            if(pSectionHeader[ind].uiRawDataPointer != 0)
            {
                int offset = pSym->lSymValue - pSectionHeader[ind].uiPysicalAddress;
                offset *= 2;
                offset += pSmallSectionHeader[ind].uiRawDataPointer;
                if(len == 1)
                {
                    *((qint16*)pValue) = *((qint16*)(&pCoffBuffer[offset]));
                }
                else if (len == 2)
                {
                    *((qint32*)pValue) = *((qint32*)&pCoffBuffer[offset]);
                }
                else
                {
                    for (quint16 i = 0;i < len;i++)
                    {
                        qint16 *den = (qint16*)pValue;
                        den[i] = *((qint16 *)(&pCoffBuffer[offset + i*2]));
                    }
                }
                if(len != 0)
                {
                    rec = true;
                }
            }
            else
            {
                TSectionHeader* pSec = FindSection(".cinit");
                if((pSec != 0) && (pSec->uiRawDataPointer != 0))
                {
                    qint16 *pRaw = (qint16*)&pCoffBuffer[pSec->uiRawDataPointer];
                    qint32 off = 0;
                    qint32 ends = pSec->uiSectionSize;
                    qint16 num = 0;
                    do
                    {
                        num = -pRaw[off];
                        off++;
                        qint32 addr = *((qint32*)&pRaw[off]);
                        off += 2;
                        if(addr == pSym->lSymValue)
                        {
                            if(num != len)
                            {
                                rec = false;
                            }
                            else if(num == 1)
                            {
                                *((qint16*)pValue) = pRaw[off];
                                rec = true;
                            }
                            else if(num == 2)
                            {
                                *((qint32*)pValue) =*((qint32*)&pRaw[off]);
                                rec = true;
                            }
                            else
                            {
                                for(ind = 0;ind < num;ind++)
                                {
                                    ((qint16*)pValue)[ind] = pRaw[ind];
                                }
                                rec = true;
                            }
                        }
                        off += num;
                    }
                    while((off < ends) && (!rec));
                }
            }
        }
    }
    return rec;
}

/**
 * @brief CCoff::SetSymbolValue
 * @param sym
 * @param pValue
 * @param len
 * @return
 */
bool CCoff::SetSymbolValue(QString &sym,void *pValue,quint16 len)
{
    TSymbolTable *pSym;
    bool rec = false;
    pSym = FindSymbol(sym);
    if(pSym != NULL)
    {
        quint16 ind = pSym->siNum - 1;
        if(pCoffHeader->usVersionID == 0x00c1)
        {
            if((pSmallSectionHeader[ind].uiRawDataPointer !=0) && (pSym->cStorClass == 6) )
            {
                int offset = pSym->lSymValue - pSmallSectionHeader[ind].uiPysicalAddress;
                offset *= 2;
                offset += pSmallSectionHeader[ind].uiRawDataPointer;
                if(len == 1)
                {
                    *((qint16*)(&pCoffBuffer[offset])) = *((qint16*)pValue);
                }
                else if (len == 2)
                {
                    *((qint32*)(&pCoffBuffer[offset])) = *((qint32*)pValue);
                }
                else
                {
                    for (quint16 i = 0;i < len;i++)
                    {
                        qint16 *src = (qint16*)pValue;
                        *((qint16 *)(&pCoffBuffer[offset + i*2])) = src[i];
                    }
                }
                if(len != 0)
                {
                    rec = true;
                }
            }
        }
        else
        {
            if(pSectionHeader[ind].uiRawDataPointer != 0)
            {
                int offset = pSym->lSymValue - pSectionHeader[ind].uiPysicalAddress;
                offset *= 2;
                offset += pSectionHeader[ind].uiRawDataPointer;
                if(len == 1)
                {
                    *((qint16*)(&pCoffBuffer[offset])) = *((qint16*)pValue);
                }
                else if (len == 2)
                {
                    *((qint32*)(&pCoffBuffer[offset])) = *((qint32*)pValue);
                }
                else
                {
                    for (quint16 i = 0;i < len;i++)
                    {
                        qint16 *src = (qint16*)pValue;
                        *((qint16 *)(&pCoffBuffer[offset + i*2])) = src[i];
                    }
                }
                if(len != 0)
                {
                    rec = true;
                }
            }
            else
            {
                TSectionHeader *pSec = FindSection(".cinit");
                if((pSec != NULL) && pSec->uiRawDataPointer != 0)
                {
                    qint16 *pRaw = (qint16*)&pCoffBuffer[pSec->uiRawDataPointer];
                    qint32 off = 0;
                    qint32 ends = pSec->uiSectionSize;
                    qint16 num = 0;
                    do
                    {
                        num = -pRaw[off];
                        off++;
                        qint32 addr = *((qint32*)&pRaw[off]);
                        off += 2;
                        if(addr == pSym->lSymValue)
                        {
                            if(num != len)
                            {
                                rec = false;
                            }
                            else if(num == 1)
                            {
                                pRaw[off] = *((qint16*)pValue);
                                rec = true;
                            }
                            else if(num == 2)
                            {
                                *((qint32*)&pRaw[off]) = *((qint32*)pValue);
                                rec = true;
                            }
                            else
                            {
                                for(ind = 0;ind < num;ind++)
                                {
                                    pRaw[ind] = ((qint16*)pValue)[ind];
                                }
                                rec = true;
                            }
                        }
                        off += num;
                    }
                    while((off < ends) && (!rec));
                }
            }
        }
    }
    return rec;
}

/**
 * @brief CCoff::Write
 * @param file
 * @return
 */
bool CCoff::Write(QString &file)
{
    bool rec = false;
    if(isGood)
    {
        QFile fout(file);
        if(fout.open(QFileDevice::WriteOnly))
        {
            QDateTime stamp;
            stamp = QDateTime::currentDateTime();
            pCoffHeader->iTimeStamp = stamp.toTime_t();
            fout.write((char*)pCoffBuffer,uiCoffFileSize);
            fout.close();
            rec =true;
        }
    }
    return rec;
}

/**
 * @brief CCoff::Resize
 * @param SecName
 * @param uResize
 * @return
 */
bool CCoff::Resize(QString &SecName,quint32 uResize)
{
    bool rec = false;
    quint16 i = 0;
    QString str("");
    while( (SecName != str) && (i < pCoffHeader->usSectionNumber))
    {
        if(pSectionHeader != NULL)
        {
            str = QString((QChar*)pSectionHeader[i].SectionName.sName,8);
            if(str.size() == 0)
            {
                str = &strList[pSectionHeader[i].SectionName.uiIndex[1]];
            }
        }
        else
        {
            if(pSmallSectionHeader != NULL)
            {
                str = QString((QChar*)pSmallSectionHeader[i].SectionName.sName,8);
                if(str.size() == 0)
                {
                    str = &strList[pSmallSectionHeader[i].SectionName.uiIndex[1]];
                }
            }
        }
        i++;
    }

    if((SecName == str) && (i != 0) && (uResize != 0))
    {
        i--;
        qint8 *newbuf = new qint8[uiCoffFileSize + uResize];
        if(newbuf != NULL)
        {
            quint32 sd,len;
            if(pSectionHeader != NULL)
            {
                if(pSectionHeader[i].uiRawDataPointer != 0)
                {
                    len = pSectionHeader[i].uiRawDataPointer + uResize*2;
                    memcpy(newbuf,pCoffBuffer,len);
                    sd = len;
                    memset(&newbuf[sd],0,uResize*2);
                    sd = pSectionHeader[i].uiRawDataPointer+pSectionHeader[i].uiSectionSize+uResize*2;

                }
            }
            if(pSmallSectionHeader != NULL)
            {
                if(pSmallSectionHeader[i].uiRawDataPointer != 0)
                {
                    len = pSmallSectionHeader[i].uiRawDataPointer + uResize*2;
                    memcpy(newbuf,pCoffBuffer,len);
                }
            }
        }
    }
    return rec;
}

/**
 * @brief CCoff::VerifyCode
 * @param RomCheck
 * @return
 */
bool CCoff::VerifyCode(QString &RomCheck)
{
    bool rec = false;
    (void)RomCheck;
    return rec;
}

/**
 * @brief CCoff::GetimeStamp
 * @return
 */
time_t CCoff::GetimeStamp(void)
{
    time_t rec = 0;
    if(isGood)
    {
        rec = pCoffHeader->iTimeStamp;
    }
    return rec;
}

/**
 * @brief CCoff::SetimeStamp
 * @param t
 */
void CCoff::SetimeStamp(time_t t)
{
    if(isGood)
    {
        pCoffHeader->iTimeStamp = t;
    }
}

/**
 * @brief CCoff::GetRawSize
 * @return
 */
quint32 CCoff::GetRawSize(void)
{
    quint32 rec = 0;
    if(pSectionHeader != NULL)
    {
        for(quint32 i = 0;i < pCoffHeader->usSectionNumber;i++)
        {
            if(pSectionHeader[i].uiRawDataPointer != 0)
            {
                rec += pSectionHeader[i].uiSectionSize;
            }
        }
    }
    else
    {
        if(pSmallSectionHeader != NULL)
        {
            for(quint32 i = 0;i < pCoffHeader->usSectionNumber;i++)
            {
                if(pSmallSectionHeader[i].uiRawDataPointer != 0)
                {
                    rec += pSmallSectionHeader[i].uiSectionSize;
                }
            }
        }
    }
    return rec;
}

/**
 * @brief CCoff::GetSectionName
 * @param secNum
 * @return
 */
QString CCoff::GetSectionName(quint16 secNum)
{
    QString res("");
    if(isGood)
    {
        if((pSectionHeader != NULL) && (pCoffHeader->usSectionNumber))
        {
            TSectionHeader sec = pSectionHeader[secNum];
            res = QString((const char*)sec.SectionName.sName);
            if(res.size() == 0)
            {
                qint32 ind = sec.SectionName.uiIndex[1];
                res =(const char*)&strList[ind];
            }
            else if(res.size() > 8)
            {
                res = res.left(8);
            }
        }
        else if((pSmallSectionHeader != NULL) && (pCoffHeader->usSectionNumber))
        {
            TSmallSectionHeader sec = pSmallSectionHeader[secNum];
            res = QString((const char*)sec.SectionName.sName);
            if(res.size() == 0)
            {
                qint32 ind = sec.SectionName.uiIndex[1];
                res = (const char*)&strList[ind];
            }
            else if(res.size() > 8)
            {
                res = res.left(8);
            }
        }
    }
    return res;
}

/**
 * @brief CCoff::FindSection
 * @param sec
 * @return
 */
TSectionHeader *CCoff::FindSection(QString sec)
{
    TSectionHeader *rec = NULL;
    qint16 index = 0;
    QString str("");
    qint16 secNum = pCoffHeader->usSectionNumber;
    do
    {
        str = (const char*)pSectionHeader[index].SectionName.sName;
        if(str.size() == 0)
        {
            str = (const char*)&strList[pSectionHeader[index].SectionName.uiIndex[1]];
        }
        else if(str.size() >= 8)
        {
            str = str.left(8);
        }
        index++;
    }
    while((index < secNum)&&(str != sec));
    if(str == sec)
    {
        index--;
        rec = &pSectionHeader[index];
    }
    return rec;
}
