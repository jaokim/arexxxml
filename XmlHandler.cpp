/**
 * XmlHandler.cpp
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

#include "XmlHandler.h"



#define ENCODING NULL
int             XmlHandler::_depth = 0;
std::string     XmlHandler::_stemString = "document";
XmlEntity *     XmlHandler::_currentEntity = NULL;


/**
 * Constructor for XmlHandler
 */
XmlHandler::XmlHandler () {
    _srcCodeset = NULL;
    _dstCodeset = NULL;
    _document = NULL;
    _parser = NULL;
    _parser = XML_ParserCreate(ENCODING);
    if (! _parser) {
        throw COULDNTALLOCATEMEMORY; //"Couldn't allocate memory for parser\n";
    }
    XML_SetElementHandler(_parser, start, end);
    XML_SetCharacterDataHandler(_parser, charData);
    _iCodesets = NULL;
    _codesetsBase = IExec->OpenLibrary("codesets.library", 0);
    _iUni = NULL;
    _uniBase = NULL;
    _mapCharHook = NULL;
    if(_codesetsBase == NULL) {
        _uniBase = IExec->OpenLibrary("uni.library" ,4);
        if(_uniBase == NULL) {
            IDOS->Printf("Couldn't load codesets.library, nor unicode.library v4. Continuing without UniCode support.\n");
        } else {
            _iUni = (struct UniIFace *) IExec->GetInterface((struct Library *) _uniBase, "main", 1, NULL);
            if ( !_iUni)
            {
                IExec->CloseLibrary( _uniBase);
                IDOS->Printf( "Unable to open interface \"IUni\". Continuing without UniCode support.\n");
                _uniBase = NULL;
                _iUni =NULL;
                _mapCharHook = NULL;
            } else {
                XML_H_DEBUG("Unicode support activated.\n");
                _mapCharHook = new struct Hook();
                _mapCharHook->h_MinNode.mln_Succ = NULL;
                _mapCharHook->h_MinNode.mln_Pred = NULL;
                _mapCharHook->h_Entry = (HOOKFUNC) LatinToUTF8MapChar;
                _mapCharHook->h_SubEntry = 0;
                _mapCharHook->h_Data = 0;
            }
        }
    } else {
        _iCodesets = (struct CodesetsIFace *) IExec->GetInterface((struct Library *) _codesetsBase, "main", 1, NULL);
        if(_iCodesets) {
            _dstCodeset = _iCodesets->CodesetsFind(NULL, NULL);
            if(!_dstCodeset) {
                IDOS->Printf( "Unable to find default dest codesest. Continuing without codesets.library support.\n");
                IExec->DropInterface( (struct Interface *) _iCodesets);
                _iCodesets = NULL;
            } else {
                XML_H_DEBUG("Using dest codeset: %s\n", _dstCodeset->name);
            }
        }
        if ( !_iCodesets) {
            IExec->CloseLibrary( _codesetsBase);
            IDOS->Printf( "Unable to open interface \"ICodesets\". Continuing without UniCode support.\n");
            _codesetsBase = NULL;
            _iCodesets = NULL;
            
        } else {
            
        }
    }

    
}

/*
 * This routine can be used by LatinToUTF8 in case a code point is greater 127
 * Registers A0 (hook) and A2 (object) or non important - they must be ignored
 */

unsigned int LatinToUTF8MapChar( REG(a0, struct Hook *h), REG(a2, APTR obj), REG(a1, unsigned int lchar) )
{
    printf( "Hook called: Mapping for character \"%ld\" should take place!\n", (long) lchar);

    return lchar;
}



/**
 * Destructor for XmlHandler
 */
XmlHandler::~XmlHandler () {
    delete _document;
    if(_parser != NULL) {
        XML_ParserFree(_parser);
    }
    if(_mapCharHook != NULL) {
        delete _mapCharHook;
    }
    IExec->DropInterface( (struct Interface *) _iCodesets);
    IExec->CloseLibrary( _codesetsBase);
    IExec->DropInterface( (struct Interface *) _iUni);
    IExec->CloseLibrary( _uniBase);
}
void XmlHandler::ReadFromStdin() {
    

    if(_document != NULL) {
        delete _document;
    }
    _document = new XmlEntity();
    XmlHandler::_currentEntity = _document;

    _buff =new char[BUFFSIZE];
    XML_ParserReset(_parser, NULL);
    XML_SetElementHandler(_parser, start, end);
    XML_SetCharacterDataHandler(_parser, charData);

    for (;;) {
    
        int done;
        int len;
        len = fread(_buff, 1, BUFFSIZE, stdin);
        if (ferror(stdin)) {
            fprintf(stderr, "Read error\n");
            throw ARexxXmlException(FILECOULDNTBEREAD);
        }
        done = feof(stdin);

        if (! XML_Parse(_parser, _buff, len, done)) {
          fprintf(stderr, "Parse error at line %d:\n%s\n",
              XML_GetCurrentLineNumber(_parser),
              XML_ErrorString(XML_GetErrorCode(_parser)));
          delete _buff;
          throw ARexxXmlException(INVALIDXML);
        }

        if (done)
          break;
    }
    delete _buff;
}

