#ifndef COFF_H
#define COFF_H
#pragma pack(1)
#include <stdint.h>
#include <QString>

#define HEADER_SIZE 	22
#define OPTIONAL_HEADER_SIZE 	28
#define SECTION_HEADER_SIZE 	40
#define SYMBOL_TAB_SIZE 	18

//define of Header flag
#define	HEADER_FLAG_RELFLG 	0x001
//Relocation information was stripped from the file.
#define	HEADER_FLAG_EXEC 		0x002
//The file is relocatable (it contains no unresolved external	references).
#define	HEADER_FLAG_LSYMS 		0x008
//Local symbols were stripped from the file.
#define	HEADER_FLAG_LITTLE 		0x100
//The target is a little-endian device.
#define	HEADER_FLAG_BIG 		0x200
//The target is a big-endian device.

//define of section type
#define	SECTION_TYPE_REG 	0x0000000
//Regular section (allocated, relocated, loaded)
#define	SECTION_TYPE_DSECT 	0x0000001
//Dummy section (relocated, not allocated, not loaded)
#define	SECTION_TYPE_NOLOAD 	0x0000002
//Noload section (allocated, relocated, not loaded)
#define	SECTION_TYPE_COPY 	0x0000010
//Copy section (relocated, loaded, but not allocated; relocation entries are processed normally)
#define	SECTION_TYPE_TEXT 	0x0000020
//Section contains executable code
#define	SECTION_TYPE_DATA 	0x0000040
//Section contains initialized data
#define	SECTION_TYPE_BSS 	0x0000080
//Section contains uninitialized data
#define	SECTION_TYPE_BLOCK 	0x0001000
//Alignment used as a blocking factor
#define	SECTION_TYPE_PASS 	0x0002000
//Section should pass through uncxhanged
#define	SECTION_TYPE_CLINK 	00004000
//Section requires conditional linking
#define	SECTION_TYPE_VECTOR 	0x0008000
//Section contains vector table
#define	SECTION_TYPE_PADDED 	0x0010000
//Section has been padded

typedef struct _tCoffHeader
{
    quint16  usVersionID; 	//indicates version of COFF file structure
    quint16 usSectionNumber; 	//Number of section headers
    qint32 iTimeStamp; 	//indicates when the file was created
    quint32 uiSymbolPointer; 	//contains the symbol table's starting address
    quint32 uiSymbolEntryNumber; 	//Number of entries in the symbol table
    quint16 uiOptionalHeaderBytes;	//Number of bytes in the optional header.
        //This field is either 0 or 28; if it is 0, there is no optional file header.
    quint16 uiFlags;
/*		Mnemonic 	Flag 		Description
        F_RELFLG 	0001h 	Relocation information was stripped from the file.
        F_EXEC 		0002h 	The file is relocatable (it contains no unresolved external	references).
                    0004h 	Reserved
        F_LSYMS 		0008h 	Local symbols were stripped from the file.
        F_LITTLE 		0100h 	The target is a little-endian device.
        F_BIG 		0200h 	The target is a big-endian device. */
    quint16 uiTargetID; 	//magic number (0099h) indicates the file can be executed in a C6000 system
}TCoffHeader;

typedef struct _tCoffOptionalHeader
{
    quint16 usOptionalHeaderID;	//Optional file header magic number (0108h) indicates the Optional header for C6000
    quint16 usVersionStamp;
    quint32 uiTextSize; 	//Integer Size (in bytes) of .text section
    quint32 uiDataSize; 	//Integer Size (in bytes) of .data section
    quint32 uiBssSize; 	//Integer Size (in bytes) of .bss section
    quint32 uiEntryPoint;
    quint32 uiTextAddress; 	//Integer Beginning address of .text section
    quint32 uiDataAddress; 	//Integer Beginning address of .data section
}TCoffOptionalHeader;

typedef union
{
    qint8 sName[8];	//An 8-character section name padded with nulls
    qint32 uiIndex[2];	//A pointer into the string table if the symbol name is longer than eight characters
}COFFNAME;


