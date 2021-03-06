/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.accountserver.resources.acl.invite;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.InviteTable;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.util.Cbor;

/**
 *
 * This class provides a set of APIs to invite a user to a group
 *
 */

public class InviteManager {

    private TypeCastingManager<InviteTable> mTypeInvite = new TypeCastingManager<>();

    private class InviteSubscriber {
        InviteSubscriber(Device subscriber, IRequest request) {
            mSubscriber = subscriber;
            mRequest = request;
        }

        public Device   mSubscriber;
        public IRequest mRequest;
    }

    private HashMap<String, InviteSubscriber> mSubscribers = new HashMap<>();

    public void addInvitation(String uid, String gid, String mid) {

        // create invitation table
        InviteTable newInviteTable = new InviteTable(uid, gid, mid);

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.KEYFIELD_INVITE_USER, uid);
        condition.put(Constants.KEYFIELD_GID, gid);
        condition.put(Constants.KEYFIELD_INVITED_USER, mid);

        if (AccountDBManager.getInstance()
                .selectRecord(Constants.INVITE_TABLE, condition).isEmpty()) {
            AccountDBManager.getInstance().insertRecord(Constants.INVITE_TABLE,
                    mTypeInvite.convertObjectToMap(newInviteTable));
            notifyToSubscriber(uid);
            notifyToSubscriber(mid);
        }
    }

    public void deleteInvitation(String mid, String gid) {
        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.REQ_GROUP_ID, gid);
        condition.put(Constants.KEYFIELD_INVITED_USER, mid);

        InviteTable getInviteTable = new InviteTable();

        List<HashMap<String, Object>> getInviteList = AccountDBManager
                .getInstance().selectRecord(Constants.INVITE_TABLE, condition);

        ArrayList<String> uidList = new ArrayList<>();
        for (HashMap<String, Object> getInvite : getInviteList) {
            getInviteTable = mTypeInvite.convertMaptoObject(getInvite,
                    getInviteTable);
            uidList.add(getInviteTable.getInviteUser());
        }

        AccountDBManager.getInstance().deleteRecord(Constants.INVITE_TABLE,
                condition);

        notifyToSubscriber(mid);
        for (String uid : uidList) {
            notifyToSubscriber(uid);
        }
    }

    public void cancelInvitation(String uid, String gid, String mid) {

        HashMap<String, Object> condition = new HashMap<>();

        condition.put(Constants.REQ_GROUP_ID, gid);
        condition.put(Constants.KEYFIELD_INVITED_USER, mid);
        condition.put(Constants.KEYFIELD_INVITE_USER, uid);

        AccountDBManager.getInstance().deleteRecord(Constants.INVITE_TABLE,
                condition);

        notifyToSubscriber(uid);
        notifyToSubscriber(mid);
    }

    public HashMap<String, Object> getInvitationInfo(String uid) {
        HashMap<String, Object> responsePayload = new HashMap<>();

        ArrayList<Object> invitePayloadData = null;
        ArrayList<Object> invitedPayloadData = null;

        List<InviteTable> inviteList = getInviteTableList(
                Constants.KEYFIELD_INVITE_USER, uid);
        if (!inviteList.isEmpty()) {
            invitePayloadData = new ArrayList<>();
            for (InviteTable invite : inviteList) {
                HashMap<String, String> inviteElement = new HashMap<>();
                inviteElement.put(Constants.REQ_GROUP_ID, invite.getGid());
                inviteElement.put(Constants.REQ_MEMBER,
                        invite.getInvitedUser());
                invitePayloadData.add(inviteElement);
            }
        }

        List<InviteTable> invitedList = getInviteTableList(
                Constants.KEYFIELD_INVITED_USER, uid);
        if (!invitedList.isEmpty()) {
            invitedPayloadData = new ArrayList<>();
            for (InviteTable invited : invitedList) {
                HashMap<String, String> invitedElement = new HashMap<>();
                invitedElement.put(Constants.REQ_GROUP_ID, invited.getGid());
                invitedElement.put(Constants.REQ_MEMBER,
                        invited.getInviteUser());
                invitedPayloadData.add(invitedElement);
            }
        }

        responsePayload.put(Constants.RESP_INVITE, invitePayloadData);
        responsePayload.put(Constants.RESP_INVITED, invitedPayloadData);

        return responsePayload;
    }

    public HashMap<String, Object> addSubscriber(String uid, Device subscriber,
            IRequest request) {

        InviteSubscriber newSubscriber = new InviteSubscriber(subscriber,
                request);
        mSubscribers.put(uid, newSubscriber);

        return getInvitationInfo(uid);
    }

    public HashMap<String, Object> removeSubscriber(String uid) {

        if (mSubscribers.containsKey(uid)) {
            mSubscribers.remove(uid);
        }

        return getInvitationInfo(uid);
    }

    private void notifyToSubscriber(String id) {

        synchronized (mSubscribers) {
            if (!mSubscribers.containsKey(id)) {
                return;
            }
            Cbor<HashMap<String, Object>> cbor = new Cbor<>();
            mSubscribers.get(id).mSubscriber.sendResponse(
                    MessageBuilder.createResponse(mSubscribers.get(id).mRequest,
                            ResponseStatus.CONTENT,
                            ContentFormat.APPLICATION_CBOR,
                            cbor.encodingPayloadToCbor(getInvitationInfo(id))));
        }
    }

    private List<InviteTable> getInviteTableList(String property, String uid) {

        InviteTable getInviteTable = new InviteTable();
        ArrayList<InviteTable> inviteList = new ArrayList<>();

        HashMap<String, Object> condition = new HashMap<>();
        condition.put(property, uid);
        ArrayList<HashMap<String, Object>> mapInviteList = AccountDBManager
                .getInstance().selectRecord(Constants.INVITE_TABLE, condition);
        if (mapInviteList == null) {
            throw new BadRequestException("uid is invalid");
        }
        for (HashMap<String, Object> mapInviteTable : mapInviteList) {

            getInviteTable = mTypeInvite.convertMaptoObject(mapInviteTable,
                    getInviteTable);
            inviteList.add(getInviteTable);
        }
        return inviteList;

    }

}
