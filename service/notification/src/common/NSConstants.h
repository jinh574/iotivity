//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _NS_CONSTANTS_H_
#define _NS_CONSTANTS_H_

#define __PRINTLOG 0
#define __NS_FILE__ ( strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__ )

#ifdef TB_LOG
#include "logger.h"
#define NS_LOG_V(level, format, ...) (OIC_LOG_V((level), __NS_FILE__, (format), __VA_ARGS__))
#define NS_LOG(level, msg) (OIC_LOG((level), __NS_FILE__, (msg)))
#else
#if (__PRINTLOG == 1)
#include "logger.h"
#define NS_CONVERT_LEVEL(level) ( \
        ((level) == 0) ? "DEBUG" : \
        ((level) == 1) ? "INFO" : \
        ((level) == 2) ? "WARNING" : \
    ((level) == 3) ? "ERROR" : "FATAL")
#define NS_LOG_V(level, format, ...) \
    { \
        printf("%s: %s ", NS_CONVERT_LEVEL(level), __NS_FILE__); \
        printf((format), __VA_ARGS__); \
        printf("\n"); \
    }
#define NS_LOG(level, msg) \
    { \
        printf("%s: %s ", NS_CONVERT_LEVEL(level), __NS_FILE__); \
        printf((msg)); \
        printf("\n"); \
    }
#else
#define NS_CONVERT_LEVEL(level)
#define NS_LOG(level, msg)
#define NS_LOG_V(level, format, ...) NS_LOG((level), ((format), __VA_ARGS__))
#endif
#endif

#define NS_TAG                     "IOT_NOTI"

// SCHEDULE //
#define THREAD_COUNT               5

// NOTIOBJ //
#define NOTIOBJ_TITLE_KEY          "title"
#define NOTIOBJ_ID_KEY             "id"
#define NOTOOBJ_CONTENT_KEY        "contentText"

#define DISCOVERY_TAG              "NS_PROVIDER_DISCOVERY"
#define SUBSCRIPTION_TAG           "NS_PROVIDER_SUBSCRIPTION"
#define INTERFACE_TAG              "NS_PROVIDER_INTERFACE"
#define NOTIFICATION_TAG           "NS_PROVIDER_NOTIFICATION"
#define SCHEDULER_TAG              "NS_PROVIDER_SCHEDULER"
#define LISTENER_TAG               "NS_PROVIDER_LISTENER"
#define RESOURCE_TAG               "NS_PROVIDER_RESOURCE"
#define TOPIC_TAG                  "NS_PROVIDER_TOPIC"

#define NS_ROOT_TYPE               "oic.r.notification"
#define NS_COLLECTION_MESSAGE_TYPE "oic.r.notification.message"
#define NS_COLLECTION_SYNC_TYPE    "oic.r.notification.sync"
#define NS_COLLECTION_TOPIC_TYPE   "oic.r.notification.topic"

#define NS_DEFAULT_INTERFACE       "oic.if.baseline"

#define NS_ROOT_URI                "/notification"
#define NS_COLLECTION_MESSAGE_URI  "/notification/message"
#define NS_COLLECTION_SYNC_URI     "/notification/sync"
#define NS_COLLECTION_TOPIC_URI    "/notification/topic"

#define NS_QUERY_SEPARATOR         "&;"
#define NS_KEY_VALUE_DELIMITER     "="

#define NS_QUERY_CONSUMER_ID       "consumerid"
#define NS_QUERY_PROVIDER_ID       "providerid"

#define NS_QUERY_ID_SIZE           10

#define NS_POLICY_PROVIDER         1
#define NS_POLICY_CONSUMER         0

#define NS_RD_PUBLISH_QUERY        "/oic/rd?rt=oic.wk.rdpub"

#define NS_VERIFY_NOT_NULL_V(obj) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            return; \
        } \
    }

#define NS_VERIFY_NOT_NULL(obj, retVal) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            return (retVal); \
        } \
    }

#define NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(obj, func) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            NS_LOG(ERROR, "execute deletion"); \
            (func); \
            return; \
        } \
    }

#define NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(obj, retVal, func) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            NS_LOG(ERROR, "execute deletion"); \
            (func); \
            return (retVal); \
        } \
    }

#define NS_VERIFY_STACK_SUCCESS_V(obj) \
    { \
        bool _ret = (obj); \
        if ( _ret != true) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            return; \
        } \
    }

#define NS_VERIFY_STACK_SUCCESS(obj, retVal) \
    { \
        bool _ret = (obj); \
        if ( _ret != true) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            return (retVal); \
        } \
    }

#define NS_VERIFY_STACK_SUCCESS_WITH_POST_CLEANING(obj, retVal, func) \
    { \
        bool _ret = (obj); \
        if ( _ret != true) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            (func); \
            return (retVal); \
        } \
    }