/**
 * Read from file
 */
void XmlHandler::ReadFromFile(const char * fileName, const char * xmlencoding, const char * destencoding) {
    _buff = NULL;
    int64 buffsize;
    if(_document != NULL) {
        delete _document;
    }
    _document = new XmlEntity();
    XmlHandler::_currentEntity = _document;

    int errorCode = 0;
    //_stemString = std::string();
    
    BPTR file = IDOS->FOpen(fileName, MODE_OLDFILE, BUFFSIZE);
    if(file == NULL) {
        errorCode = FILENOTFOUND;
        goto exit;
    }
    
    if(_iCodesets) {
        if(xmlencoding == NULL) {
            // we're defaulting to UTF-8
            // todo: pick this up from the parser?
            xmlencoding = (char *)"UTF-8";
        }
        _dstCodeset = _iCodesets->CodesetsFind(destencoding, NULL);
        if(!_dstCodeset) {
            errorCode = UNKNOWNDSTENCODING;
            goto exit;
        }
        _srcCodeset = _iCodesets->CodesetsFind(xmlencoding, NULL);
        if(!_srcCodeset) {
            errorCode = UNKNOWNXMLENCODING;
            goto exit;
        }
        XML_H_DEBUG("Loading file %s, from %s enc to %s enc\n", fileName, _srcCodeset->name, _dstCodeset->name);
    } else {
        XML_H_DEBUG("Loading file %s\n", fileName);
    }
    if(XML_ParserReset(_parser, ENCODING) == XML_FALSE) {
        errorCode = COULDNTALLOCATEMEMORY;
        goto exit;
    }
    buffsize = IDOS->GetFileSize(file);
    XML_H_DEBUG("Filelength: %lld\n", buffsize);
    _buff = new char[buffsize];

    XML_SetElementHandler(_parser, start, end);
    XML_SetCharacterDataHandler(_parser, charData);


    for (;;) {
        bool done;
        uint32 len;
        len = IDOS->FRead(file, _buff, 1, buffsize);
        if (len == 0) {
            XML_H_DEBUG("len == 0, done=true\n");
            done = true;
        }
        XML_H_DEBUG("Read %lu\n", len);

        if (! XML_Parse(_parser, _buff, len, done)) {
            _buff[BUFFSIZE] = '\0';
            fprintf(stderr, "Parse error at line %d:\n%s\n%s\n",
            XML_GetCurrentLineNumber(_parser),
            XML_ErrorString(XML_GetErrorCode(_parser)), _buff);
            errorCode = INVALIDXML;
            goto exit;
        }
        
        if (done)
            break;
    }

exit:
    IDOS->FClose(file);
    if(_buff != NULL) {
        delete _buff;
    }
    if(errorCode > 0) {
        throw ARexxXmlException(errorCode);
    } else {
        return;
    }
}

void XmlHandler::DisposeDocument() {
    delete _document;
    _document = NULL;

}

/**
 * Returns the value of the element or attribute
 * defined by tag.
 * tag can be something like:
 * root.element         The text content of <root><element>.
 * root.element[attr]   The attribute content of <root><element attr="">.
 * root.element[1]      The 2nd element.
 * <test>
 *    <info/>
 *    <remadan yttre="mupp">POP</remadan>
 *    <list>
 *        <f1>1</f1>
 *        <f1>2</f1>
 *    </list>
 *    <end value="5"/>
 *</test>
 *<!--
 *test.info=0
 *test.oups=UNDEFINED            returns 5
 *test.remadan=POP
 *test.remadan[yttre]=mupp
 *test.list=2
 *test.list.f1=1
 *test.list.f1[0]=1
 *test.list.f1[1]=2
 *test.end[value]=5
 *-->
 */
