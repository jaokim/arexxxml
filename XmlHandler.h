/**
 * XmlHandler.h
 * @date 2015-03-03
 * @author Joakim Nordstrom
 *
 * MIT License
 *
 * Copyright (c) 2016 Joakim Nordstrom
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef XMLHANDLER_H
#define XMLHANDLER_H
#define BUFFSIZE    8192

#include <cstdio>
#include <string>
#include <cstring>
#include <list>
#include <proto/expat.h>
#include <proto/dos.h>
#include <proto/uni.h>
#include <proto/codesets.h>
#include <proto/exec.h>

#include "ARexxXmlException.h"

#ifdef DEBUG
#define XML_H_DEBUG printf
#define XML_H_TRACE(x, y...) {}
#else
#define XML_H_DEBUG(x, y...) {}
#define XML_H_TRACE(x, y...) {}
#endif

//extern struct ExecIFace *IExec;
//
// Xml Handlers
//
extern void start(void *data, const XML_Char *el, const XML_Char **attr);
extern void end(void *data, const XML_Char *el);
extern void charData(void *data, const XML_Char *el, int len);

class XmlEntity {
    public:
    XmlEntity();

    void SetId(const char * id);
    void SetValue(const char * value);
    ~XmlEntity ();
    /**
     * GetChild with name id and the index:th occurence.
     */
    XmlEntity * GetChild(std::string id, int index);
    /**
     * GetAttribute with name id and the index:th occurence.
     */
    XmlEntity * GetAttribute(std::string id);
    /**
     * GetChild with name id and the index:th occurence.
     */
    int GetNumOfChildren(std::string id);

    std::string _id;
    std::string *_value;
    std::list<XmlEntity*> *_children;
    std::list<XmlEntity*> *_attributes;
    XmlEntity * _parent;
};

class XmlHandler {
public:
    XmlHandler();
    ~XmlHandler();

    void ReadFromStdin();
    void ReadFromFile(const char * fileName, const char * xmlencoding, const char * destencoding);
    const char * GetTag(const char * tag);
    void DisposeDocument();
    static std::string _stemString;
    static XmlEntity * _currentEntity;
    static int _depth;
protected:
    XmlEntity *_document;
    const char* ConvertStringToLatin1(std::string);
private:
    char *_buff;
    XML_Parser _parser;
    struct Library *_codesetsBase;
    struct Library *_uniBase;
    struct CodesetsIFace *_iCodesets;
    struct UniIFace *_iUni;
    struct Hook *_mapCharHook;
    struct codeset *_srcCodeset;
    struct codeset *_dstCodeset;
};
extern unsigned int LatinToUTF8MapChar( REG(a0, struct Hook *h), REG(a2, APTR obj), REG(a1, unsigned int lchar));

#endif
