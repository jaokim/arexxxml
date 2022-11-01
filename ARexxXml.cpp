/**
 * ARexxXml.cpp
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

#include "ARexxXml.h"


//typedef (void)(*TARexxMethod)( struct ARexxCmd *, struct RexxMsg *);
void reply_callback(struct Hook *, Object *, struct RexxMsg *);
void rexx_LoadXml (struct ARexxCmd *, struct RexxMsg *);
void rexx_GetTag (struct ARexxCmd *, struct RexxMsg *);
void rexx_Dispose (struct ARexxCmd *, struct RexxMsg *);
void rexx_Quit (struct ARexxCmd *, struct RexxMsg *);
/* Our reply hook function. */
STATIC struct Hook reply_hook;


BOOL ARexxXml::running = TRUE;
XmlHandler * ARexxXml::xmlHandler = NULL;
/* The following commands are valid for this demo. */
STATIC struct ARexxCmd Commands[] =
{
    { (char *)"LOADXML",    ARexxXml::REXX_LOADXML,   (void(*)(ARexxCmd*, RexxMsg*))rexx_LoadXml,  (char *)"FILE,XMLENC,DESTENC",    0,  NULL,   0,  0,  NULL },
    //{ (char *)"LOADURL",    ARexxXml::REXX_LOADURL,   (void(*)())rexx_LoadUrl,  (char *)"URL/F",    0,  NULL,   0,  0,  NULL },
    { (char *)"GETTAG",     ARexxXml::REXX_TAG,       (void(*)(ARexxCmd*, RexxMsg*))rexx_GetTag,   (char *)"TAG/F",       0,  NULL,   0,  0,  NULL },
    { (char *)"CLOSE",     ARexxXml::REXX_DISPOSE,       (void(*)(ARexxCmd*, RexxMsg*))rexx_Dispose,   NULL,       0,  NULL,   0,  0,  NULL },
    { (char *)"QUIT",     ARexxXml::REXX_QUIT,       (void(*)(ARexxCmd*, RexxMsg*))rexx_Quit,   NULL,       0,  NULL,   0,  0,  NULL }/*,

    { NULL,         0,              NULL,           NULL,       0,  NULL,   0,  0,  NULL }                           */
};

/**
 * Constructor for ARexxXml
 */
ARexxXml::ARexxXml () {
    xmlHandler = NULL;
}


/**
 * Destructor for ARexxXml
 */
ARexxXml::~ARexxXml () {
   if(xmlHandler != NULL) {
        delete xmlHandler;
   }
}


/**
 *
 */
void ARexxXml::StartMainLoop() {
    xmlHandler = new XmlHandler();
    running = TRUE;


    /* Create host object. */
    arexx_obj = (Object*)ARexxObject,
        AREXX_HostName, "AREXXXML",
        AREXX_Commands, Commands,
        AREXX_NoSlot, _noSlot,
        AREXX_ReplyHook, &reply_hook,
    End;

    
    if (arexx_obj)
    {
        

         
            
        ULONG rxsig = 0, signal;
        

        /* Setup the reply callback hook. */
        /*reply_hook.h_Entry = (HOOKFUNC)reply_callback;
        reply_hook.h_SubEntry = NULL;
        reply_hook.h_Data = NULL;
          */
        /* Try to start the macro "Demo.rexx". Note that the
        * current directory and REXX: will be searched for this
        * macro. Our reply hook will get the results of our
        * efforts to start this macro. To be totally robust, we
        * should have also passed pointers for the various result
        * variables.
        */
        //IIntuition->IDoMethod(arexx_obj, AM_EXECUTE, "Demo.rexx", NULL, NULL, NULL, NULL, NULL);

        /* Obtain wait masks. */

        IIntuition->GetAttr(AREXX_SigMask, arexx_obj, &rxsig);

        /* Event loop... */
        do
        {
            signal = IExec->Wait(rxsig | SIGBREAKF_CTRL_C);

            /* ARexx event? */
            if (signal & rxsig)
                RA_HandleRexx(arexx_obj);

                    
            if (signal & SIGBREAKF_CTRL_C)
            {
                AREXXXML_H_DEBUG("Got Ctrl-C\n");
                running = FALSE;
            }
        }
        while (running);
            
        IIntuition->DisposeObject(arexx_obj);
    }
    else
        IDOS->Printf("Could not create the ARexx host.\n");
    if(xmlHandler != NULL) {
        delete xmlHandler;
        xmlHandler = NULL;
    }
    
}

