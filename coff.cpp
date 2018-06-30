#include "coff.h"
#include <QDateTime>
#include <QFile>
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

CCoff::~CCoff()
{
    if(pCoffBuffer != NULL)
    {
        delete [] pCoffBuffer;
    }
}

CCoff::CCoff(QString &FileName)
{
    isGood = this->open(FileName);
}

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

TSymbolTable* CCoff::FindSymbol(QString &sym)
{
    TSymbolTable *pRec = NULL;
    quint32 ind = 0;
    QString str("");
    if(isGood && (sym.size() != 0))
    {
        while((ind < pCoffHeader->uiSymbolEntryNumber)&&(str!=sym))
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
        }
    }
    return rec;
}

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
        }
    }
    return rec;
}

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

bool CCoff::VerifyCode(QString &RomCheck)
{
    bool rec = false;
    (void)RomCheck;
    return rec;
}

time_t CCoff::GetimeStamp(void)
{
    time_t rec = 0;
    if(isGood)
    {
        rec = pCoffHeader->iTimeStamp;
    }
    return rec;
}

void CCoff::SetimeStamp(time_t t)
{
    if(isGood)
    {
        pCoffHeader->iTimeStamp = t;
    }
}

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
