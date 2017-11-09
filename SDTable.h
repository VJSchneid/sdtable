/*
 *      Created on: 17.11.2016
 *      Author: Viktor Schneider
 *      E-Mail: info@vjs.io
 * */

#ifndef SDTABLE_SDTABLE_H
#define SDTABLE_SDTABLE_H

#include <cstdio>
#include <cstdint>
#include <sys/stat.h>

#define SDTABLE_VERSION_1           3
#define SDTABLE_VERSION_2           0
#define HEADER_STATIC_SIZE          (sizeof(::database::SDTable::Head) - sizeof(::database::SDTable::Head::elements))
#define FILE_DEFAULT_BUFFER_SIZE    256

namespace database {
    
    struct Element {
        uint32_t id;
        uint32_t size;
        uint32_t value;
    };
    
    class SDTable {
    private:
        FILE* file;
        char* fileBuffer;

        enum Frame {CONTENT, FREEDLINE};

        struct Head {
            // Static Content
            uint16_t  version1;
            uint16_t  version2;
            uint32_t  headerSize;
            uint32_t  elementCount;
            uint32_t  lineSize;
            uint64_t  bodySize;
            uint32_t  lineCount;
            uint32_t  freedLineCount;
            // Dynamic Content
            Element   *elements;
        } head;

        void setFilePos(unsigned int line, Frame frame = CONTENT, unsigned int offset = 0);

        void flushHead();
        bool writeHead();
        bool readHead();
        void setHead(uint32_t elementCount, uint32_t lineCount, uint32_t freedLineCount,
                     Element* elementSize);
        int checkHead();
        // @warning call only if FD is opened
        int requestLine();
        bool removeLine(unsigned int line);
        bool freedLine(uint32_t line);
        bool checkFreed(unsigned int line);

    public:
        SDTable();
        SDTable(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        ~SDTable();

        int create(const char* path, unsigned int elementCount, Element* elementSize, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        int open(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        void close();

        // Return value of -1 means an error occurred
        int addLine(void* container);
        bool clearLine(unsigned int line);

        bool getElement(unsigned int line, unsigned int element, void* container);
        bool setElement(unsigned int line, unsigned int element, void* container);

        bool setLine(unsigned int line, void* container);
        bool getLine(unsigned int line, void* container);

        bool isFreed(unsigned int line);

        // HEADER QUERY INTERFACE
        unsigned short  getVersion1();
        unsigned short  getVersion2();
        unsigned int    getHeaderSize();
        unsigned int    getElementCount();
        unsigned int    getLineSize();
        unsigned int    getLineCount();
        unsigned long   getBodySize();
        unsigned int    getFreedLineCount();
        bool            getElement(unsigned int no, Element *element);
    };
}

#endif //SDTABLE_SDTABLE_H