/* This function gets called whenever we get an ARexx reply. In this example,
* we will see a reply come back from the REXX server when it has finished
* attempting to start the Demo.rexx macro.
*/
void reply_callback(struct Hook *hook __attribute__((unused)), Object *o __attribute__((unused)), struct RexxMsg *rxm)
{
    AREXXXML_H_DEBUG("Args[0]: %s\nResult1: %ld Result2: %ld\n",
    rxm->rm_Args[0], rxm->rm_Result1, rxm->rm_Result2);
}

/* Load XML file.
 * Removes any instance of xmlHandler first.
 */
void rexx_LoadXml( struct ARexxCmd *ac, struct RexxMsg *rxm __attribute__((unused)))
{
    ac->ac_Result = NULL;
    try {
        char * filename = NULL;
        char * destenc = NULL;
        char * xmlenc = NULL;
        /* return the program name. */
        if (ac->ac_ArgList[2]) {
            destenc =(STRPTR)ac->ac_ArgList[2];
        }
        if (ac->ac_ArgList[1]) {
            xmlenc = (STRPTR)ac->ac_ArgList[1];
        }
        if (ac->ac_ArgList[0]) {
            filename = (STRPTR)ac->ac_ArgList[0];
        }
        if(filename != NULL) {
            ARexxXml::xmlHandler->ReadFromFile(filename, xmlenc, destenc);
            ac->ac_RC = 0;
        } else {
            ac->ac_RC = FILENOTFOUND;
        }
    } catch( ARexxXmlException axe ) {
        AREXXXML_H_DEBUG("%s %d\n", axe.what(), axe.GetErrorCode());

        ac->ac_RC = axe.GetErrorCode();
    }
    
}

/* VERSION */
void rexx_GetTag( struct ARexxCmd *ac, struct RexxMsg *rxm __attribute__((unused)))
{
    ac->ac_Result = NULL;
    ac->ac_RC = 0;
    const char * res;
    try {
        if (ac->ac_ArgList[0]) {
            AREXXXML_H_DEBUG("GETTAG: %s\n", (STRPTR)ac->ac_ArgList[0]);
            res = ARexxXml::xmlHandler->GetTag((const char *)ac->ac_ArgList[0]);

            ac->ac_Result = strdup(res);
            delete res;
            AREXXXML_H_DEBUG("GETTAG res: %s\n", ac->ac_Result);
        } else {
            throw ARexxXmlException(NOXMLTAGINREQUEST);
        }
    
        
    } catch( ARexxXmlException axe ) {
        AREXXXML_H_DEBUG("ARexxXml: %s %d\n", axe.what(), axe.GetErrorCode());
        ac->ac_Result = NULL;
        ac->ac_RC = axe.GetErrorCode();
    } catch(...) {
        IDOS->Printf("Got exception in GETTAG, throwing it again.\n");
        ac->ac_Result = NULL;
        ac->ac_RC = 21;
        throw;
    }
}

void rexx_Dispose( struct ARexxCmd *ac, struct RexxMsg *rxm __attribute__((unused)))  {
   AREXXXML_H_DEBUG("Close\n");
   ARexxXml::xmlHandler->DisposeDocument();
   ac->ac_Result = NULL;
   ac->ac_RC = 0;
}

void rexx_Quit( struct ARexxCmd *ac, struct RexxMsg *rxm __attribute__((unused)))  {
   AREXXXML_H_DEBUG("Quit\n");
   ARexxXml::running = FALSE;
   ac->ac_Result = NULL;
   ac->ac_RC = 0;
}