const char * XmlHandler::GetTag(const char* tag) {

    int elmIndex = 0;
    unsigned int start = 0, end = 0;
    unsigned int attrStart = 0, attrEnd = 0;
    int len;
    std::string attr;
    std::string tagToFind (tag);
    std::string strPart = "";
    std::string elmIndexStr;
    std::string returnStr = "";

    XmlEntity * entity = _document;

    XML_H_TRACE("tag: \"%s\" / \"%s\"\n", tag, tagToFind.c_str());

    if(entity == NULL) {
        throw ARexxXmlException(NOFILELOADED);
    }
    if(tagToFind.empty()) {
        throw ARexxXmlException(NOXMLTAGINREQUEST);
    }
    char buff[50];

    XML_H_TRACE("Start %d-%d \"%s\" (%d %d)\n", start, end, tagToFind.c_str(), tagToFind.size(), end);

    while(tagToFind.size() > 0 && end < tagToFind.size()) {
        attr = "";

        // find the dot in "element."
        end = tagToFind.find(".", start);
        if(end == std::string::npos) {
            end = tagToFind.size();
        }

        XML_H_TRACE("Look %d-%d %s\n", start, end, tagToFind.c_str());
        strPart = tagToFind.substr(start, end-start);
        if(!strPart.empty()) {
            XML_H_TRACE("Found %s\n", strPart.c_str());
        }
        // find the [ in "element[0]."
        attrStart = strPart.find("[");
        if(attrStart != std::string::npos) {
            
            //strPart = tagToFind.substr(start, attrStart-start);
            // found an attr or index
            attrEnd = strPart.rfind("]");
            XML_H_TRACE("looking index %d %d\n", attrStart, attrEnd);
            if((attrStart+1) >= attrEnd-2) {
                elmIndex = 0;
            }
            elmIndexStr = strPart.substr(attrStart+1, attrEnd-attrStart-1);
            XML_H_TRACE("looking index %s\n", elmIndexStr.c_str());
            strPart = strPart.substr(0, attrStart);

            elmIndex = atoi(elmIndexStr.c_str());
            XML_H_TRACE("Found index %s (%c)\n", elmIndexStr.c_str(), elmIndex);
            if(elmIndex == 0 && elmIndexStr.size() > 1) {
               elmIndex = -1;
            }
        } else {
           XML_H_TRACE("No index\n");
           elmIndex = 0;
        }

        
        attrStart = strPart.find("(");
        if(attrStart != std::string::npos) {
            // found an attr or index
            attrEnd = strPart.rfind(")");
            attr = strPart.substr(attrStart+1, attrEnd-attrStart-1);
            XML_H_TRACE("Found attr %s\n", attr.c_str());
            
            strPart = strPart.substr(0, attrStart);
        } else {
            XML_H_TRACE("No attr\n");
        }
        if(elmIndex >= 0) {
            XML_H_TRACE("Getting child: %s\n", strPart.c_str());
            entity = entity->GetChild(strPart, elmIndex);
            
        } else {
            XML_H_TRACE("Checking possible command: %s\n", elmIndexStr.c_str());
            if(elmIndexStr.compare("length") == 0) {
                XML_H_DEBUG("Got length command.\n");
                if(entity->_parent == NULL) throw ARexxXmlException(NOXMLTAGINREQUEST);
                len = entity->GetNumOfChildren(strPart);
                sprintf(buff, "%d", len);
                returnStr = (buff);
                return strdup(returnStr.c_str());
            }
        }

        if(entity == NULL) {
            XML_H_TRACE("Entity was null, found no child, breaking for-loop\n");
            break;
        } else if(!attr.empty()) {
            XML_H_TRACE("Entity wasn't null, but foudn an attribute to search for: %s\n", attr.c_str());
            entity = entity->GetAttribute(attr);
            // if we have an attribute we're at end
            break;
        }
        
        start = end+1;
    }
    if(entity != NULL ) {
        if(entity->_value != NULL) {
            XML_H_TRACE("Found entity: %s\n", entity->_value->c_str());
            returnStr = (STRPTR)entity->_value->c_str();
        } else {
            XML_H_TRACE("Found empty entity\n");
            returnStr = "";
        }
    } else {
        XML_H_DEBUG("Did not find element at all, throwing exception.\n");
        throw ARexxXmlException(ELEMENTNOTFOUND);
    }
    return ConvertStringToLatin1(returnStr);
}

const char * XmlHandler::ConvertStringToLatin1(std::string str) {
    int inlen;
    const char * in_str;
    char * out_str;
    char * temp;
    int ret;
    
    if(_iCodesets != NULL) {
        in_str = str.c_str();
        if((temp = _iCodesets->CodesetsConvertStr(CSA_SourceCodeset, _srcCodeset,
                                 CSA_DestCodeset,   _dstCodeset,
                                 CSA_Source,       in_str,
                                 TAG_DONE)))
        {
            out_str = new char[strlen(temp)+1];
            strcpy(out_str, temp);
            _iCodesets->CodesetsFreeA(temp, NULL);
        } else {
            XML_H_DEBUG("codesets.library couldn't convert \"%s\".\n", in_str);
            out_str = new char[strlen(in_str)+1];
            strcpy(out_str, in_str);
        }

        XML_H_DEBUG("CodesetsConvert returned: %s\n", out_str);
        return out_str;

    } else if(_iUni == NULL ) {
        XML_H_DEBUG("No Unicode support, returning string.\n");
        out_str = new char[strlen(str.c_str())+1];
        strcpy(out_str, str.c_str());
        return out_str;
    } else {
        inlen = str.length();
        in_str = str.c_str();
        out_str = new char[inlen+1];
        ret =_iUni->UTF8ToLatin(in_str, inlen, out_str, inlen, NULL);
        XML_H_DEBUG("UTF8ToLatin returned: %d\n", ret);
        out_str[ret] = '\0';
        return out_str;
    }
}

