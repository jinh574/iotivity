/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "caipinterface.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <errno.h>

#ifdef __linux__
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#endif

#include "octhread.h"
#include "caadapterutils.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define TAG "OIC_CA_IP_MONITOR"

/**
 * Mutex for synchronizing access to cached interface and IP address information.
 */
static oc_mutex g_networkMonitorContextMutex = NULL;

/**
 * Used to storing network interface.
 */
static u_arraylist_t *g_netInterfaceList = NULL;

static CAIPConnectionStateChangeCallback g_networkChangeCallback = NULL;

static CAResult_t CAIPInitializeNetworkMonitorList();
static void CAIPDestroyNetworkMonitorList();
static CAInterface_t *CANewInterfaceItem(int index, const char *name, int family,
                                         const char *addr, int flags);

static CAResult_t CAIPInitializeNetworkMonitorList()
{
    if (!g_networkMonitorContextMutex)
    {
        g_networkMonitorContextMutex = oc_mutex_new();
        if (!g_networkMonitorContextMutex)
        {
            OIC_LOG(ERROR, TAG, "oc_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }

    if (!g_netInterfaceList)
    {
        g_netInterfaceList = u_arraylist_create();
        if (!g_netInterfaceList)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_create has failed");
            CAIPDestroyNetworkMonitorList();
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}

static void CAIPDestroyNetworkMonitorList()
{
    if (g_netInterfaceList)
    {
        u_arraylist_destroy(g_netInterfaceList);
        g_netInterfaceList = NULL;
    }

    if (g_networkMonitorContextMutex)
    {
        oc_mutex_free(g_networkMonitorContextMutex);
        g_networkMonitorContextMutex = NULL;
    }
}

static bool CACmpNetworkList(uint32_t ifiindex)
{
    if (!g_netInterfaceList)
    {
        OIC_LOG(ERROR, TAG, "g_netInterfaceList is NULL");
        return false;
    }

    oc_mutex_lock(g_networkMonitorContextMutex);

    uint32_t list_length = u_arraylist_length(g_netInterfaceList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CAInterface_t *currItem = (CAInterface_t *) u_arraylist_get(g_netInterfaceList, list_index);
        if (currItem->index == ifiindex)
        {
            oc_mutex_unlock(g_networkMonitorContextMutex);
            return true;
        }
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    return false;
}

static CAResult_t CAAddNetworkMonitorList(CAInterface_t *ifitem)
{
    VERIFY_NON_NULL(g_netInterfaceList, TAG, "g_netInterfaceList is NULL");
    VERIFY_NON_NULL(ifitem, TAG, "ifitem is NULL");

    oc_mutex_lock(g_networkMonitorContextMutex);
    bool result = u_arraylist_add(g_netInterfaceList, (void *) ifitem);
    if (!result)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
        oc_mutex_unlock(g_networkMonitorContextMutex);
        return CA_STATUS_FAILED;
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    return CA_STATUS_OK;
}

static void CARemoveNetworkMonitorList(int ifiindex)
{
    VERIFY_NON_NULL_VOID(g_netInterfaceList, TAG, "g_netInterfaceList is NULL");

    oc_mutex_lock(g_networkMonitorContextMutex);

    uint32_t list_length = u_arraylist_length(g_netInterfaceList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CAInterface_t *removedifitem = (CAInterface_t *) u_arraylist_get(
                g_netInterfaceList, list_index);
        if (removedifitem && ((int)removedifitem->index) == ifiindex)
        {
            if (u_arraylist_remove(g_netInterfaceList, list_index))
            {
                OICFree(removedifitem);
                oc_mutex_unlock(g_networkMonitorContextMutex);
                return;
            }
            continue;
        }
    }
    oc_mutex_unlock(g_networkMonitorContextMutex);
    return;
}

CAResult_t CAIPStartNetworkMonitor()
{
    return CAIPInitializeNetworkMonitorList();
}

CAResult_t CAIPStopNetworkMonitor()
{
    CAIPDestroyNetworkMonitorList();
    return CA_STATUS_OK;
}

int CAGetPollingInterval(int interval)
{
    return interval;
}

void CAIPSetNetworkMonitorCallback(CAIPConnectionStateChangeCallback callback)
{
    g_networkChangeCallback = callback;
}

static CAInterface_t *CANewInterfaceItem(int index, const char *name, int family,
                                         const char *addr, int flags)
{
    CAInterface_t *ifitem = (CAInterface_t *)OICCalloc(1, sizeof (CAInterface_t));
    if (!ifitem)
    {
        OIC_LOG(ERROR, TAG, "Malloc failed");
        return NULL;
    }

    OICStrcpy(ifitem->name, sizeof (ifitem->name), name);
    ifitem->index = index;
    ifitem->family = family;
    OICStrcpy(ifitem->addr, sizeof (ifitem->addr), addr);
    ifitem->flags = flags;

    return ifitem;
}

CAInterface_t *CAFindInterfaceChange()
{
    CAInterface_t *foundNewInterface = NULL;
#ifdef __linux__
    char buf[4096] = { 0 };
    struct nlmsghdr *nh = NULL;
    struct sockaddr_nl sa = { .nl_family = 0 };
    struct iovec iov = { .iov_base = buf,
                         .iov_len = sizeof (buf) };
    struct msghdr msg = { .msg_name = (void *)&sa,
                          .msg_namelen = sizeof (sa),
                          .msg_iov = &iov,
                          .msg_iovlen = 1 };

    size_t len = recvmsg(caglobals.ip.netlinkFd, &msg, 0);

    for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, len); nh = NLMSG_NEXT(nh, len))
    {
        if (nh != NULL && nh->nlmsg_type != RTM_NEWLINK)
        {
            continue;
        }

        struct ifinfomsg *ifi = (struct ifinfomsg *)NLMSG_DATA(nh);

        int ifiIndex = ifi->ifi_index;
        u_arraylist_t *iflist = CAIPGetInterfaceInformation(ifiIndex);

        if ((!ifi || (ifi->ifi_flags & IFF_LOOPBACK) || !(ifi->ifi_flags & IFF_RUNNING)))
        {
            bool isFound = CACmpNetworkList(ifiIndex);
            if (isFound)
            {
                CARemoveNetworkMonitorList(ifiIndex);
                if (g_networkChangeCallback)
                {
                    g_networkChangeCallback(CA_ADAPTER_IP ,CA_INTERFACE_DOWN);
                }
            }
            continue;
        }

        if (!iflist)
        {
            OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
            return NULL;
        }

        uint32_t listLength = u_arraylist_length(iflist);
        for (uint32_t i = 0; i < listLength; i++)
        {
            CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
            if (!ifitem)
            {
                continue;
            }

            if ((int)ifitem->index != ifiIndex)
            {
                continue;
            }

            foundNewInterface = CANewInterfaceItem(ifitem->index, ifitem->name, ifitem->family,
                                                   ifitem->addr, ifitem->flags);
            break;    // we found the one we were looking for
        }
        u_arraylist_destroy(iflist);
    }
#endif
    return foundNewInterface;
}

u_arraylist_t *CAIPGetInterfaceInformation(int desiredIndex)
{
    if (desiredIndex < 0)
    {
        OIC_LOG_V(ERROR, TAG, "invalid index : %d", desiredIndex);
        return NULL;
    }

    u_arraylist_t *iflist = u_arraylist_create();
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create iflist: %s", strerror(errno));
        return NULL;
    }

    struct ifaddrs *ifp = NULL;
    if (-1 == getifaddrs(&ifp))
    {
        OIC_LOG_V(ERROR, TAG, "Failed to get ifaddrs: %s", strerror(errno));
        u_arraylist_destroy(iflist);
        return NULL;
    }
    OIC_LOG(DEBUG, TAG, "Got ifaddrs");

    struct ifaddrs *ifa = NULL;
    for (ifa = ifp; ifa; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        int family = ifa->ifa_addr->sa_family;
        if ((ifa->ifa_flags & IFF_LOOPBACK) || (AF_INET != family && AF_INET6 != family))
        {
            continue;
        }

        int ifindex = if_nametoindex(ifa->ifa_name);
        if (desiredIndex && (ifindex != desiredIndex))
        {
            continue;
        }

        int length = u_arraylist_length(iflist);
        int already = false;
        for (int i = length-1; i >= 0; i--)
        {
            CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);

            if (ifitem
                && (int)ifitem->index == ifindex
                && ifitem->family == (uint16_t)family)
            {
                already = true;
                break;
            }
        }
        if (already)
        {
            continue;
        }

        CAInterface_t *ifitem = (CAInterface_t *)OICCalloc(1, sizeof(CAInterface_t));
        if (!ifitem)
        {
            OIC_LOG(ERROR, TAG, "Malloc failed");
            goto exit;
        }

        OICStrcpy(ifitem->name, INTERFACE_NAME_MAX, ifa->ifa_name);
        ifitem->index = ifindex;
        ifitem->family = family;
        ifitem->flags = ifa->ifa_flags;

        if (ifitem->family == AF_INET6)
        {
            struct sockaddr_in6 *in6 = (struct sockaddr_in6*) ifa->ifa_addr;
            inet_ntop(ifitem->family, (void *)&(in6->sin6_addr), ifitem->addr,
                      sizeof(ifitem->addr));
        }
        else if (ifitem->family == AF_INET)
        {
            struct sockaddr_in *in = (struct sockaddr_in*) ifa->ifa_addr;
            inet_ntop(ifitem->family, (void *)&(in->sin_addr), ifitem->addr,
                      sizeof(ifitem->addr));
        }

        bool result = u_arraylist_add(iflist, ifitem);
        if (!result)
        {
            OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
            goto exit;
        }

        bool isFound = CACmpNetworkList(ifitem->index);
        if (!isFound)
        {
            CAInterface_t *newifitem = CANewInterfaceItem(ifitem->index, ifitem->name, ifitem->family,
                                                          ifitem->addr, ifitem->flags);
            CAResult_t ret = CAAddNetworkMonitorList(newifitem);
            if (CA_STATUS_OK != ret)
            {
                OICFree(newifitem);
                goto exit;
            }
            if (g_networkChangeCallback)
            {
                g_networkChangeCallback(CA_ADAPTER_IP, CA_INTERFACE_UP);
            }
            OIC_LOG_V(DEBUG, TAG, "Added interface: %s (%d)", ifitem->name, ifitem->family);
        }
    }
    freeifaddrs(ifp);
    return iflist;

exit:
    freeifaddrs(ifp);
    u_arraylist_destroy(iflist);
    return NULL;
}
