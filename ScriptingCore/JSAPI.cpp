/**********************************************************\ 
Original Author: Richard Bateman (taxilian)

Created:    Sept 24, 2009
License:    Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html

Copyright 2009 Richard Bateman, Firebreath development team
\**********************************************************/

#include "JSAPI.h"
#include "BrowserHostWrapper.h"

using namespace FB;

JSAPI::JSAPI(void)
{
    registerMethod( "toString", (CallMethodPtr)&JSAPI::callToString );
    
    registerProperty( "valid", (GetPropPtr)&JSAPI::getValid, NULL );
}

JSAPI::~JSAPI(void)
{
}

// Methods for managing event sinks (BrowserHostWrapper objects)
void JSAPI::attachEventSink(void *context, BrowserHostWrapper *sink)
{
    m_sinkMap[context] = sink;
}

void JSAPI::detachEventSink(void *context)
{
    EventSinkMap::iterator fnd = m_sinkMap.find(context);
    if (fnd != m_sinkMap.end()) {
        m_sinkMap.erase(fnd);
    }
}

void JSAPI::FireEvent(std::string eventName, variant *args, int argCount)
{
    std::pair<EventMap::iterator, EventMap::iterator> range = m_eventMap.equal_range(eventName);

    for (EventMap::iterator eventIt = range.first; eventIt != range.second; eventIt++) {
        for (EventSinkMap::iterator sinkIt = m_sinkMap.begin(); sinkIt != m_sinkMap.end(); sinkIt++) {
            if (eventIt->second.context == sinkIt->first)
                sinkIt->second->FireEvent(eventName, eventIt->second.func, args, argCount);
        }
    }
}

// Example function call and read-only property; override these if desired in derived classes
bool JSAPI::callToString(variant args[], int argCount, variant &retVal)
{
    retVal = "JSAPI Javascript Object";
    return true;
}

bool JSAPI::getValid(variant &retVal)
{
    retVal = true;
    return true;
}

// Methods for registering properties and functions to the auto-table
void JSAPI::registerMethod(std::string name, CallMethodPtr func)
{
    m_methodMap[name].callFunc = func;
}

void JSAPI::registerProperty(std::string name, GetPropPtr getFunc, SetPropPtr setFunc)
{
    m_propertyMap[name].getFunc = getFunc;
    m_propertyMap[name].setFunc = setFunc;
}

void JSAPI::registerEvent(std::string name)
{
    // Add a blank entry to indicate that the event exists on this interface
    m_eventMap.insert(EventPair(name, EventInfo()));
}


// Methods to query existance of members on the API
bool JSAPI::HasMethod(std::string methodName)
{
    MethodMap::iterator fnd = m_methodMap.find(methodName);
    if (fnd != m_methodMap.end()) {
        return true;
    } else {
        return false;
    }
}

bool JSAPI::HasProperty(std::string propertyName)
{
    PropertyMap::iterator fnd = m_propertyMap.find(propertyName);
    if (fnd != m_propertyMap.end()) {
        return true;
    } else {
        return false;
    }
    return false;
}

bool JSAPI::HasEvent(std::string eventName)
{
    EventMap::iterator fnd = m_eventMap.find(eventName);
    if (fnd != m_eventMap.end()) {
        return true;
    } else {
        return false;
    }
}


// Methods to manage properties on the API
bool JSAPI::GetProperty(std::string propertyName, variant &retVal)
{
    PropertyMap::iterator fnd = m_propertyMap.find(propertyName);
    if (fnd != m_propertyMap.end() && fnd->second.getFunc != NULL) {
        return (this->*fnd->second.getFunc)(retVal);
    } else {
        return false;
    }
}

bool JSAPI::SetProperty(std::string propertyName, variant &value)
{
    PropertyMap::iterator fnd = m_propertyMap.find(propertyName);
    if (fnd->second.setFunc != NULL) {
        return (this->*fnd->second.setFunc)(value);
    } else {
        return false;
    }
    return false;
}

bool JSAPI::HasPropertyIndex(int idx)
{
    // By default do not support indexing
    // To use array style access, override this method in your API object
    return false;
}

bool JSAPI::GetProperty(int idx, variant &retVal)
{
    // By default do not support indexing
    // To use array style access, override this method in your API object
    return false;
}

bool JSAPI::SetProperty(int idx, variant &value)
{
    // By default do not support indexing
    // To use array style access, override this method in your API object
    return false;
}


// Methods to manage methods on the API
bool JSAPI::Invoke(std::string methodName, variant args[], int argCount, variant &retVal)
{
    MethodMap::iterator fnd = m_methodMap.find(methodName);
    if (fnd != m_methodMap.end() && fnd->second.callFunc != NULL) {
        return (this->*fnd->second.callFunc)(args, argCount, retVal);
    } else {
        return false;
    }    
    return false;
}