/*****************
 * XmlEntity methods
 *****************/

/** Constructor */
XmlEntity::XmlEntity() {
    _value = new std::string();
    _parent = NULL;
    _children = new std::list<XmlEntity*>();
    _attributes = new std::list<XmlEntity*>();
}

void XmlEntity::SetId(const char * id) {
    _id = std::string(id);
}
void XmlEntity::SetValue(const char * value) {
    _value = new std::string(value);
}

XmlEntity::~XmlEntity () {
    std::list<XmlEntity *>::iterator it;
    // delete all children
    if(_children != NULL) {
        for ( it = _children->begin() ; it != _children->end(); it++ ) {
            delete *it;
        }
    }
    // delete all attributes
    if(_children != NULL) {
        for ( it = _attributes->begin() ; it != _attributes->end(); it++ ) {
            delete *it;
        }
    }
    delete _children;
    delete _attributes;
    delete _value;
}

/**
* GetChild with name id and the index:th occurence.
*/
XmlEntity * XmlEntity::GetChild(std::string id, int index) {
   std::list<XmlEntity *>::iterator it;
   XmlEntity * ret = NULL;
   int cnt = -1;
   if(_children == NULL || _children->empty()) {
       XML_H_TRACE("Empty children\n");
       return NULL;
   }
   for ( it = _children->begin() ; it != _children->end(); it++ ) {
       XML_H_TRACE("Looking child %s==%s\n", id.c_str(), (*it)->_id.c_str());
       if(id.compare((*it)->_id) == 0) {
           cnt++;
           if(cnt == index) {
               ret = *it;
               return ret;
           }
       }
   }
   return ret;
}

/**
* GetChild with name id and the index:th occurence.
*/
int XmlEntity::GetNumOfChildren(std::string id) {
   std::list<XmlEntity *>::iterator it;
   int cnt = 0;
   if(_children == NULL || _children->empty()) {
       XML_H_TRACE("Empty children in %s\n", this->_id.c_str());
       return 0;
   }
   for ( it = _children->begin() ; it != _children->end(); it++ ) {
       XML_H_TRACE("Looking child %s==%s\n", id.c_str(), (*it)->_id.c_str());
       if(id.compare((*it)->_id) == 0) {
           cnt++;            
       }
   }
   return cnt;
}

/**
* GetAttribute with name id and the index:th occurence.
*/
XmlEntity * XmlEntity::GetAttribute(std::string id) {
   std::list<XmlEntity *>::iterator it;
   XmlEntity * ret = NULL;
   if(_attributes == NULL || _attributes->empty()) {
       return NULL;
   }

   for ( it = _attributes->begin() ; it != _attributes->end(); it++ ) {
       XML_H_TRACE("Looking attr %s==%s\n", id.c_str(), (*it)->_id.c_str());
       if(id.compare((*it)->_id) == 0) {
           ret = *it;
           break;
       }
   }
   return ret;
}

/**
 Start element handler
 */
void start(void *data, const XML_Char *el, const XML_Char **attr) {
  int i;
  
  for (i = 0; i < XmlHandler::_depth; i++)
    XML_H_TRACE("  ");

  XmlEntity *entity = new XmlEntity();
  entity->SetId(el);
  entity->_parent = XmlHandler::_currentEntity;
  
  XmlHandler::_currentEntity->_children->push_back(entity);
  XmlHandler::_currentEntity = entity;

  XML_H_TRACE("%s", el);
  


  for (i = 0; attr[i]; i += 2) {
    XmlEntity * ent = new XmlEntity();
    ent->SetId(attr[i]);
    ent->SetValue(attr[i+1]);
    XmlHandler::_currentEntity->_attributes->push_back(ent);
    XML_H_TRACE(" %s='%s'", attr[i], attr[i + 1]);
  }

  XML_H_TRACE("\n");
  XmlHandler::_depth++;
}  /* End of start handler */

/**
 End element handler
 */
void end(void *data, const XML_Char *el) {
  XmlHandler::_depth--;
  XmlHandler::_currentEntity = XmlHandler::_currentEntity->_parent;
}  /* End of end handler */

/**
 * char data handler
 */
void charData(void *data, const XML_Char *el, int len) {
    char * buff = new char[len];
    strncpy(buff, el, len);
    if(XmlHandler::_currentEntity->_value == NULL) {
        XmlHandler::_currentEntity->_value = new std::string();
    }
    XmlHandler::_currentEntity->_value->append(buff,0,len);
    delete [] buff;
}
