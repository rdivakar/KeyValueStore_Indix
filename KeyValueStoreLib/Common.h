#pragma once

#include <sstream>

#define DEFAULT_BUFLEN 1024
#define BUF_LENGTH	sizeof(unsigned int)

using namespace std;

#define __MODULE_NAME_ "KeyValueStore"

#define __LOG_BUFF_SIZE__ 4096
#define CONTROLLER_PORT 10000

#define REGISTER_XML_FORMAT "<RegisterStore> <id>%d</id> <portNumber>%d< / portNumber></RegisterStore>"
#define KEYVALUE_XML_FORMAT "<KeyValue> <key>%s</key> <value>%s</value> <time>%s</time> </KeyValue>"
#define RESPONSE_XML_FORMAT "<Response> <errorCode>%s</errorCode> </Response>"