#define NSOICFree(obj) \
    { \
        if ((obj)) \
        { \
            OICFree((obj)); \
            (obj) = NULL; \
            NS_LOG_V(DEBUG, "%s : %s Removed", __func__, #obj); \
        } \
    }

#define VERSION        "1.2.0"

#define NS_ATTRIBUTE_VERSION "VERSION"
#define NS_ATTRIBUTE_POLICY "ACCEPTER"
#define NS_ATTRIBUTE_MESSAGE "MESSAGE_URI"
#define NS_ATTRIBUTE_SYNC "SYNC_URI"
#define NS_ATTRIBUTE_TOPIC "TOPIC_URI"
#define NS_ATTRIBUTE_MESSAGE_ID "MESSAGE_ID"
#define NS_ATTRIBUTE_PROVIDER_ID "PROVIDER_ID"
#define NS_ATTRIBUTE_CONSUMER_ID "CONSUMER_ID"
#define NS_ATTRIBUTE_TOPIC_LIST "TOPIC_LIST"
#define NS_ATTRIBUTE_TOPIC_NAME "TOPIC_NAME"
#define NS_ATTRIBUTE_TOPIC_SELECTION "TOPIC_STATE"
#define NS_ATTRIBUTE_TITLE "TITLE"
#define NS_ATTRIBUTE_TEXT "CONTENTTEXT"
#define NS_ATTRIBUTE_SOURCE "SOURCE"
#define NS_ATTRIBUTE_STATE "STATE"
#define NS_ATTRIBUTE_DEVICE "DEVICE"
#define NS_ATTRIBUTE_TYPE "TYPE"
#define NS_ATTRIBUTE_DATETIME "DATE_TIME"
#define NS_ATTRIBUTE_TTL "TTL"
#define NS_ATTRIBUTE_ICON_IMAGE "ICON_IMAGE"

typedef enum eConnectionState
{
    DISCONNECTED = 0,
    CONNECTED = 1,

} NSConnectionState;

typedef enum eSchedulerType
{
    CALLBACK_RESPONSE_SCHEDULER = 0,
    DISCOVERY_SCHEDULER = 1,
    SUBSCRIPTION_SCHEDULER = 2,
    NOTIFICATION_SCHEDULER = 3,
    TOPIC_SCHEDULER = 4,
} NSSchedulerType;

typedef enum eTaskType
{
    TASK_REGISTER_RESOURCE = 1000,
    TASK_PUBLISH_RESOURCE = 1001,

    TASK_START_PRESENCE = 2000,
    TASK_STOP_PRESENCE = 2001,

    TASK_RECV_SUBSCRIPTION = 3000,
    TASK_RECV_UNSUBSCRIPTION = 3001,
    TASK_SEND_POLICY = 3002,
    TASK_SEND_ALLOW = 3003,
    TASK_SEND_DENY = 3004,
    TASK_SYNC_SUBSCRIPTION = 3005,

    TASK_SEND_NOTIFICATION = 4000,
    TASK_SEND_PENDING_NOTI = 4001,

    TASK_RECV_SYNCINFO = 5000,
    TASK_RECV_READ = 5001,
    TASK_RECV_DISMISS = 5003,
    TASK_SEND_SYNCINFO = 5099,
    TASK_MAKE_SYNCINFO = 5100,
    TASK_SEND_READ = 5101,
    TASK_SEND_DISMISS = 5102,

    TASK_CONSUMER_REQ_DISCOVER = 8001,
    TASK_CONSUMER_REQ_SUBSCRIBE = 8002,
    TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL = 8003,
    TASK_CONSUMER_SENT_REQ_OBSERVE = 8004,
    TASK_CONSUMER_RECV_PROVIDER_CHANGED = 8005,
    TASK_CONSUMER_RECV_MESSAGE = 8101,

    TASK_CONSUMER_PROVIDER_DISCOVERED = 8201,
    TASK_CONSUMER_PROVIDER_DELETED = 8202,
    TASK_CONSUMER_RECV_CONFIRM = 8206,

    TASK_CONSUMER_REQ_TOPIC_URI = 8299,
    TASK_CONSUMER_REQ_TOPIC_LIST = 8300,
    TASK_CONSUMER_RECV_TOPIC_LIST = 8031,
    TASK_CONSUMER_SELECT_TOPIC_LIST = 8303,

    TASK_EVENT_CONNECTED = 9000,
    TASK_EVENT_CONNECTED_TCP = 9001,
    TASK_EVENT_DISCONNECTED = 9002,

    TASK_CB_SUBSCRIPTION = 10000,
    TASK_CB_SYNC = 10001,

    TASK_SEND_TOPICS = 11000,
    TASK_REGISTER_TOPIC = 11001,
    TASK_UNREGISTER_TOPIC = 11002,
    TASK_SUBSCRIBE_TOPIC = 11003,
    TASK_UNSUBSCRIBE_TOPIC = 11004,
    TASK_POST_TOPIC = 11005,
    TASK_GET_TOPICS = 11006,
    TAST_GET_CONSUMER_TOPICS = 11007

} NSTaskType;

typedef enum eCache
{
    NS_CONSUMER_BLACKLIST = 0,
    NS_CONSUMER_WHITELIST = 1,

} NSCache;

typedef enum eCacheType
{
    NS_PROVIDER_CACHE_SUBSCRIBER = 1000,
    NS_PROVIDER_CACHE_MESSAGE = 1001,
    NS_PROVIDER_CACHE_CONSUMER_TOPIC_NAME = 1002,
    NS_PROVIDER_CACHE_CONSUMER_TOPIC_CID = 1003,
    NS_PROVIDER_CACHE_REGISTER_TOPIC = 1004,
    NS_PROVIDER_CACHE_SUBSCRIBER_OBSERVE_ID = 1005,

    NS_CONSUMER_CACHE_PROVIDER = 2000,
    NS_CONSUMER_CACHE_MESSAGE = 2001,
} NSCacheType;

typedef enum eResourceType
{
    NS_RESOURCE_MESSAGE = 1000,
    NS_RESOURCE_SYNC = 1001,
    NS_RESOURCE_TOPIC = 1002,
} NSResourceType;

#endif /* _NS_CONSTANTS_H_ */
