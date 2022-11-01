/**
 * ARexxXmlException.h
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

#ifndef AREXXMLEXCEPTION_H
#define AREXXMLEXCEPTION_H

#include <exception>

#define COULDNTALLOCATEMEMORY 20
#define FILECOULDNTBEREAD   19
#define FILENOTFOUND        18
#define NOFILELOADED        17
#define INVALIDXML          16
#define UNKNOWNXMLENCODING  15
#define UNKNOWNDSTENCODING  14
#define ELEMENTNOTFOUND     5
#define NOXMLTAGINREQUEST   10
class ARexxXmlException : public std::exception {
  public:
    ARexxXmlException();
     ARexxXmlException(int i);

    int GetErrorCode() {
        return _what;
    }
    virtual const char* what() const throw()
    {
        switch(_what) {
            case COULDNTALLOCATEMEMORY:
                return "Couldn't allocate memory";
            case FILECOULDNTBEREAD:
                return "File couldn't be read";

            case FILENOTFOUND:
                return "File couldn't be found";

            case NOFILELOADED:
                return "No file was loaded";

            case INVALIDXML:
                return "Invalid XML";

            case ELEMENTNOTFOUND:
                return "Element couldn't be found";
            case NOXMLTAGINREQUEST:
                return "No element defined";
            default:
                return "Unknown exception";

        }
    }
  protected:
    int _what;
  private:

};
#endif