typedef struct _tSectionHeader
{
    COFFNAME SectionName;
    quint32 uiPysicalAddress;	//Section's physical address (Run Address)
    quint32 uiVirtalAddress; 	//Section's virtual address (Load Address)
    quint32 uiSectionSize; 	//Section size in bytes
    quint32 uiRawDataPointer; 	//File pointer to raw data
    quint32 uiRelocationEntryPointer;	//File pointer to relocation entries
    quint32 uiLineEntryPointer;
    quint32 uiRelocationEntryNumber;	//Number of relocation entries
    quint32 uiLineEntryNumber;
    quint32 uiFlags;
/*		STYP_REG 	00000000h Regular section (allocated, relocated, loaded)
        STYP_DSECT 	00000001h Dummy section (relocated, not allocated, not loaded)
        STYP_NOLOAD 	00000002h Noload section (allocated, relocated, not loaded)
        STYP_COPY 	00000010h Copy section (relocated, loaded, but not allocated; relocation entries are processed normally)
        STYP_TEXT 	00000020h Section contains executable code
        STYP_DATA 	00000040h Section contains initialized data
        STYP_BSS 	00000080h Section contains uninitialized data
        STYP_BLOCK 	00001000h Alignment used as a blocking factor
        STYP_PASS 	00002000h Section should pass through unchanged
        STYP_CLINK 	00004000h Section requires conditional linking
        STYP_VECTOR 	00008000h Section contains vector table
        STYP_PADDED 	00010000h Section has been padded */
    quint16 usReserved;
    quint16 usMemoryPageNumber;
}TSectionHeader;

typedef struct _tSmallSectionHeader
{
    COFFNAME SectionName;
    quint32 uiPysicalAddress;	//Section's physical address (Run Address)
    quint32 uiVirtalAddress; 	//Section's virtual address (Load Address)
    quint32 uiSectionSize; 	//Section size in bytes
    quint32 uiRawDataPointer; 	//File pointer to raw data
    quint32 uiRelocationEntryPointer;	//File pointer to relocation entries
    quint32 uiLineEntryPointer;
    quint16 uiRelocationEntryNumber;	//Number of relocation entries
    quint16 uiLineEntryNumber;
    quint16 uiFlags;
/*		STYP_REG 	00000000h Regular section (allocated, relocated, loaded)
        STYP_DSECT 	00000001h Dummy section (relocated, not allocated, not loaded)
        STYP_NOLOAD 	00000002h Noload section (allocated, relocated, not loaded)
        STYP_COPY 	00000010h Copy section (relocated, loaded, but not allocated; relocation entries are processed normally)
        STYP_TEXT 	00000020h Section contains executable code
        STYP_DATA 	00000040h Section contains initialized data
        STYP_BSS 	00000080h Section contains uninitialized data
        STYP_BLOCK 	00001000h Alignment used as a blocking factor
        STYP_PASS 	00002000h Section should pass through unchanged
        STYP_CLINK 	00004000h Section requires conditional linking
        STYP_VECTOR 	00008000h Section contains vector table
        STYP_PADDED 	00010000h Section has been padded */
    quint8 usReserved;
    quint8 usMemoryPageNumber;
}TSmallSectionHeader;


typedef struct
{
    COFFNAME sName;
    qint32 lSymValue;
    qint16 siNum;
    qint16 res;
    qint8 cStorClass;
    qint8 cMEM;
}TSymbolTable;

class CCoff
{
    public:
        CCoff();
        CCoff(QString &FileName);
        ~CCoff();
        bool open(QString &CoffFileName);
        QString GetCoffInfo(void);
        QString GetSectionName(quint16 secNum);
        TSymbolTable *FindSymbol(QString &sym);
        bool GetSymbolValue(QString &sym,void *pValue,quint16 len=1);
        bool SetSymbolValue(QString &sym,void *pValue,quint16 len=1);
        bool Resize(QString &SecName,quint32 uResize);
        bool Write(QString &file);
        bool VerifyCode(QString &RomCheck);
        TCoffHeader *GetCoffHeader(){return pCoffHeader;}
        TCoffOptionalHeader *GetOptioHeader(){return pCoffOptionalHeader;}
        TSectionHeader *GetSectionHeader(){return pSectionHeader;}
        TSmallSectionHeader *GetSmalSectionHeader(){return pSmallSectionHeader;}
        TSymbolTable *GetSymbolTable(){return pSymbolsTab;}
        char *GetString(void){return strList;}
        time_t GetimeStamp(void);
        void SetimeStamp(time_t t);
        quint32 GetRawSize(void);
        bool GetStaus(void){return isGood;}
        quint16 GetCoffType(void){return pCoffHeader->usVersionID; }
    private:
        qint8 *pCoffBuffer;
        TCoffHeader *pCoffHeader;
        TCoffOptionalHeader *pCoffOptionalHeader;
        TSectionHeader *pSectionHeader;
        TSmallSectionHeader *pSmallSectionHeader;
        TSymbolTable *pSymbolsTab;
        char *strList;
        quint32 uiCoffFileSize;
        bool isGood;
};

#endif
